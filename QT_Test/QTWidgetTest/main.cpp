#include "QTWidgetTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTWidgetTest w;
    w.show();
    return a.exec();
}
