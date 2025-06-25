#include "mainwindow.h"

#include <QApplication>
#include "myopenglwidget_camera.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1280, 720);
    w.show();//Qtのイベントループを開始

    return a.exec();
}
