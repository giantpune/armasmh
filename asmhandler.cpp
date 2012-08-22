#include <QDir>
#include <QProcess>
#include "tools.h"

#include "asmhandler.h"

namespace AsmHandler
{
QStringList Convert( const QList< quint32 > &input )
{
    if( !input.size() )
    {
        return QStringList();
    }

    QString sFile =
            "    .text\n"
            "    .align 4\n"
            "    .code 32\n"
            "    .global MyFunction\n"
            "MyFunction:\n"
            ;

    foreach( quint32 n, input )
    {
        sFile += QString( "    .long 0x%1\n" ).arg( n, 8, 16, QChar( '0' ) );
    }

    if( !WriteFile( QDir::currentPath() + "/tmp/asm.s", sFile.toLatin1().constData() ) )
    {
        return QStringList();
    }

    QProcess p;
    p.setProcessChannelMode( QProcess::MergedChannels );
    QStringList args = QStringList()
            << "-o" << QDir::currentPath() + "/tmp/asm.o"
            << QDir::currentPath() + "/tmp/asm.s";

    QString prog( QDir::currentPath() + "/tools/as" );

    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QStringList();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "as didn\'t run right:" << output;
        return QStringList();
    }

    prog = QDir::currentPath() + "/tools/objdump";
    args = QStringList() << "-D" << "--section=.text" << QDir::currentPath() + "/tmp/asm.o";
    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QStringList();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "objdump didn\'t run right:" << output;
        return QStringList();
    }

    QString output( p.readAllStandardOutput() );
    int pos = output.indexOf( "<MyFunction>" );
    if( pos < 0 )
    {
        qDebug() << "unexpected output" << output;
        return QStringList();
    }
    output.remove( 0, pos + 13 );
    QStringList lines = output.split( "\n", QString::SkipEmptyParts );
    if( lines.size() < input.size() )
    {
        qDebug() << "unexpected output" << output;
        return QStringList();
    }
    QStringList ret;
    for( int i = 0; i < input.size(); i++ )
    {
        QString line = lines.at( i );
        pos = line.indexOf( ":\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }
        line.remove( 0, pos + 2 );
        if( line.size() < 11 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }

        // make sure it matches the input
        if( line.left( 8 ).toULong( NULL, 16 ) != input.at( i ) )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }

        pos = line.indexOf( "\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }
        line.remove( 0, pos + 1 );

        // remove junk and add "0x" to some offsets
        pos = line.indexOf( "<" );
        if( pos > 0 )
        {
            line.resize( pos );
            pos = line.indexOf( "\t" );
            if( pos > 0 && line.size() > pos - 1 && line.startsWith( "b", Qt::CaseInsensitive ) )
            {
                const QString &numStr = line.mid( pos );
                bool ok = false;
                quint32 num = numStr.toLong( &ok, 16 );

                // add comments for negative branches
                if( ok && ( num & 0x80000000 ) )
                {
                    line += QString( "\t@ -0x%1" ).arg( 0x100000000ull - num, 0, 16 );
                }
                line.insert( pos + 1, "0x" );
            }
        }
        line.replace( ";", "@" );


        ret << line;
    }

    return ret;
}

QList< quint32 > Convert( const QStringList &input )
{
    if( !input.size() )
    {
        return QList< quint32 >();
    }

    QString sFile =
            "    .text\n"
            "    .align 4\n"
            "    .code 32\n"
            "    .global MyFunction\n"
            "MyFunction:\n"
            ;
    foreach( QString str, input )
    {
        // remove comments
        /*int pos = str.indexOf( ";" );
        if( pos > 0 )
        {
            str.resize( pos );
        }*/

        // add it
        sFile += "\t" + str + "\n";
    }

    if( !WriteFile( QDir::currentPath() + "/tmp/asm.s", sFile.toLatin1().constData() ) )
    {
        return QList< quint32 >();
    }

    QProcess p;
    p.setProcessChannelMode( QProcess::MergedChannels );
    QStringList args = QStringList()
            << "-o" << QDir::currentPath() + "/tmp/asm.o"
            << QDir::currentPath() + "/tmp/asm.s";

    QString prog( QDir::currentPath() + "/tools/as" );

    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QList< quint32 >();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "as didn\'t run right:" << output;
        return QList< quint32 >();
    }

    prog = QDir::currentPath() + "/tools/objdump";
    args = QStringList() << "-D" << "--section=.text" << QDir::currentPath() + "/tmp/asm.o";
    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QList< quint32 >();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "objdump didn\'t run right:" << output;
        return QList< quint32 >();
    }

    QString output( p.readAllStandardOutput() );
    int pos = output.indexOf( "<MyFunction>" );
    if( pos < 0 )
    {
        qDebug() << "unexpected output" << output;
        return QList< quint32 >();
    }
    output.remove( 0, pos + 13 );
    QStringList lines = output.split( "\n", QString::SkipEmptyParts );
    if( lines.size() < input.size() )
    {
        qDebug() << "unexpected output" << output;
        return QList< quint32 >();
    }
    QList< quint32 > ret;
    for( int i = 0; i < input.size(); i++ )
    {
        QString line = lines.at( i );
        pos = line.indexOf( ":\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint32 >();
        }
        line.remove( 0, pos + 2 );
        if( line.size() < 11 )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint32 >();
        }

        bool ok = false;
        quint32 num = line.left( 8 ).toULong( &ok, 16 );
        if( !ok )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint32 >();
        }

        ret << num;
    }

    return ret;
}

