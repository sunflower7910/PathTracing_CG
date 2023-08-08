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
    Object() {}//����
    virtual ~Object() {}//������
    // �ж������Ƿ��������ཻ
    virtual bool intersect(const Ray& ray) = 0;
    // �ж������Ƿ��������ཻ�������ؽ�����Ϣ
    virtual bool intersect(const Ray& ray, float &, uint32_t &) const = 0;
    // ��ȡ����������Ľ�����Ϣ
    virtual Intersection getIntersection(Ray _ray) = 0;
    // ��ȡ���㴦��������ԣ����編��������������
    virtual void getSurfaceProperties(const Vector3f &, const Vector3f &, const uint32_t &, const Vector2f &, Vector3f &, Vector2f &) const = 0;
    // ��ȡ�������ĳ�����������ɫ
    virtual Vector3f evalDiffuseColor(const Vector2f &) const =0;
    // ��ȡ����İ�Χ��
    virtual Bounds3 getBounds()=0;
    // ��ȡ����ı����
    virtual float getArea()=0;
    // ���������������һ���㣬������������ܶȺ�����ֵ
    virtual void Sample(Intersection &pos, float &pdf)=0;
    // �ж������Ƿ��ǹ�Դ�����Ƿ����
    virtual bool hasEmit()=0;
};



#endif //RAYTRACING_OBJECT_H
