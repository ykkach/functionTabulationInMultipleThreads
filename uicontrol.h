#ifndef UICONTROL_H
#define UICONTROL_H

#include <QMainWindow>
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QElapsedTimer>
#include <QMessageBox>
#include <sstream>
#include <Windows.h>
#include <process.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class UiControl; }
QT_END_NAMESPACE

class UiControl : public QMainWindow
{
    Q_OBJECT

public:
    UiControl(QWidget *parent = nullptr);
    ~UiControl();

private slots:
    void on_create_clicked();

    void on_runall_clicked();

    void on_run_clicked();

    void on_Pause_clicked();

    void on_killThread_clicked();

    void on_priority_currentIndexChanged(int index);

    void on_runwithouttime_clicked();

private:
    QString priority(int);

private:
    Ui::UiControl *ui;
    HANDLE *hThreads;
    int numOfThreads;
};

unsigned int _stdcall dThread(LPVOID);
unsigned int _stdcall sThread(LPVOID);
unsigned int _stdcall tThread(LPVOID);

#endif // UICONTROL_H
