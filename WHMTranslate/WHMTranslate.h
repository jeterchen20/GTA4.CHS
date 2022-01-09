#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WHMTranslate.h"

class WHMTranslate : public QMainWindow
{
    Q_OBJECT

public:
    WHMTranslate(QWidget *parent = Q_NULLPTR);

private:
    Ui::WHMTranslateClass ui;
};
