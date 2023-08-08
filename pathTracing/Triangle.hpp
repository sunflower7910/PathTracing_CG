#pragma once

#include "BVH.hpp"
#include "Intersection.hpp"
#include "Material.hpp"
#include "OBJ_Loader.hpp"
#include "Object.hpp"
#include "Triangle.hpp"
#include <cassert>
#include <array>

// 判断射线和三角形是否相交，如果相交则返回交点的参数u、v和距离tnear
bool rayTriangleIntersect(const Vector3f& v0, const Vector3f& v1,
                          const Vector3f& v2, const Vector3f& orig,
                          const Vector3f& dir, float& tnear, float& u, float& v)
{
    // 计算两条边和射线方向的叉积
    Vector3f edge1 = v1 - v0;
    Vector3f edge2 = v2 - v0;
    Vector3f pvec = crossProduct(dir, edge2);
    float det = dotProduct(edge1, pvec);

    // 判断是否平行或背面相交
    if (det == 0 || det < 0)
        return false;

    Vector3f tvec = orig - v0;
    u = dotProduct(tvec, pvec);
    if (u < 0 || u > det)
        return false;

    Vector3f qvec = crossProduct(tvec, edge1);
    v = dotProduct(dir, qvec);
    if (v < 0 || u + v > det)
        return false;

    // 计算交点的参数u、v和距离tnear
    float invDet = 1 / det;

    tnear = dotProduct(edge2, qvec) * invDet;
    u *= invDet;
    v *= invDet;

    return true;
}

// 三角形类，继承自Object
class Triangle : public Object
{
public:
    Vector3f v0, v1, v2; // vertices A, B ,C , counter-clockwise order  三个顶点，顶点顺序按逆时针方向排列

    Vector3f e1, e2;     // 2 edges v1-v0, v2-v0;  两条边v1-v0, v2-v0
    Vector3f t0, t1, t2; // texture coords  纹理坐标
    Vector3f normal;     // 三角形法线
    float area;          // 三角形面积
    Material* m;         // 材质指针


    // 构造函数，传入三个顶点和材质指针
    Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material* _m = nullptr)
        : v0(_v0), v1(_v1), v2(_v2), m(_m)
    {
        e1 = v1 - v0;
        e2 = v2 - v0;
        normal = normalize(crossProduct(e1, e2));
        area = crossProduct(e1, e2).norm()*0.5f;
    }

    // 判断射线和三角形是否相交
    bool intersect(const Ray& ray) override;
    // 判断射线和三角形是否相交，如果相交则返回交点距离和索引
    bool intersect(const Ray& ray, float& tnear,
                   uint32_t& index) const override;

    // 返回射线与三角形的交点信息
    Intersection getIntersection(Ray ray) override;

    // 获取表面属性，例如法线和纹理坐标
    void getSurfaceProperties(const Vector3f& P, const Vector3f& I,
                              const uint32_t& index, const Vector2f& uv,
                              Vector3f& N, Vector2f& st) const override
    {
        N = normal;
        //        throw std::runtime_error("triangle::getSurfaceProperties not
        //        implemented.");
    }

    // 获取三角形表面的漫反射颜色
    Vector3f evalDiffuseColor(const Vector2f&) const override;

    // 返回三角形的包围盒
    Bounds3 getBounds() override;

    // 在三角形上采样一个点，并返回采样点的概率密度函数（pdf）
    void Sample(Intersection &pos, float &pdf){
        // 随机得到三角形内一点，并得到该点pdf（该点的概率密度）为1/三角形面积


        float x = std::sqrt(get_random_float()); 
        float y = get_random_float();// 0-1

        //随机得到三角形内一点
        pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);//???
        pos.normal = this->normal;
        
        //得到该点pdf（该点的概率密度）为1/三角形面积
        pdf = 1.0f / area;
    }

    // 获取三角形的面积
    float getArea(){
        return area;
    }

    // 判断三角形是否是光源
    bool hasEmit(){
        return m->hasEmission();
    }
};

// MeshTriangle类，用于加载OBJ文件的三角形网格模型
class MeshTriangle : public Object
{
public:
    // 构造函数，从OBJ文件加载三角形网格模型
    MeshTriangle(const std::string& filename, Material *mt = new Material())
    {
        // 从OBJ文件加载三角形网格
        objl::Loader loader;
        loader.LoadFile(filename);
        area = 0;
        m = mt;
        assert(loader.LoadedMeshes.size() == 1);
        auto mesh = loader.LoadedMeshes[0];

        Vector3f min_vert = Vector3f{std::numeric_limits<float>::infinity(),
                                     std::numeric_limits<float>::infinity(),
                                     std::numeric_limits<float>::infinity()};
        Vector3f max_vert = Vector3f{-std::numeric_limits<float>::infinity(),
                                     -std::numeric_limits<float>::infinity(),
                                     -std::numeric_limits<float>::infinity()};

        // 遍历所有三角形面片，并创建对应的Triangle对象
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            std::array<Vector3f, 3> face_vertices;

            for (int j = 0; j < 3; j++) {
                auto vert = Vector3f(mesh.Vertices[i + j].Position.X,
                                     mesh.Vertices[i + j].Position.Y,
                                     mesh.Vertices[i + j].Position.Z);
                face_vertices[j] = vert;

                // 更新包围盒的边界
                min_vert = Vector3f(std::min(min_vert.x, vert.x),
                                    std::min(min_vert.y, vert.y),
                                    std::min(min_vert.z, vert.z));
                max_vert = Vector3f(std::max(max_vert.x, vert.x),
                                    std::max(max_vert.y, vert.y),
                                    std::max(max_vert.z, vert.z));
            }

            // 创建Triangle对象并加入到triangles列表中
            triangles.emplace_back(face_vertices[0], face_vertices[1],
                                   face_vertices[2], mt);
        }

