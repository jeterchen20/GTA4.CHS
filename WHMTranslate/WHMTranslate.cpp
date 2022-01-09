#include "WHMTranslate.h"

WHMTranslate::WHMTranslate(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    statusBar()->showMessage(QStringLiteral("拖入whm文件开始编辑"));
}
