# AdvertEaseTool

简体中文 | [English](README.md)

这个项目是为了完成C语言课程设计而创建。它的目标是将 Python 版本的项目 [PyPhoneAutomate](https://github.com/MidnightCrowing/PyPhoneAutomate.git) 的主要功能迁移到C++环境中实现。

## 程序功能

该项目利用深度学习技术和视觉方法尝试解决李跳跳在某些情况下无法关闭广告的问题。支持连接多部手机，支持手机使用WiFi连接和USB数据线连接。

## 使用平台

- 客户端：该软件支持 Windows 平台，但未经过 Mac 和 Linux 平台的测试。
- 服务端：由于项目使用了第三方软件 `ADB` 和 `scrcpy`，因此仅支持 `Android` 设备，不支持 `iOS` 设备。

## 适用场景

本项目可以在自动化APP方面发挥作用，结合 `Appium` 可以实现自动签到等功能。

## 文件结构

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

`/common` 包含模型调用代码，为命令行和图形化界面的公共调用部分。

`/cli` 包含项目命令行部分的代码。

`/gui` 包含项目图形化界面部分的代码；`/resource` 为界面需要的资源文件，其中 `/icon` 为程序使用的图标文件， `/theme` 为程序使用的主题资源文件。

## 概述

在 Python 版本中，获取关闭按钮坐标的逻辑是这样的：首先，读取图像并调用 `yolo` 模型，该模型输入获取到的图像，并输出带有标签、置信度和位置信息的标注结果。接下来，将这些标签的置信度信息映射到所有可能的标签集，如果某个标签不在结果中，则其置信度为 `0.0` 。然后，调用 `sklearn` 模型进行预测，以确定要操作的标签（如果没有要操作的标签，则返回 `no`）。最后，根据 `yolo` 得到的标签位置信息，确定需要点击的坐标位置。

在 C++ 版本中，依然沿用了相同的逻辑，但由于没有 `yolo` 和 `sklearn` 环境，需要将这两个模型转换为 `onnx` 格式。可以通过以下步骤实现转换：

1. `yolo` 模型可以通过框架中自带的 `export.py` 脚本转换为 `onnx` 模型。
2. `sklearn` 模型则需要使用 `skl2onnx` 库将其转换为 `onnx` 格式。具体转换方法可以在网上找到相应的教程。

转换完成后，可以使用以下库进行模型调用：

- 对于 `yolo` 模型，可以使用 `opencv` 的 `dnn` 模块进行调用。
- 对于 `sklearn` 模型，需要使用 `onnxruntime` 库进行调用。

详细的调用方法可以在网上找到相应的资料和教程。这样，在 C++ 环境中也可以继续使用相同的逻辑来获取关闭按钮的坐标。

通过使用第三方软件 `adb` 和 `scrcpy`，程序可以实现获取手机的屏幕截图，并且可以在屏幕上实现模拟点击操作，从而实现相应的功能。

## 运行指南

本项目使用 `CLion` 和 `Qt Creator` 编写，并使用 `CMake` + `MinGW` 编译项目，其中 `Qt` 版本为 `Qt6` 。要在 `CLion` 中加载此项目，请在项目根目录中打开 `CLion`，然后分别生成命令行版本和图形化界面版本。

如果需要在 `Qt Creator` 中修改 UI，请在 `/gui` 目录下选择所有文件及文件夹后打开。

注意事项：

- 在 `Qt Creator` 中可能无法成功编译项目，原因是缺少 `yaml-cpp` 库，而在 `CLion` 中可以正常编译。
- 在 `CLion` 中编译图形化界面时，加载的 `CMake` 文件为 `/gui` 目录下的 `CMakeLists_clion.txt` ，而在 `Qt Creator` 中编译时加载的 `CMake` 文件为根目录下的 `CMakeLists.txt` 文件。

## 环境配置

在编译本项目之前，你需要在电脑上配置好依赖的三个库，分别为 `opencv` 、 `onnxruntime` 、 `yaml-cpp` 。本项目使用的具体版本为：

- `opencv` ：4.9.0
- `onnxruntime` ：onnxruntime-win-x64-1.17.3
- `yaml-cpp` ：0.8.0#1

其中的 `onnxruntime` 使用的是CPU版本，如想使用GPU则需下载GPU版本。

具体配置教程可在网上找到。

## 鸣谢

本项目使用了以下外部代码库：

- [Qt](https://www.qt.io/zh-cn/)
- [scrcpy](https://github.com/Genymobile/scrcpy)
- [opencv](https://github.com/opencv/opencv.git)
- [onnxruntime](https://github.com/microsoft/onnxruntime)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)

训练模型所使用的代码库包括：

- [yolo](https://github.com/ultralytics/yolov5.git)
- [scikit-learn](https://github.com/scikit-learn/scikit-learn)

模型转换所使用的代码库包括：

- [skl2onnx](https://pypi.org/project/skl2onnx/)

本项目使用的软件图标是使用 `Stable Diffusion` 生成的，模型链接： [Icons.Redmond - App Icons Lora for SD XL 1.0](https://civitai.com/models/122827/iconsredmond-app-icons-lora-for-sd-xl-10)

项目图形化界面使用的 Qt 样式主题来自于 [Qt-Material](https://github.com/UN-GCPDS/qt-material) ，使用的图标资源来自于 [IntelliJ Platform UI Guidelines](https://jetbrains.design/intellij/resources/icons_list/)

感谢各位编程工作者对开源社区的无私奉献！
