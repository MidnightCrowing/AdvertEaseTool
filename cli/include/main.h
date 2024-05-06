#include <iostream>
#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "../include/args_parsing.h"
#include "../../common/include/onnx_mark_model.h"
#include "../../common/include/onnx_decision_model.h"

vector<string> loadYamlFile(const string &file_path);

vector<float> toDecisionModelInput(const vector<DetectLabel> &detect_result, vector<string> decision_labels);


#ifndef C_CLASS_DEMO_MAIN_H
#define C_CLASS_DEMO_MAIN_H

#endif //C_CLASS_DEMO_MAIN_H
