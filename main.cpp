#include "widget.h"

#include <QApplication>
#include <QDate>
#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();
    return a.exec();
}
