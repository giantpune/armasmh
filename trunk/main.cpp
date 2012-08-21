#include <QtGui/QApplication>
#include <QPointer>
#include "mainwindow.h"

// directs qDebug() and friends to the GUI in a nice thhread-safe manner
QPointer<LogBrowser> logBrowser;
void myMessageOutput( QtMsgType type, const char *msg )
{
    if( logBrowser )
    {
        logBrowser->outputMessage( type, msg );
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    logBrowser = new LogBrowser( &w );
    qInstallMsgHandler( myMessageOutput );

    w.show();
    
    return a.exec();
}
