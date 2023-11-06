/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         ioserial.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/
#include "ioserial.h"

IOserial::IOserial(QIODevice *iodevice, QObject *parent)
    :IODvice{iodevice,parent}
{

  if(!iodevice)
  {
    QLOG(QString(LOGFRONT+"serialport instance is nullptr!!"))
    return;
  }
  iodevice_=iodevice;
  serial=qobject_cast<QSerialPort*>(iodevice_);
  setPortName(serial->portName ());
  connect (serial,&QSerialPort::errorOccurred,this,[&](QSerialPort::SerialPortError err){
    if(err==QSerialPort::SerialPortError::NoError)
    {
      return;
    }
    emit errorOccurred (Enum_Value2Key<QSerialPort::SerialPortError>(err));
  });
}

IOserial::~IOserial()
{

}

QByteArray IOserial::syncWriteRead(const QByteArray &data, int timeout, quint64 readsize)
{
  QByteArray redata;

  if(!serial->isOpen ()&&data.isEmpty ())
  {
    return redata;
  }

  if(serial_write(data)!=data.size ())
  {
    return redata;
  }
  QEventLoop readloop;
  QTimer tim;
  if(timeout)
  {
    tim.singleShot (timeout,&readloop,&QEventLoop::quit);
    tim.start ();
  }
  connect (serial,&QIODevice::readyRead,&readloop,&QEventLoop::quit);

  readloop.exec();

  if(!readsize)
  {
    redata= serial_readAll ();
  }
  else
  {
    redata=io_Read(timeout,readsize);
  }

  return redata;
}

void IOserial::io_Open()
{
  if(serial->isOpen ()){
    return;
  }
  serial->open (QIODevice::ReadWrite);
  setRw_Info ({IO_CMD," opened "+portName()});
}

void IOserial::io_Close()
{
  if(serial->isOpen ())
  {
    serial->close ();
    setRw_Info ({IO_CMD," closed "+portName()});
  }
}

void IOserial::io_Ini()
{
  serial->setBaudRate (serinfo->baudRate_);
  serial->setPortName (serinfo->portname_);
  serial->setDataBits (serinfo->DataBits_);
  serial->setFlowControl (serinfo->FlowControl_);
  serial->setStopBits (serinfo->StopBits_);
}

bool IOserial::io_isOpen()
{
  if(serial.isNull ())
  {
    return false;
  }
  return serial->isOpen ();
}

qint32 IOserial::io_Write(QByteArray data,int timeout,bool async)
{
  if(async)
  {
    if(timeout)
    {
      io_asyncsleep(timeout);
    }
  }
  else
  {
    if(timeout)
    {
      QThread::msleep(timeout);
    }
  }

  return serial_write (data);
}

QByteArray IOserial::io_Read(int timeout,qint32 readsize)
{
  if(!serial||!serial->isOpen ()){return QByteArray();}
  int remaindersize=serial->bytesAvailable();
  if(timeout)
  {
    if(remaindersize<readsize||!readsize)
    {
      QEventLoop loop;
      QTimer tim;
      tim.singleShot (timeout,&loop,&QEventLoop::quit);
      tim.start ();
      connect(serial,&QSerialPort::readyRead,&loop,&QEventLoop::quit);
      //new 2022/10/18 add send_stop() signal   to in advance stop eventloop
      connect(this,&IOserial::send_Stop ,&loop,&QEventLoop::quit);
      loop.exec ();
    }
  }
  if(!readsize)
  {
    return serial_readAll ();
  }
  else
  {
    QByteArray readdata;
    if(readsize>serial->bytesAvailable())
    {
      return io_SetSize2Read(readsize,200);//@ GNSS-ToolsGNSSTOOL-87 100->200
    }

    readdata.resize (readsize);
    if(!serial_read (readdata.data (),readsize))
    {
      return QByteArray();
    }
    return readdata;
  }
}

//QByteArray IOserial::io_Read(int timeout,qint32 readsize)
//{
//  int remaindersize=serial->bytesAvailable();
//  if(timeout&&!remaindersize)
//  {
//    if(remaindersize<readsize||!readsize)
//    {
//      QEventLoop loop;
//      QTimer tim;
//      tim.setTimerType (Qt::PreciseTimer);
//      tim.singleShot (timeout,&loop,&QEventLoop::quit);
//      tim.start ();
//      connect(serial,&QSerialPort::readyRead,&loop,&QEventLoop::quit);
//      loop.exec ();
//    }
//  }
//  if(!readsize||remaindersize)
//  {
//    return serial_readAll ();
//  }
//  else
//  {
//    QByteArray readdata;
//    if(readsize>serial->bytesAvailable())
//    {
//      return io_SetSize2Read(readsize,200);//@ GNSS-ToolsGNSSTOOL-87 100->200
//    }

//    readdata.resize (readsize);
//    if(!serial_read (readdata.data (),readsize))
//    {
//      return QByteArray();
//    }
//    return readdata;
//  }
//}

int IOserial::get_bytesAvailable()
{
  return  serial->bytesAvailable ();
}

void IOserial::set_RTS(bool enable)
{
  serial->setRequestToSend (enable);
}

void IOserial::set_DTR(bool enable)
{
  serial->setDataTerminalReady (enable);
}

void IOserial::set_stopbits(QSerialPort::StopBits sbits)
{
  serial->setStopBits (sbits);
}

