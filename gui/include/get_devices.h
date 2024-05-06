#ifndef GUI_GET_DEVICES_H
#define GUI_GET_DEVICES_H

#include <deque>
#include "QThread"
#include "QListWidget"
#include "QListWidgetItem"
#include "QLabel"
#include "QPushButton"
#include "device_control.h"
#include "utils.h"

std::vector<std::string> getConnectedDevices();

// 继承 QThread 类并重写 run() 函数，该函数将在新线程中执行
class StartADBThread : public QThread {
Q_OBJECT

public:
    StartADBThread();

    void set(QLabel *m_label, QPushButton *btn);

protected:
    void run() override;

private:
    QLabel *m_label;
    QPushButton *m_btn;
};

class GetConnectedDevicesThread : public QThread {
Q_OBJECT

public:
    GetConnectedDevicesThread();

    void set(QListWidget *list_widget);

protected:
    void run() override;

private:
    QListWidget *m_list_widget;
};

#endif //GUI_GET_DEVICES_H
