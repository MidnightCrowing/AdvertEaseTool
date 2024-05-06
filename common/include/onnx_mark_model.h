#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>      // 深度学习模块
#include <opencv2/imgproc.hpp>  // 图像处理模块
#include <opencv2/highgui.hpp>  // GUI图形用户界面

#include<ctime>

// 命名空间
using namespace cv;
using namespace dnn;
using namespace std;

// 自定义配置结构
struct Configuration {
public:
    float confThreshold; // Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    float objThreshold;  //Object Confidence threshold
    string modelpath;
};
struct DetectLabel {
    int classid;
    string label;
    float conf;
    int x1;
    int y1;
    int x2;
    int y2;
};

// 模型
class YOLOv5 {
public:
    // 初始化
    YOLOv5(const Configuration &config, vector<string> classes, bool isCuda);

    vector<DetectLabel> detect(Mat frame);  // 检测函数
private:
    float confThreshold;
    float nmsThreshold;
    float objThreshold;
    int inpWidth;
    int inpHeight;
    int num_classes;
    vector<string> classes;
    //vector<string> class_names;
    const bool keep_ratio = true;
    Net net;   // dnn里的
    void drawPred(float conf, int left, int top, int right, int bottom, Mat &frame, int classid);

    Mat resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left) const;
};

pair<vector<DetectLabel>, double> detectImage(
        const Configuration &yolo_nets,
        const vector<string> &label_class,
        const string &input_img_path,
        const string &output_img_path,
        bool is_cude
);

pair<vector<DetectLabel>, Mat> reDetectImage(
        YOLOv5 *yolo_model,
        Mat srcimg
);

#ifndef C_CLASS_DEMO_ONNX_MARK_MODEL_H
#define C_CLASS_DEMO_ONNX_MARK_MODEL_H

#endif //C_CLASS_DEMO_ONNX_MARK_MODEL_H
