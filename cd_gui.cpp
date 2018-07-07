#include "cd_gui.h"
#include "ui_cd_gui.h"
#include <QTimer>

cd_gui::cd_gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cd_gui)
{
    ui->setupUi(this);
    ui->stopTime->setEnabled(0); //disable stop button in startup
    resetTime(); //reset Timer
    updateDisplay(); //update display
}

cd_gui::~cd_gui()
{
    delete ui;
}

//action for language selection button
void cd_gui::on_lang_clicked(){
    if (!vn){
        ui->lang->setText("VN");
        vn = 1;
        ui->startTime->setText("CHẠY");
        ui->stopTime->setText("DỪNG");
        ui->resetTime->setText("GỐC");
        //change title language when Timer is still running
        if (t > 0){
            ui->title->setText("Đồng hồ đếm lùi");
        }
    } else {
        ui->lang->setText("EN");
        vn = 0;
        ui->startTime->setText("START");
        ui->stopTime->setText("STOP");
        ui->resetTime->setText("RESET");
        //change title language when Timer is still running
        if (t > 0){
            ui->title->setText("Countdown Timer");
        }
    }
}

//action for speed settings button
void cd_gui::on_speedSet_clicked(){
    if (!sp){
        sp = 1;

        //configure buttons for speed settings
        disableSettings();
        ui->stopTime->setEnabled(0);
        ui->resetTime->setEnabled(1);
        configButtonOnSpeed();

        //change display to speed settings
        ui->days->display("SP");
        ui->hours->display("D ");
        ui->colon1->setText("");
        ui->colon2->setText("");

    } else {
        //switch back to normal
        sp = 0;
        ui->colon1->setText(":");
        ui->colon2->setText(":");
        updateDisplay();
        enableSettings();
    }
}

//action for x button
void cd_gui::on_exit_clicked()
{
    qApp->quit(); //quit
}

//action for day settings
void cd_gui::on_add1Day_clicked(){
    if (!x10mode){ //check for x10 mode
        if (checkValueForx1(days, 99)){ //check days in x1 mode
            days = days + 1;
            updateDisplay();
            ui->sub1Day->setEnabled(1); //turn on button when days is 0 < days < 99
        }
        if (days >= 99){
            ui->add1Day->setEnabled(0); //turn off button when days >= 99
        }
    } else {
        if (checkValueForx10Add(days, 89)){ //check value in x10 mode
            days = days + 10;
            updateDisplay();
            ui->sub1Day->setEnabled(1);
        }
        if (days >= 90){
            ui->add1Day->setEnabled(0); //turn off button when days >= 90
        }
    }
}
void cd_gui::on_sub1Day_clicked(){
    if (!x10mode){ //check for x10 mode
        if (checkValueForx1(days,99)){ //check value in x1 mode
            days = days - 1;
            updateDisplay();
            ui->add1Day->setEnabled(1); //turn on button when days is 0 < days < 99
        }
        if (days<=0){
            ui->sub1Day->setEnabled(0); //turn off button when days <= 0
        }
    } else {
        if (checkValueForx10Sub(days, 99)){ //check value in x10 mode
            days = days - 10;
            updateDisplay();
            ui->add1Day->setEnabled(1);
        }
        if (days<10){
            ui->sub1Day->setEnabled(0); //turn off button when days < 10
        }
        }
}

//action for hour settings
void cd_gui::on_add1Hour_clicked(){
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(hours, 23)){ //check hours in x1 mode
            hours = hours + 1;
            updateDisplay();
            ui->sub1Hour->setEnabled(1); //turn on button when hours is 0 < hours < 23
        }
        if (hours >= 23){
            ui->add1Hour->setEnabled(0); //turn off button when hours >= 23
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Add(hours, 19)){ //check value in x10 mode
            hours = hours + 10;
            updateDisplay();
            ui->sub1Hour->setEnabled(1);
        }
        if (hours >= 20){
            ui->add1Hour->setEnabled(0); //turn off button when hours >= 20
        }
    }

    //this is for speed settings: +10s
    if (sp){
        speed = speed + 10000;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Hour_clicked(){
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(hours,23)){ //check value in x1 mode
            hours = hours - 1;
            updateDisplay();
            ui->add1Hour->setEnabled(1); //turn on button when hours is 0 < hours < 23
        }
        if (hours<=0){
            ui->sub1Hour->setEnabled(0); //turn off button when hours <= 0
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Sub(hours, 23)){ //check value in x10 mode
            hours = hours - 10;
            updateDisplay();
            ui->add1Hour->setEnabled(1);
        }
        if (hours<10){
            ui->sub1Hour->setEnabled(0); //turn off button when hours < 10
        }
    }

    //this is for speed settings: -10s
    if (sp){
        speed = speed - 10000;
        configButtonOnSpeed();
    }
}

