#include "cd_gui.h"
#include "blink-led.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //setup for LEDs and buttons
    initial_iosetup();

    QApplication a(argc, argv);
    cd_gui w;
    short int size;
    cout << "Choose size options:" << endl;
    cout << "1 - Windowed" << endl;
    cout << "Any - Fullscreen" << endl;
    size = 2;
    /*
    cout << "Choose: ";
    cin >> size;
    */
    if (size==1){
        w.show();
    } else {
        w.showFullScreen();
    }
    return a.exec();
}
