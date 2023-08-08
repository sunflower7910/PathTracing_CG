#pragma once
#ifndef RAYTRACING_OBJECT_H
#define RAYTRACING_OBJECT_H

#include "Vector.hpp"
#include "global.hpp"
#include "Bounds3.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

class Object
{
public:
    Object() {}//构造
    virtual ~Object() {}//虚析构
    // 判断射线是否与物体相交
    virtual bool intersect(const Ray& ray) = 0;
    // 判断射线是否与物体相交，并返回交点信息
    virtual bool intersect(const Ray& ray, float &, uint32_t &) const = 0;
    // 获取射线与物体的交点信息
    virtual Intersection getIntersection(Ray _ray) = 0;
    // 获取交点处表面的属性，例如法向量和纹理坐标
    virtual void getSurfaceProperties(const Vector3f &, const Vector3f &, const uint32_t &, const Vector2f &, Vector3f &, Vector2f &) const = 0;
    // 获取物体表面某点的漫反射颜色
    virtual Vector3f evalDiffuseColor(const Vector2f &) const =0;
    // 获取物体的包围盒
    virtual Bounds3 getBounds()=0;
    // 获取物体的表面积
    virtual float getArea()=0;
    // 在物体上随机采样一个点，并返回其概率密度函数的值
    virtual void Sample(Intersection &pos, float &pdf)=0;
    // 判断物体是否是光源，即是否发射光
    virtual bool hasEmit()=0;
};



#endif //RAYTRACING_OBJECT_H
