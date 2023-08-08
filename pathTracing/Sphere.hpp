
#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "Object.hpp"
#include "Vector.hpp"
#include "Bounds3.hpp"
#include "Material.hpp"


// 球体类，继承自Object类
class Sphere : public Object{
public:
    Vector3f center; // 球心坐标
    float radius, radius2; // 球体半径及其平方，用于加速计算
    Material *m; // 球体的材质指针
    float area; // 球体的表面积，用于光源采样
     
    // 构造函数，初始化球体的位置、半径和材质
    Sphere(const Vector3f &c, const float &r, Material* mt = new Material()) : center(c), radius(r), radius2(r * r), m(mt), area(4 * M_PI *r *r) {}

    // 判断射线是否与球体相交，使用解析解进行求解
    bool intersect(const Ray& ray) {
        // analytic solution
        Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        float area = 4 * M_PI * radius2;
        if (!solveQuadratic(a, b, c, t0, t1)) return false;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }

    // 判断射线是否与球体相交，并返回相交的距离和索引（球体没有索引，此处并不需要）
    bool intersect(const Ray& ray, float &tnear, uint32_t &index) const
    {
        // analytic solution
        Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        if (!
        solveQuadratic(a, b, c, t0, t1)) return false;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        tnear = t0;

        return true;
    }

    // 获取与射线相交的交点信息，包括交点坐标、法向量、材质等
    Intersection getIntersection(Ray ray){
        Intersection result;
        result.happened = false;
        Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        if (!solveQuadratic(a, b, c, t0, t1)) return result;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return result;

		// 相交判定修改
		if (t0 > 0.5) {
			result.happened = true;

			result.coords = Vector3f(ray.origin + ray.direction * t0);
			result.normal = normalize(Vector3f(result.coords - center));
			result.m = this->m;
			result.obj = this;
			result.distance = t0;
		}
        return result;

    }

    // 获取球体表面的漫反射颜色（这里返回空向量，需要根据具体材质进行实现）
    Vector3f evalDiffuseColor(const Vector2f &st)const {
        //return m->getColor();
        return {};
    }


    void getSurfaceProperties(const Vector3f &P, const Vector3f &I, const uint32_t &index, const Vector2f &uv, Vector3f &N, Vector2f &st) const
    { N = normalize(P - center); }

    
    // 获取球体的包围盒
    Bounds3 getBounds(){
        return Bounds3(Vector3f(center.x-radius, center.y-radius, center.z-radius),
                       Vector3f(center.x+radius, center.y+radius, center.z+radius));
    }

    // 在球体表面随机采样一点并计算其概率密度函数（pdf）
    void Sample(Intersection &pos, float &pdf){
        float theta = 2.0 * M_PI * get_random_float(), phi = M_PI * get_random_float();
        Vector3f dir(std::cos(phi), std::sin(phi)*std::cos(theta), std::sin(phi)*std::sin(theta));
        pos.coords = center + radius * dir;
        pos.normal = dir;
        pos.emit = m->getEmission();
        pdf = 1.0f / area;
    }

    // 获取球体的表面积
    float getArea(){
        return area;
    }

    // 判断球体是否为光源（根据材质的发光属性判断）
    bool hasEmit(){
        return m->hasEmission();
    }
};


#endif //RAYTRACING_SPHERE_H
