#include "user.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UserWindow w;
    w.show();
//    MainWindow s;
//    s.show();

    return a.exec();
}
