#include "../include/mainwindow.h"
#include "../ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::AdvertEaseTool) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowTitle("AdvertEaseTool");
    setWindowIcon(QIcon("://AdvertEaseTool.ico"));

    connect(ui->min_btn, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));  // 最小化按钮
    connect(ui->max_btn, SIGNAL(clicked(bool)), this, SLOT(trigMaximize()));   // 最大化按钮
    connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(trigExit()));     // 退出按钮

    ui->get_devices_btn->setDisabled(true);
    ui->devices_setting_stackedWidget->hide();
    ui->android_device_setting_btn->click();
    ui->run_setting_frame->hide();
    ui->more_frame->hide();
    ui->pause_btn->hide();

    // // 加载要显示的图片
    // QPixmap pixmap(":/icon/software/icon/loader.svg");

    // // 设置定时器，每隔一段时间旋转一次图片
    // QTimer timer;
    // timer.setInterval(50); // 设置旋转间隔为 50 毫秒
    // QObject::connect(&timer, &QTimer::timeout, [&]() {
    //     static qreal angle = 0;
    //     angle += 5; // 每次旋转 5 度
    //     QPixmap rotatedPixmap = pixmap.transformed(QTransform().rotate(angle));
    //     ui->load_gif->setPixmap(rotatedPixmap);
    //     ui->load_gif->update();
    // });
    // timer.start();

    ui->load_gif->hide();
    QMovie *movie = new QMovie(":/icon/software/icon/loading.gif");
    QSize si(ui->load_gif->width(), ui->load_gif->height());
    movie->setScaledSize(si);
    ui->scrollAreaWidgetContents->setCheckBox(ui->checkBox_5, ui->checkBox_6);
    ui->main_frame->setCurrentIndex(0);
    ui->load_gif->setMovie(movie);  // 1. 设置要显示的 GIF 动画图片
    movie->start();                 // 2. 启动动画

    this->isAppMaximized = false;
    this->isMoveAllowed = false;
    this->adb_start = false;
    this->run_state = 0;  // 0 -> stop, 1 -> run, 2 -> pause, 3 -> debug
    this->run_thread = nullptr;

    ui->target_detection_model_lineEdit->setText("models/advertising_interface_yolov5.onnx");
    ui->target_detection_model_labels_lineEdit->setText("labels/yolo_labels.yaml");
    ui->svm_model_lineEdit->setText("models/advertising_interface_sklearn_remake.onnx");
    ui->svm_model_labels_lineEdit->setText("labels/sklearn_labels.yaml");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::trigMinimize() {
    // 实现最小化功能的代码
}

void MainWindow::trigMaximize() {
    // 实现最大化功能的代码
    if (isAppMaximized == false) {
        this->showMaximized();
        // 最大化以后给最大化按钮换个图标
        ui->max_btn->setIcon(QIcon(":/icon/software/icon/restore_dark.svg"));
        isAppMaximized = true;
    }  // 窗口不是最大化时，最大化窗口
    else {
        this->showNormal();
        ui->max_btn->setIcon(QIcon(":/icon/software/icon/maximize_dark.svg"));
        isAppMaximized = false;
    }  // 窗口最大化时，将窗口恢复正常大小
}

