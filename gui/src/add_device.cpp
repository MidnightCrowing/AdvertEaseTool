#include "../include/add_device.h"

using namespace std;

AddDeviceThread::AddDeviceThread() {}

void AddDeviceThread::set(
        QListWidgetItem *item_,
        QCheckBox *raw_check_box_,
        QCheckBox *result_check_box_,
        MainWidget *main_widget_
) {
    this->item = item_;
    this->name = item_->text();
    this->raw_check_box = raw_check_box_;
    this->result_check_box = result_check_box_;
    this->main_widget = main_widget_;
}

void AddDeviceThread::run() {
    auto *process = new QProcess();
    QStringList common_list;
    common_list << "-s" << this->name << "--lock-video-orientation=0";
    process->start("scrcpy/scrcpy.exe", common_list);

    // Timer for 10-second timeout
    QTimer timer;
    timer.setSingleShot(true); // Make timer fire only once

    // Try to find the window within 10 seconds
    WId winId = 0;
    QObject::connect(&timer, &QTimer::timeout, [&winId]() {
        if (!winId) {
            // Exit if window not found
            return;
        }
    });

    timer.start(10000); // 10 seconds timeout

    // Try to find the window within 10 seconds
    while (!winId && timer.isActive()) {
        winId = (WId) FindWindow(L"SDL_app", NULL);
        if (!winId) {
            QApplication::processEvents();  // Process events to keep application responsive
            QThread::msleep(100);           // Add a delay of 100 milliseconds
        }
    }

    if (winId) {
        //
        DeviceGroup *device_group = new DeviceGroup(this->main_widget);

        QWindow *window = QWindow::fromWinId(winId);
        QWidget *raw_widget = QWidget::createWindowContainer(window);
        raw_widget->setParent(device_group);

        QLabel *result_widget = new QLabel();
        result_widget->setParent(device_group);

        device_group->setWidget(raw_widget, result_widget);
        device_group->setCheckBox(this->raw_check_box, this->result_check_box);

        Device *device = new Device();
        device->setName(this->item->text());
        device->setLocal(!isValidIPAddress(this->item->text().toStdString()));
        device->setUi(device_group);
        device->setProcess(process);

        addDevice(device);

        this->main_widget->sizeUpdate();

        this->item->setCheckState(Qt::Checked);
    }

    return;
}
