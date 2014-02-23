#include "display.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TDisplay w;
    w.show();

    return a.exec();
}
