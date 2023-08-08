#pragma once

#include "Vector.hpp"
#include "Light.hpp"
#include "global.hpp"


// 表示面光源的类，继承自光源类
class AreaLight : public Light
{
public:
    // 构造函数，通过给定的位置和光强来初始化面光源
    AreaLight(const Vector3f &p, const Vector3f &i) : Light(p, i)
    {
        // 设置面光源的法线为向下的y轴方向
        normal = Vector3f(0, -1, 0);
        // 设置面光源的u方向为x轴方向
        u = Vector3f(1, 0, 0);
        // 设置面光源的v方向为z轴方向
        v = Vector3f(0, 0, 1);
        // 设置面光源的长度为100
        length = 100;
    }

    // 采样面光源上的一个点
    Vector3f SamplePoint() const
    {
        // 生成两个在[0, 1]范围内的随机数
        auto random_u = get_random_float();
        auto random_v = get_random_float();
        // 计算采样点的位置并返回
        return position + random_u * u + random_v * v;
    }

    float length;   // 面光源的长度
    Vector3f normal;    // 面光源的法线方向
    Vector3f u;         // 面光源的u方向
    Vector3f v;         // 面光源的v方向
};
