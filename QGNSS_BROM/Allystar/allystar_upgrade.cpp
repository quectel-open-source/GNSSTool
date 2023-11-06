#include "allystar_upgrade.h"
#include <QApplication>
#include <QCryptographicHash>

uint16_t calcrc(uint8_t *ptr, int count)
{
  uint32_t crc, i;

  crc = 0;

  while(--count >= 0)
  {
    crc = crc ^ (int)*ptr++ << 8;
    for (i = 0; i < 8; ++i)
      if (crc & 0x8000)
        crc = crc << 1 ^ 0x1021;
      else
        crc = crc << 1;
  }
  return(crc  & 0xFFFF);
}

AllyStar_Upgrade::AllyStar_Upgrade(QObject *parent)
    :AllyStar_Base{parent}
{
  QMap <QString,QVariant> map;
  map.insert ("PORTNAME",QVariant());
  map.insert ("DOWNLOADBAUDRATE",QVariant());
  map.insert ("DownLoadMode",QVariant());
  map.insert ("DownLoadRate",QVariant());
  map.insert ("DownLoadTotaltime",QVariant());

  map.insert ("SYNC_CHAR",QVariant());
  map.insert ("DA_VERSION",QVariant());
  map.insert ("FLASH_DEVICE_ID",QVariant());
  map.insert ("FLASH_SIZE",QVariant());
  map.insert ("FLASH_MANUFACTURE_CODE",QVariant());
  map.insert ("FLASH_DEVICE_CODE",QVariant());
  map.insert ("FLASH_EXT_DEVICE_CODE1",QVariant());
  map.insert ("FLASH_EXT_DEVICE_CODE2",QVariant());
  map.insert ("EXT_SRAM_SIZE",QVariant());
  map.insert ("TooltipTXT",QVariant());
  setFlashInfo (map);
}

AllyStar_Upgrade::~AllyStar_Upgrade()
{

}

AllyStar_Upgrade::Download_State AllyStar_Upgrade::download_state() const
{
  return download_state_;
}

void AllyStar_Upgrade::init_IO(IODvice *io)
{
  if(!io){return;}
  io_=io;
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  connect (seril,&IOserial::errorOccurred,this,[&](QString err){
    QERROR ("[Init IO]"+LOGFRONT+"serialport error:"+err);
  });

  init_baudRate_=seril->get_baudRate ();
  flashInfo()["PORTNAME"]=seril->portName ();
  emit flashInfoChanged ();
  seril=nullptr;
}

bool AllyStar_Upgrade::download_stop()
{
  setDownload_state (Download_State::Custom_DownLoad_Stoping);
  downloadstoping=true;
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  Q_EMIT seril->send_Stop ();//stop reading func
  seril->io_asyncsleep (500);
  seril->io_clear ();
  return true;
}

void AllyStar_Upgrade::reset_buadRate()
{
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  /*send different baudRates   seven */
  seril->set_BuadRate(init_baudRate_);
  seril=nullptr;
}

