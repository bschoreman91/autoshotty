#include "autoshotty.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoShotty w;
    w.show();
    return a.exec();
}
