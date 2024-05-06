#include "../include/args_parsing.h"

using namespace std;

/**
 * 打印帮助信息
 */
void printHelp() {
    cout << "Usage: c_class_demo <command> [options]\n"
            "\n"
            "c_class_demo is a program for processing images using annotation and decision models.\n"
            "\n"
            "Options:\n"
            "  -h, --help                          Show this help message and exit.\n"
            "  -i                                  Path to the input image.\n"
            "  -o                                  Path to save the output image. (Default: input_image_path_mark)\n"
            "  --mark_model_path                   Path to the annotation model. (Required)\n"
            "  --mark_model_labels_path            Path to the annotation model labels. (Required)\n"
            "  --decision_model_path               Path to the decision model. (Required)\n"
            "  --decision_model_labels_path        Path to the decision model labels. (Required)\n"
            "  --cude                              Indicates if CUDA is being used. (Default: false)\n";
}

/**
 * 解析程序输入参数
 */
Args processArguments(int argc, char *argv[]) {
    Args args;
    string inputImagePath;
    string outputImagePath;
    string markModelPath;
    string markModelLabelsPath;
    string decisionModelPath;
    string decisionModelLabelsPath;
    bool is_cude = false;

    args.process = false;
    args.help = false;

    if (argc < 2) {
        cerr << "Error: Insufficient arguments.\n";
        printHelp();
        return args;
    }

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputImagePath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputImagePath = argv[++i];
        } else if (arg == "--mark_model_path" && i + 1 < argc) {
            markModelPath = argv[++i];
        } else if (arg == "--mark_model_labels_path" && i + 1 < argc) {
            markModelLabelsPath = argv[++i];
        } else if (arg == "--decision_model_path" && i + 1 < argc) {
            decisionModelPath = argv[++i];
        } else if (arg == "--decision_model_labels_path" && i + 1 < argc) {
            decisionModelLabelsPath = argv[++i];
        } else if (arg == "--cude" && i + 1 < argc) {
            is_cude = true;
        } else if (arg == "-h" || arg == "--help") {
            printHelp();
            args.help = true;
            return args;
        }
    }

    // 设置默认参数
    if (outputImagePath.empty() && !inputImagePath.empty()) {
        // 分割路径
        tuple<string, string, string> result = parsingFilePath(inputImagePath);
        string root_path, fname, ext; // 根路径, 文件名, 扩展名
        tie(root_path, fname, ext) = result;

        // 修改文件名
        fname = fname + "_mark";

        // 合并路径
        outputImagePath = combinePath(root_path, fname, ext);
    }

    // 检查是否缺少必需参数
    bool args_empty = (
            inputImagePath.empty()
            || outputImagePath.empty()
            || markModelPath.empty()
            || markModelLabelsPath.empty()
            || decisionModelPath.empty()
            || decisionModelLabelsPath.empty()
    );
    if (args_empty) {
        cerr << "Error: Missing required arguments.\n";
        printHelp();
        return args;
    }

    // 输出参数
#pragma ide diagnostic ignored "Simplify"
    if (false) {
        cout << "Input Image Path: " << inputImagePath << endl;
        cout << "Output Image Path: " << outputImagePath << endl;
        cout << "Mark Model Path: " << markModelPath << endl;
        cout << "Decision Model Path: " << decisionModelPath << endl;
    }

    // 解析成功
    args.process = true;
    args.inputImagePath = inputImagePath;
    args.outputImagePath = outputImagePath;
    args.markModelPath = markModelPath;
    args.markModelLabelsPath = markModelLabelsPath;
    args.decisionModelPath = decisionModelPath;
    args.decisionModelLabelsPath = decisionModelLabelsPath;
    args.is_cude = is_cude;
    return args;
}

/**
 * 分割路径，将路径字符串解析为驱动器名、文件夹路径、文件名和扩展名，并返回这些部分
 *
 * @param path_buffer 待解析的路径字符串
 * @return 元组，包含驱动器名、文件夹路径、文件名和扩展名
 */
tuple<string, string, string> parsingFilePath(const string &path_buffer) {
    char drive[_MAX_DRIVE];    // 驱动器名
    char dir[_MAX_DIR];        // 文件夹路径去掉驱动器名之后的路径
    char fname[_MAX_FNAME];    // 去掉扩展名的文件名
    char ext[_MAX_EXT];        // 扩展名

    // 将 path_buffer 转换为 const char* 类型
    const char *path_cstr = path_buffer.c_str();

    // 调用函数解析路径
    _splitpath(path_cstr, drive, dir, fname, ext);

    // 使用字符串拼接将 drive 和 dir 连接起来
    string root_path = string(drive) + string(dir);

    return make_tuple(root_path, string(fname), string(ext));
}

/**
 * 合并路径，将根路径、文件名和扩展名组合成一个完整的路径
 *
 * @param root_path 根路径
 * @param fname 文件名
 * @param ext 扩展名
 * @return 合并后的路径字符串
 */
string combinePath(const string &root_path, const string &fname, const string &ext) {
    // 将根路径、文件名和扩展名组合成一个完整的路径
    filesystem::path full_path(root_path);
    full_path /= fname;
    full_path.replace_extension(ext);

    // 返回合并后的路径字符串
    return full_path.string();
}