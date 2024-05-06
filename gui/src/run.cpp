#include "../include/run.h"

using namespace std;

/**
 * 从 YAML 文件中加载数据并返回一个字符串向量
 *
 * @param file_path YAML 文件的路径
 * @return 包含从 YAML 文件加载的数据的字符串向量
 */
vector<string> loadYamlFile(const string &file_path) {
    vector<string> data;

    try {
        // 打开 YAML 文件
        ifstream fin(file_path);
        if (!fin.is_open()) {
            cerr << "Error: Unable to open file " << file_path << endl;
            return data;
        }

        // 解析 YAML 文件
        YAML::Node root = YAML::Load(fin);

        // 遍历 YAML 数据
        for (const auto &item: root) {
            string value;
            try {
                value = item.as<string>();
            } catch (YAML::BadConversion &e) {
                cerr << "Error: Unable to convert YAML value to string." << endl;
                continue;
            }
            data.push_back(value);
        }

        // 关闭文件
        fin.close();
    } catch (YAML::ParserException &e) {
        cerr << "Error: Failed to parse YAML file: " << e.what() << endl;
    }

    return data;
}

/**
 * 将检测结果转换为决策模型的输入特征向量
 *
 * @param detect_result 检测结果，包含检测标签及其对应的置信度
 * @param decision_labels 决策标签的列表
 * @return 决策模型的输入特征向量
 */
vector<float> toDecisionModelInput(const vector<DetectLabel> &detect_result, vector<string> decision_labels) {
    // 用于保存特征的向量
    vector<float> features;
    features.reserve(decision_labels.size()); // 为了提高效率，预先分配足够的空间

    // Lambda函数：查找标签并将其对应的置信度添加到特征向量中
    auto findAndAppendConfidence = [&](const string &decision_label) {
        bool found = false;
        // 在检测标签中查找决策标签
        for (const auto &detect_label: detect_result) {
            if (decision_label == detect_label.label) {
                // 找到了对应的标签，将其置信度添加到特征向量中
                features.push_back(detect_label.conf);
                found = true;
                break;
            }
        }
        // 如果没有找到对应的标签，将0.0添加到特征向量中
        if (!found) {
            features.push_back(0.0);
        }
    };

    // 对每个决策标签应用Lambda函数
    for_each(decision_labels.begin(), decision_labels.end(), findAndAppendConfidence);

    return features;
}

/**
 * @brief 将 QPixmap 对象转换为 OpenCV 的 Mat 对象。
 *
 * @param pixmap 要转换的 QPixmap 对象。
 * @return 转换后的 Mat 对象。
 */
Mat QPixmapToMat(const QPixmap &pixmap) {
    // 将 QPixmap 转换为 QImage
    QImage image = pixmap.toImage();

    // 将 QImage 转换为 Mat
    Mat mat;
    mat = Mat(image.height(), image.width(), CV_8UC4, (uchar *) image.bits(), image.bytesPerLine());
    cvtColor(mat, mat, COLOR_BGRA2BGR);  // 如果图像格式是BGRA，则需将其转换为BGR格式

    return mat;
}

/**
 * @brief 将 OpenCV 的 Mat 对象转换为 QPixmap 对象。
 *
 * @param mat 要转换的 Mat 对象。
 * @return 转换后的 QPixmap 对象。
 */
