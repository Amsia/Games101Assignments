// clang-format off
#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"
#include <fstream>

constexpr double MY_PI = 3.1415926;
//#define RH

std::vector<float> step_sizes;
float get_steps(int i);

int main()
{
    Vector3f eye_pos = { 0, 0, 5 };
    float eye_fov = 45;
    float aspect_ratio = 1;
#ifdef RH
    std::cout << "z       \tpersp2ortho\t mvp    \t Viewport" << std::endl << "[4.9, -45]\t[-0.1, -50]\t [1, -1] \t[-0.1, -50]" << std::endl;
    float zNear = -0.1, zFar = -50;
#else
    std::cout << "z       \tpersp2ortho\t mvp    \t Viewport" << std::endl << "[4.9, -45]\t[0.1, 50]\t [-1, 1] \t[0.1, 50]" << std::endl;
    float zNear = 0.1, zFar = 50;
#endif


    Eigen::Matrix4f view;
    view << 1, 0, 0, -eye_pos[0],
        0, 1, 0, -eye_pos[1],
        0, 0, 1, -eye_pos[2],
        0, 0, 0, 1;

    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    float height = 2.0 * tan(eye_fov / 180.0 * MY_PI) * fabs(zNear);
    float width = height * aspect_ratio;
    float deep = fabs(zNear - zFar);
    Eigen::Matrix4f scale, translate, persp2ortho, proj;
    scale << // 3.缩放到标准化范围[-1,1]
        2.0 / width, 0, 0, 0,
        0, 2.0 / height, 0, 0,
        0, 0, 2.0 / deep, 0,
        0, 0, 0, 1;

    translate << // 2.将视景体中心移动到(0, 0).用fov和ratio计算xy轴均匀分布,不需要平移
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(zNear + zFar) / 2.0,
        0, 0, 0, 1;

#ifdef RH
    persp2ortho << // 1.将透视投影转化为正交投影
        zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, zNear + zFar, -zNear * zFar,
        0, 0, 1, 0;
#else
    persp2ortho << // 1.将透视投影转化为正交投影
        zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, -(zNear + zFar), -zNear * zFar,
        0, 0, -1, 0;
#endif

    proj = persp2ortho * view * model;

    float zr = 0;
    float f1 = fabs(zNear - zFar) / 2.0;
    float f2 = (zNear + zFar) / 2.0;

    std::cout << std::fixed << std::setprecision(5);
    Vector4f v;
    int i = 0;
    for (float z = 4.9; z >= -45; z -= get_steps(i++))
    {
        std::cout << z << " \t";
        v = { 0, 0, z, 1 };  v = proj * v;
        v /= v.w();
        std::cout << v.z() << " \t";
        v = scale * translate * v;
        v /= v.w();
        std::cout << v.z() << " \t" << v.z() * f1 + f2 << std::endl;
    }

    return 0;
}

float get_steps(int ind)
{
    float start = 4.9;
    float end = -45;
    int steps = 30;

    if (step_sizes.size() <= 0)
    {
        // 计算总范围
        float range = start - end;

        // 使用反向指数函数生成步长序列
        float initial_step_size = 0.00001; // 初始步长非常小
        float final_step_size = range / steps; // 最终步长较大

        for (int i = 0; i < steps; ++i) {
            // 计算当前步长，前期小，后期大
            float t = static_cast<float>(i) / (steps - 1);
            float step_size = initial_step_size * std::pow(final_step_size / initial_step_size, t);
            step_sizes.push_back(step_size);
        }

        // 归一化步长，使其总和等于总范围
        float total_step_size = 0;
        for (float size : step_sizes) {
            total_step_size += size;
        }
        float normalization_factor = range / total_step_size;
        for (int i = 0; i < steps; ++i) {
            step_sizes[i] *= normalization_factor;
        }
    }

    if (ind >= steps)
    {
        return 0.1;
    }
    else
    {
        return step_sizes[ind];
    }
}