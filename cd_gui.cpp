#include "cd_gui.h"
#include "ui_cd_gui.h"
#include "blink-led.h"
#include <QTimer>

cd_gui::cd_gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cd_gui)
{
    ui->setupUi(this);
    ui->stopTime->setEnabled(0); //disable stop button in startup
    resetTime(); //reset Timer
    t = loadTimeRemain(); //load previous time value
    updateDisplay(); //update display

    //initialize timer to read button status
    button = new QTimer(this);
    connect(button, SIGNAL(timeout()), this, SLOT(buttonRead()));
    button->start(180);
    //setup for LEDs and buttons
    initial_iosetup();

    //initialize timer for countdown (not active)
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    //load timeout message to program
    timeOutMsg = loadTimeOutMsg();
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
        cout << "Da doi ngon ngu" << endl;
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
        cout << "Language switched" << endl;
    }
}

//action for speed settings button
void cd_gui::on_speedSet_clicked(){
    if (!sp){
        sp = 1;
        //set speed settings for buttons
        funcButton = 4;
        ui->fswitch0->setEnabled(1);
        ui->fswitch1->setEnabled(0);
        ui->fswitch2->setEnabled(0);
        ui->fswitch3->setEnabled(0);

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
        //set speed settings for buttons
        funcButton = 0;
        ui->fswitch0->setEnabled(1);
        ui->fswitch1->setEnabled(0);
        ui->fswitch2->setEnabled(0);
        ui->fswitch3->setEnabled(0);

        ui->colon1->setText(":");
        ui->colon2->setText(":");
        updateDisplay();
        enableSettings();
    }
}

//action for x button
void cd_gui::on_exit_clicked()
{
    saveTimeRemain();
    qApp->quit(); //quit
    unmap_io();
    cout << "." << endl;
}

//action for day settings
void cd_gui::on_add1Day_clicked(){ //when +1 day is clicked
    if (!x10mode){ //check for x10 mode
        t = t + 86400;
        configTimex1();
    } else {
        t = t + 864000;
        configTimex10();
    }
}
void cd_gui::on_sub1Day_clicked(){ //when -1 day is clicked
    if (!x10mode){ //check for x10 mode
        t = t - 86400;
        configTimex1();
    } else {
        t = t - 864000;
        configTimex10();
    }
}

