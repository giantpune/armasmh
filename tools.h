#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

QByteArray ReadFile( const QString &path );
bool WriteFile( const QString &path, const QByteArray &ba );

inline QString HexStr( quint32 n )
{
    return QString( "%1" ).arg( n, 8, 16, QChar( '0' ) );
}

inline QString HexStr( quint16 n )
{
    return QString( "%1" ).arg( n, 4, 16, QChar( '0' ) );
}

#endif // TOOLS_H
