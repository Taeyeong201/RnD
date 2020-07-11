#include "QTWidgetTest.h"

QTWidgetTest::QTWidgetTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    MyWidget* my = new MyWidget(this);


    ui.frame->setLayout(layout);

    layout->addWidget(my);
}
void QTWidgetTest::resetFrame()
{
    QLayoutItem* wItem;

    while ((wItem = layout->layout()->takeAt(0)) != 0)
    {
        if (wItem->widget())
            wItem->widget()->setParent(NULL);
        delete wItem;
    }
}
