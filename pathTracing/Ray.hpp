#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H
#include "Vector.hpp"
struct Ray{
    // ��������� origin �ͷ��� direction ��ɣ������� t_min �� t_max ���������ߵ���Чʱ�䷶Χ
    Vector3f origin;     // ���
    Vector3f direction;  // ����
    Vector3f direction_inv; // �������
    double t;            // ���ߵĴ���ʱ��
    double t_min, t_max; // ���ߵ���Чʱ�䷶Χ

    // ���캯��
    Ray(const Vector3f& ori, const Vector3f& dir, const double _t = 0.0): origin(ori), direction(dir),t(_t) {
        direction_inv = Vector3f(1./direction.x, 1./direction.y, 1./direction.z);
        t_min = 0.0;
        t_max = std::numeric_limits<double>::max();

    }

    // �ò��� t �õ������ϵĵ�
    Vector3f operator()(double t) const{return origin+direction*t;}

    // ���������������������������Ϣ
    friend std::ostream &operator<<(std::ostream& os, const Ray& r){
        os<<"[origin:="<<r.origin<<", direction="<<r.direction<<", time="<< r.t<<"]\n";
        return os;
    }
};
#endif //RAYTRACING_RAY_H
