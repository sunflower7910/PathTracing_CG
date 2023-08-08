
#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include <atomic>
#include <vector>
#include <memory>
#include <ctime>
#include "Object.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Vector.hpp"

struct BVHBuildNode;
// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;

// BVHAccel Declarations
// BVH加速器类
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;
class BVHAccel {

public:
    // BVHAccel Public Types
    // 分割方法：NAIVE（朴素）和SAH（表面积启发式）
    enum class SplitMethod { NAIVE, SAH };

    // BVHAccel Public Methods
    // 构造函数，传入物体集合p、每个节点的最大物体数目maxPrimsInNode和分割方法splitMethod
    BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);
    // 获取整个场景的边界
    Bounds3 WorldBound() const;
    ~BVHAccel();

    // 光线与场景中物体的相交测试，返回相交信息
    Intersection Intersect(const Ray &ray) const;

    // 最终获取场景中某个三角形和该光线的交点信息
    Intersection getIntersection(BVHBuildNode* node, const Ray& ray)const;
    // 光线与场景中物体的相交测试，返回是否相交
    bool IntersectP(const Ray &ray) const;
    // BVH根节点指针
    BVHBuildNode* root;

    // BVHAccel Private Methods
    // 递归构建BVH加速结构，传入物体集合objects
    BVHBuildNode* recursiveBuild(std::vector<Object*>objects);

    // BVHAccel Private Data
    const int maxPrimsInNode;// 每个节点的最大物体数目
    const SplitMethod splitMethod;// 分割方法
    std::vector<Object*> primitives; // 物体集合

    // 获取节点在整个加速结构中的采样点和采样概率
    void getSample(BVHBuildNode* node, float p, Intersection &pos, float &pdf);
    // 对整个加速结构进行采样，返回采样点和采样概率
    void Sample(Intersection &pos, float &pdf);
};

// BVH构建节点结构体
struct BVHBuildNode {
    Bounds3 bounds; // 节点边界
    BVHBuildNode* left; // 左子节点
    BVHBuildNode* right; // 右子节点
    Object* object; // 关联的物体指针
    float area; // 物体表面积

public:
    int splitAxis=0, firstPrimOffset=0, nPrimitives=0; // 分割轴，首个物体偏移量，物体数量
    // BVHBuildNode Public Methods
    BVHBuildNode(){
        bounds = Bounds3();
        left = nullptr;right = nullptr;
        object = nullptr;
    }
};




#endif //RAYTRACING_BVH_H
