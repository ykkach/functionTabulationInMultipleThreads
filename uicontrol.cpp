#include "uicontrol.h"
#include "ui_uicontrol.h"

UiControl::UiControl(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UiControl)
{
    ui->setupUi(this);
}

UiControl::~UiControl()
{
    delete ui;
}

