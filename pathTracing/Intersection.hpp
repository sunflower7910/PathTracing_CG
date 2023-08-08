
#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"

// 前向声明
class Object;
class Sphere;


// 相交结构体，用于保存光线与物体相交的信息
struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened; //是否发生碰撞
    Vector3f coords; //碰撞发生的坐标
    Vector3f tcoords; // 纹理坐标（若使用纹理）
    Vector3f normal; //相交三角形的法线
    Vector3f emit; //光源光
    double distance; //碰撞点距光源的距离

    Object* obj;     // 相交的物体指针
    Material* m;    // 相交点处的材质指针
};
#endif //RAYTRACING_INTERSECTION_H