        // 创建BVH加速结构，加速三角形的相交测试
        bounding_box = Bounds3(min_vert, max_vert); 

        std::vector<Object*> ptrs;  
        for (auto& tri : triangles){
            ptrs.push_back(&tri);
            area += tri.area;
        }
        bvh = new BVHAccel(ptrs);
    }

    // 判断射线和三角形网格是否相交
    bool intersect(const Ray& ray) { return true; }

    // 判断射线和三角形网格是否相交，如果相交则返回交点距离和索引
    bool intersect(const Ray& ray, float& tnear, uint32_t& index) const
    {
        bool intersect = false;
        for (uint32_t k = 0; k < numTriangles; ++k) {
            const Vector3f& v0 = vertices[vertexIndex[k * 3]];
            const Vector3f& v1 = vertices[vertexIndex[k * 3 + 1]];
            const Vector3f& v2 = vertices[vertexIndex[k * 3 + 2]];
            float t, u, v;
            if (rayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t,
                                     u, v) &&
                t < tnear) {
                tnear = t;
                index = k;
                intersect |= true;
            }
        }

        return intersect;
    }

    // 返回三角形网格的包围盒
    Bounds3 getBounds() { return bounding_box; }

    // 获取三角形网格的表面属性，例如法线和纹理坐标
    void getSurfaceProperties(const Vector3f& P, const Vector3f& I,
                              const uint32_t& index, const Vector2f& uv,
                              Vector3f& N, Vector2f& st) const
    {
        const Vector3f& v0 = vertices[vertexIndex[index * 3]];
        const Vector3f& v1 = vertices[vertexIndex[index * 3 + 1]];
        const Vector3f& v2 = vertices[vertexIndex[index * 3 + 2]];
        Vector3f e0 = normalize(v1 - v0);
        Vector3f e1 = normalize(v2 - v1);
        N = normalize(crossProduct(e0, e1));
        const Vector2f& st0 = stCoordinates[vertexIndex[index * 3]];
        const Vector2f& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
        const Vector2f& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
        st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
    }

    // 获取三角形网格的漫反射颜色
    Vector3f evalDiffuseColor(const Vector2f& st) const
    {
        float scale = 5;
        float pattern =
            (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
        return lerp(Vector3f(0.815, 0.235, 0.031),
                    Vector3f(0.937, 0.937, 0.231), pattern);
    }


    // 获取射线与三角形网格的交点信息
    Intersection getIntersection(Ray ray)
    {
        Intersection intersec;

        if (bvh) {
            intersec = bvh->Intersect(ray);
        }

        return intersec;
    }
    
    // 在三角形网格上采样一个点，并返回采样点的概率密度函数（pdf）
    void Sample(Intersection &pos, float &pdf){
        //首先通过bvh随机采样三角形，在通过这个三角形随机采样光源
        bvh->Sample(pos, pdf);
        pos.emit = m->getEmission();
    }

    // 获取三角形网格的面积
    float getArea(){
        return area;
    }

    //是否是发光物
    bool hasEmit(){
        return m->hasEmission();
    }

    Bounds3 bounding_box; //包围盒  
    std::unique_ptr<Vector3f[]> vertices; //顶点集合的指针
    uint32_t numTriangles;  //三角形数量
    std::unique_ptr<uint32_t[]> vertexIndex;    //顶点集合索引
    std::unique_ptr<Vector2f[]> stCoordinates;  //纹理坐标集合的指针

    std::vector<Triangle> triangles;    //三角形集合

    BVHAccel* bvh; //MeshTriangle 的 bvh树的根指针（用来划分三角形）
    float area; //表面积之和

    Material* m; //材质指针
};

// 空实现，用于MeshTriangle类
inline bool Triangle::intersect(const Ray& ray) { return true; }

// 空实现，用于MeshTriangle类
inline bool Triangle::intersect(const Ray& ray, float& tnear,
                                uint32_t& index) const
{
    return false;
}

// 返回三角形的包围盒，用于BVH加速
inline Bounds3 Triangle::getBounds() { return Union(Bounds3(v0, v1), v2); }


// 返回射线与三角形的交点信息
inline Intersection Triangle::getIntersection(Ray ray)
{
	Intersection inter;

    // 判断射线和三角形是否背面相交
	if (dotProduct(ray.direction, normal) > 0)
		return inter;

	double u, v, t_tmp = 0;
	Vector3f pvec = crossProduct(ray.direction, e2);
	double det = dotProduct(e1, pvec);

    // 判断是否平行或背面相交
	if (fabs(det) < EPSILON)
		return inter;

	double det_inv = 1. / det;
	Vector3f tvec = ray.origin - v0;
	u = dotProduct(tvec, pvec) * det_inv;

    // 判断是否在三角形的边界内
	if (u < 0 || u > 1)
		return inter;
	Vector3f qvec = crossProduct(tvec, e1);
	v = dotProduct(ray.direction, qvec) * det_inv;
	if (v < 0 || u + v > 1)
		return inter;

    // 计算交点的距离
	t_tmp = dotProduct(e2, qvec) * det_inv;

	if (t_tmp < 0)
	{
		return inter;
	}

    // 设置交点信息并返回
	inter.distance = t_tmp;
	inter.coords = ray(t_tmp);
	inter.happened = true;
	inter.m = m;
	inter.normal = normal;
	inter.obj = this;

	return inter;
}

// 获取三角形表面的漫反射颜色，这里返回一个默认的灰色
inline Vector3f Triangle::evalDiffuseColor(const Vector2f&) const
{
    return Vector3f(0.5, 0.5, 0.5);
}
