#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

// 定义材质类型枚举
enum MaterialType { DIFFUSE, Microfacet};


class Material{
private:

    // 计算Fresnel方程
    void fresnel(const Vector3f &I, const Vector3f &N, const float &ior, float &kr) const
    {
        float cosi = clamp(-1, 1, dotProduct(I, N));
        float etai = 1, etat = ior;
        if (cosi > 0) {  std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1) {
            kr = 1;
        }
        else {
            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosi = fabsf(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2;
        }
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    }

    // 将局部坐标a转换为世界坐标
    Vector3f toWorld(const Vector3f &a, const Vector3f &N){
        Vector3f B, C;
        //将N分解为B和C
        //条件判断应该是为了防止除0
        if (std::fabs(N.x) > std::fabs(N.y)){
            float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
            C = Vector3f(N.z * invLen, 0.0f, -N.x *invLen);
        }
        else {
            float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
            C = Vector3f(0.0f, N.z * invLen, -N.y *invLen);
        }
        B = crossProduct(C, N);
        return a.x * B + a.y * C + a.z * N;
    }
private:
    // GGX分布函数
	float DistributionGGX(Vector3f N, Vector3f H, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float NdotH = std::max(dotProduct(N, H), 0.0f);
		float NdotH2 = NdotH * NdotH;

		float nom = a2;
		float denom = (NdotH2 * (a2 - 1.0) + 1.0);
		denom = M_PI * denom * denom;

		return nom / std::max(denom, 0.0000001f); // prevent divide by zero for roughness=0.0 and NdotH=1.0  防止在roughness=0.0和NdotH=1.0时除以0
	}

    // GGX几何函数Schlick近似
	float GeometrySchlickGGX(float NdotV, float roughness)
	{
		float r = (roughness + 1.0);
		float k = (r*r) / 8.0;

		float nom = NdotV;
		float denom = NdotV * (1.0 - k) + k;

		return nom / denom;
	}

    // GGX几何函数Smith
	float GeometrySmith(Vector3f N, Vector3f V, Vector3f L, float roughness)
	{
		float NdotV = std::max(dotProduct(N, V), 0.0f);
		float NdotL = std::max(dotProduct(N, L), 0.0f);
		float ggx2 = GeometrySchlickGGX(NdotV, roughness);
		float ggx1 = GeometrySchlickGGX(NdotL, roughness);

		return ggx1 * ggx2;
	}


public:
    MaterialType m_type; //只有diffuse材质（漫反射材质）
    //Vector3f m_color;
    Vector3f m_emission; //辐射量（发光体）
    float ior;           // 折射率
    Vector3f Kd, Ks;     //Kd漫反射系数，Ks高光镜面反射系数
    float specularExponent; // 高光镜面反射指数
    //Texture tex;

    inline Material(MaterialType t=DIFFUSE, Vector3f e=Vector3f(0,0,0));

    // 获取辐射量
    inline Vector3f getEmission();
    // 判断是否有辐射量（是否为光源）
    inline bool hasEmission();

    // sample a ray by Material properties 根据材质属性对入射光线进行采样
    inline Vector3f sample(const Vector3f &wi, const Vector3f &N);

    // given a ray, calculate the PdF of this ray 计算采样光线的概率密度函数
    inline float pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);

    // given a ray, calculate the contribution of this ray
    // 计算入射、出射方向及法线下的BRDF值
    // 返回 BRDF
    inline Vector3f eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);

};

Material::Material(MaterialType t, Vector3f e){
    m_type = t;
    //m_color = c;
    m_emission = e;
}


Vector3f Material::getEmission() {return m_emission;}
bool Material::hasEmission() {
    if (m_emission.norm() > EPSILON) return true;
    else return false;
}



Vector3f Material::sample(const Vector3f &wi, const Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // uniform sample on the hemisphere
            // 在半球上均匀采样
            float x_1 = get_random_float(), x_2 = get_random_float();
            //z∈[0,1]，是随机半球方向的z轴向量
            float z = std::fabs(1.0f - 2.0f * x_1);
            //r是半球半径随机向量以法线为旋转轴的半径
            //phi是r沿法线旋转轴的旋转角度
            float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;//phi∈[0,2*pi]
            Vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);//半球面上随机的光线的弹射方向

            return toWorld(localRay, N);//转换到世界坐标
            
            break;
        }
		case Microfacet:
		{
			// uniform sample on the hemisphere
            // 在半球上均匀采样
			float x_1 = get_random_float(), x_2 = get_random_float();
			float z = std::fabs(1.0f - 2.0f * x_1);
			float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
			Vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);
			return toWorld(localRay, N);

			break;
		}
    }
}

float Material::pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // uniform sample probability 1 / (2 * PI)
            // 均匀采样概率为 1 / (2 * PI)
            if (dotProduct(wo, N) > 0.0f)
                return 0.5f / M_PI;
            else
                return 0.0f;
            break;
        }
		case Microfacet:
		{
			// uniform sample probability 1 / (2 * PI)
            // 均匀采样概率为 1 / (2 * PI)
			if (dotProduct(wo, N) > 0.0f)
				return 0.5f / M_PI;
			else
				return 0.0f;
			break;
		}
    }
}


Vector3f Material::eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N){
    /**
     * @brief BRDF 双向反射函数，通过入射、出射、法线，来计算双向反射函数（比例）BRDF
     * wi:入射方向
     * wo:出射防线
     * N：法线
     */
    switch(m_type){
        case DIFFUSE:
        {
            // calculate the contribution of diffuse   model
            // 计算漫反射模型的贡献
            float cosalpha = dotProduct(N, wo);
            if (cosalpha > 0.0f) {
                Vector3f diffuse = Kd / M_PI;
                return diffuse;
            }
            else
                return Vector3f(0.0f);
            break;
        }
		case Microfacet:
		{
			// Disney PBR 方案
			float cosalpha = dotProduct(N, wo);
			if (cosalpha > 0.0f) {
				float roughness = 0.35;

				Vector3f V = -wi;
				Vector3f L = wo;
				Vector3f H = normalize(V + L);

				// 计算 distribution of normals: D
                // 计算法线分布函数：D
				float D = DistributionGGX(N, H, roughness);

				// 计算 shadowing masking term: G
                // 计算阴影掩蔽项：G
				float G = GeometrySmith(N, V, L, roughness);

				// 计算 fresnel 系数: F
				float F;
				float etat = 1.85;
				fresnel(wi, N, etat, F);

				Vector3f nominator = D * G * F;
				float denominator = 4 * std::max(dotProduct(N, V), 0.0f) * std::max(dotProduct(N, L), 0.0f);
				Vector3f specular = nominator / std::max(denominator, 0.001f);

				// 能量守恒
				float ks_ = F;
				float kd_ = 1.0f - ks_;

				Vector3f diffuse = 1.0f / M_PI;

				// 因为在 specular 项里已经考虑了折射部分的比例：F，所以折射部分不需要再乘以 ks_ （ks_ * Ks * specular）
				return Ks * specular + kd_ * Kd * diffuse;
			}
			else
				return Vector3f(0.0f);
			break;
		}
    }
}

#endif //RAYTRACING_MATERIAL_H