bool AllyStar_Upgrade::qGNSS_DownLoad(QString Path, int module, int mode)
{
  bool flag=false;
  // time
  QTime start=QTime::currentTime ();
  QTimer timer;
  timer.setInterval (500);
  timer.start ();
  connect (&timer,&QTimer::timeout,this,[&](){
    QTime end=QTime::currentTime ();
    int t_times=start.secsTo (end);
    setTotaltime (t_times);
    flashInfo ()["DownLoadTotaltime"]=QString::number (t_times)+" S";
    emit flashInfoChanged ();
  });
  char enter_mode=0x10;
  switch((QGNSS_Model)module)
  {
    case quectel_gnss::LC29YIA:
      start_address=0x00100000;
      enter_mode=0x10;
      break;
    case quectel_gnss::LC760Z:
    case quectel_gnss::LC260Z:
      start_address=0x90000000;
      enter_mode=0x01;
        break;
    default:
      start_address=0x00100000;
      break;
  }
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  seril->io_clear ();
  init_baudRate_=seril->get_baudRate ();
  switch ((DownLoadMode)mode)
  {
    case AllyStar_Upgrade::BOOT:
    {
      //if(!BOOT3_UAHRY_SET(download_baudRate_,QByteArray::fromHex ("31FF"))) {break;}

      if(!Unzip_File(Path)) {break;}
      if(!MD5_Check()) {break;}
      if((QGNSS_Model)module==quectel_gnss::LC760Z||(QGNSS_Model)module==quectel_gnss::LC260Z)
      {
        if(!BOOT3_RFF_SET (download_baudRate_)){break;}
        seril->io_clear ();
       // if(!BOOT3_UAHRY_SET(download_baudRate_,QByteArray::fromHex ("F1D905010200F403FF16"))) {break;}
       if(!BOOT2_SPIFLSINFO_POLL ()) {break;}
       if(!BOOT2_SPIFLS_WRSR_SET ()) {break;}

      if(!Download()){break;}
      if(!CFG_SIMPLERST ()){break;}
      if(!BOOT_Resert()){break;}
      }else
          if((QGNSS_Model)module==quectel_gnss::LC29YIA)
      {
        if(!Query_Version()) {break;}
        //if(!BOOT3_RFF_SET ()){break;}
        if(!BOOT3_UAHRY_SET(download_baudRate_,QByteArray::fromHex ("F1D905010200F403FF16"))) {break;}
        seril->io_clear ();
        if(!BOOT3_FLSEARSE_SET()) {break;}
        if(!Download()){break;}
        if(!BOOT_Resert()){break;}
        if(!Query_Version()) {break;}

      }
    }
      flag=true;
      break;
    case AllyStar_Upgrade::USER:
    {
      IOserial *seril=static_cast<IOserial *>(io_.data ());
      seril->io_clear ();
      if(!Unzip_File(Path)) {break;}
      if((QGNSS_Model)module==quectel_gnss::LC760Z||(QGNSS_Model)module==quectel_gnss::LC260Z)
      {
        if(!USER_CFG_PRT (download_baudRate_)) {break;}
      }
      if(!USER_Enter_Upgrade_Mode(enter_mode)) {break;}
      if(!USER_Send_SOH_PKG()) {break;}
      if(!USER_Send_STX_PKG()) {break;}
      if(!USER_Send_EOT_SOH_PKG()) {break;}
    }
      flag=true;
      break;
  }

  if(downloadstoping)
  {
    setDownload_state (Download_State::Custom_DownLoad_Stoped);
    downloadstoping=false;
  }
  setStartdownload(false);
  reset_buadRate();
  timer.stop ();
  return flag;
}

void AllyStar_Upgrade::qGNSS_SetDownLoadBaudRate(int baudrate)
{
  download_baudRate_=baudrate;
  flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
  emit flashInfoChanged ();
}

