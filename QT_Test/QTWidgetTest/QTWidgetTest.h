#pragma once

#include <QtWidgets/QMainWindow>
#include <QBoxLayout>
#include "ui_QTWidgetTest.h"
#include "MyWidget.h"

class QTWidgetTest : public QMainWindow
{
    Q_OBJECT

public:
    QTWidgetTest(QWidget *parent = Q_NULLPTR);

private:
    Ui::QTWidgetTestClass ui;
};
