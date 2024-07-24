#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_rotation(Vector3f axis, float angle)
{
    float theta = angle / 180.0 * MY_PI;
    axis.normalize();
    Eigen::Matrix3f xProductM;
    xProductM << 
        0,         -axis.z(), axis.y(),  
        axis.z(),  0,         -axis.x(), 
        -axis.y(), axis.x(),  0;
    Eigen::Matrix3f rotation = cos(theta)* Eigen::Matrix3f::Identity() 
                            + (1.0-cos(theta))*axis*axis.transpose()
                            + sin(theta) * xProductM;
    Eigen::Matrix4f ret = Eigen::Matrix4f::Identity();
    ret.block(0, 0, 3, 3) << rotation;
    return ret;
}

Eigen::Matrix4f get_model_matrix(Vector3f rotation_axis, float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    // // 角度转弧度制
    // float theta = rotation_angle / 180.0 * MY_PI;
    // // 绕z轴旋转的旋转矩阵
    // Eigen::Matrix4f rotate;
    // rotate << 
    //     cos(theta), -sin(theta),0, 0,
    //     sin(theta), cos(theta), 0, 0,
    //     0,          0,          1, 0,
    //     0,          0,          0, 1;

    // 按任意轴旋转
    Eigen::Matrix4f rotate = get_rotation(rotation_axis, rotation_angle);

    model = rotate * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    // 计算视景体的数据
    float height = 2.0 * tan(eye_fov / 180.0 * MY_PI) * fabs(zNear);
    float width = height * aspect_ratio;
    float deep = zNear - zFar; 

    Eigen::Matrix4f scale, translate, persp2ortho;
    scale << // 缩放到标准化范围[-1,1]
        2.0/width, 0,          0,        0,
        0,         2.0/height, 0,        0,
        0,         0,          2.0/deep, 0,
        0,         0,          0,        1;

    translate << // 将视景体中心移动到(0, 0).用fov和ratio计算xy轴均匀分布,不需要平移
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -deep/2.0, 
        0, 0, 0, 1;

    persp2ortho << // 将透视投影转化为正交投影
        zNear, 0,     0,            0,
        0,     zNear, 0,            0,
        0,     0,     zNear + zFar, -zNear*zFar,
        0,     0,     1,            0;

    projection = scale * translate * persp2ortho * projection;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "../images/output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
            filename = "../images/" + filename;
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    Eigen::Vector3f rotation_axis = {1, 1, -1};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(rotation_axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, -0.1, -50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(rotation_axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, -0.1, -50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
