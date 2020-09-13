#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.setWindowTitle("Magda-demo");
    w.setWindowIcon(QIcon(":/m.png"));
    w.show();
    return a.exec();
}
