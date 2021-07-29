#include "WHMTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WHMTool w;
    w.show();
    return a.exec();
}
