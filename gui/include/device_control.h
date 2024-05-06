#ifndef GUI_DEVICE_CONTROL_H
#define GUI_DEVICE_CONTROL_H

#include <vector>
#include <regex>
#include <QtWidgets>
#include <QString>
#include "utils.h"

class DeviceGroup : public QWidget {

public:
    DeviceGroup(QWidget *parent = nullptr);

    void setWidget(QWidget *raw_widget_, QLabel *result_widget_);

    void setCheckBox(QCheckBox *raw_check_box_, QCheckBox *result_check_box_);

    void uiUpdate(int layoutSpacing);

    QWidget *raw_widget;
    QLabel *result_widget;

private:
    QCheckBox *raw_check_box;
    QCheckBox *result_check_box;
};

class Device {
public:
    QString name;
    bool local;
    DeviceGroup *ui;
    WId winId;
    QProcess *process;
    int width;
    int height;
    bool deviationed = false;
    int d_x;             // x方向上偏差
    int d_y;             // y方向上偏差
    double scale_ratio;  // 缩放比

    // Constructors
    Device();

    Device(const QString &name, bool local);

    // Destructor
    ~Device();

    // Getter and Setter methods
    QString getName() const;

    void setName(const QString &name);

    bool isLocal() const;

    void setLocal(bool local);

    void setUi(DeviceGroup *ui_);

    void setWId(WId winId_);

    void setProcess(QProcess *process_);

    void parseScreenSize(const std::string &output);

    void getDeviation(int img_width, int img_height);
};

void addDevice(Device *device);

std::vector<Device *> getDeviceList();

void removeDevice(Device *device);

bool ifNameInDeviceList(QString name);

bool compareByOrder(const Device &a, const Device &b);

#endif //GUI_DEVICE_CONTROL_H
