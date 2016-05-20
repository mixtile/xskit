#include "xsmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon(":xs_icon/images/icon.jpg");
    a.setWindowIcon(appIcon);
    XSMainWindow w;
    w.show();

    return a.exec();
}
