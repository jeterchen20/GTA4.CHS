#include "WHMTranslate.h"

WHMTranslate::WHMTranslate(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    statusBar()->showMessage(QStringLiteral("����whm�ļ���ʼ�༭"));
}