void AllyStar_Upgrade::setDownload_state(const AllyStar_Upgrade::Download_State &newDownload_state)
{
  download_state_ = newDownload_state;
  switch (newDownload_state)
  {
    case AllyStar_Upgrade::State_Unstart:
      statekeyvalue()->setKey (tr("Ready to download"));
      break;
    case BOOT_Unzip_File:
      statekeyvalue()->setKey (tr("Unzip File..."));
      break;
    case BOOT_MD5_Check:
      statekeyvalue()->setKey (tr("MD5 Check..."));
      break;
    case BOOT_Query_Version:
      statekeyvalue()->setKey (tr("Query Version..."));
      break;
    case BOOT_UAHRY_SET:
      statekeyvalue()->setKey (tr("Sets UART baud rate..."));
      break;
    case BOOT_FLSEARSE_SET:
       statekeyvalue()->setKey (tr("Erases flash ..."));
      break;
    case CFG_SIMPLERST_:
      statekeyvalue()->setKey (tr("Reset the module..."));
      break;
    case BOOT_RFF_SET:
       statekeyvalue()->setKey (tr("Sets reference clock frequency and baud rate...."));
        break;
    case BOOT_FWPG_SET:
       statekeyvalue()->setKey (tr("Sends firmware ..."));
       break;
    case BOOT_SPIFLS_WRSR_SET:
      statekeyvalue()->setKey (tr("Sets Flash status register ..."));
      break;
    case BOOT_SPIFLSINFO_POLL:
       statekeyvalue()->setKey (tr("Queries SPI flash ID or status. ..."));
      break;
    case BOOT_RESERT:
      statekeyvalue()->setKey (tr("Reset ..."));
      break;
    case USEREnterUpgradeMode:
      statekeyvalue()->setKey (tr("Enter Upgrade Mode..."));
      break;
    case USERSendSOHPKG:
      statekeyvalue()->setKey (tr("Send SOH PKG..."));
      break;
    case USERSendSTXPKG:
      statekeyvalue()->setKey (tr("Send STX PKG..."));
      break;
    case USERSendEOTSOHPKG:
      statekeyvalue()->setKey (tr("Send EOT/SOH PKG..."));
      break;
    case USERCFGPRT:
      statekeyvalue()->setKey (tr("Sets the UART port to be used and the baud rate of the UART..."));
      break;
    case AllyStar_Upgrade::State_Finished:
      statekeyvalue()->setKey (tr("Finished..."));
      break;

    case AllyStar_Upgrade::State_CMD_Fail:
      statekeyvalue()->setKey (tr("Download Fail"));
      break;
    case AllyStar_Upgrade::Custom_DownLoad_Stoping:
      statekeyvalue()->setKey (tr("Downloading is stopping"));
      break;
    case AllyStar_Upgrade::Custom_DownLoad_Stoped:
      statekeyvalue()->setKey (tr("Downloading is stopped"));
      break;
  }
  statekeyvalue()->setValue ((int)newDownload_state);
  emit statekeyvalueChanged ();
  emit download_stateChanged(download_state_);
}

void AllyStar_Upgrade::setDownload_stateStr(QString state)
{
  statekeyvalue()->setKey (state);
  emit statekeyvalueChanged ();
  emit download_stateChanged(download_state_);
}

bool AllyStar_Upgrade::Unzip_File(QString path)
{
  setDownload_state (BOOT_Unzip_File);
  firmwaredata_.clear ();
  QFileInfo fileinfo(path);
  if(!fileinfo.isFile ())
  {
    return false;
  }
  filename= fileinfo.fileName ().split ('.')[0]+"16.gz";
  QProcess myProcess(this);
  setDownload_percentage (0);
  QEventLoop loop;
  bool correct=true;
  connect (&myProcess,&QProcess::started,this,[&]()
          {
            loop.exec ();
          });
  connect (&myProcess,&QProcess::errorOccurred,this,[&](QProcess::ProcessError error)
          {
            QString err=IODvice::Enum_Value2Key<QProcess::ProcessError>(error);
            QERROR("[Unzip File Error]"+LOGFRONT+QString(" ERROR:%1")
                                                         .arg (QString(err))
                   );
            correct=false;
            loop.exit();
          });

  connect (&myProcess,&QProcess::readyReadStandardOutput,this,[&]()
          {
            firmwaredata_+=myProcess.readAllStandardOutput ();

          });
  connect (&myProcess,&QProcess::readyReadStandardError ,this,[&]()
          {
            QString read=myProcess.readAllStandardError ();
            QERROR("[Unzip File]"+QString("%1")
                                        .arg (read)
                   );
          });

  connect(&myProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          [&](int exitCode, QProcess::ExitStatus exitStatus)
          {
            Q_UNUSED (exitCode)
            Q_UNUSED (exitStatus)
            loop.exit();
          });
  QStringList args;
  args<<"x"<<path<<"-so";
  myProcess.start(QString("%1/7z.exe").arg (QApplication::applicationDirPath ()),args);
  myProcess.close ();
  setDownload_percentage (100);
  return correct;
}

