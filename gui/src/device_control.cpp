#include "../include/device_control.h"

using namespace std;

vector<Device *> devices_list;


DeviceGroup::DeviceGroup(QWidget *parent) : QWidget(parent) {}

void DeviceGroup::setWidget(QWidget *raw_widget_, QLabel *result_widget_) {
    this->raw_widget = raw_widget_;
    this->result_widget = result_widget_;
}

void DeviceGroup::setCheckBox(QCheckBox *raw_check_box_, QCheckBox *result_check_box_) {
    this->raw_check_box = raw_check_box_;
    this->result_check_box = result_check_box_;
}

void DeviceGroup::uiUpdate(int layoutSpacing) {
    bool show_raw = this->raw_check_box->isChecked();
    bool show_result = this->result_check_box->isChecked();

    int p_w = this->width();
    int p_h = this->height();

    if (show_raw && show_result) {
        // 均显示
        int c_w = (p_w - layoutSpacing) / 2;
        int c_h = p_h;

        this->raw_widget->move(0, 0);
        this->raw_widget->resize(c_w, c_h);
        this->raw_widget->show();

        this->result_widget->move(c_w + layoutSpacing, 0);
        this->result_widget->resize(c_w, c_h);
        this->result_widget->show();
    } else if (show_raw || show_result) {
        // 二显一
        int c_w = p_w;
        int c_h = p_h;

        this->raw_widget->move(0, 0);
        this->raw_widget->resize(c_w, c_h);
        show_raw ? this->raw_widget->show() : this->raw_widget->hide();

        this->result_widget->move(0, 0);
        this->result_widget->resize(c_w, c_h);
        show_result ? this->result_widget->show() : this->result_widget->hide();
    } else {
        // 不显示
        this->raw_widget->hide();
        this->result_widget->hide();
    }
}

// Constructors
Device::Device() : name(""), local(true) {}

Device::Device(const QString &name, bool local) : name(name), local(local) {}

// Destructor
Device::~Device() {}

// Getter and Setter methods
QString Device::getName() const {
    return name;
}

void Device::setName(const QString &name_) {
    this->name = name_;
}

bool Device::isLocal() const {
    return local;
}

void Device::setLocal(bool local_) {
    this->local = local_;
}

void Device::setUi(DeviceGroup *ui_) {
    this->ui = ui_;
}

void Device::setWId(WId winId_) {
    this->winId = winId_;
}

void Device::setProcess(QProcess *process_) {
    this->process = process_;
}

// 使用正则解析屏幕尺寸
void Device::parseScreenSize(const string &output) {
    // 正则表达式模式匹配屏幕尺寸
    regex pattern(R"(Physical size: (\d+)x(\d+))");

    // 进行匹配
    smatch match;
    if (regex_search(output, match, pattern)) {
        if (match.size() == 3) {
            // 提取匹配到的宽度和高度
            this->width = stoi(match[1]);
            this->height = stoi(match[2]);
        }
    }
}

void Device::getDeviation(int img_width, int img_height) {
    // 计算宽度和高度的缩放比例
    this->scale_ratio = min(static_cast<double>(img_width) / this->width,
                            static_cast<double>(img_height) / this->height);

    // 计算缩放后的宽度和高度
    int scaled_width = this->width * this->scale_ratio;
    int scaled_height = this->height * this->scale_ratio;

    // 计算水平和垂直偏移
    this->d_x = (img_width - scaled_width) / 2;
    this->d_y = (img_height - scaled_height) / 2;

    this->deviationed = true;
}

void addDevice(Device *device) {
    devices_list.push_back(device);
}

vector<Device *> getDeviceList() {
    // 对列表中的元素按照order和name属性进行排序
//    sort(devices_list.begin(), devices_list.end(), compareByOrder);

    return devices_list;
}

void removeDevice(Device *device) {
    devices_list.erase(remove(devices_list.begin(), devices_list.end(), device), devices_list.end());
    delete device;
    device = nullptr;
}

bool ifNameInDeviceList(QString name) {
    // 使用 any_of 算法来判断是否存在设备名与 name 相同的设备
    bool check = any_of(devices_list.begin(), devices_list.end(), [&](Device *device) {
        return device->name == name;
    });
    return check;
}

// 创建一个比较函数，用于排序
bool compareByOrder(const Device &a, const Device &b) {
    if (a.isLocal() != b.isLocal()) {
        return a.isLocal();
    } else {
        return a.getName() < b.getName();
    }
}