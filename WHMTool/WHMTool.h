#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WHMTool.h"

class WHMTool : public QMainWindow
{
    Q_OBJECT

public:
    WHMTool(QWidget *parent = Q_NULLPTR);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    Ui::WHMToolClass ui;
};
