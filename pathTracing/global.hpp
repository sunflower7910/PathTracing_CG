#pragma once
#include <iostream>
#include <cmath>
#include <random>


// ���峣���ͺ�
#undef M_PI
#define M_PI 3.141592653589793f

extern const float  EPSILON;
const float kInfinity = std::numeric_limits<float>::max();


// ��������������ֵv����������[lo, hi]��
inline float clamp(const float &lo, const float &hi, const float &v)
{ return std::max(lo, std::min(hi, v)); }


// �������������һԪ���η��� ax^2 + bx + c = 0 �ĸ����������洢��x0��x1��
inline  bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
                  -0.5 * (b + sqrt(discr)) :
                  -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);
    return true;
}

// ��������������һ����Χ��[0, 1)֮������������
inline float get_random_float()
{
    std::random_device dev;//
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

// ������������ʾ��������������ʾ�������
inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};
