
#include <QDir>
#include <QFileInfo>
#include <QtEndian>
#include "asmhandler.h"
#include "mainwindow.h"
#include "tools.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi( this );

    ui->plainTextEdit_hex->setMaximumWidth( ui->plainTextEdit_hex->fontMetrics().width( "00000000        " ) );

    // setup dir for tmp .o and .s
    QFileInfo f( QDir::currentPath() + "/tmp" );
    if( !f.exists() && !QDir().mkpath( QDir::currentPath() + "/tmp" ) )
    {
        qDebug() << "error making tmp directory";
    }



}

MainWindow::~MainWindow()
{
    QFile( QDir::currentPath() + "/tmp/asm.o" ).remove();
    QFile( QDir::currentPath() + "/tmp/asm.s" ).remove();
    QDir( QDir::currentPath() ).rmdir( "tmp" );
    delete ui;
}

void MainWindow::on_pushButton_toAsm_clicked()
{
    QStringList asmTxt;
    if( ui->checkBox_thumb->isChecked() )
    {
        asmTxt = AsmHandler::ConvertThumb( ParseHexTxtThumb() );
    }
    else
    {
        asmTxt = AsmHandler::Convert( ParseHexTxt() );
    }

    if( !asmTxt.size() )
    {
        return;
    }
    ui->plainTextEdit_asm->clear();
    ui->plainTextEdit_asm->appendPlainText( asmTxt.join( "\n" ) );
}

QList<quint32>MainWindow::ParseHexTxt()
{
    QList<quint32>ret;
    QString doc = ui->plainTextEdit_hex->document()->toPlainText();
    QByteArray hexTxt = QByteArray::fromHex( doc.toLatin1().constData() );
    quint32 cnt = hexTxt.size() / 4;
    const quint32 *p = (const quint32*)hexTxt.constData();
    for( quint32 i = 0; i < cnt; i++ )
    {
        ret << qToBigEndian( p[ i ] );
    }
    return ret;
}

QList<quint16>MainWindow::ParseHexTxtThumb()
{
    QList<quint16>ret;
    QString doc = ui->plainTextEdit_hex->document()->toPlainText();
    QByteArray hexTxt = QByteArray::fromHex( doc.toLatin1().constData() );
    quint32 cnt = hexTxt.size() / 2;
    const quint16 *p = (const quint16*)hexTxt.constData();
    for( quint32 i = 0; i < cnt; i++ )
    {
        ret << qToBigEndian( p[ i ] );
    }
    return ret;
}

void MainWindow::on_pushButton_toHex_clicked()
{
    QString doc = ui->plainTextEdit_asm->document()->toPlainText();
    QStringList lines = doc.split( "\n", QString::SkipEmptyParts );

    if( ui->checkBox_thumb->isChecked() )
    {
        QList< quint16 > hexVals = AsmHandler::ConvertThumb( lines );
        if( !hexVals.size() )
        {
            return;
        }

        ui->plainTextEdit_hex->clear();
        foreach( quint16 n, hexVals )
        {
            ui->plainTextEdit_hex->appendPlainText( HexStr( n ) );
        }
    }
    else
    {
        QList< quint32 > hexVals = AsmHandler::Convert( lines );
        if( !hexVals.size() )
        {
            return;
        }

        ui->plainTextEdit_hex->clear();
        foreach( quint32 n, hexVals )
        {
            ui->plainTextEdit_hex->appendPlainText( HexStr( n ) );
        }
    }
}

void MainWindow::on_pushButton_flipEndian_clicked()
{
    if( ui->checkBox_thumb->isChecked() )
    {
        QList<quint16> nums = ParseHexTxtThumb();
        ui->plainTextEdit_hex->clear();
        foreach( quint16 n, nums )
        {
            ui->plainTextEdit_hex->appendPlainText( HexStr( qToBigEndian( n ) ) );
        }
    }
    else
    {
        QList<quint32> nums = ParseHexTxt();
        ui->plainTextEdit_hex->clear();
        foreach( quint32 n, nums )
        {
            ui->plainTextEdit_hex->appendPlainText( HexStr( qToBigEndian( n ) ) );
        }
    }
}

void MainWindow::OutputMessage( QtMsgType type, const QString &msg )
{
    bool needToScroll = ui->plainTextEdit_log->verticalScrollBar()->value() == ui->plainTextEdit_log->verticalScrollBar()->maximum();//if the text window is already showing the last line
    switch( type )
    {
    case QtDebugMsg:
        ui->plainTextEdit_log->appendHtml( QString( msg ) );
        break;
    case QtWarningMsg:
        {
            QString htmlString = "<b><text style=\"color:#0000ff\">" + msg + "</text></b>";
            ui->plainTextEdit_log->appendHtml( htmlString );
        }
        break;
    case QtCriticalMsg:
        {
            QString htmlString = "<b><text style=\"color:#ff0000\">" + msg + "</text></b>";
            ui->plainTextEdit_log->appendHtml( htmlString );
        }
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg.toLatin1().data() );
        abort();
        break;
    }
    if( needToScroll )//scroll to the new bottom of the screen
    {
        ui->plainTextEdit_log->verticalScrollBar()->setValue( ui->plainTextEdit_log->verticalScrollBar()->maximum() );
    }
}

