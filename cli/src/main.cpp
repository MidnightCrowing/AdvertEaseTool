#include "../include/main.h"

using namespace std;

int main(int argc, char *argv[]) {
    // 解析传入参数
    Args input_args = processArguments(argc, argv);
    if (!input_args.process) {
        return !input_args.help;
    }
    // 测试代码
//    Args input_args;
//    input_args.markModelPath = "F:\\advertising_interface_yolov5.onnx";
//    input_args.markModelLabelsPath = "F:\\yolo_labels.yaml";
//    input_args.decisionModelPath = "F:\\advertising_interface_sklearn_remake.onnx";
//    input_args.decisionModelLabelsPath = "F:\\sklearn_labels.yaml";
//    input_args.inputImagePath = "F:\\20240424_223759.jpg";
//    input_args.outputImagePath = "F:\\20240424_223810_mark.jpg";
//    input_args.is_cude = false;

    // 读取yaml文件
    vector<string> mark_labels = loadYamlFile(input_args.markModelLabelsPath);
    vector<string> decision_labels = loadYamlFile(input_args.decisionModelLabelsPath);

    // 图片预测
    pair detect_image_result_pair = detectImage(
            {0.3, 0.5, 0.3, input_args.markModelPath},
            mark_labels,
            input_args.inputImagePath,
            input_args.outputImagePath,
            input_args.is_cude
    );
    vector<DetectLabel> detect_image_result = detect_image_result_pair.first;
    double detect_image_run_time = detect_image_result_pair.second;
    cout << "detect image run time: " << detect_image_run_time << endl;

    // 置信度预测
    vector<float> decision_model_input = toDecisionModelInput(detect_image_result, decision_labels);
    pair<string, double> detect_conf_result_pair = detectConf(
            input_args.decisionModelPath,
            decision_model_input,
            input_args.is_cude
    );
    string detect_conf_result = detect_conf_result_pair.first;
    double detect_conf_run_time = detect_conf_result_pair.second;
    cout << "detect conf run time: " << detect_conf_run_time << endl;
    cout << "Inference result: " << detect_conf_result << endl;

    return 0;
}

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
