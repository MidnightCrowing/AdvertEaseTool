# AdvertEaseTool

[简体中文](README_CN.md) | English

This project was created for completing a C language course design. Its goal is to migrate the main functionalities of the Python version project [PyPhoneAutomate](https://github.com/MidnightCrowing/PyPhoneAutomate.git) to a C++ environment.

## Program Features

This project attempts to solve the problem of Li Tiao Tiao's inability to close ads in certain scenarios using deep learning technology and visual methods. It supports connecting multiple mobile phones, with support for both Wi-Fi and USB data cable connections.

## Platform Compatibility

- Client: This software supports the Windows platform, but has not been tested on Mac and Linux platforms.
- Server: Due to the use of third-party software `ADB` and `scrcpy`, the program only supports `Android` devices and does not support `iOS` devices.

## Use Cases

This project can be used for automating APPs. Combined with `Appium`, it can automate tasks like auto check-ins.

## File Structure

```
├─common
│  ├─include
│  └─src
├─cli
│  ├─include
│  └─src
└─gui
    ├─include
    ├─resource
    │  ├─icon
    │  └─theme
    ├─src
    └─ui
```

`/common` contains model invocation code, which is common for both command-line and graphical user interface parts.

`/cli` contains the code for the command-line part of the project.

`/gui` contains the code for the graphical user interface part of the project. `/resource` contains the resources required for the interface. `/icon` contains the program's icon files, and `/theme` contains the program's theme resource files.

## Overview

In the Python version, the logic for obtaining the coordinates of the close button is as follows: First, the image is read and the `yolo` model is called. This model takes the acquired image as input and outputs annotated results containing labels, confidence levels, and position information. Then, the confidence information of these labels is mapped to all possible label sets, with a confidence level of `0.0` if a label is not present in the result. Next, the `sklearn` model is called to make predictions to determine the labels to be operated on (returns `no` if there are no labels to operate on). Finally, based on the label position information obtained from `yolo`, the coordinates of the button to be clicked are determined.

In the C++ version, the same logic is used, but since there is no `yolo` and `sklearn` environment in C++, these two models need to be converted to `onnx` format. This can be achieved as follows:

1. The `yolo` model can be converted to `onnx` format using the `export.py` script provided by the framework.
2. The `sklearn` model needs to be converted to `onnx` format using the `skl2onnx` library. Specific conversion methods can be found online.

Once the conversion is complete, the models can be called using the following libraries:

- For the `yolo` model, it can be invoked using the `dnn` module of `opencv`.
- For the `sklearn` model, `onnxruntime` library is used for invocation.

Detailed calling methods can be found online. This way, the same logic can be used in C++ environment to obtain the coordinates of the close button.

By using third-party software `adb` and `scrcpy`, the program can capture screenshots of mobile phones and simulate clicking operations on the screen to achieve corresponding functions.

## Running Guide

This project was written using `CLion` and `Qt Creator`, and compiled using `CMake` + `MinGW`. The `Qt` version used is `Qt6`. To load this project in `CLion`, open `CLion` in the project root directory, then generate the command-line version and graphical interface version separately.

If you need to modify the UI in `Qt Creator`, open all files and folders under the `/gui` directory.

Note:

- Compiling the project in `Qt Creator` may fail due to the lack of the `yaml-cpp` library, while it can compile normally in `CLion`.
- When compiling the graphical interface in `CLion`, the `CMake` file loaded is `CMakeLists_clion.txt` under the `/gui` directory, while when compiling in `Qt Creator`, the loaded `CMake` file is `CMakeLists.txt` in the root directory.

## Environment Configuration

Before compiling this project, you need to configure three dependent libraries on your computer: `opencv`, `onnxruntime`, and `yaml-cpp`. The specific versions used in this project are:

- `opencv`: 4.9.0
- `onnxruntime`: onnxruntime-win-x64-1.17.3
- `yaml-cpp`: 0.8.0#1

The `onnxruntime` used here is the CPU version. If you want to use the GPU version, you need to download the GPU version.

Detailed configuration tutorials can be found online.

## Acknowledgments

This project uses the following external code libraries:

- [Qt](https://www.qt.io/zh-cn/)
- [scrcpy](https://github.com/Genymobile/scrcpy)
- [opencv](https://github.com/opencv/opencv.git)
- [onnxruntime](https://github.com/microsoft/onnxruntime)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)

The following code libraries were used for model training:

- [yolo](https://github.com/ultralytics/yolov5.git)
- [scikit-learn](https://github.com/scikit-learn/scikit-learn)

The following code libraries were used for model conversion:

- [skl2onnx](https://pypi.org/project/skl2onnx/)

The software icons used in this project were generated using `Stable Diffusion`. The model link is: [Icons.Redmond - App Icons Lora for SD XL 1.0](https://civitai.com/models/122827/iconsredmond-app-icons-lora-for-sd-xl-10)

The Qt style theme used in the project's graphical interface is from [Qt-Material](https://github.com/UN-GCPDS/qt-material), and the icon resources used are from [IntelliJ Platform UI Guidelines](https://jetbrains.design/intellij/resources/icons_list/).

Thanks to all the programmers for their selfless dedication to the open-source community!
