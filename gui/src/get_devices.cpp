#include "../include/get_devices.h"

using namespace std;

// 获取连接的设备列表
vector<string> getConnectedDevices() {
    vector<string> devices;

    string output = ExeCmd("scrcpy/adb devices");
    size_t pos = output.find("\n"); // 找到第一个换行符
    if (pos != string::npos) {
        output = output.substr(pos + 1); // 跳过第一行
        pos = output.find("\n"); // 找到第二行
        while (pos != string::npos) {

            if (output != "\r\n") {
                size_t spacePos = output.find('\t');

                // 提取设备编号部分
                string deviceID = output.substr(0, spacePos);

                // 将设备编号添加到设备列表中
                devices.push_back(deviceID);
            }

            output = output.substr(pos + 1); // 继续处理下一行
            pos = output.find("\n");
        }
    }

    return devices;
}

/* StartADBThread */

StartADBThread::StartADBThread() {}

void StartADBThread::set(QLabel *label, QPushButton *btn) {
    m_label = label;
    m_btn = btn;
}

void StartADBThread::run() {
    m_label->setText(QString("正在启动ADB..."));

    string output = ExeCmd("scrcpy/adb start-server");

    m_label->setText(QString("ADB正在运行"));

    m_btn->setDisabled(false);
}

/* GetConnectedDevicesThread */

GetConnectedDevicesThread::GetConnectedDevicesThread() {}

void GetConnectedDevicesThread::set(QListWidget *list_widget) {
    m_list_widget = list_widget;
}

void GetConnectedDevicesThread::run() {
    vector<string> connectedDevices = getConnectedDevices();

    // 清空列表中的所有项目
    m_list_widget->clear();

    for (const auto &device: connectedDevices) {

        // 创建 QListWidgetItem，并设置文本
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(device));

        // 根据设备是否是网络设备设置图标
        if (isValidIPAddress(device)) {
            // 网络设备
            item->setIcon(QIcon(":/icon/software/icon/remoteSwiftPackageDependency.svg"));
        } else {
            // 本地设备
            item->setIcon(QIcon(":/icon/software/icon/swiftPackage.svg"));
        }

        // 根据 check 设置 item 的选中状态
        item->setCheckState(ifNameInDeviceList(item->text()) ? Qt::Checked : Qt::Unchecked);

//        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

        // 添加 item 到 QListWidget
        m_list_widget->addItem(item);
    }
}