void MainWindow::trigExit() {
    // 实现退出功能的代码
    QApplication::quit();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e) {
    if (e->position().y() <= 38) {
        trigMaximize();
    }  // e的y值就是鼠标点击相对于窗体的y坐标，32为标题栏高度，这个值小于等于32像素即为在标题栏上双击，在其他地方双击鼠标不起作用
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    if (e->position().y() <= 38) {
        this->isMoveAllowed = true;
        this->originalPos = e->pos();
    }  // 在标题栏上按下鼠标，准备拖拽时执行；isMoveAllowed为真时允许移动窗体，originalPos记录了此时鼠标相对桌面的坐标
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if (this->isMoveAllowed == true) {
        if (isAppMaximized == true) {
            trigMaximize();
            this->move(e->globalPosition().toPoint());
        }  // 如果程序被最大化，先恢复正常大小的窗口，然后再继续

        this->move(e->globalPosition().toPoint() - this->originalPos);  // e->globalPos()为鼠标相对于窗口的位置
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e) {
    this->isMoveAllowed = false;  // 禁止窗体移动
}

void MainWindow::on_android_device_setting_btn_clicked() {
    if (ui->devices_setting_stackedWidget->isHidden()) {
        ui->devices_setting_stackedWidget->show();
        ui->devices_setting_stackedWidget->setCurrentIndex(0);
        ui->android_device_setting_btn->setChecked(true);
    } else {
        if (!ui->android_device_setting_btn->isChecked()) {
            ui->devices_setting_stackedWidget->hide();
            ui->android_device_setting_btn->setChecked(false);
        } else {
            ui->devices_setting_stackedWidget->setCurrentIndex(0);
            ui->devices_window_setting_btn->setChecked(false);
            ui->android_device_setting_btn->setChecked(true);
        }
    }
}

void MainWindow::on_devices_window_setting_btn_clicked() {
    if (ui->devices_setting_stackedWidget->isHidden()) {
        ui->devices_setting_stackedWidget->show();
        ui->devices_setting_stackedWidget->setCurrentIndex(1);
        ui->devices_window_setting_btn->setChecked(true);
    } else {
        if (!ui->devices_window_setting_btn->isChecked()) {
            ui->devices_setting_stackedWidget->hide();
            ui->devices_window_setting_btn->setChecked(false);
        } else {
            ui->devices_setting_stackedWidget->setCurrentIndex(1);
            ui->android_device_setting_btn->setChecked(false);
            ui->devices_window_setting_btn->setChecked(true);
        }
    }
}

void MainWindow::on_run_setting_btn_clicked() {
    if (ui->run_setting_frame->isHidden()) {
        ui->run_setting_frame->show();
    } else {
        ui->run_setting_frame->hide();
    };
}

void MainWindow::on_target_detection_model_open_file_btn_clicked() {
    //打开单个文件
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            "/home",
            tr("Model File (*.onnx)"));//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty()) {
        ui->target_detection_model_lineEdit->setText(fileName);
    }
}

void MainWindow::on_target_detection_model_labels_open_file_btn_clicked() {
    //打开单个文件
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            "/home",
            tr("Model Labels File (*.yaml)"));//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty()) {
        ui->target_detection_model_labels_lineEdit->setText(fileName);
    }
}

void MainWindow::on_svm_model_open_file_btn_clicked() {
    //打开单个文件
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            "/home",
            tr("Model File (*.onnx)"));//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty()) {
        ui->svm_model_lineEdit->setText(fileName);
    }
}

void MainWindow::on_svm_model_labels_open_file_btn_clicked() {
    //打开单个文件
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            "/home",
            tr("Model Labels File (*.yaml)"));//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty()) {
        ui->svm_model_labels_lineEdit->setText(fileName);
    }
}

void MainWindow::on_more_btn_clicked() {
    if (ui->more_frame->isHidden()) {
        ui->more_frame->show();
        ui->more_btn->setIcon(QIcon(":/icon/software/icon/chevronDown_dark.svg"));
    } else {
        ui->more_frame->hide();
        ui->more_btn->setIcon(QIcon(":/icon/software/icon/chevronRight_dark.svg"));
    };
}

void MainWindow::on_run_btn_clicked() {
    if (this->run_state == 0) {
        this->run_state = 1;

        // 逻辑处理部分
        this->run_thread = new RunThread(this->run_state);
        this->run_thread->set(
                ui->is_cuda_check->isChecked(),
                ui->radioButton_2,
                ui->radioButton_3,
                ui->radioButton,
                ui->target_detection_model_lineEdit->text(),
                ui->target_detection_model_labels_lineEdit->text(),
                ui->svm_model_lineEdit->text(),
                ui->svm_model_labels_lineEdit->text()
        );
        // struct RunArgs {
        //    // 程序运行状态
        //    int run_state;
        //    // 是否使用CUDE
        //    bool is_cude;
        //    // 运行进度
        //    bool run_yolo;
        //    bool run_detector;
        //    bool run_output;
        //    // 模型文件及其标签集路径
        //    string mark_model_path;
        //    string mark_model_labels_path;
        //    string decision_model_path;
        //    string decision_model_labels_path;
        //    // WId列表, 用于输入原始图片
        //    vector<WId> winId_list;
        //    // QLabel列表, 用于输出标注好的图片
        //    vector<QLabel *> result_widget_list;
        //};
        run_thread->start();

        // ui处理部分
        ui->pause_btn->show();
        ui->run_btn->setIconSize(QSize(25, 25));
        ui->run_btn->setIcon(QIcon(":/icon/software/icon/suspend_dark.svg"));
        ui->pause_btn->setIconSize(QSize(25, 25));
        ui->pause_btn->setIcon(QIcon(":/icon/software/icon/pause.svg"));
        ui->is_cuda_check->setDisabled(true);
    } else if (this->run_state == 1 || this->run_state == 2) {
        this->run_state = 0;

        // 逻辑处理部分
        if (this->run_thread != nullptr) {
            this->run_thread->quit(); // 请求线程退出
            this->run_thread->wait(); // 等待线程完成
        }

        // ui处理部分
        ui->pause_btn->hide();
        ui->run_btn->setIconSize(QSize(17, 17));
        ui->run_btn->setIcon(QIcon(":/icon/software/icon/run_dark.svg"));
        ui->is_cuda_check->setDisabled(false);
    }
}