//action for hour settings
void cd_gui::on_add1Hour_clicked(){ //when +1 hour is clicked
    if (!x10mode && !sp){ //check for x10 mode
        t = t + 3600;
        configTimex1();
    } else if (x10mode && !sp){
        t = t + 36000;
        configTimex10();
    }

    //this is for speed settings: +10s
    if (sp){
        speed = speed + 10000;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Hour_clicked(){ //when -1 hour is clicked
    if (!x10mode && !sp){ //check for x10 mode
        t = t - 3600;
        configTimex1();
    } else if (x10mode && !sp){
        t = t - 36000;
        configTimex10();
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
        t = t + 60;
        configTimex1();
    } else if (x10mode && !sp){
        t = t + 600;
        configTimex10();
    }

    //this is for speed settings: +1s
    if (sp){
        speed = speed + 1000;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Minute_clicked(){ //when -1 minute is clicked
    if (!x10mode && !sp){ //check for x10 mode
        t = t - 60;
        configTimex1();
    } else if (x10mode && !sp){
        t = t - 600;
        configTimex10();
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
        t = t + 1;
        configTimex1();
    } else if (x10mode && !sp){
        t = t + 10;
        configTimex10();
    }

    //this is for speed settings: +100ms
    if (sp){
        speed = speed + 100;
        configButtonOnSpeed();
    }
}
void cd_gui::on_sub1Second_clicked(){ //when -1 second is clicked
    if (!x10mode && !sp){ //check for x10 mode
        t = t - 1;
        configTimex1();
    } else if (x10mode && !sp){
        t = t - 10;
        configTimex10();
    }

    //this is for speed settings: -100ms
    if (sp){
        speed = speed - 100;
        configButtonOnSpeed();
    }
}

//action for start button
void cd_gui::on_startTime_clicked(){
    timer->start(speed);
    ledTimeChange();
    printTimeToConsole();
    disableSettings();
    //disable speed settings
    ui->speedSet->setEnabled(0);
}

//action for stop button
void cd_gui::on_stopTime_clicked(){
    timer->stop();
    if (!vn){
        cout << "\nTime stopped." << endl;
    } else {
        cout << "\nDung dong ho." << endl;
    }
    saveTimeRemain();
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
        saveTimeRemain();
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
        configTimex10();

    } else {
        x10mode = 0; //switch to x1 if previously mode was x10
        ui->add10Time->setText("+1"); //change label

        //check if days, hours, minutes, seconds are out of range in x1 mode
        configTimex1();

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
    //reset buttons according to x1 or x10
    if (!sp){
        configTimex1();
    } else {
        configTimex10();
    }
}

//update time on display
void cd_gui::updateDisplay(){
    //change to display value
    days = t / 86400;
    hours = t % 86400 / 3600;
    minutes = t % 3600 / 60;
    seconds = t % 60;
    ui->days->display(days);

    if (hours < 10){
        twoZero[1]= '0' + hours;
        ui->hours->display(twoZero);
        twoZero[1]= '0';
    } else {
        ui->hours->display(hours);
    }

    if (minutes < 10){
        twoZero[1]= '0' + minutes;
        ui->minutes->display(twoZero);
        twoZero[1]= '0';
    } else {
        ui->minutes->display(minutes);
    }

    if (seconds < 10){
        twoZero[1]= '0' + seconds;
        ui->seconds->display(twoZero);
        twoZero[1]= '0';
    } else {
        ui->seconds->display(seconds);
    }
}

//update Timer
void cd_gui::updateTime(){
    //remove 1 unit every second
    t = t - 1;
    if (t > 0){
        //this is for blinking colon
        if (t % 2 == 1){
            colon = ' ';
            ui->colon1->setText("");
            ui->colon2->setText("");
            ui->colon3->setText("");
        } else {
            colon = ':';
            ui->colon1->setText(":");
            ui->colon2->setText(":");
            ui->colon3->setText(":");
        }
        updateDisplay();
        printTimeToConsole();
        ledTimeChange();
    } else { //time's up
        t = 0;
        updateDisplay();
        printTimeToConsole();
        ledTimeChange();
        ui->colon1->setText(":");
        ui->colon2->setText(":");
        ui->colon3->setText(":");
        //display timeout message
        ui->title->setText(timeOutMsg);
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

    if (!x10mode){
        //condition for x1 mode. Note if any value = limit, the +/- will be disabled accordingly
        configTimex1();
    } else {
        //condition for x10 mode. Note if any value = limit, the +/- will be disabled accordingly
        configTimex10();
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

//button config for speed settings: hour for +10 seconds, minute for +1 second, second for +100 msec
void cd_gui::configButtonOnSpeed(){
    cout << speed << "ms/tick" << endl;
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

//button config for time settings in x1 mode
void cd_gui::configTimex1(){
    cout << "t= " << t << endl;

    ui->add1Day->setEnabled(1);
    ui->add1Hour->setEnabled(1);
    ui->add1Minute->setEnabled(1);
    ui->add1Second->setEnabled(1);
    if (t > 0){
        ui->sub1Second->setEnabled(1);
    } else {
        ui->sub1Second->setEnabled(0);
    }
    if (t >= 60){
        ui->sub1Minute->setEnabled(1);
    } else {
        ui->sub1Minute->setEnabled(0);
    }
    if (t >= 3600){
        ui->sub1Hour->setEnabled(1);
    } else {
        ui->sub1Hour->setEnabled(0);
    }
    //if we reach 1 day
    if (t >= 86400){
        ui->sub1Day->setEnabled(1);
    } else {
        ui->sub1Day->setEnabled(0);
    }
    //if we reach 99 days 0:00:00
    if (t >= 8553600){
        ui->add1Day->setEnabled(0);
    }
    //if we reach 99 days 23:00:00
    if (t >= 8636400){
        ui->add1Hour->setEnabled(0);
    }
    //if we reach 99 days 23:59:00
    if (t >= 8639940){
        ui->add1Minute->setEnabled(0);
    }
    //if we reach 99 days 23:59:59
    if (t >= 8639999){
        ui->add1Second->setEnabled(0);
    }
    updateDisplay();
}

//button config for time settings x10 mode
void cd_gui::configTimex10(){
    cout << "[x10 mode] t= " << t << endl;

    ui->add1Day->setEnabled(1);
    ui->add1Hour->setEnabled(1);
    ui->add1Minute->setEnabled(1);
    ui->add1Second->setEnabled(1);
    if (t >= 10){
        ui->sub1Second->setEnabled(1);
    } else {
        ui->sub1Second->setEnabled(0);
    }
    //if we reach 0 day 0:10:00
    if (t >= 600){
        ui->sub1Minute->setEnabled(1);
    } else {
        ui->sub1Minute->setEnabled(0);
    }
    //if we reach 0 day 10:00:00
    if (t >= 36000){
        ui->sub1Hour->setEnabled(1);
    } else {
        ui->sub1Hour->setEnabled(0);
    }
    //if we reach 10 days 0:00:00
    if (t >= 864000){
        ui->sub1Day->setEnabled(1);
    } else {
        ui->sub1Day->setEnabled(0);
    }
    //if we reach 90 days 0:00:00
    if (t >= 7776000){
        ui->add1Day->setEnabled(0);
    }
    //if we reach 99 days 14:00:00
    if (t >= 8604000){
        ui->add1Hour->setEnabled(0);
    }
    //if we reach 99 days 23:50:00
    if (t >= 8639400){
        ui->add1Minute->setEnabled(0);
    }
    //if we reach 99 days 23:59:50
    if (t >= 8639990){
        ui->add1Second->setEnabled(0);
    }
    updateDisplay();
}

//output time remaining to console
void cd_gui::printTimeToConsole(){
    //this is for blinking colon
    if (t % 2 == 1){
        colon = ' ';
    } else {
        colon = ':';
    }
    cout << "Time Remaining " << days << "d " << setfill('0') << setw(2) << hours << colon << setw(2) << minutes << colon << setw(2) << seconds << "\r" << flush;
}

//change LED status in countdown
void cd_gui::ledTimeChange(){
    if (t % 5 == 4){
        gpio_set(GPIO_MODULE1, USR0_LED_D1);
        gpio_set(GPIO_MODULE1, USR1_LED_D2);
        gpio_set(GPIO_MODULE1, USR2_LED_D3);
        gpio_set(GPIO_MODULE1, USR3_LED_D4);
    }
    if (t % 5 == 3){
        gpio_set(GPIO_MODULE1, USR0_LED_D1);
        gpio_set(GPIO_MODULE1, USR1_LED_D2);
        gpio_set(GPIO_MODULE1, USR2_LED_D3);
        gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    }
    if (t % 5 == 2){
        gpio_set(GPIO_MODULE1, USR0_LED_D1);
        gpio_set(GPIO_MODULE1, USR1_LED_D2);
        gpio_reset(GPIO_MODULE1, USR2_LED_D3);
        gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    }
    if (t % 5 == 1){
        gpio_set(GPIO_MODULE1, USR0_LED_D1);
        gpio_reset(GPIO_MODULE1, USR1_LED_D2);
        gpio_reset(GPIO_MODULE1, USR2_LED_D3);
        gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    }
    if (t % 5 == 0){
        gpio_reset(GPIO_MODULE1, USR0_LED_D1);
        gpio_reset(GPIO_MODULE1, USR1_LED_D2);
        gpio_reset(GPIO_MODULE1, USR2_LED_D3);
        gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    }
}

//read buttons
void cd_gui::buttonRead(){
    //start and stop countdown
    if ( gpio_read(GPIO_MODULE2, IN, USR0_KEY_SW1) ){
        if (!timer->isActive() && !sp){
            on_startTime_clicked();
        } else {
            on_stopTime_clicked();
        }
     //reset speed when in speed settings
        if (!timer->isActive() && sp){
            speed = 1000;
            configButtonOnSpeed();
        }
    }

    if ( gpio_read(GPIO_MODULE2, IN, USR1_KEY_SW2) )
        if (!timer->isActive()){
            if (funcButton == 0 || funcButton == 4){on_add1Second_clicked();}
            if (funcButton == 1 || funcButton == 5){on_add1Minute_clicked();}
            if (funcButton == 2 || funcButton == 6){on_add1Hour_clicked();}
            //1Day button only works in time settings
            if (funcButton == 3 && !sp){on_add1Day_clicked();}
            if (t >= 8640000){
                t = 8639999;
                cout << "Reset t= " << t << endl;
                updateDisplay();
            }
            if (speed > 60000 && sp){
                speed = 60000;
                cout << "Reset speed= " << speed << endl;
                updateSpeed();
            }
        }

    if ( gpio_read(GPIO_MODULE0, IN, USR2_KEY_SW3) )
        if (!timer->isActive()){
            if (funcButton == 0 || funcButton == 4){on_sub1Second_clicked();}
            if (funcButton == 1 || funcButton == 5){on_sub1Minute_clicked();}
            if (funcButton == 2 || funcButton == 6){on_sub1Hour_clicked();}
            //1Day button only works in time settings
            if (funcButton == 3 && !sp){on_sub1Day_clicked();}
            if (t < 0){
                t = 0;
                cout << "Reset t= " << t << endl;
                updateDisplay();
            }
            if (speed < 100 && sp){
                speed = 100;
                cout << "Reset speed= " << speed << endl;
                updateSpeed();
            }
        }

    /*change to settings for seconds(0),minutes(1),hours(2),days(3),
     * speed +/-0.1s(4),1s(5),10s(6)
     */
    if ( gpio_read(GPIO_MODULE2, IN, USR3_KEY_SW4) )
        if (!timer->isActive()){
            if (funcButton == 0){
                funcButton = 1;
                ui->fswitch0->setEnabled(0);
                ui->fswitch1->setEnabled(1);
                ui->fswitch2->setEnabled(0);
                ui->fswitch3->setEnabled(0);
            } else if (funcButton == 1){
                funcButton = 2;
                ui->fswitch0->setEnabled(0);
                ui->fswitch1->setEnabled(0);
                ui->fswitch2->setEnabled(1);
                ui->fswitch3->setEnabled(0);
            } else if (funcButton == 2){
                funcButton = 3;
                ui->fswitch0->setEnabled(0);
                ui->fswitch1->setEnabled(0);
                ui->fswitch2->setEnabled(0);
                ui->fswitch3->setEnabled(1);
            } else if (funcButton == 3){
                on_speedSet_clicked();
            } else if (funcButton == 4){
                funcButton = 5;
                ui->fswitch0->setEnabled(0);
                ui->fswitch1->setEnabled(1);
                ui->fswitch2->setEnabled(0);
                ui->fswitch3->setEnabled(0);
            } else if (funcButton == 5){
                funcButton = 6;
                ui->fswitch0->setEnabled(0);
                ui->fswitch1->setEnabled(1);
                ui->fswitch2->setEnabled(1);
                ui->fswitch3->setEnabled(0);
            } else if (funcButton == 6){
                on_speedSet_clicked();
            }
        }
}

//load message from text file -> Qt
QString cd_gui::loadTimeOutMsg(){
    ifstream fileText;
    string msg;
    string fileName = "message.txt";
    QString Qmsg;
    fileText.open(fileName);
    if (fileText.is_open()){
        cout << "Load " << fileName << " successful." << endl;
        getline(fileText, msg);

        //action whether message is too long or empty
        if (msg.size() > 19) {
            cout << "Message is too long." << endl;
        } else if (msg.size() < 1) {
            cout << "File is empty. Use default message." << endl;
            msg = "Time's up";
        }
        fileText.close();

        //set LED D2 to inform that message.txt is loaded
        gpio_set(GPIO_MODULE1, USR1_LED_D2);
     } else {
        cout << "Failed to load " << fileName << endl;
        cout << "Proceed with default timeout message." << endl;
        msg = "Time's up";
        ofstream fileText (fileName);
        fileText << msg;
        fileText.close();
        cout << "Created new " << fileName << endl;
     }
    Qmsg = QString::fromStdString(msg);
    return Qmsg;
}

//load previous saved time remaining
int cd_gui::loadTimeRemain(){
    //input to read timerem
    ifstream timeFile;
    string fileName = "timerem";
    //remaining time
    string time;
    int t_remain;
    timeFile.open(fileName);
    if (timeFile.is_open()){
        cout << "Load " << fileName << " successful." << endl;
        getline(timeFile, time);

        //turn string to int
        t_remain = stoi(time);
        timeFile.close();

        //set LED D3 to inform that timerem is loaded
        gpio_set(GPIO_MODULE1, USR2_LED_D3);
    } else {
        cout << "Failed to load " << fileName << endl;
        t_remain = 60;
        ofstream timeFile (fileName);
        timeFile << t_remain;
        timeFile.close();
        cout << "Created new " << fileName << endl;
    }
    //prevent time overflow
    if (t < 0 || t > 8640000){
        cout << "Value out of bound (0 < t < 8640000)" << endl;
        t_remain = 60;
        ofstream timeFile (fileName);
        timeFile << t_remain;
        timeFile.close();
        cout << "Created new " << fileName << endl;
    }
    return t_remain;
}

//save previous saved time remaining
void cd_gui::saveTimeRemain(){
    ofstream timeFile;
    string fileName = "timerem";
    timeFile.open(fileName);
    timeFile << t;
    timeFile.close();
    cout << "Saved " << t << "." << endl;
}