//action for minute settings
void cd_gui::on_add1Minute_clicked(){ //when +1 minute is clicked
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(minutes, 59)){ //check minutes in x1 mode
            minutes = minutes + 1;
            updateDisplay();
            ui->sub1Minute->setEnabled(1); //turn on button when minutes is 0 < minutes < 59
        }
        if (minutes >= 59){
            ui->add1Minute->setEnabled(0); //turn off button when minutes >= 59
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Add(minutes, 49)){ //check value in x10 mode
            minutes = minutes + 10;
            updateDisplay();
            ui->sub1Minute->setEnabled(1);
        }
        if (minutes >= 50){
            ui->add1Minute->setEnabled(0); //turn off button when minutes >= 50
        }
    }

    //this is for speed settings: +1s
    if (sp){
        speed = speed + 1000;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Minute_clicked(){ //when -1 minute is clicked
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(minutes,59)){ //check value in x1 mode
            minutes = minutes - 1;
            updateDisplay();
            ui->add1Minute->setEnabled(1); //turn on button when minutes is 0 < minutes < 59
        }
        if (minutes<=0){
            ui->sub1Minute->setEnabled(0); //turn off button when minutes <= 0
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Sub(minutes, 59)){ //check value in x10 mode
            minutes = minutes - 10;
            updateDisplay();
            ui->add1Minute->setEnabled(1);
        }
        if (minutes<10){
            ui->sub1Minute->setEnabled(0); //turn off button when minutes < 10
        }
    }

    //this is for speed settings: -1s
    if (sp){
        speed = speed - 1000;
        configButtonOnSpeed();
    }
}

