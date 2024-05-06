cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 17)

# 包含其他配置文件
include(common/CMakeLists.txt)

# 判断是否启用 GUI 版本
option(GUI_VERSION "Build GUI version" ON)

# 添加 GUI 版本的可执行文件
if (ON)
    project(AdvertEaseTool VERSION 0.1 LANGUAGES CXX)
    include(gui/CMakeLists_clion.txt)
else ()
    project(aet)
    include(cli/CMakeLists.txt)
endif ()
