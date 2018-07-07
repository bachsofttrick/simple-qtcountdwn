#include "cd_gui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cd_gui w;
    w.showFullScreen();
    return a.exec();
}
