#ifndef GUI_ADD_DEVICE_H
#define GUI_ADD_DEVICE_H

#include <QApplication>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QWindow>
#include <QWidget>
#include <QScreen>
#include <QPixmap>
#include <windows.h>
#include "device_control.h"
#include "mainwidget.h"
#include "utils.h"

class AddDeviceThread : public QThread {
Q_OBJECT

public:
    AddDeviceThread();

    void set(QListWidgetItem *item_, QCheckBox *raw_check_box_, QCheckBox *result_check_box_, MainWidget *main_widget_);

protected:
    void run() override;

private:
    QString name;
    QListWidgetItem *item;
    QCheckBox *raw_check_box;
    QCheckBox *result_check_box;
    MainWidget *main_widget;
};

#endif //GUI_ADD_DEVICE_H
