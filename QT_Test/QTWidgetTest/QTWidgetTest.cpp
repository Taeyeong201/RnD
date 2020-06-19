#include "QTWidgetTest.h"

QTWidgetTest::QTWidgetTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    MyWidget* my = new MyWidget(this);

    auto layout = new QVBoxLayout();

    layout->addWidget(my);
    ui.frame->setLayout(layout);
}
