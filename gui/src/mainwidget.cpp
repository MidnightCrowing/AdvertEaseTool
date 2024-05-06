#include "../include/mainwidget.h"
#include <QDebug>

using namespace std;


MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {}

void MainWidget::setCheckBox(QCheckBox *raw_check_box_, QCheckBox *result_check_box_) {
    this->raw_check_box = raw_check_box_;
    this->result_check_box = result_check_box_;
}

void MainWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if (this->is_resize) {
        is_resize = false;

        this->sizeUpdate();

        is_resize = true;
    }
}

void MainWidget::sizeUpdate() {
    // 获取子组件列表
    QList<QWidget *> childWidgets = this->findChildren<QWidget *>();

    // 遍历子组件并设置隐藏
    for (QWidget *childWidget: childWidgets) {
        childWidget->hide();
    }

    int p_w = this->width(), p_h;        // 父组件大小
    int c_w_default, c_h_default = 482;  // 子组件默认大小
    int c_w, c_h;                        // 子组件大小
    int max_n;                           // 单行能容纳的最大数量
    int n_x = 1, n_y = 1;                // 棋盘坐标
    int p_x, p_y;                        // 位置坐标
    float doub;                          // 子组件缩放的倍率

    bool show_raw = this->raw_check_box->isChecked();
    bool show_result = this->result_check_box->isChecked();
    c_w_default = show_raw && show_result ? 424 : (show_raw || show_result ? 210 : 0);
    max_n = floor((p_w - this->layoutMargin * 2 + this->layoutSpacing) / (c_w_default + this->layoutSpacing));
    if (max_n != 0) {
        c_w = (p_w - this->layoutMargin * 2 - this->layoutSpacing * (max_n - 1)) / max_n;
    } else {
        c_w = p_w - this->layoutMargin * 2;
    }
    c_h = c_w_default != 0 ? c_h_default * c_w / c_w_default : c_h_default;

    vector<Device *> device_list = getDeviceList();

    for (Device *device: device_list) {
        p_x = this->layoutMargin + (n_x - 1) * (c_w + this->layoutSpacing);
        p_y = this->layoutMargin + (n_y - 1) * (c_h + this->layoutSpacing);

        device->ui->move(p_x, p_y);
        device->ui->resize(c_w, c_h);
        device->ui->show();
        device->ui->uiUpdate(this->layoutSpacing);

        n_x++;
        if (n_x > max_n) {
            n_x = 1;
            n_y++;
        }
    }

    if (n_x == 1) {
        n_y--;
    }
    p_h = this->layoutMargin * 2 + n_y * (c_h + this->layoutSpacing) - this->layoutSpacing;
    this->setMinimumHeight(p_h);

    // Add some devices to test
    // Device *d = new Device("Device1", 1, true);
    // DeviceGroup *w = new DeviceGroup(this);
    // w->setStyleSheet("background-color:white");
    // d->setUi(w);
    // devices_list.push_back(d);
}
