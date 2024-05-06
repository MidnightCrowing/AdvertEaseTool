#ifndef GUI_RUN_H
#define GUI_RUN_H

#include <QPixmap>
#include <QScreen>
#include <QImage>
#include <QLabel>
#include <QRadioButton>
#include <QThread>
#include "yaml-cpp/yaml.h"
#include <opencv2/dnn.hpp>  // 深度学习模块
#include "device_control.h"
#include "utils.h"
#include "../../common/include/onnx_mark_model.h"
#include "../../common/include/onnx_decision_model.h"

using namespace cv;

vector<string> loadYamlFile(const string &file_path);

vector<float> toDecisionModelInput(const vector<DetectLabel> &detect_result, vector<string> decision_labels);

Mat QPixmapToMat(const QPixmap &pixmap);

QPixmap matToPixmap(const Mat &mat);

struct RunArgs {
    // 程序运行状态
    int run_state;
    // 是否使用CUDE
    bool is_cude;
    // 运行进度
    bool run_yolo;
    bool run_detector;
    bool run_output;
    // 模型文件及其标签集路径
    string mark_model_path;
    string mark_model_labels_path;
    string decision_model_path;
    string decision_model_labels_path;
    // WId列表, 用于输入原始图片
    vector<WId> winId_list;
    // QLabel列表, 用于输出标注好的图片
    vector<QLabel *> result_widget_list;
};

// 声明一个函数指针类型，该函数不接受参数, 返回结构体RunArgs
typedef RunArgs (*FuncRun)();

void mainRun(
        vector<Device *> devices,
        YOLOv5 *yolo_model,
        Detector *detector_model,
        vector<string> decision_labels,
        bool run_yolo,
        bool run_detector,
        bool run_output
);

class RunThread : public QThread {
Q_OBJECT

public:
    RunThread(int &runState);
//    RunThread(
//            int &run_state_,
//            bool &is_cude_,
//            bool &run_yolo_,
//            bool &run_detector_,
//            bool &run_output_,
//            string &mark_model_path_,
//            string &mark_model_labels_path_,
//            string &decision_model_path_,
//            string &decision_model_labels_path_
//    );

//    void set(FuncRun func_);
    void set(
            bool is_cude_,
            QRadioButton *qrbtn_1_,
            QRadioButton *qrbtn_2_,
            QRadioButton *qrbtn_3_,
            QString mark_model_path_,
            QString mark_model_labels_path_,
            QString decision_model_path_,
            QString decision_model_labels_path_,
            bool debug_ = false
    );

    void load_args();

protected:
    void run() override;

private:
//    FuncRun func;
    bool debug;

    QRadioButton *qrbtn_1;
    QRadioButton *qrbtn_2;
    QRadioButton *qrbtn_3;

    // 程序运行状态
    int &run_state;
    // 是否使用CUDE
    bool is_cude;
    // 运行进度
    bool run_yolo;
    bool run_detector;
    bool run_output;
    // 模型文件及其标签集路径
    string mark_model_path;
    string mark_model_labels_path;
    string decision_model_path;
    string decision_model_labels_path;
};

#endif //GUI_RUN_H
