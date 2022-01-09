#include "WHMTranslate.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WHMTranslate w;
    w.show();
    return a.exec();
}