bool AllyStar_Upgrade::MD5_Check()
{
  setDownload_state (BOOT_MD5_Check);
  setDownload_percentage (0);
  QByteArray md5Str = QCryptographicHash::hash(firmwaredata_.mid (16),QCryptographicHash::Md5);
  if(md5Str.compare (firmwaredata_.mid (0,16),Qt::CaseSensitivity::CaseInsensitive)==0)
  {
    setDownload_percentage (100);
    return true;
  }
  else
  {
    return false;
  }
}

bool AllyStar_Upgrade::Query_Version()
{
  setDownload_state (BOOT_Query_Version);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0x0A,0x04};
  ushort length=0;
  char * payload=nullptr;
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,2+2+2+32+2);
  if(read.size ()!=40)
  {
    QERROR ("Query_Version Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    return false;
  }
  QINFO("[SWVER] "+QString("%1").arg (QString(read.toHex ().mid (6,16)))
       +"  [HWVER] "+QString("%2").arg (QString(read.toHex ().mid (22,16)))
        );
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::BOOT3_UAHRY_SET(int baudrate,QByteArray readback)
{
  setDownload_state (BOOT_UAHRY_SET);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x03};
  ushort length=8;
  char * payload=(char*)&baudrate;
  char pd[8]={0};
  memcpy (pd,payload,4);
  memcpy (pd+4,payload,4);
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,pd).sendbytarry,0,true);
  io_->io_asyncsleep (300);
  io_->set_BuadRate (baudrate);
  io_->io_asyncsleep (200);
//  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT);//F1D905010200F403FF16
//    if(!read.contains (readback/*QByteArray::fromHex ("F1D905010200F403FF16")*/))
//    {
//      QERROR ("BOOT3_UAHRY_SET Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
//      return false;
//    }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::BOOT3_FLSEARSE_SET()
{
  setDownload_state (BOOT_FLSEARSE_SET);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x08};
  ushort length=4;
  uchar  payload[4]={0x00,0x02,0x00,0x80};
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,2+2+2+4+2);//F1D905010200F408041B
  if(!read.contains (QByteArray::fromHex ("F1D905010200F408041B")))
  {
    QERROR ("BOOT3_FLSEARSE_SET Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    return false;
  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::CFG_SIMPLERST()
{
  setDownload_state (CFG_SIMPLERST_);
  QByteArray hex="F1D906400100004721";
  io_->io_Write (QByteArray::fromHex (hex),0,true);
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::BOOT3_RFF_SET(int baudrate)
{
  setDownload_state (BOOT_RFF_SET);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x00};
  ushort length=8;
  uchar  payload[8]={0x80,0xBA,0x8c,0x01};//,0x00,0x08,0x07,0x00
  char *bd=(char*)&baudrate;
  memcpy (payload+4,bd,4);
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)payload).sendbytarry,0,true);
  io_->io_asyncsleep (300);
  io_->set_BuadRate (baudrate);
  io_->io_asyncsleep (200);