void IOserial::set_Parity(QSerialPort::Parity sparity)
{
  serial->setParity (sparity);
}

void IOserial::waitwritte()
{
  serial->waitForBytesWritten ();
}

void IOserial::set_disable_waitWT()
{
    enablewaitWT=false;
}

void IOserial::set_enable_waitWT()
{
    enablewaitWT=true;
}

/**
 * @brief IOserial::io_SetSize2Read set read size if set size>timeout read size  async read
 * while reach set size
 * @param readsize
 * @param timeout
 * @return QByteArray size =set size
 */
QByteArray IOserial::io_SetSize2Read( int readsize,int timeout)
{
  static int emptytimes=2;/*available size <=0 times avoid dead loop*/
  int times=emptytimes;
  QByteArray returndata;
  if(!readsize)
  {
    returndata.append (serial_readAll ());
  }
  int remaindersize=serial->bytesAvailable();
  returndata.append (serial_readAll ());
  int remain=readsize-remaindersize;

  while(remain>0&&times)
  {
    QEventLoop loop;
    QTimer tim;
    tim.singleShot (timeout,&loop,&QEventLoop::quit);
    tim.start ();
    connect(serial,&QSerialPort::readyRead,&loop,&QEventLoop::quit);
    //new 2022/10/18 add send_stop() signal   to in advance stop eventloop
    connect(this,&IOserial::send_Stop ,&loop,&QEventLoop::quit);
    loop.exec ();
    int size=serial->bytesAvailable();
    if(remain-size<=0)
    {
      QByteArray data;
      data.resize (remain);
      serial_read (data.data (),remain);
      returndata.append (data);
      break;
    }
    else
    {
      returndata.append (serial_readAll ());
    }
    remain=remain-size;
    if(size<=0)
    {
      times--;
    }
  }

  return  returndata;
}

int IOserial::get_baudRate()
{
  return serial->baudRate ();
}
bool IOserial::get_RTS()
{
  return serial->isRequestToSend ();
}

bool IOserial::get_DTR()
{
  return serial->isDataTerminalReady ();
}

Qt::HANDLE IOserial::get_handle()
{
  return serial->handle ();
}

QSerialPort::StopBits IOserial::get_StopBits()
{
  return serial->stopBits ();
}

QSerialPort::Parity IOserial::get_parity()
{
  return serial->parity ();
}

void IOserial::io_clear()
{
  serial->clear ();
}

void IOserial::io_flush()
{
  serial->flush ();
}

QList<QSerialPortInfo> IOserial::getPorts()
{
  return QSerialPortInfo::availablePorts ();
}

IOserial::Serial_INFO const *IOserial::getSerial_INFO()
{
  if(!serinfo)
  {
    serinfo=new Serial_INFO;
  }
  serinfo->Parity_=serial->parity ();
  serinfo->DataBits_=serial->dataBits ();
  serinfo->StopBits_=serial->stopBits ();
  serinfo->baudRate_=serial->baudRate ();
  serinfo->portname_=serial->portName ();
  serinfo->FlowControl_=serial->flowControl ();
  return serinfo;
}

void IOserial::set_SerialInfo( const Serial_INFO &name)
{
  serinfo=const_cast<Serial_INFO*>(&name);
}
/**
 * @brief IOserial::set_SerialInfo
 * set default param
 * @param portNme
 **/
void IOserial::set_SerialInfo(QString portNme, int baudRate)
{
  IOserial::Serial_INFO *serialInfo=new IOserial::Serial_INFO();
  serialInfo->DataBits_=QSerialPort::DataBits::Data8;
  serialInfo->FlowControl_=QSerialPort::FlowControl::NoFlowControl;
  serialInfo->Parity_=QSerialPort::Parity::NoParity;
  serialInfo->StopBits_=QSerialPort::StopBits::OneStop;

  serialInfo->portname_=portNme;
  serialInfo->baudRate_=baudRate;
  set_SerialInfo(*serialInfo);
}

void IOserial::set_BuadRate(int32_t rate)
{
  serial->setBaudRate (rate);
  setRw_Info ({IO_CMD,QString::number (rate)});
}

void IOserial::set_Flow(QSerialPort::FlowControl flow)
{
  serial->setFlowControl (flow);
}

qint64 IOserial::serial_write(const QByteArray &data)
{
  qint64 size=0;
  if(serial&&serial->isOpen ())
  {
    size= serial->write (data);
    if(enablewaitWT)
    {
      bool flag= serial->waitForBytesWritten ();
      if(!flag)
      {
        return -1;
      }
    }
    setRw_Info ({IO_WRITE,data.toHex (' ')});
  }
  return size;
}

QByteArray IOserial::serial_readAll()
{
  QByteArray read;
  if(serial&&serial->isOpen ())
  {
    read=serial->readAll ();
    setRw_Info ({IO_READ,read.toHex (' ')});
  }
  return read;
}

qint64 IOserial::serial_read(char *data, qint64 maxlen)
{
  qint64 size=0;
  if(serial&&serial->isOpen ())
  {
    size=serial->read (data,maxlen);
    setRw_Info ({IO_READ,QByteArray(data).toHex (' ')});
  }
  return size;
}

const QString &IOserial::portName() const
{
  return portName_;
}

void IOserial::setPortName(const QString &newPortName)
{
  if (portName_ == newPortName)
    return;
  portName_ = newPortName;
  emit portNameChanged();
}
