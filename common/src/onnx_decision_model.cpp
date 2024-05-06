#include "../include/onnx_decision_model.h"

using namespace std;

namespace utils {
    // 将 char* 转换为 wstring
    wstring charToWstring(const char *charArray) {
        wstring wstr;
        // 假设 char* 是以 null 结尾的，因此我们可以使用 wstring 的构造函数来转换
        wstr.assign(charArray, charArray + strlen(charArray));
        return wstr;
    }

    // 计算向量中所有元素的乘积
    size_t vectorProduct(const vector <int64_t> &vec) {
        size_t product = 1;
        for (int64_t num: vec) {
            product *= num;
        }
        return product;
    }
}

Detector::Detector(const string &modelPath, const bool &isGPU) {
    env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX_DETECTION");
    session_options = Ort::SessionOptions();
    vector<string> availableProviders = Ort::GetAvailableProviders();
    auto cudaAvailable = find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
    OrtCUDAProviderOptions cudaOption;

    if (isGPU && (cudaAvailable == availableProviders.end())) {
//        cout << "GPU is not supported by your ONNXRuntime build. Fallback to CPU." << endl;
//        cout << "Inference device: CPU" << endl;
    } else if (isGPU && (cudaAvailable != availableProviders.end())) {
//        cout << "Inference device: GPU" << endl;
        session_options.AppendExecutionProvider_CUDA(cudaOption);
    } else {
//        cout << "Inference device: CPU" << endl;
    }

    wstring w_modelPath = utils::charToWstring(modelPath.c_str());
    session = Ort::Session(env, w_modelPath.c_str(), session_options);

    Ort::AllocatorWithDefaultOptions allocator;
    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
    vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();

    inputNames.push_back(session.GetInputNameAllocated(0, allocator).get());
    outputNames.push_back(session.GetOutputNameAllocated(0, allocator).get());

    //cout << "Input name: " << inputNames[0] << endl;
    //cout << "Output name: " << outputNames[0] << endl;
}


string Detector::detect(vector<float> inputs) {
    float *blob = inputs.data();
    vector<int64_t> inputTensorShape{1, 14};

    size_t inputTensorSize = utils::vectorProduct(inputTensorShape);
    vector<float> inputTensorValues(blob, blob + inputTensorSize);

    vector<Ort::Value> inputTensors;
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
            OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault
    );
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
            memoryInfo, inputs.data(), inputTensorSize,  // .data()返回一个指向向量内部使用的数组中第一个元素的指针
            inputTensorShape.data(), 2
    ));

    vector<const char *> input_names_char(inputNames.size(), nullptr);
    transform(begin(inputNames), end(inputNames), begin(input_names_char),
              [&](const string &str) { return str.c_str(); });
    vector<const char *> output_names_char(outputNames.size(), nullptr);
    transform(begin(outputNames), end(outputNames), begin(output_names_char),
              [&](const string &str) { return str.c_str(); });

    vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{nullptr},
                                                         input_names_char.data(),
                                                         inputTensors.data(),
                                                         1,
                                                         output_names_char.data(),
                                                         1);

    string rawOutput = outputTensors[0].GetTensorData<char>();

    return rawOutput;

}

//int main() {
//    // 模型文件路径
//    string modelPath = "F:\\advertising_interface_sklearn_remake.onnx";
//    // 是否使用 GPU
//    bool isGPU = false;
//
//    // 创建 Detector 对象
//    Detector detector(modelPath, isGPU);
//
//    // 测试示例输入数据
//    vector<float> inputs = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.943885207f, 0.87722683f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
//
//    // 执行推理
//    string result = detector.detect(inputs);
//
//    // 输出推理结果
//    cout << "Inference result: " << result << endl;
//
//    return 0;
//}

/**
 * 使用指定的模型路径和输入置信度信息执行检测，并返回决策信息。
 *
 * @param modelPath 模型文件的路径
 * @param inputs 输入数据，为置信度信息数组
 * @param is_cude 是否使用 GPU 进行推理，默认为 false
 * @return 一个 pair，包含决策信息和运行时间（单位：秒）。
 */
pair<string, double> detectConf(const string &modelPath, vector<float> inputs, const bool &is_cude) {
    clock_t startTime, endTime; //计算时间

    // 创建 Detector 对象
    Detector detector(modelPath, is_cude);

    startTime = clock();                      // 计时开始
    string result = detector.detect(inputs);  // 执行推理
    endTime = clock();                        // 计时结束
    double clock_running = (double) (endTime - startTime) / CLOCKS_PER_SEC;

    return make_pair(result, clock_running);
}

string reDetectConf(Detector *detector, vector<float> inputs) {

    string result = detector->detect(inputs);  // 执行推理

    return result;
}
