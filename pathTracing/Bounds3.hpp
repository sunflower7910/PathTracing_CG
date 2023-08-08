#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H
#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>

class Bounds3
{
  public:
    Vector3f pMin, pMax; // 用两个点表示包围盒的最小和最大顶点

    // 默认构造函数，初始化包围盒为无穷大范围
    Bounds3()
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = Vector3f(minNum, minNum, minNum);
        pMin = Vector3f(maxNum, maxNum, maxNum);
    }
    // 构造函数，通过一个点来构建一个只包含该点的包围盒
    Bounds3(const Vector3f p) : pMin(p), pMax(p) {}
    // 构造函数，通过两个点来构建一个包围盒
    Bounds3(const Vector3f p1, const Vector3f p2)
    {
        pMin = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    // 获取包围盒的对角线向量
    Vector3f Diagonal() const { return pMax - pMin; }
    // 获取包围盒中最长的边的维度
    int maxExtent() const
    {
        Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    // 计算包围盒的表面积
    double SurfaceArea() const
    {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    // 计算包围盒的中心点
    Vector3f Centroid() { return 0.5 * pMin + 0.5 * pMax; }
    
    // 计算两个包围盒的交集
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y), fmax(pMin.z, b.pMin.z)),
                       Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y), fmin(pMax.z, b.pMax.z)));
    }

    //计算点相对于包围盒 '左上角' 的偏移量
    Vector3f Offset(const Vector3f& p) const
    {
        Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    // 判断两个包围盒是否有重叠
    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    // 判断点是否在包围盒内
    //p 是否在 b 内
    bool Inside(const Vector3f& p, const Bounds3& b)
    {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    //Bounds[0]'左上角', Bounds[1]'右下角'
    inline const Vector3f& operator[](int i) const
    {
        return (i == 0) ? pMin : pMax;
    }
    //射线和bounds是否有相交
    inline bool IntersectP(const Ray& ray, const Vector3f& invDir,
                           const std::array<int, 3>& dirisNeg) const;
};



// 射线和包围盒是否相交
inline bool Bounds3::IntersectP(const Ray& ray, const Vector3f& invDir, const std::array<int, 3>& dirIsNeg) const
{
    // invDir: 光线方向(x,y,z)，invDir=(1.0/x, 1.0/y, 1.0/z)，使用该形式是因为乘法比除法更快
    // dirIsNeg: 光线方向(x,y,z)，dirIsNeg=[int(x>0), int(y>0), int(z>0)]，使用该形式是为了简化逻辑判断
    // TODO 测试射线是否和包围盒相交
	// 光线进入点
	float tEnter = -std::numeric_limits<float>::infinity();
	// 光线离开点
	float tExit = std::numeric_limits<float>::infinity();
	for (int i = 0; i < 3; i++)
	{
		float min = (pMin[i] - ray.origin[i]) * invDir[i];
		float max = (pMax[i] - ray.origin[i]) * invDir[i];
		// 坐标为负的话，需要进行交换
		if (!dirIsNeg[i])
		{
			std::swap(min, max);
		}
		tEnter = std::max(min, tEnter);
		tExit = std::min(max, tExit);
	}
	return tEnter <= tExit && tExit >= 0;
}

// 计算两个包围盒的并集
inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

// 计算包围盒和点的并集（bounds3 和 p）
inline Bounds3 Union(const Bounds3& b, const Vector3f& p)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
