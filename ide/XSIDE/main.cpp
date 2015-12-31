#include "xsmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XSMainWindow w;
    w.show();

    return a.exec();
}