void MainWindow::on_pause_btn_clicked() {
    if (this->run_state == 1) {
        ui->pause_btn->setIconSize(QSize(17, 17));
        ui->pause_btn->setIcon(QIcon(":/icon/software/icon/run_dark.svg"));
        this->run_state = 2;
    } else if (this->run_state == 2) {
        ui->pause_btn->setIconSize(QSize(25, 25));
        ui->pause_btn->setIcon(QIcon(":/icon/software/icon/pause.svg"));
        this->run_state = 1;
    }
}

void MainWindow::on_debug_btn_clicked() {
    int last_state = this->run_state;
    this->run_state = 3;

    // 逻辑处理部分
    int state = 3;
    this->run_thread = new RunThread(state);
    this->run_thread->set(
            ui->is_cuda_check->isChecked(),
            ui->radioButton_2,
            ui->radioButton_3,
            ui->radioButton,
            ui->target_detection_model_lineEdit->text(),
            ui->target_detection_model_labels_lineEdit->text(),
            ui->svm_model_lineEdit->text(),
            ui->svm_model_labels_lineEdit->text(),
            true
    );
    run_thread->start();

    this->run_state = last_state;
}

void MainWindow::on_check_adb_btn_clicked() {
    // 创建 StartADBThread 对象
    StartADBThread *thread = new StartADBThread();

    // 设置参数
    thread->set(ui->adb_state_label, ui->get_devices_btn);

    // 启动线程
    thread->start();
}

void MainWindow::on_get_devices_btn_clicked() {
    // 创建 getConnectedDevicesThread 对象
    GetConnectedDevicesThread *thread = new GetConnectedDevicesThread();

    // 设置参数
    thread->set(ui->listWidget);

    // 启动线程
    thread->start();
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index) {
    QListWidgetItem *item = ui->listWidget->item(index.row());
    if (item) {
        qDebug() << "Double clicked item text:" << item->text();
        qDebug() << "Double clicked item text:" << item->checkState();

        std::vector<Device *> device_list = getDeviceList();

        bool check = false;  // 判断设备是否存在
        Device *find_device;

        for (Device *device: device_list) {
            if (device->name == item->text()) {
                check = true;
                find_device = device;
                break;
            }
        }

        if (!check) {
            // 添加设备

//            // 创建 AddDeviceThread 对象
//            AddDeviceThread *thread = new AddDeviceThread();
//
//            // 设置参数
//            thread->set(item->text(), ui->scrollAreaWidgetContents);
//
//            // 启动线程
//            thread->start();

            auto *process = new QProcess();
            QStringList common_list;
            common_list << "-s" << item->text() << "--lock-video-orientation=0";
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
                DeviceGroup *device_group = new DeviceGroup(ui->scrollAreaWidgetContents);

                QWindow *window = QWindow::fromWinId(winId);
                QWidget *raw_widget = QWidget::createWindowContainer(window);
                raw_widget->setParent(device_group);

                QLabel *result_widget = new QLabel();
                result_widget->setParent(device_group);
                result_widget->setScaledContents(true);  // 设置缩放模式为铺满 QLabel

                device_group->setWidget(raw_widget, result_widget);
                device_group->setCheckBox(ui->checkBox_5, ui->checkBox_6);

                Device *device = new Device();
                device->setName(item->text());
                device->setLocal(!isValidIPAddress(item->text().toStdString()));
                device->setUi(device_group);
                device->setWId(winId);
                device->setProcess(process);
                device->parseScreenSize(ExeCmd("adb -s " + item->text().toStdString() + " shell wm size"));

                addDevice(device);

                ui->scrollAreaWidgetContents->sizeUpdate();

                item->setCheckState(Qt::Checked);
            }
        } else {
            // 删除设备

            delete find_device->ui;
            find_device->process->terminate();
            removeDevice(find_device);
            item->setCheckState(Qt::Unchecked);
            ui->scrollAreaWidgetContents->sizeUpdate();
        }

        if (getDeviceList().size() == 0) {
            ui->main_frame->setCurrentIndex(0);
        } else {
            ui->main_frame->setCurrentIndex(1);
        }
    }
}

void MainWindow::on_checkBox_5_clicked() {
    ui->scrollAreaWidgetContents->sizeUpdate();
}


void MainWindow::on_checkBox_6_clicked() {
    ui->scrollAreaWidgetContents->sizeUpdate();
}