//action for second settings
void cd_gui::on_add1Second_clicked(){ //when +1 second is clicked
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(seconds, 59)){ //check seconds in x1 mode
            seconds = seconds + 1;
            updateDisplay();
            ui->sub1Second->setEnabled(1); //turn on button when seconds is 0 < seconds < 59
        }
        if (seconds >= 59){
            ui->add1Second->setEnabled(0); //turn off button when seconds >= 59
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Add(seconds, 49)){ //check value in x10 mode
            seconds = seconds + 10;
            updateDisplay();
            ui->sub1Second->setEnabled(1);
        }
        if (seconds >= 50){
            ui->add1Second->setEnabled(0); //turn off button when seconds >= 50
        }
    }

    //this is for speed settings: +100ms
    if (sp){
        speed = speed + 100;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Second_clicked(){ //when -1 second is clicked
    if (!x10mode && !sp){ //check for x10 mode
        if (checkValueForx1(seconds,59)){ //check value in x1 mode
            seconds = seconds - 1;
            updateDisplay();
            ui->add1Second->setEnabled(1); //turn on button when seconds is 0 < seconds < 59
        }
        if (seconds<=0){
            ui->sub1Second->setEnabled(0); //turn off button when seconds <= 0
        }
    } else if (x10mode && !sp){
        if (checkValueForx10Sub(seconds, 59)){ //check value in x10 mode
            seconds = seconds - 10;
            updateDisplay();
            ui->add1Second->setEnabled(1);
        }
        if (seconds<10){
            ui->sub1Second->setEnabled(0); //turn off button when seconds < 10
        }
    }

    //this is for speed settings: -100ms
    if (sp){
        speed = speed - 100;
        configButtonOnSpeed();
    }
}

//action for start button
void cd_gui::on_startTime_clicked(){
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(speed);
    disableSettings();
    //disable speed settings
    ui->speedSet->setEnabled(0);
}

//action for stop button
void cd_gui::on_stopTime_clicked(){
    timer->stop();
    enableSettings();
    ui->title->setStyleSheet("QLabel { color : black; }");
    if (!vn){
        ui->title->setText("Countdown Timer");
    } else {
        ui->title->setText("Đồng hồ đếm lùi");
    }
}

//action for reset button
void cd_gui::on_resetTime_clicked(){
    if (!sp){
        resetTime();
        updateDisplay();
    } else {
        speed = 1000;
        configButtonOnSpeed();
    }
}

//action for x10 mode button
void cd_gui::on_add10Time_clicked(){
    if (!x10mode){ //check for x10 mode
        x10mode = 1; //switch to x10 if previously mode was x1
        ui->add10Time->setText("+10"); //change label

        //check if days, hours, minutes, seconds are out of range in x10 mode
        if (days > 0 && days < 10){
            ui->sub1Day->setEnabled(0);
        }
        if (days >= 90){
            ui->add1Day->setEnabled(0);
        }
        if (hours > 0 && hours < 10){
            ui->sub1Hour->setEnabled(0);
        }
        if (hours >= 20){
            ui->add1Hour->setEnabled(0);
        }
        if (minutes > 0 && minutes < 10){
            ui->sub1Minute->setEnabled(0);
        }
        if (minutes >= 50){
            ui->add1Minute->setEnabled(0);
        }
        if (seconds > 0 && seconds < 10){
            ui->sub1Second->setEnabled(0);
        }
        if (seconds >= 50){
            ui->add1Second->setEnabled(0);
        }

    } else {
        x10mode = 0; //switch to x1 if previously mode was x10
        ui->add10Time->setText("+1"); //change label

        //check if days, hours, minutes, seconds are out of range in x1 mode
        if (checkValueSwitchMode(days, 90, 99)){
            ui->add1Day->setEnabled(1);
            ui->sub1Day->setEnabled(1);
        }
        if (checkValueSwitchMode(hours, 20, 23)){
            ui->add1Hour->setEnabled(1);
            ui->sub1Hour->setEnabled(1);
        }
        if (checkValueSwitchMode(minutes, 50, 59)){
            ui->add1Minute->setEnabled(1);
            ui->sub1Minute->setEnabled(1);
        }
        if (checkValueSwitchMode(seconds, 50, 59)){
            ui->add1Second->setEnabled(1);
            ui->sub1Second->setEnabled(1);
        }

    }
}

//reset Timer
void cd_gui::resetTime(){
    t = 60;
    days = 0;
    hours = 0;
    minutes = 1;
    seconds = 0;
    ui->title->setStyleSheet("QLabel { color : black; }");
    if (!vn){
        ui->title->setText("Countdown Timer");
    } else {
        ui->title->setText("Đồng hồ đếm lùi");
    }
    ui->add1Day->setEnabled(1);
    ui->sub1Day->setEnabled(0);
    ui->add1Hour->setEnabled(1);
    ui->sub1Hour->setEnabled(0);
    // +/- 1 minute doesn't need to be adjusted because default is 0 day and 1 minute
    ui->add1Second->setEnabled(1);
    ui->sub1Second->setEnabled(0);
}

//update display
void cd_gui::updateDisplay(){
    ui->days->display(days);
    ui->hours->display(hours);
    ui->minutes->display(minutes);
    ui->seconds->display(seconds);
}

//check condition if time value is within range
bool cd_gui::checkValueForx1(short int x,short int y){ // this one for x1 mode
    return (x >= 0 && x <= y);
}
bool cd_gui::checkValueForx10Add(short int x, short int y){ //this one for x10 mode add
    return (x >= 0 && x <= y);
}
bool cd_gui::checkValueForx10Sub(short int x, short int y){ //this one for x10 mode subtract
    return (x >= 10 && x <= y);
}
bool cd_gui::checkValueSwitchMode(short int x, short int y, short int z){ //check value whenever x10 -> x1
    return ((x > 0 && x < 10) || (x >= y && x < z));
}

//update Timer
void cd_gui::updateTime(){
    //change all values to lowest unit
    t = days*86400 + hours*3600 + minutes*60 + seconds;
    //remove 1 unit every second
    t = t - 1;
    if (t > 0){
        //this is for blinking colon
        if (t % 2 == 1){
            ui->colon1->setText("");
            ui->colon2->setText("");
            ui->colon3->setText("");
        } else {
            ui->colon1->setText(":");
            ui->colon2->setText(":");
            ui->colon3->setText(":");
        }
        //change back to display value
        days = t / 86400;
        hours = t % 86400 / 3600;
        minutes = t % 3600 / 60;
        seconds = t % 60;
        updateDisplay();
    } else { //time's up
        seconds = 0;
        updateDisplay();
        ui->colon1->setText(":");
        ui->colon2->setText(":");
        ui->colon3->setText(":");
        //display timeout message
        ui->title->setText("Insert message here");
        //change color to emphasize message
        if (!red){
            ui->title->setStyleSheet("QLabel { color : red; }");
            red = 1;
        } else {
            ui->title->setStyleSheet("QLabel { color : blue; }");
            red = 0;
        }
    }
}

//disable buttons while Timer is running
void cd_gui::disableSettings(){
    //buttons for adjusting Timer
    ui->add1Day->setEnabled(0);
    ui->sub1Day->setEnabled(0);
    ui->add1Hour->setEnabled(0);
    ui->sub1Hour->setEnabled(0);
    ui->add1Minute->setEnabled(0);
    ui->sub1Minute->setEnabled(0);
    ui->add1Second->setEnabled(0);
    ui->sub1Second->setEnabled(0);
    //menu below
    ui->startTime->setEnabled(0);
    ui->stopTime->setEnabled(1);
    ui->resetTime->setEnabled(0);
    ui->add10Time->setEnabled(0);
}

//enable buttons when Timer is stopped
void cd_gui::enableSettings(){
    //reopen speed settings
    ui->speedSet->setEnabled(1);
    //display colon
    ui->colon1->setText(":");
    ui->colon2->setText(":");
    ui->colon3->setText(":");
    //buttons for adjusting Timer
    ui->add1Day->setEnabled(1);
    ui->sub1Day->setEnabled(1);
    ui->add1Hour->setEnabled(1);
    ui->sub1Hour->setEnabled(1);
    ui->add1Minute->setEnabled(1);
    ui->sub1Minute->setEnabled(1);
    ui->add1Second->setEnabled(1);
    ui->sub1Second->setEnabled(1);

    if (!x10mode){

        //condition for x1 mode. Note if any value = limit, the +/- will be disabled accordingly
        //days
        if (days >= 99) {ui->add1Day->setEnabled(0);}
        if (days <= 0) {ui->sub1Day->setEnabled(0);}
        //hours
        if (hours >= 23) {ui->add1Hour->setEnabled(0);}
        if (hours <= 0) {ui->sub1Hour->setEnabled(0);}
        //minutes
        if (minutes >= 59) {ui->add1Minute->setEnabled(0);}
        if (minutes <= 0) {ui->sub1Minute->setEnabled(0);}
        //seconds
        if (seconds >= 59) {ui->add1Second->setEnabled(0);}
        if (seconds <= 0) {ui->sub1Second->setEnabled(0);}

    } else {

        //condition for x10 mode. Note if any value = limit, the +/- will be disabled accordingly
        //days
        if (days >= 90) {ui->add1Day->setEnabled(0);}
        if (days < 10) {ui->sub1Day->setEnabled(0);}
        //hours
        if (hours >= 20) {ui->add1Hour->setEnabled(0);}
        if (hours < 10) {ui->sub1Hour->setEnabled(0);}
        //minutes
        if (minutes >= 50) {ui->add1Minute->setEnabled(0);}
        if (minutes < 10) {ui->sub1Minute->setEnabled(0);}
        //seconds
        if (seconds >= 50) {ui->add1Second->setEnabled(0);}
        if (seconds < 10) {ui->sub1Second->setEnabled(0);}

    }

    //menu below
    ui->startTime->setEnabled(1);
    ui->stopTime->setEnabled(0);
    ui->resetTime->setEnabled(1);
    ui->add10Time->setEnabled(1);
}

//update Timer spped to display
void cd_gui::updateSpeed(){
    ui->minutes->display(speed / 1000);
    ui->seconds->display(speed % 1000 / 10);
}

//button config: hour for +10 seconds, minute for +1 second, second for +100 msec
void cd_gui::configButtonOnSpeed(){
    ui->add1Hour->setEnabled(1);
    ui->add1Minute->setEnabled(1);
    ui->add1Second->setEnabled(1);
    if (speed > 100){
        ui->sub1Second->setEnabled(1);
    } else {
        ui->sub1Second->setEnabled(0);
    }
    if (speed > 1000){
        ui->sub1Minute->setEnabled(1);
    } else {
        ui->sub1Minute->setEnabled(0);
    }
    if (speed > 9900){
        ui->sub1Hour->setEnabled(1);
    } else {
        ui->sub1Hour->setEnabled(0);
    }
    if (speed >= 60000){
        ui->add1Second->setEnabled(0);
    }
    if (speed > 59000){
        ui->add1Minute->setEnabled(0);
    }
    if (speed > 50000){
        ui->add1Hour->setEnabled(0);
    }
    updateSpeed();
}
