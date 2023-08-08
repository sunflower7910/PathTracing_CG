
#include "Scene.hpp"

#pragma once

// 存储光线与物体相交的相关信息
struct hit_payload
{
    float tNear; // 交点距离光源的距离
    uint32_t index; // 物体的索引（用于加速查找）
    Vector2f uv; // 交点处的纹理坐标
    Object* hit_obj; // 相交的物体指针
};

class Renderer
{
public:
    void Render(const Scene& scene);

private:
};
