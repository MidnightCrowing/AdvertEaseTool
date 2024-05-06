#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QTimer>
#include <QMovie>
#include <vector>
#include <algorithm> // For std::remove
#include "get_devices.h"
#include "device_control.h"
#include "add_device.h"
#include "run.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AdvertEaseTool;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:

    void trigMinimize();

    void trigMaximize();

    void trigExit();

    void mouseDoubleClickEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void on_android_device_setting_btn_clicked();

    void on_devices_window_setting_btn_clicked();

    void on_run_setting_btn_clicked();

    void on_target_detection_model_open_file_btn_clicked();

    void on_target_detection_model_labels_open_file_btn_clicked();

    void on_svm_model_open_file_btn_clicked();

    void on_svm_model_labels_open_file_btn_clicked();

    void on_more_btn_clicked();

    void on_run_btn_clicked();

    void on_pause_btn_clicked();

    void on_check_adb_btn_clicked();

    void on_get_devices_btn_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_debug_btn_clicked();

private:
    Ui::AdvertEaseTool *ui;

    bool isAppMaximized;

    bool isMoveAllowed;

    bool adb_start;

    QPoint originalPos;

    int run_state;

    RunThread *run_thread;
};

#endif // MAINWINDOW_H