QStringList ConvertThumb( const QList< quint16 > &input )
{
    if( !input.size() )
    {
        return QStringList();
    }

    QString sFile =
            "    .text\n"
            "    .syntax unified\n"
            "    .align 4\n"
            "    .code 16\n"
            "    .global MyFunction\n"
            "MyFunction:\n"
            ;

    foreach( quint32 n, input )
    {
        sFile += QString( "    .short 0x%1\n" ).arg( n, 4, 16, QChar( '0' ) );
    }

    if( !WriteFile( QDir::currentPath() + "/tmp/asm.s", sFile.toLatin1().constData() ) )
    {
        return QStringList();
    }

    QProcess p;
    p.setProcessChannelMode( QProcess::MergedChannels );
    QStringList args = QStringList()
            << "-o" << QDir::currentPath() + "/tmp/asm.o"
            << QDir::currentPath() + "/tmp/asm.s";

    QString prog( QDir::currentPath() + "/tools/as" );

    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QStringList();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "as didn\'t run right:" << output;
        return QStringList();
    }

    prog = QDir::currentPath() + "/tools/objdump";
    args = QStringList() << "-D" << "--section=.text" << QDir::currentPath() + "/tmp/asm.o";
    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QStringList();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "objdump didn\'t run right:" << output;
        return QStringList();
    }

    QString output( p.readAllStandardOutput() );
    int pos = output.indexOf( "<MyFunction>" );
    if( pos < 0 )
    {
        qDebug() << "unexpected output" << output;
        return QStringList();
    }
    output.remove( 0, pos + 13 );
    QStringList lines = output.split( "\n", QString::SkipEmptyParts );
    if( lines.size() < input.size() )
    {
        qDebug() << "unexpected output" << output;
        return QStringList();
    }
    QStringList ret;
    for( int i = 0; i < input.size(); i++ )
    {
        QString line = lines.at( i );
        pos = line.indexOf( ":\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }
        line.remove( 0, pos + 2 );
        if( line.size() < 11 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }

        // make sure it matches the input
        if( line.left( 4 ).toULong( NULL, 16 ) != input.at( i ) )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }

        pos = line.indexOf( "\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QStringList();
        }
        line.remove( 0, pos + 1 );

        // remove junk and add "0x" to some offsets
        pos = line.indexOf( "<" );
        if( pos > 0 )
        {
            line.resize( pos );
            pos = line.indexOf( "\t" );
            if( pos > 0 && line.size() > pos - 1 && line.startsWith( "b", Qt::CaseInsensitive ) )
            {
                const QString &numStr = line.mid( pos );
                bool ok = false;
                quint32 num = numStr.toLong( &ok, 16 );

                // add comments for negative branches
                if( ok && ( num & 0x80000000 ) )
                {
                    line += QString( "\t@ -0x%1" ).arg( 0x100000000ull - num, 0, 16 );
                }
                line.insert( pos + 1, "0x" );
            }
        }
        line.replace( ";", "@" );

        ret << line;
    }

    return ret;
}


QList< quint16 > ConvertThumb( const QStringList &input )
{
    if( !input.size() )
    {
        return QList< quint16 >();
    }

    QString sFile =
            "    .text\n"
            "    .syntax unified\n"
            "    .align 4\n"
            "    .code 16\n"
            "    .global MyFunction\n"
            "MyFunction:\n"
            ;
    foreach( QString str, input )
    {
        // remove comments
        /*int pos = str.indexOf( ";" );
        if( pos > 0 )
        {
            str.resize( pos );
        }*/

        // add it
        sFile += "\t" + str + "\n";
    }

    if( !WriteFile( QDir::currentPath() + "/tmp/asm.s", sFile.toLatin1().constData() ) )
    {
        return QList< quint16 >();
    }

    QProcess p;
    p.setProcessChannelMode( QProcess::MergedChannels );
    QStringList args = QStringList()
            << "-o" << QDir::currentPath() + "/tmp/asm.o"
            << QDir::currentPath() + "/tmp/asm.s";

    QString prog( QDir::currentPath() + "/tools/as" );

    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QList< quint16 >();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "as didn\'t run right:" << output;
        return QList< quint16 >();
    }

    prog = QDir::currentPath() + "/tools/objdump";
    args = QStringList() << "-D" << "--section=.text" << QDir::currentPath() + "/tmp/asm.o";
    p.start( prog, args );
    if( !p.waitForFinished( -1 ) )
    {
        qDebug() << "failed to start" << prog;
        return QList< quint16 >();
    }
    if( p.exitStatus() != QProcess::NormalExit || p.exitCode() )
    {
        QByteArray output = p.readAllStandardOutput();
        qDebug() << "objdump didn\'t run right:" << output;
        return QList< quint16 >();
    }

    QString output( p.readAllStandardOutput() );
    int pos = output.indexOf( "<MyFunction>" );
    if( pos < 0 )
    {
        qDebug() << "unexpected output" << output;
        return QList< quint16 >();
    }
    output.remove( 0, pos + 13 );
    QStringList lines = output.split( "\n", QString::SkipEmptyParts );
    if( lines.size() < input.size() )
    {
        qDebug() << "unexpected output" << output;
        return QList< quint16 >();
    }
    QList< quint16 > ret;
    for( int i = 0; i < input.size(); i++ )
    {
        QString line = lines.at( i );
        pos = line.indexOf( ":\t" );
        if( pos < 0 )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint16 >();
        }
        line.remove( 0, pos + 2 );
        if( line.size() < 7 )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint16 >();
        }

        bool ok = false;
        quint16 num = line.left( 4 ).toUShort( &ok, 16 );
        if( !ok )
        {
            qDebug() << "unexpected output" << output;
            return QList< quint16 >();
        }

        ret << num;
    }

    return ret;
}

}
