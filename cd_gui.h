#ifndef CD_GUI_H
#define CD_GUI_H

#include <QMainWindow>
#include <QString>

//library for loading text file
#include <fstream>
#include <string>

//library for the leading 0 (01:02:03)
#include <iomanip>

//using default namespace without typing std::cout
#include <iostream>
using namespace std;

namespace Ui {
class cd_gui;
}

class cd_gui : public QMainWindow
{
    Q_OBJECT

public:
    explicit cd_gui(QWidget *parent = 0);
    ~cd_gui();

private slots:
    void on_lang_clicked();
    void on_speedSet_clicked();
    void on_exit_clicked();
    void on_add1Day_clicked();
    void on_sub1Day_clicked();
    void on_add1Hour_clicked();
    void on_sub1Hour_clicked();
    void on_add1Minute_clicked();
    void on_sub1Minute_clicked();
    void on_add1Second_clicked();
    void on_sub1Second_clicked();
    void on_startTime_clicked();
    void on_stopTime_clicked();
    void on_resetTime_clicked();
    void on_add10Time_clicked();
    void resetTime();
    void updateDisplay();
    void updateTime();
    void disableSettings();
    void enableSettings();
    void updateSpeed();
    void configButtonOnSpeed();
    void configTimex1();
    void configTimex10();
    void printTimeToConsole();
    QString loadTimeOutMsg();
    int loadTimeRemain();
    void saveTimeRemain();

private:
    Ui::cd_gui *ui;
    QTimer *timer; //declare timer for update Timer
    int t; //declare total time
    int speed = 1000; //declare default Timer speed
    short int days,hours,minutes,seconds; //declare variable to display (easier to see when coding)
    char colon = ':'; //blinking colon
    QString twoZero = "00";
    QString timeOutMsg; //message when timeout
    bool x10mode = 0; //variable for add 10 mode
    bool red = 0; //change to red color when timeout
    bool vn = 0; //change language
    bool sp = 0; //switch to speed settings
};

#endif // CD_GUI_H
