# Yaml
set(yaml-cpp_DIR D:/vcpkg/installed/x64-mingw-dynamic/share/yaml-cpp)
find_package(yaml-cpp CONFIG REQUIRED)

# 编译 args_parsing 为 DLL，并链接 OpenCV 的 DNN 模块
add_library(args_parsing SHARED
        cli/src/args_parsing.cpp
)
# 设置 PREFIX 属性为 ""，以去除生成的库文件名前的 lib 前缀
set_target_properties(args_parsing PROPERTIES PREFIX "")

# 创建可执行文件 aet.exe，并链接所需库文件
add_executable(aet
        cli/src/main.cpp
)
target_link_libraries(aet
        PRIVATE
        ${OpenCV_LIBS}
        ${OnnxRuntime_LIBS}
        yaml-cpp::yaml-cpp   # 这里的顺序不要改, 编译会报错
        args_parsing         # 链接 args_parsing.dll
        onnx_mark_model
        onnx_decision_model
)
