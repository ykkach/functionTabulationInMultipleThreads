#include "uicontrol.h"
#include "ui_uicontrol.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QElapsedTimer>
#include <QMessageBox>
#include <sstream>
#include <windows.h>
#include <cmath>
#include <QDebug>

std::string SSTUDENTDATA = "Yaroslav Kachmar 12700969";
std::string PATH = "C:\\Users\\Yaroslav\\Documents\\build-functionTabulationInMultipleThreads-Desktop_Qt_5_14_1_MinGW_32_bit-Debug\\";

struct SDefaultThreadStartArguments
{
    int numberOfIterations;
};

struct STabulationThreadStartArguments
{
    int numberOfIterations;
    double ABorder{0.1};
    double BBorder{0.9};
};

struct SSearchThreadStartArguments
{
    int threadIndex;
    int *array;
    int *minElements;
    int numOfThreads;
    int* minimalElement;
    int startingPoint;
    int endPoint;
};

using SDTSA = SDefaultThreadStartArguments;
using STTSA = STabulationThreadStartArguments;
using SSTSA = SSearchThreadStartArguments;

UiControl::UiControl(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UiControl)
{
    ui->setupUi(this);
}

UiControl::~UiControl()
{
    for(int i = 0; i < numOfThreads; ++i){
       CloseHandle(hThreads[i]);
        }
    delete[] hThreads;
    delete ui;
}

void UiControl::on_create_clicked()
{
    numOfThreads = ui->numOfThreads->currentText().toInt();
    hThreads = new HANDLE[numOfThreads];
    ui->availableThreads->setRowCount(numOfThreads);
    ui->availableThreads->setColumnCount(3);
    ui->availableThreads->setHorizontalHeaderItem(0, new QTableWidgetItem("Thread ID"));
    ui->availableThreads->setHorizontalHeaderItem(1, new QTableWidgetItem("Priority"));
    ui->availableThreads->setHorizontalHeaderItem(2, new QTableWidgetItem("State"));

    double division = 0.8/numOfThreads, A = 0.1, B = 0.9;
    int numOfIterations = ui->steps->toPlainText().toInt()/numOfThreads;

    int startingPoint = 0, endpoint = numOfIterations;
    int *array = new int[ui->steps->toPlainText().toInt()];
    int *minElements = new int[numOfThreads]{0};
    int *minimalElement = new int;

    for(int i = 0 ; i < numOfIterations*numOfThreads; i++)
       array[i] = rand() % 10000 + 10;

    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, FALSE};

    for(int i = 0; i < numOfThreads; ++i){
        unsigned ThreadID;
        switch(ui->chosenTask->currentIndex())
        {
        case 0:{
            SDTSA* DTSA = new SDTSA();
            DTSA->numberOfIterations = numOfIterations;
            hThreads[i] = (HANDLE)_beginthreadex(&sa, 4096, &dThread, (PVOID)DTSA, CREATE_SUSPENDED, &ThreadID);
            break;
        }
        case 1:{
            B = A + division;
            STTSA* TTSA = new STTSA();
            TTSA->numberOfIterations = numOfIterations;
            TTSA->ABorder = A;
            TTSA->BBorder = B;
            hThreads[i] = (HANDLE)_beginthreadex(&sa, 4096, &tThread, (PVOID)TTSA, CREATE_SUSPENDED, &ThreadID);
            A = B;
            break;
        }
        case 2:{
            SSTSA* STSA = new SSTSA();
            STSA->array = array;
            STSA->minElements = minElements;
            STSA->numOfThreads = numOfThreads;
            STSA->threadIndex = i;
            STSA->startingPoint = startingPoint;
            STSA->minimalElement = minimalElement;
            STSA->endPoint = endpoint;
            hThreads[i] = (HANDLE)_beginthreadex(&sa, 4096, &sThread, (PVOID)STSA, CREATE_SUSPENDED, &ThreadID);
            endpoint += numOfIterations;
            startingPoint += numOfIterations;
            break;
        }
        default:;
        }
        if(hThreads[i] == INVALID_HANDLE_VALUE) {
              std::cerr << "Invalid handle value" << std::endl;
           return;
        }
        //delete[] STSA->array;
        //delete[] STSA->minElements;
        //delete STSA;
        ui->availableThreads->setItem(i,0,new QTableWidgetItem(QString::number(ThreadID)));
        ui->availableThreads->setItem(i,1,new QTableWidgetItem(priority(GetThreadPriority(hThreads[i]))));
        ui->availableThreads->setItem(i,2,new QTableWidgetItem("Suspended"));
    }
}

unsigned int _stdcall dThread(LPVOID arg)
{
    SDTSA* DTSA = (SDTSA*)arg;
    std::fstream fRes;
    std::string filePath = PATH + "studentData.txt";
    fRes.open(filePath, std::ios::out| std::ios::app);
    if(fRes.is_open())
    {
        for(int i = 0;i < (DTSA->numberOfIterations); ++i){
            fRes << SSTUDENTDATA << " " << GetCurrentThreadId() << std::endl;
        }
        fRes.close();
    }else{
        std::cerr << "Error: file could not be opened" << std::endl;
        return 1;
    }
    //delete DTSA;
    return 0;
}

