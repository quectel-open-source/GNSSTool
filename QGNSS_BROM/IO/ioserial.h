/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         ioserial.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/
#ifndef IOSERIAL_H
#define IOSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTime>
#include "QGNSS_BROM_global.h"
#include "iodvice.h"
#include <QtSerialPort/qserialportglobal.h>
class QGNSS_BROM_EXPORT IOserial :public IODvice
{
    Q_OBJECT
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)

public:
    IOserial(QIODevice *iodevice,QObject   *parent = nullptr);
    virtual ~IOserial();

    typedef struct{
        QString portname_;
        int32_t baudRate_;
        QSerialPort::StopBits StopBits_;
        QSerialPort::DataBits DataBits_;
        QSerialPort::Parity   Parity_;
        QSerialPort::FlowControl FlowControl_;
    }Serial_INFO;
    QList<QSerialPortInfo>  getPorts();
    const Serial_INFO      *getSerial_INFO();

public Q_SLOTS:

    QByteArray syncWriteRead(const QByteArray &data, int timeout,quint64 readsize=0) override;
           void setPortName(const QString &newPortName);

public:

    int baudRate[7]=
        {
            4800,
            9600,
            14400,
            19200,
            38400,
            57600,
            115200,
        };
    Q_INVOKABLE void io_Open()  override;
    Q_INVOKABLE void io_Close() override;
    Q_INVOKABLE void io_Ini()  override;
    Q_INVOKABLE bool io_isOpen() override;
    Q_INVOKABLE void io_clear() override;
    Q_INVOKABLE void io_flush() override;
    Q_INVOKABLE void set_BuadRate( int32_t rate) override;
    Q_INVOKABLE void set_Flow( QSerialPort::FlowControl flow) ;
    Q_INVOKABLE void set_SerialInfo( const Serial_INFO &name);
    Q_INVOKABLE void set_SerialInfo(QString portNme,int baudRate);
    Q_INVOKABLE qint32 io_Write(QByteArray data,int timeout=MAX_WRITE_TIMEOUT,bool async=true) override;
    Q_INVOKABLE QByteArray io_Read(int timeout=MAX_READ_TIMEOUT,qint32 readsize=0) override;
    Q_INVOKABLE int get_bytesAvailable();
    Q_INVOKABLE void set_RTS(bool enable);
    Q_INVOKABLE void set_DTR(bool enable);
    Q_INVOKABLE void set_stopbits(QSerialPort::StopBits sbits);
    Q_INVOKABLE void set_Parity(QSerialPort::Parity sparity);
    Q_INVOKABLE virtual void waitwritte() override;
    Q_INVOKABLE void set_disable_waitWT();
    Q_INVOKABLE void set_enable_waitWT();
    QByteArray io_SetSize2Read(int readsize,int timeout=100/*ms*/);

    int get_baudRate();
    bool get_RTS();
     bool get_DTR();
     Qt::HANDLE get_handle();
    QSerialPort::StopBits get_StopBits();
    QSerialPort::Parity  get_parity();
    const QString &portName() const;

signals:
    void portNameChanged();

private:
    QPointer<QSerialPort>  serial=nullptr;
    Serial_INFO * serinfo=nullptr;
    int32_t    baudrate_;

    qint64 serial_write(const QByteArray &data);
    QByteArray serial_readAll();
    qint64 serial_read(char *data, qint64 maxlen);

    QString portName_;
    bool enablewaitWT=true;
};
Q_DECLARE_METATYPE (IOserial::Serial_INFO)
#endif // IOSERIAL_H
