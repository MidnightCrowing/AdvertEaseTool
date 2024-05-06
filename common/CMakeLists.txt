# OnnxRuntime
set(OnnxRuntime_LIBS D:/onnxruntime-win-x64-1.17.3/lib/onnxruntime.dll)
include_directories(D:/onnxruntime-win-x64-1.17.3/include)  # 添加头文件的路径

# OpenCV
set(OpenCV_DIR D:/opencv-4.9.0/build_mingw64/install)
find_package(OpenCV REQUIRED)

# 编译 onnx_mark_model 为 DLL，并链接 OpenCV
add_library(onnx_mark_model SHARED
        common/src/onnx_mark_model.cpp
)
target_link_libraries(onnx_mark_model
        PRIVATE
        ${OpenCV_LIBS}
)
set_target_properties(onnx_mark_model PROPERTIES PREFIX "")

# 编译 onnx_decision_model 为 DLL，并链接
add_library(onnx_decision_model SHARED
        common/src/onnx_decision_model.cpp
)
target_link_libraries(onnx_decision_model
        PRIVATE
        ${OnnxRuntime_LIBS}
)
set_target_properties(onnx_decision_model PROPERTIES PREFIX "")
