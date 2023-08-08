#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    // 构建BVH加速结构
    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    // 递归构建BVH加速结构
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    // 计算包围盒
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        // 创建叶子节点
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        node->area = objects[0]->getArea();
        return node;
    }
    else if (objects.size() == 2) {
        // 创建包含两个子节点的节点
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
        return node;
    }
    else {
        // 选择最长边作为分割维度，对物体进行排序
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    // 光线与BVH加速结构中物体的相交测试，返回相交信息
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    /**
     * @brief 最终获取场景中某个三角形和该光线的交点信息
     */
	// TODO Traverse the BVH to find intersection
    Intersection inter;

	// 光线方向
	float x = ray.direction.x;
	float y = ray.direction.y;
	float z = ray.direction.z;
	// 判断坐标是否为负
	std::array<int, 3> dirsIsNeg{ int(x > 0),int(y > 0),int(z > 0) };

	// 判断结点的包围盒与光线是否相交
	if (node->bounds.IntersectP(ray, ray.direction_inv, dirsIsNeg) == false) return inter;

	if (node->left == nullptr && node->right == nullptr)
	{
		inter = node->object->getIntersection(ray);
		return inter;
	}

	// 递归判断子节点是否存在与光线相交的情况
	auto hit1 = getIntersection(node->left, ray);
	auto hit2 = getIntersection(node->right, ray);

	if (hit1.distance < hit2.distance)
		return hit1;
	return hit2;
}


void BVHAccel::getSample(BVHBuildNode* node, float p, Intersection &pos, float &pdf){
     // 通过 p 来划分 BVH 中的对象，并对该对象进行一个采样

    //当前节点为叶子节点叶子节点
    if(node->left == nullptr || node->right == nullptr){
        //在三角形中随机采样
        node->object->Sample(pos, pdf);
        pdf *= node->area;
        return;
    }
    if(p < node->left->area) getSample(node->left, p, pos, pdf);
    else getSample(node->right, p - node->left->area, pos, pdf);
}

void BVHAccel::Sample(Intersection &pos, float &pdf){
    // p 是 bvh树 中的一个划分
    float p = std::sqrt(get_random_float()) * root->area;
    //采样
    getSample(root, p, pos, pdf);

    pdf /= root->area;
}