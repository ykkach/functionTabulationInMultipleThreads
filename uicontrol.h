#ifndef UICONTROL_H
#define UICONTROL_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class UiControl; }
QT_END_NAMESPACE

class UiControl : public QMainWindow
{
    Q_OBJECT

public:
    UiControl(QWidget *parent = nullptr);
    ~UiControl();

private:
    Ui::UiControl *ui;
};
#endif // UICONTROL_H