unsigned int _stdcall tThread(LPVOID arg){
    STTSA* TTSA = (STTSA*)arg;
    std::fstream fRes;
    std::string filePath = PATH + "tabulation.txt";
    fRes.open(filePath, std::ios::out | std::ios::app);
    if(fRes.is_open())
    {
        double A = TTSA->ABorder, B = TTSA->BBorder, steps_quantity = TTSA->numberOfIterations;
        double eps = 0.001, total{0}, holder{1};
        for(double x = A, step = (B-A)/steps_quantity; x < B; x+= step)
        {
            int sign_ind = 0;
            holder = 1;
            total = 0;
            while(fabs(holder) >= eps)
            {
                total += pow(-1, sign_ind)*holder;
                sign_ind++;
                holder = pow(x, sign_ind);
            }
            fRes << x << ' ' << total << ' ' << pow(1+x, -1) << std::endl;
        }
        fRes.close();
        //delete TTSA;
    }else{
        std::cerr << "Error: file could not be opened" << std::endl;
        return 1;
    }
    return 0;
}

unsigned int _stdcall sThread(LPVOID arg){
    SSTSA* STSA = (SSTSA*)arg;
    std::fstream fRes;
    std::string filePath = PATH + "array.txt";
    fRes.open(filePath, std::ios::out | std::ios::app);
    STSA->minElements[STSA->threadIndex] = STSA->array[STSA->startingPoint];

    for(int i = STSA->startingPoint + 1; i < STSA->endPoint; i++){
        if(STSA->minElements[STSA->threadIndex] > STSA->array[i])
            STSA->minElements[STSA->threadIndex] = STSA->array[i];
    }
    for(int i = STSA->startingPoint; i < STSA->endPoint; i++){
        fRes << STSA->array[i] << std::endl;
    }
    STSA->minimalElement = &STSA->minElements[STSA->threadIndex];

    for (int k = STSA->threadIndex - 1; k >= 0; k--){
        if(*STSA->minimalElement > STSA->minElements[k])
            STSA->minimalElement = &STSA->minElements[k];
    }
    qDebug() << *STSA->minimalElement << " in threads 0-" << STSA->threadIndex;
    fRes.close();
    return 0;
}

QString UiControl::priority(int code){
    switch (code)
    {
        case 1: return "Above normal";
        case -1: return "Below normal";
        case 0: return "Normal";
        case -15: return "Idle";
        case 2: return "High";
    default: return "";
    }
}

void UiControl::on_runall_clicked()
{
    QElapsedTimer timer;
    timer.start();

    for(int i = 0; i < numOfThreads; ++i){
        ui->availableThreads->item(i, 2)->setText("Running");
        ResumeThread(hThreads[i]);
        WaitForSingleObject(hThreads[i], INFINITE);
    }
    timer.nsecsElapsed();
    ui->time->setText(QString::number(timer.nsecsElapsed()/1000000.0) + " ms");

    for(int i = 0; i < numOfThreads; ++i){
         ui->availableThreads->item(i, 2)->setText("Finished");
    }
}
void UiControl::on_runwithouttime_clicked()
{
    for(int i = 0; i < numOfThreads; ++i){
        ui->availableThreads->item(i, 2)->setText("Running");
        ResumeThread(hThreads[i]);
    }
}

void UiControl::on_run_clicked()
{
    ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 2)->setText("Running");
    qApp->processEvents();
    ResumeThread(hThreads[ui->availableThreads->currentIndex().row()]);
    WaitForSingleObject(hThreads[ui->availableThreads->currentIndex().row()], INFINITE);
    ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 2)->setText("Finnished");
    ui->availableThreads->item(ui->availableThreads->currentIndex().row(),1)->setText(priority(GetPriorityClass(hThreads[ui->availableThreads->currentIndex().row()])));

}

void UiControl::on_Pause_clicked()
{
    ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 2)->setText("Suspended");
     SuspendThread(hThreads[ui->availableThreads->currentIndex().row()]);
}

void UiControl::on_killThread_clicked()
{
    TerminateProcess(hThreads[ui->availableThreads->currentIndex().row()],0);
    ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 2)->setText("Terminated");
}

void UiControl::on_priority_currentIndexChanged(int index)
{
    switch(index){
    case 0: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()],-15);
            ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("Idle");
            break;}
    case 1: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()], -1);
         ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("Below normal");
            break;}
    case 2: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()], 0);
         ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("Normal");
            break;}
    case 3: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()], 1);
        ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("Above normal");
            break;}
    case 4: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()], 2);
         ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("High");
            break;}
    case 5: {
        SetThreadPriority(hThreads[ui->availableThreads->currentIndex().row()], 15);
        ui->availableThreads->item(ui->availableThreads->currentIndex().row(), 1)->setText("Real time");
            break;}
    }
}
