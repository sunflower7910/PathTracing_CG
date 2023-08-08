#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

//sampleLight : 得到lightInter（场景中光源区域的任意一点），pdf（该光源的密度）
void Scene::sampleLight(Intersection &pos, float &pdf) const
{
	/**
	 * @brief 
	 * pos:得到lightInter（场景中光源区域的任意一点），
	 * pdf:pdf（该光源的概率密度）
	 */
    float emit_area_sum = 0; //保存总的光源面积（为均匀分布的概率中找到obj使用）
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }

	//随机生成一个服从[0,1]的均匀分布的数
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){//按光源面积比例，随机找到一个光源面，再在这个光源面中找到一个点
				//这里调用的是 MeshTriangle 中的 Sample
                objects[k]->Sample(pos, pdf);//pos为该光源面中随机找到的一个点，pdf为 1/该模型的面积
                break;
            }
        }
    }
}


// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
	/**
	 * @brief 路径追踪
	 * 1.求出该光线与场景的交点
	 * 2.如果交点为光源
	 		如果射线第一次打到光源，则直接返回光源颜色。
			如果射线打到光源，但不是该像素的直接光照，则返回0。该问题在交点为物体时求解。
	 * 3.如果交点为物体
	 		生成一条由该物体指向随机生成的光源的一条光线，与场景求交，交点为light2obj
			如果该光线击中光源，计算直接光照值
			（递归）光线是否继续弹射，计算间接光照？（俄罗斯轮盘赌）
	 * 4.返回得到的光线值
	 */

	// 1.
	// 光线 与 BVH 求交
	Intersection inter = intersect(ray);
	if (inter.happened)
	{
		// 2.
		// 如果射线第一次打到光源，直接返回光源颜色；如果不是第一次，则返回(0,0,0)
		if (inter.m->hasEmission())
		{
			if (depth == 0) 
			{
				return inter.m->getEmission();//光源颜色
			}
			else return Vector3f(0, 0, 0);
		}

		// 3.
		Vector3f L_dir(0, 0, 0);//当前：直接光照
		Vector3f L_indir(0, 0, 0);//当前：间接光照(递归结果)

		// 随机生成光线 lightInter
		// 随机 sample 灯光，用该 sample 的结果判断射线是否击中光源
		// lightInter（场景中光源区域的任意一点），pdf（该光源的概率密度）
		Intersection lightInter;
		float pdf_light = 0.0f;
		sampleLight(lightInter, pdf_light);

		// 物体表面法线
		auto& N = inter.normal;
		// 灯光表面法线
		auto& NN = lightInter.normal;

		auto& objPos = inter.coords;
		auto& lightPos = lightInter.coords;

		auto diff = lightPos - objPos;
		auto lightDir = diff.normalized();
		float lightDistance = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		Ray light(objPos, lightDir);
		// 与场景求交，交点为light2obj 
		Intersection light2obj = intersect(light);


		// 如果该光线击中光源（及该光源可以直接照射到该点），计算直接光照值
		if (light2obj.happened && (light2obj.coords - lightPos).norm() < 1e-2)
		{
			//获取改材质的brdf，这里的 BRDF 为漫反射（brdf=Kd/pi）
			Vector3f f_r = inter.m->eval(ray.direction, lightDir, N);
			
			//直接光照光 = 光源光 * brdf * 光线和物体角度衰减 * 光线和光源法线角度衰减 / 光线距离 / 该点的概率密度（1/该光源的面积）
			L_dir = lightInter.emit * f_r * dotProduct(lightDir, N) * dotProduct(-lightDir, NN) / lightDistance / pdf_light;
		}

		//俄罗斯轮盘赌，确定是否继续弹射光线
		if (get_random_float() < RussianRoulette)
		{
			//获取半平面上的随机弹射方向
			Vector3f nextDir = inter.m->sample(ray.direction, N).normalized();
			//定义弹射光线
			Ray nextRay(objPos, nextDir);
			//获取相交点
			Intersection nextInter = intersect(nextRay);
			//如果有相交，且是与物体相交
			if (nextInter.happened && !nextInter.m->hasEmission())
			{
				//该点间接光= 弹射点反射光 * brdf * 角度衰减 / pdf(认为该点四面八方都接收到了该方向的光强，为1/(2*pi)) / 俄罗斯轮盘赌值(强度矫正值)
				float pdf = inter.m->pdf(ray.direction, nextDir, N);
				Vector3f f_r = inter.m->eval(ray.direction, nextDir, N);
				L_indir = castRay(nextRay, depth + 1) * f_r * dotProduct(nextDir, N) / pdf / RussianRoulette;
			}
		}

		//最后返回直接光照和间接光照
		return L_dir + L_indir;
	}

	//如果光线与场景无交点sample
	return Vector3f(0, 0, 0);
}
