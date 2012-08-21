#include <QDebug>
#include <QFile>
#include <QString>


#include "tools.h"

QByteArray ReadFile( const QString &path )
{
    QFile file( path );
    if( !file.exists() || !file.open( QIODevice::ReadOnly ) )
    {
        qWarning() << "ReadFile -> can't open" << path;
        return QByteArray();
    }
    QByteArray ret = file.readAll();
    file.close();
    return ret;
}

bool WriteFile( const QString &path, const QByteArray &ba )
{
    QFile file( path );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        qWarning() << "WriteFile -> can't open" << path;
        return false;
    }
    if( file.write( ba ) != ba.size() )
    {
        file.close();
        qWarning() << "WriteFile -> can't write all the data to" << path;
        return false;
    }
    file.close();
    return true;
}
