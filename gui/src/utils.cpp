#include "../include/utils.h"

using namespace std;

// 将 string 转换为 wstring
wstring Str2Wstr(const string &str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t *buffer = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, len);
    wstring wstr(buffer);
    delete[] buffer;
    return wstr;
}

// 执行命令并返回输出结果
string ExeCmd(const string &command) {
    wstring command_w = Str2Wstr(command);

    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        cerr << "CreatePipe failed!" << endl;
        return "";
    }

    STARTUPINFO si = {sizeof(STARTUPINFO)};
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;

    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<LPWSTR>(command_w.c_str()), NULL, NULL, TRUE,
                       0, NULL, NULL, &si, &pi)) {
        cerr << "CreateProcess failed!" << endl;
        CloseHandle(hWrite);
        return "";
    }

    CloseHandle(hWrite);

    string output;
    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
        output.append(buffer, bytesRead);
    }

    CloseHandle(hRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return output;
}

// 判断是否为ipv4或ipv6
bool isValidIPAddress(const string &str) {
    // IPv4 正则表达式
    const regex ipv4Regex(
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(:\\d+)?$");

    // IPv6 正则表达式
    const regex ipv6Regex("^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}(\\:\\d+)?$");

    // 使用正则表达式匹配字符串
    return regex_match(str, ipv4Regex) || regex_match(str, ipv6Regex);
}
