#include "nmea_0813v14.h"
#include <QDebug>

NMEA_0813V14::NMEA_0813V14(QObject *parent)
             : NMEA_Base(parent)
{

}

NMEA_0813V14::~NMEA_0813V14()
{

}

bool NMEA_0813V14::isContainsNMEA( QString msg,QString regex)
{
    if(!msg.contains (QRegExp(regex)))
    {
        return  false;
    }
    return true;
}
