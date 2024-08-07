// clang-format off
#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // TODO: Copy-paste your implementation from the previous assignment.
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    // copy from Assignment1
    // 计算视景体的数据
    float height = 2.0 * tan(eye_fov / 180.0 * MY_PI) * fabs(zNear);
    float width = height * aspect_ratio;
    float deep = fabs(zNear - zFar); 

    Eigen::Matrix4f scale, translate, persp2ortho;
    scale << // 缩放到标准化范围[-1,1]
        2.0/width, 0,          0,        0,
        0,         2.0/height, 0,        0,
        0,         0,          2.0/deep, 0,
        0,         0,          0,        1;

    translate << // 将视景体中心移动到(0, 0).用fov和ratio计算xy轴均匀分布,不需要平移
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(zNear + zFar)/2.0, 
        0, 0, 0, 1;

#ifdef RH
    persp2ortho << // 将透视投影转化为正交投影 右手坐标系
        zNear, 0,     0,            0,
        0,     zNear, 0,            0,
        0,     0,     (zNear + zFar), -zNear*zFar,
        0,     0,     1,            0;
#else
    persp2ortho << // 将透视投影转化为正交投影 左手坐标系
        zNear, 0,     0,            0,
        0,     zNear, 0,            0,
        0,     0,     -(zNear + zFar), -zNear*zFar,
        0,     0,     -1,            0;
#endif

    projection = scale * translate * persp2ortho * projection;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc == 2)
    {
        command_line = true;
        filename = std::string(argv[1]);
    }

    rst::rasterizer r(700, 700);
    r.set_ssaa(2);

    Eigen::Vector3f eye_pos = {0,0,5};
    #ifdef RH
        float zNear = -0.1, zFar = -50;
    #else
        float zNear = 0.1, zFar = 50;
    #endif

    std::vector<Eigen::Vector3f> pos
            {
                    {2, 0, -2},
                    {0, 2, -2},
                    {-2, 0, -2},
                    {3.5, -1, -5},
                    {2.5, 1.5, -5},
                    {-1, 0.5, -5}
            };

    std::vector<Eigen::Vector3i> ind
            {
                    {0, 1, 2},
                    {3, 4, 5}
            };

    std::vector<Eigen::Vector3f> cols
            {
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0}
            };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, zNear, zFar));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        filename = "../images/" + filename;
        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, zNear, zFar));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
    }

    return 0;
}
// clang-format on