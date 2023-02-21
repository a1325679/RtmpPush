#pragma once

#include <QtWidgets/QWidget>
#include "ui_XRtmp_OpenCV_SRS.h"

class XRtmp_OpenCV_SRS : public QWidget
{
    Q_OBJECT

public:
    XRtmp_OpenCV_SRS(QWidget *parent = Q_NULLPTR);

private:
    Ui::XRtmp_OpenCV_SRSClass ui;
};
