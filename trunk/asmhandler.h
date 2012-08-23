#ifndef ASMHANDLER_H
#define ASMHANDLER_H

#include <QDebug>
#include <QList>
#include <QStringList>

namespace AsmHandler
{

extern QString asmFileName;
extern QString objFileName;

QStringList Convert( const QList< quint32 > &input );
QList< quint32 > Convert( const QStringList &input );

QStringList ConvertThumb( const QList< quint16 > &input );
QList< quint16 > ConvertThumb( const QStringList &input );
}

#endif // ASMHANDLER_H
