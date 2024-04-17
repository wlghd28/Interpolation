#include "cinterpolation.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CInterpolation w;
    w.show();
    return a.exec();
}
