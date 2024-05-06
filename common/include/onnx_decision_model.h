#include "onnxruntime_cxx_api.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

namespace utils {
    std::wstring charToWstring(const char *charArray);

    size_t vectorProduct(const std::vector<int64_t> &vec);
}

class Detector {
public:
    Detector(const std::string &modelPath, const bool &isGPU);

    std::string detect(std::vector<float> inputs);

private:
    Ort::Env env{nullptr};
    Ort::SessionOptions session_options{nullptr};
    Ort::Session session{nullptr};

    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
};


std::pair<std::string, double> detectConf(
        const std::string &modelPath,
        std::vector<float> inputs,
        const bool &is_cude = false
);

std::string reDetectConf(Detector *detector, std::vector<float> inputs);

#ifndef C_CLASS_DEMO_ONNX_DECISION_MODEL_H
#define C_CLASS_DEMO_ONNX_DECISION_MODEL_H

#endif //C_CLASS_DEMO_ONNX_DECISION_MODEL_H
