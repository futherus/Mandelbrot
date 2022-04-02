#include <stdlib.h>
#include <QApplication>
#include <QtWidgets>

#include "mandelbrot_GUI.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MandelbrotWidget wgt;
    wgt.resize(600, 400);

    wgt.show();
    return a.exec();
}
