#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtWidgets>
#include <vector>
#include "device_control.h"

class MainWidget : public QWidget {

public:
    MainWidget(QWidget *parent = nullptr);

    void setCheckBox(QCheckBox *raw_check_box_, QCheckBox *result_check_box_);

    void sizeUpdate();

protected:
    int layoutMargin = 7;
    int layoutSpacing = 4;
    bool is_resize = true;
    QCheckBox *raw_check_box;
    QCheckBox *result_check_box;

    void resizeEvent(QResizeEvent *event) override;
};


#endif // MAINWIDGET_H
