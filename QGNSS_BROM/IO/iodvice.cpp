/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         iodvice.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/
#include "iodvice.h"

IODvice::IODvice(QObject *parent)
    : QObject{parent}
{

}
/**
 * @brief IODvice::IODvice
 * get instance from QGNSS
 * @param iodevice
 * @param parent
 */
IODvice::IODvice(QIODevice *iodevice,QObject   *parent )
    : QObject {parent}
{
    if(!iodevice)
    {
        iodevice_=iodevice;
    }
}

IODvice::~IODvice()
{

}

void IODvice::io_asyncsleep(int timeout)
{
    QEventLoop loop;
    QTimer::singleShot (timeout,Qt::TimerType::PreciseTimer,&loop,&QEventLoop::quit);
    loop.exec ();
}

QByteArray IODvice::cchrAppend(std::initializer_list<const char*>t)
{
    QByteArray data;
    for(auto i=t.begin ();i!=t.end ();++i)
    {
        data.append (QByteArray::fromHex (*i));
    }
    return data;
}

/**
 * @brief IODvice::uint2float uint16[2] 2 float
 * @param f
 * @param t
 * @param order
 */
void IODvice::uint2float(float &f, uint16_t *t, IODvice::Byteorder order)
{
        if(!t){return;}
        uint8_t *fbytes=reinterpret_cast<uint8_t*>(&f);
        uint8_t *highb= reinterpret_cast<uint8_t*>(&t[0]);
        uint8_t *lowb=  reinterpret_cast<uint8_t*>(&t[1]);
        uint8_t *dstb[4];
        if(!is_bigEndian())
        {
            dstb[0]=&fbytes[0];
            dstb[1]=&fbytes[1];
            dstb[2]=&fbytes[2];
            dstb[3]=&fbytes[3];
            switch(order)
            {
            case Byteorder::ABCD:
                *dstb[3]=highb[1];
                *dstb[2]=highb[0];
                *dstb[1]=lowb[1];
                *dstb[0]=lowb[0];
                break;
            case Byteorder::CDAB:
                *dstb[3]=lowb[1];
                *dstb[2]=lowb[0];
                *dstb[1]=highb[1];
                *dstb[0]=highb[0];
                break;
            case Byteorder::BADC:
                *dstb[3]=highb[0];
                *dstb[2]=highb[1];
                *dstb[1]=lowb[0];
                *dstb[0]=lowb[1];
                break;
            case Byteorder::DCBA:
                *dstb[3]=lowb[0];
                *dstb[2]=lowb[1];
                *dstb[1]=highb[0];
                *dstb[0]=highb[1];
                break;
            }

        }
        else
        {
            dstb[0]=&fbytes[3];
            dstb[1]=&fbytes[2];
            dstb[2]=&fbytes[1];
            dstb[3]=&fbytes[0];
            switch(order){
            case Byteorder::ABCD:
                *dstb[3]=highb[0];
                *dstb[2]=highb[1];
                *dstb[1]=lowb[0];
                *dstb[0]=lowb[1];
                break;
            case Byteorder::CDAB:
                *dstb[3]=lowb[0];
                *dstb[2]=lowb[1];
                *dstb[1]=highb[0];
                *dstb[0]=highb[1];
                break;
            case Byteorder::BADC:
                *dstb[3]=highb[1];
                *dstb[2]=highb[0];
                *dstb[1]=lowb[1];
                *dstb[0]=lowb[0];
                break;
            case Byteorder::DCBA:
                *dstb[3]=lowb[1];
                *dstb[2]=lowb[0];
                *dstb[1]=highb[1];
                *dstb[0]=highb[0];
                break;
            }
        }
}

void IODvice::setRw_Info(const QPair<QString, QString> &newRw_bytes)
{
    if (rw_Info_ == newRw_bytes)
        return;
    rw_Info_ = newRw_bytes;
    emit rw_InfoChanged(rw_Info_);
}
const QPair<QString, QString> &IODvice::rw_Info() const
{
    return rw_Info_;
}

