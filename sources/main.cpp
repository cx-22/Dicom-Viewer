#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pybind11::scoped_interpreter guard{};
    MainWindow w;
    w.show();
    return a.exec();
}
