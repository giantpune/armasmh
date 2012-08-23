#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QMetaType>
#include <QScrollBar>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_toAsm_clicked();
    void on_pushButton_toHex_clicked();
    void on_pushButton_flipEndian_clicked();

public slots:

    // don't use a references here because the message handler modifies the string
    void OutputMessage( QtMsgType type, QString msg );

private:
    Ui::MainWindow *ui;
    QList<quint32>ParseHexTxt();
    QList<quint16>ParseHexTxtThumb();
};

class LogBrowser : public QObject
{
    Q_OBJECT
public:
    explicit LogBrowser( MainWindow *win ) : QObject( win ), w( win )
    {
        qRegisterMetaType<QtMsgType>("QtMsgType");
        connect(this, SIGNAL( sendMessage(QtMsgType,QString)), w, SLOT(OutputMessage(QtMsgType,QString)), Qt::QueuedConnection );
    }

public slots:
    void outputMessage( QtMsgType type, const QString &msg )
    {
        emit sendMessage( type, msg );
    }
signals:
    void sendMessage( QtMsgType type, const QString &msg );

private:
    MainWindow *w;
};

#endif // MAINWINDOW_H
