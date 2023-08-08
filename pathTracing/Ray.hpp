#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H
#include "Vector.hpp"
struct Ray{
    // 射线由起点 origin 和方向 direction 组成，可以用 t_min 和 t_max 来设置射线的有效时间范围
    Vector3f origin;     // 起点
    Vector3f direction;  // 方向
    Vector3f direction_inv; // 方向的逆
    double t;            // 射线的传输时间
    double t_min, t_max; // 射线的有效时间范围

    // 构造函数
    Ray(const Vector3f& ori, const Vector3f& dir, const double _t = 0.0): origin(ori), direction(dir),t(_t) {
        direction_inv = Vector3f(1./direction.x, 1./direction.y, 1./direction.z);
        t_min = 0.0;
        t_max = std::numeric_limits<double>::max();

    }

    // 用参数 t 得到射线上的点
    Vector3f operator()(double t) const{return origin+direction*t;}

    // 重载输出运算符，用于输出射线信息
    friend std::ostream &operator<<(std::ostream& os, const Ray& r){
        os<<"[origin:="<<r.origin<<", direction="<<r.direction<<", time="<< r.t<<"]\n";
        return os;
    }
};
#endif //RAYTRACING_RAY_H