//  QByteArray read= io_->io_Read (2*MAX_READ_TIMEOUT,2+2+2+8+2);//F1D905010200F408041B
//  if(!read.contains (QByteArray::fromHex ("F1D905010200F400FC13")))
//  {
//    QERROR ("BOOT3_FLSEARSE_SET Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
//    return false;
//  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::BOOT2_SPIFLSINFO_POLL()
{
  setDownload_state (BOOT_SPIFLSINFO_POLL);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x07};
  ushort length=1;
  uchar  payload[1]={0x02};
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (2*MAX_READ_TIMEOUT,2+2+2+1+2);
//  if(!read.contains (QByteArray::fromHex ("F1D9F4070400000A0E213844")))
//  {
//    QERROR ("BOOT_SPIFLSINFO_POLL Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
//    return false;
//  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::BOOT2_SPIFLS_WRSR_SET()
{
  setDownload_state (BOOT_SPIFLS_WRSR_SET);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x13};
  ushort length=5;
  uchar  payload[5]={0x00,0x00,0x02,0x00,0x00};
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (2*MAX_READ_TIMEOUT,2+2+2+5+2);
  if(!read.contains (QByteArray::fromHex ("F1D905010200F4130F26")))
  {
    QERROR ("BOOT_SPIFLS_WRSR_SET Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    return false;
  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::FUR_CMD_BOOT2_FWPG_SET(int size,uint16_t number,QByteArray data)
{
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0xF4,0x05};
  uint offset=start_address+size;
  uchar* address=(uchar*)&offset;
  char *payload=new char[data.size ()+6]{0};
  memcpy (payload,address,4);
  memcpy (payload+4,(char*)&number,2);
  if(number==0)
  {
    data=nullptr;
  }
  else
  {
    memcpy (payload+6,data.data (),data.size ());
  }
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,data.size ()+6,payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,10);//F1D905010200F4050118
  if(!read.contains (QByteArray::fromHex ("F1D905010200F4050118")))
  {
    QERROR ("BOOT_FWPG_SET Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    return false;
  }

  return true;
}

bool AllyStar_Upgrade::Download()
{
  setDownload_state (BOOT_FWPG_SET);
  int padd=0x410;
  int totalsendsize=padd;
  int PKG=1024;
  int totalsize=firmwaredata_.size();
  ushort number=0;
  if(!FUR_CMD_BOOT2_FWPG_SET(0,0,QByteArray())) {return false;}
  while (totalsize-totalsendsize>0)
  {
     QTime start_t=QTime::currentTime ();
    if(downloadstoping)
    {
      return false;
    }
    QByteArray senddata=firmwaredata_.mid (totalsendsize,PKG);
    totalsendsize+=PKG;
    number++;
    if(!FUR_CMD_BOOT2_FWPG_SET(totalsendsize-padd,number,senddata)) {return false;}
    setDownload_percentage ((totalsendsize)*100/(totalsize));
    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg (totalsendsize>totalsize?totalsize:totalsendsize).arg (QString::number (1024/ms/1024,'f',1)).arg (totalsize);
    emit flashInfoChanged ();
  }
   if(!FUR_CMD_BOOT2_FWPG_SET(0,number++,firmwaredata_.mid (16,1024))) {return false;}
  return true;
}

bool AllyStar_Upgrade::BOOT_Resert()
{
  setDownload_state (BOOT_RESERT);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0x06,0x40};
  ushort length=1;
  char  payload=0;
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)&payload).sendbytarry,0,true);
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::USER_CFG_PRT(int baudrate)
{
  setDownload_state (USERCFGPRT);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0x06,0x00};
  ushort length=8;
  char *bd=(char*)&baudrate;
  uchar  payload[8]={0x00,0x00,0x00,0x00};
  memcpy (payload+4,bd,4);
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)payload).sendbytarry,0,true);
  io_->io_asyncsleep (300);
  io_->set_BuadRate (baudrate);
  io_->io_asyncsleep (200);
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::USER_Enter_Upgrade_Mode(char mode)
{
  setDownload_state (USEREnterUpgradeMode);
  setDownload_percentage (0);
  uchar SYNC[2]={0xF1,0xD9};
  uchar MeaasgeID[2]={0x06,0x50};
  ushort length=1;
  char  payload=mode;
  io_->io_clear ();
  io_->io_Write (FUR_Binary_MSG_PKG(SYNC,MeaasgeID,length,(char*)&payload).sendbytarry,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT*5,2+2+2+2+2);
  if(!read.contains (QByteArray::fromHex ("F1D90501020006505E87")))
  {
    QERROR ("USER_Enter_Upgrade_Mode Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    return false;
  }
  char ack='C';
  read= io_->io_Read (MAX_READ_TIMEOUT*5,1);
  if(!read.contains (ack))
  {
    QERROR ("USER_Enter_Upgrade_Mode Result Error "+LOGFRONT+QString("%1").arg (QString(read)));
    return false;
  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::USER_Send_SOH_PKG()
{
  setDownload_state (USERSendSOHPKG);
  setDownload_percentage (0);
  char header=0x01;
  char PKG=0x00;
  char PKG_NUM=0xFF;
  QByteArray fnme=filename.toUtf8 ();
  char PAD=0x00;
  QByteArray flen=QByteArray::number (firmwaredata_.size ()-16);
  QByteArray payload;
  payload.append (fnme);
  payload.append (PAD);
  payload.append (flen);
  payload.append (PAD);
  int padsize=128-payload.size ();
  QByteArray payloadpadd(padsize,0);
  payload.append (payloadpadd);
  uint16_t crc= calcrc ((uint8_t*)payload.data (),payload.size ());
  char *crcq=(char*)&crc;
  QByteArray qcrc(crcq,2);
  std::reverse(qcrc.begin (),qcrc.end ());
  QByteArray send;
  send.append (header);
  send.append (PKG);
  send.append(PKG_NUM);
  send.append (payload);
  send.append (qcrc);
  io_->io_Write (send,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,1);//0x06
  if(read!=QByteArray::fromHex ("06"))
  {
    QERROR ("USER_Send_SOH_PKG Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
  }
  read= io_->io_Read (MAX_READ_TIMEOUT,1);           //'C'
  if(!read.contains ('C'))
  {
    QERROR ("USER_Send_SOH_PKG Result Error "+LOGFRONT+QString("%1").arg (QString(read)));
    return false;
  }
  setDownload_percentage (100);
  return true;
}

bool AllyStar_Upgrade::USER_Send_STX_PKG()
{
  setDownload_state (USERSendSTXPKG);

  int totalsendsize=16;
  int totalsize=firmwaredata_.size ();
  int PKG=1024;
  char header=0x02;
  char number=1;
  char PKG_NM=0xFE;
  while (totalsize-totalsendsize>0)
  {
    if(downloadstoping)
    {
      return false;
    }
     QTime start_t=QTime::currentTime ();
    if(totalsize-totalsendsize<1024)
    {
      PKG=totalsize-totalsendsize;
    }
    QByteArray send;
    send.append (header);
    send.append (number);
    send.append (PKG_NM);
    QByteArray data;
    data.append (firmwaredata_.mid (totalsendsize,PKG));
    if(PKG!=1024)
    {
      QByteArray pad(1024-PKG,0x1A);
      data.append (pad);
    }
    send.append (data);
    uint16_t crc= calcrc ((uint8_t*)data.data (),data.size ());
    QByteArray qcrc((char*)(&crc),2);
    std::reverse(qcrc.begin (),qcrc.end ());
    send.append (qcrc);
    io_->io_Write (send,0,true);
    QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,1);//0x06
    if(read!=QByteArray::fromHex ("06"))
    {
      QERROR ("USER_Send_STX_PKG Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
    }
    totalsendsize+=PKG;
    number++;
    PKG_NM--;

    setDownload_percentage ((totalsendsize)*100/(totalsize));
    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg (totalsendsize).arg (QString::number (1024/ms/1024,'f',1)).arg (totalsize);
    emit flashInfoChanged ();
  }

  return true;
}

bool AllyStar_Upgrade::USER_Send_EOT_SOH_PKG()
{
  char EOT=0x04;
  io_->io_Write (QByteArray(&EOT,1),0,true);
  char header=0x01;
  char PKG=0x00;
  char PKG_NUM=0xFF;
  QByteArray fnme(128,0);
  uint16_t crc= calcrc ((uint8_t*)fnme.data (),fnme.size ());
  char *crcq=(char*)&crc;
  QByteArray send;
  send.append (header);
  send.append (PKG);
  send.append(PKG_NUM);
  send.append (fnme);
  send.append (crcq,2);
  io_->io_Write (send,0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,9);//064306431818181818 And  the  module  responds  with  ACK  and  character  “C”  twice  and  CAN  five  times
  if(read!=QByteArray::fromHex ("064306431818181818"))
  {
    QERROR ("USER_Send_EOT_SOH_PKG Result Error "+LOGFRONT+QString("%1").arg (QString(read.toHex ())));
  }
  return true;
}
