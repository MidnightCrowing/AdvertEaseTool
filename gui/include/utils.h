#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <string>
#include <regex>
#include "windows.h"
#include <iostream>


std::wstring Str2Wstr(const std::string &str);

std::string ExeCmd(const std::string &command);

bool isValidIPAddress(const std::string &str);

#endif //GUI_UTILS_H