QPixmap matToPixmap(const Mat &mat) {
    // 如果图像为空，则返回空 QPixmap
    if (mat.empty()) {
        return QPixmap();
    }

    // 将 BGR 格式的图像转换为 RGB 格式
    Mat rgbImage;
    cvtColor(mat, rgbImage, COLOR_BGR2RGB);

    // 创建 QImage 对象，从 OpenCV 的图像数据中构建
    QImage image(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    return QPixmap::fromImage(image);
}


void mainRun(
        vector<Device *> devices,
        YOLOv5 *yolo_model,
        Detector *detector_model,
        vector<string> decision_labels,
        bool run_yolo,
        bool run_detector,
        bool run_output
) {
    // 遍历 devices
    for (Device *device: devices) {
        WId winId = device->winId;
        QLabel *result_widget = device->ui->result_widget;

        if (run_yolo) {
            // 图片获取
            QScreen *screen = QGuiApplication::primaryScreen();  // 获取屏幕对象
            QPixmap screenshot = screen->grabWindow(winId);      // 截取指定窗口的图像
            Mat srcimg = QPixmapToMat(screenshot);               // 转为OpenCV对象

            // 图片预测
            pair<vector<DetectLabel>, Mat> detect_image_result_pair = reDetectImage(yolo_model, srcimg);
            vector<DetectLabel> detect_image_result = detect_image_result_pair.first;
            Mat markimg = detect_image_result_pair.second;

            // 图片显示
            QPixmap pixmap = matToPixmap(markimg);
            result_widget->setPixmap(pixmap);  // 将 QPixmap 设置为 QLabel 的背景

            stringstream tool_text;

            if (run_detector) {
                // 置信度预测
                vector<float> decision_model_input = toDecisionModelInput(detect_image_result, decision_labels);
                string conf_result = reDetectConf(detector_model, decision_model_input);

                if (run_output) {
                    // 结果输出
                    if (conf_result == "no") {
                        return;  // 不做处理
                    }

                    bool find = false;
                    int x1, y1, x2, y2;
                    for (DetectLabel detect_label: detect_image_result) {
                        if (detect_label.label == conf_result) {
                            find = true;
                            x1 = detect_label.x1;
                            y1 = detect_label.y1;
                            x2 = detect_label.x2;
                            y2 = detect_label.y2;
                            break;
                        }
                    }

                    if (find) {
                        // 计算需要点击的坐标
                        int s_x, s_y, d_x, d_y;

                        s_x = (x1 + x2) / 2;
                        s_y = (y1 + y2) / 2;

//                        if (!device->deviationed) {
                            device->getDeviation(srcimg.cols, srcimg.rows);
//                        }

                        d_x = (s_x - device->d_x) / device->scale_ratio;
                        d_y = (s_y - device->d_y) / device->scale_ratio;

                        // 构建命令字符串
                        stringstream ss;
                        ss << "scrcpy/adb -s " << device->name.toStdString()
                           << " shell input tap " << d_x << " " << d_y;
                        string cmd = ss.str();

                        tool_text << "detector: " << conf_result << "\nx: " << d_x << "\ny: " << d_y;
                        result_widget->setToolTip(QString::fromStdString(tool_text.str()));

                        // 执行点击操作
                        ExeCmd(cmd);
                    } else {
                        return;  // 异常情况, 暂不做处理
                    }
                } else {
                    tool_text << "detector: " << conf_result;
                    result_widget->setToolTip(QString::fromStdString(tool_text.str()));
                }
            } else {
                tool_text << "无操作";
                result_widget->setToolTip(QString::fromStdString(tool_text.str()));
            }
        }
    }
}

RunThread::RunThread(int &runState) : run_state(runState) {}
// 构造函数，接受多个引用参数，并将它们存储在类的引用成员变量中
//RunThread::RunThread(
//        int &run_state_,
//        bool &is_cude_,
//        bool &run_yolo_,
//        bool &run_detector_,
//        bool &run_output_,
//        string &mark_model_path_,
//        string &mark_model_labels_path_,
//        string &decision_model_path_,
//        string &decision_model_labels_path_
//) :
//        run_state(run_state_),
//        is_cude(is_cude_),
//        run_yolo(run_yolo_),
//        run_detector(run_detector_),
//        run_output(run_output_),
//        mark_model_path(mark_model_path_),
//        mark_model_labels_path(mark_model_labels_path_),
//        decision_model_path(decision_model_path_),
//        decision_model_labels_path(decision_model_labels_path_),
//        winId_list(winId_list_),
//        result_widget_list(result_widget_list_)
//{}

//void RunThread::set(FuncRun func_) {
//    this->func = func_;
//}
void RunThread::set(
        bool is_cude_,
        QRadioButton *qrbtn_1_,
        QRadioButton *qrbtn_2_,
        QRadioButton *qrbtn_3_,
        QString mark_model_path_,
        QString mark_model_labels_path_,
        QString decision_model_path_,
        QString decision_model_labels_path_,
        bool debug_
) {
    this->is_cude = is_cude_;
    this->qrbtn_1 = qrbtn_1_;
    this->qrbtn_2 = qrbtn_2_;
    this->qrbtn_3 = qrbtn_3_;
    this->mark_model_path = mark_model_path_.toStdString();
    this->mark_model_labels_path = mark_model_labels_path_.toStdString();
    this->decision_model_path = decision_model_path_.toStdString();
    this->decision_model_labels_path = decision_model_labels_path_.toStdString();
    this->debug = debug_;
}

void RunThread::load_args() {
    this->run_yolo = true;
    this->run_detector = this->qrbtn_2->isChecked() || this->qrbtn_1->isChecked();
    this->run_output = this->qrbtn_1->isChecked();
}
//void RunThread::run() {
//    // 获取运行参数
//    RunArgs args = this->func();
//    string mark_model_path = args.mark_model_path;
//    string mark_model_labels_path = args.mark_model_labels_path;
//    string decision_model_path = args.decision_model_path;
//    string decision_model_labels_path = args.decision_model_labels_path;
//    bool is_cude = args.is_cude;
//    vector<WId> winId_list = args.winId_list;
//    vector<QLabel *> result_widget_list = args.result_widget_list;
//
//    // 读取yaml文件
//    vector<string> mark_labels = loadYamlFile(mark_model_labels_path);
//    vector<string> decision_labels = loadYamlFile(decision_model_labels_path);
//
//    // 创建模型
//    YOLOv5 *yolo_model = new YOLOv5({0.3, 0.5, 0.3, mark_model_path}, mark_labels, is_cude);
//    Detector *detector_model = new Detector(decision_model_path, is_cude);
//
//    while (true) {
//        switch (args.run_state) {
//            // 0 -> stop, 1 -> run, 2 -> pause, 3 -> debug
//            case 0:
//                // 结束
//                return;
//            case 1:
//                mainRun(
//                        winId_list,
//                        result_widget_list,
//                        yolo_model,
//                        detector_model,
//                        decision_labels,
//                        args.run_yolo,
//                        args.run_detector,
//                        args.run_output
//                );
//
//                args = this->func();
//                break;
//            case 2:
//                // 等待
//                QThread::msleep(500); // 500 毫秒
//                args = this->func();
//                break;
//            case 3:
//                mainRun(
//                        winId_list,
//                        result_widget_list,
//                        yolo_model,
//                        detector_model,
//                        decision_labels,
//                        args.run_yolo,
//                        args.run_detector,
//                        args.run_output
//                );
//
//                return;
//            default:
//                // 其他情况暂不做报错处理
//                return;
//        }
//    }
//}

void RunThread::run() {
    this->load_args();

    // 读取yaml文件
    vector<string> mark_labels = loadYamlFile(this->mark_model_labels_path);
    vector<string> decision_labels = loadYamlFile(this->decision_model_labels_path);

    // 创建模型
    YOLOv5 *yolo_model = new YOLOv5({0.3, 0.5, 0.3, this->mark_model_path}, mark_labels, this->is_cude);
    Detector *detector_model = new Detector(this->decision_model_path, this->is_cude);

    while (true) {
        switch (debug ? 3 : this->run_state) {
            // 0 -> stop, 1 -> run, 2 -> pause, 3 -> debug
            case 0:
                // 结束
                return;
            case 1:
                mainRun(
                        getDeviceList(),
                        yolo_model,
                        detector_model,
                        decision_labels,
                        this->run_yolo,
                        this->run_detector,
                        this->run_output
                );

                this->load_args();

                break;
            case 2:
                // 等待
                QThread::msleep(500); // 500 毫秒

                this->load_args();

                break;
            case 3:
                mainRun(
                        getDeviceList(),
                        yolo_model,
                        detector_model,
                        decision_labels,
                        this->run_yolo,
                        this->run_detector,
                        this->run_output
                );

                return;
            default:
                // 其他情况暂不做报错处理
                return;
        }
    }
}
