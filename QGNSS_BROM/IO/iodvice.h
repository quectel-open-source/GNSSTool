/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         iodvice.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/
#ifndef IODVICE_H
#define IODVICE_H

#include <QObject>
#include <QIODevice>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QPointer>
#include <QPair>
#include <QMetaEnum>
#include "QGNSS_BROM_global.h"

#define _INFO_TYPR_
#ifdef  _INFO_TYPR_
#define IO_READ "READ"
#define IO_WRITE "WRITE"
#define IO_CMD    "CMD"
#define IO_ERROR  "ERROR"

#define OPENLOG true
//#define LOGFRONT QString("FUNC:%1 LINE:%2  ").arg (Q_FUNC_INFO).arg (__LINE__)
#define LOGFRONT QString("[LINE:%1] ").arg (__LINE__)
#define QLOG(x)          \
    if(OPENLOG)          \
{                        \
    qDebug()<<x;         \
}

/**
 * \param list QStringList
 * \param em   QEnum
 *
 * */
#define ENUMAPPEND(list,em)                      \
    QMetaEnum downmd=QMetaEnum::fromType<em>();  \
    for(int i=0;i<downmd.keyCount ();i++)        \
    {                                            \
        list.append (downmd.key (i));            \
    }


#define HEX2BYTES(hexstr) QByteArray::fromHex (hexstr)


#define MAX_WRITE_TIMEOUT 0
#define MAX_READ_TIMEOUT  10000

#endif
/**
 * @brief The IODvice class  io  device   for tcp udp ... prepare
 *
 * syncwrite
 *
 */
class QGNSS_BROM_EXPORT IODvice : public QObject
{
    Q_OBJECT
    /*state msg info*/
    Q_PROPERTY(pair_value rw_Info READ rw_Info WRITE setRw_Info NOTIFY rw_InfoChanged)

public:

    Q_INVOKABLE virtual void io_Ini()=0;                                            /*config serialport param*/
    Q_INVOKABLE virtual void io_Open()=0;                                           /*open serialport*/
    Q_INVOKABLE virtual void io_Close()=0;                                          /*close port*/
    Q_INVOKABLE virtual bool io_isOpen()=0;                                         /*return is open*/
    Q_INVOKABLE virtual void io_clear()=0;                                          /*io clear*/
    Q_INVOKABLE virtual void io_flush() =0;
    Q_INVOKABLE  void io_asyncsleep(int timeout);                                   /*async sleep*/
    Q_INVOKABLE virtual QByteArray io_Read(int timeout,qint32 readsize=0)=0;        /*read*/
    Q_INVOKABLE virtual qint32 io_Write(QByteArray data,int timeout,bool async)=0;  /*write*/
    Q_INVOKABLE virtual void set_BuadRate( int32_t rate)=0;
    Q_INVOKABLE virtual void waitwritte()=0;
    /**
    * @brief syncWriteRead  sync write read timeout return empty array
    * @param data
    * @param timeout
    * @return
    */
    Q_INVOKABLE virtual QByteArray syncWriteRead(const QByteArray &data,int timeout=1000,quint64 readsize=0)=0;

    template<typename T>
    static QString Enum_Value2Key(T t){

        QMetaEnum downmd=QMetaEnum::fromType<T>();
        return    downmd.valueToKey ((int)t);
    }

    template<typename T>
    static int Enum_Key2Value(QString key){

        QMetaEnum downmd=QMetaEnum::fromType<T>();
        return    downmd.keyToValue (key.toLocal8Bit ());
    }
public:
    explicit IODvice(QObject   *parent = nullptr);
    explicit IODvice(QIODevice *iodevice,QObject   *parent = nullptr);
    virtual ~IODvice();
    typedef   QPair<QString, QString> pair_value;

    const QPair<QString, QString> &rw_Info() const;

    /**
     * @brief cchrAppend string append
     * @param t
     * @return
     */
    static QByteArray cchrAppend(std::initializer_list<const char*>t);


    enum class Byteorder{
        ABCD,
        CDAB,
        DCBA,
        BADC
        };
    Q_ENUM (Byteorder)

    bool is_bigEndian(){
        uint16_t a=0x1234;
        char *c=(char*)&a;
        if(*c==0x12){return true;}
        else{
            return false;
        }
    }

    void uint2float(float &f, uint16_t *t, Byteorder order);

Q_SIGNALS:

    void rw_InfoChanged(const QPair<QString, QString> &pair);
    void errorOccurred(QString errostr);
    ///中途停止信号
    void send_Stop();
public Q_SLOTS:
    void setRw_Info(const QPair<QString, QString> &newRw_bytes);
private:
    QPair<QString, QString> rw_Info_;

protected:
    QPointer<QIODevice> iodevice_;
};

#endif // IODVICE_H
