
#include "Scene.hpp"

#pragma once

// �洢�����������ཻ�������Ϣ
struct hit_payload
{
    float tNear; // ��������Դ�ľ���
    uint32_t index; // ��������������ڼ��ٲ��ң�
    Vector2f uv; // ���㴦����������
    Object* hit_obj; // �ཻ������ָ��
};

class Renderer
{
public:
    void Render(const Scene& scene);

private:
};
