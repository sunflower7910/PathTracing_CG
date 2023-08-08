#include <thread>
#include <mutex>

// 互斥锁，用于线程同步
std::mutex mutex_ins;


#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


// 将角度转换为弧度的辅助函数
inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

// 定义一个很小的常量
const float EPSILON = 0.00001;

// 渲染函数，主要实现光线追踪算法，渲染场景并保存结果
void Renderer::Render(const Scene& scene)
{
	// 创建一个存储像素颜色的缓冲区
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

	// 计算摄像机参数
    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800); // 摄像机位置
    int m = 0;  // 用于记录当前像素的索引

	// 射线数量：设置每个像素点的采样数量（光线追踪次数）
	int spp = 100; // 每个像素只进行一次采样（无抗锯齿）
	std::cout << "SPP: " << spp << "\n";

	int process = 0; // 用于记录渲染进度

	// 创造匿名函数，为不同线程划分不同块
	auto castRayMultiThreading = [&](uint32_t rowStart, uint32_t rowEnd, uint32_t colStart, uint32_t colEnd)
	{
		for (uint32_t j = rowStart; j < rowEnd; ++j) {
			int m = j * scene.width + colStart;
			for (uint32_t i = colStart; i < colEnd; ++i) {
				// generate primary ray direction 生成主光线方向
				float x = (2 * (i + 0.5) / (float)scene.width - 1) * imageAspectRatio * scale;
				float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;
				Vector3f dir = normalize(Vector3f(-x, y, 1)); // 计算光线方向

				for (int k = 0; k < spp; k++) {
					// 对场景中的每一个像素进行光线追踪，生成颜色并累加到framebuffer中（路径追踪）
					framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;//光线追踪
				}
				m++;
				process++;
			}

			// 互斥锁，用于打印处理进程
			std::lock_guard<std::mutex> g1(mutex_ins);
			UpdateProgress(1.0*process / scene.width / scene.height);
		}
	};

	// 分块计算光线追踪
	int id = 0;
	constexpr int bx = 5; // x方向分块数
	constexpr int by = 5; // y方向分块数
	std::thread th[bx * by];

	int strideX = (scene.width + 1) / bx;  // x方向每块的像素数
	int strideY = (scene.height + 1) / by;  // y方向每块的像素数

	// 分块计算光线追踪
	for (int i = 0; i < scene.height; i += strideX)
	{
		for (int j = 0; j < scene.width; j += strideY)
		{
			// 将不同块的光线追踪任务分配给不同的线程
			th[id] = std::thread(castRayMultiThreading, i, std::min(i + strideX, scene.height), j, std::min(j + strideY, scene.width));
			id++;
		}
	}

	// 等待所有线程执行完毕
	for (int i = 0; i < bx*by; i++) th[i].join();

	//进度条
	UpdateProgress(1.f);


	// 将渲染结果保存到文件中
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
		// 将颜色进行gamma校正，然后映射到0-255的范围，并保存到文件中
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
