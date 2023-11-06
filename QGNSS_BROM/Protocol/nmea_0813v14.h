#ifndef NMEA_0813V14_H
#define NMEA_0813V14_H

#include <QObject>
#include "nmea_base.h"
class NMEA_0813V14 : public NMEA_Base
{
    Q_OBJECT
public:
    explicit NMEA_0813V14(QObject *parent = nullptr);
    virtual ~NMEA_0813V14();
Q_SIGNALS:
public Q_SLOTS:

    // NMEA_Base interface
public:

   Q_INVOKABLE  bool isContainsNMEA( QString ,QString regex=NMEA_REGEX) override;
};

#endif // NMEA_0813V14_H
