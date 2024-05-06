#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <filesystem> // C++17 的头文件，用于获取文件路径常量

struct Args {
    bool process{};                       // 是否解析成功
    bool help{};                          // 参数是否为help, 用来判断退出代码
    std::string inputImagePath;           // 输入图片路径
    std::string outputImagePath;          // 输出图片路径
    std::string markModelPath;            // yolo模型路径, onnx文件
    std::string markModelLabelsPath;      // yolo模型标签路径, yaml文件
    std::string decisionModelPath;        // sklearn模型路径, onnx文件
    std::string decisionModelLabelsPath;  // sklearn模型标签路径, yaml文件
    bool is_cude;                         // 是否使用GPU
};

// 函数声明
void printHelp();

Args processArguments(int argc, char *argv[]);

std::tuple<std::string, std::string, std::string> parsingFilePath(const std::string &path_buffer);

std::string combinePath(const std::string &root_path, const std::string &fname, const std::string &ext);

#ifndef C_CLASS_DEMO_ARGS_PARSING_H
#define C_CLASS_DEMO_ARGS_PARSING_H

#endif //C_CLASS_DEMO_ARGS_PARSING_H
