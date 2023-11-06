#include "unicorecomm_upgrade.h"

const uint32_t QL_MAGIC_BOOTLOADER = 0xf0f1f2f3;
const uint32_t QL_MAGIC_BOOT_CONFIG = 0xf2f3f4f5;
const uint32_t QL_MAGIC_FW_IMAGE_ITCM = 0xf3f4f5f6;
const uint32_t QL_MAGIC_UNDEF = 0xf6f7f8f9;
const uint16_t QL_IMAGE_HEADER_SIZE = 32;
const uint16_t PACKAGE_MAX_BUFFER_SIZE = 1024;

const char XMODEM_CRC_CHR = 'C';
const int XMODEM_DATA_SIZE_SOH = 1024;
const int XMODEM_MAX_ERR_RETRY_NUM = 5; 	//失败最大重发次数
const int XMODEM_PACKET_NUMBER_MAX = 255;
//const int XMODEM_REPO
const int XMODEM_START_HEADER_SIZE_DEFINE = 1;
const int XMODEM_FRAME_ID_SIZE = 2;
const int XMODEM_DATA_SIZE_DEFINE = XMODEM_DATA_SIZE_SOH;
const int XMODEM_CRC_SIZE = 2;
const int XMODEM_PACKET_SIZE_DEFINE = 	XMODEM_START_HEADER_SIZE_DEFINE +
                                      XMODEM_FRAME_ID_SIZE  +
                                      XMODEM_DATA_SIZE_DEFINE +
                                      XMODEM_CRC_SIZE;

clock_t markTime;
#define START_TIMING() {markTime = clock();}
#define CHECK_OVER_TIMING(MAX_Interval_Ms)  (clock() - markTime) > MAX_Interval_Ms ? false : true

bool Unicorecomm_Upgrade::VerifyImageInfo(FILE* fStream, uint32_t packetageType)
{
  char imageHeaderArray[QL_IMAGE_HEADER_SIZE];
  char imagePacketArray[PACKAGE_MAX_BUFFER_SIZE];
  Ql_Image_Header_Typedef qlImageHeader;

  size_t readLength = fread(imageHeaderArray, sizeof(char), QL_IMAGE_HEADER_SIZE, fStream);
  if (readLength < QL_IMAGE_HEADER_SIZE)
  {
    return false;
  }

  uint8_t* ptr = (uint8_t*)&qlImageHeader;
  for (int i = 0; i < 8; i++)
  {
    uint32_t field = (uint32_t)CovertArrayToUint32(imageHeaderArray, 4 * i);
    memcpy((uint8_t*)(ptr + 4 * i), (uint8_t*)&field, 4);
  }

  //Image Header 校验
  int crcResultNumber = Calculate_CRC16(imageHeaderArray, (uint16_t)(QL_IMAGE_HEADER_SIZE - 4));
  if (crcResultNumber != (qlImageHeader.HeaderCRC & 0xFFFF))
  {
    return false;
  }
  //包头的类型
  if ((qlImageHeader.MagicNumber == packetageType) != true)
  {
    return false;
  }
  //Imgae packet 校验
  int readResultNumber = 0;
  uint32_t checkNum = 0;
  uint32_t packetSize = qlImageHeader.PkgSize;
  uint32_t onceReadData = 0;

  do
  {
    onceReadData = packetSize > PACKAGE_MAX_BUFFER_SIZE ? PACKAGE_MAX_BUFFER_SIZE : packetSize;
    readResultNumber = fread(imagePacketArray, sizeof(char), onceReadData, fStream);
    qDebug()<<"packetSize "<< packetSize<<"readResultNumber "<<readResultNumber;
    for (int i = 0; i <= readResultNumber - 4; i += 4)
    {
      checkNum += CovertArrayToUint32(imagePacketArray, (uint16_t)i);
    }
    packetSize -= packetSize > readResultNumber ? readResultNumber : packetSize;
  } while (packetSize > 0);

  if (checkNum != qlImageHeader.PkgChecksum)
  {
    return false;
  }
  return true;
}

bool Unicorecomm_Upgrade::FwIsValid(FILE* fStream)
{
  if (fStream == NULL)
  {
    return false;
  }

  if (!VerifyImageInfo(fStream, QL_MAGIC_BOOTLOADER))
  {
    return false;
  }

  if (!VerifyImageInfo(fStream, QL_MAGIC_BOOT_CONFIG))
  {
    return false;
  }

  if (!VerifyImageInfo(fStream, QL_MAGIC_UNDEF))
  {
    return false;
  }
  if (!VerifyImageInfo(fStream, QL_MAGIC_FW_IMAGE_ITCM))
  {
    return false;
  }
  return true;
}

Unicorecomm_Upgrade::Unicorecomm_Upgrade(QObject *parent)
    :Unicorecomm_Base{parent}
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

Unicorecomm_Upgrade::Download_State Unicorecomm_Upgrade::download_state() const
{
  return download_state_;
}

void Unicorecomm_Upgrade::init_IO(IODvice *io)
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

bool Unicorecomm_Upgrade::download_stop()
{
  setDownload_state (Download_State::Custom_DownLoad_Stoping);
  downloadstoping=true;
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  Q_EMIT seril->send_Stop ();//stop reading func
  seril->io_asyncsleep (500);
  seril->io_clear ();
  return true;
}

void Unicorecomm_Upgrade::reset_buadRate()
{
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  seril->set_BuadRate(init_baudRate_);
  seril=nullptr;
}

bool Unicorecomm_Upgrade::qGNSS_DownLoad(QString Path, int module)
{
  bool flag=false;
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
  FILE *BFW= fopen (Bootloader_pth.toLocal8Bit (),"rb");
  FILE *FW= fopen (Path.toLocal8Bit (),"rb");
  if (FW == NULL) {QERROR (LOGFRONT+ QString(" File: %1 not exists!").arg (Path));return false;}
  while (1)
  {
    if(downloadstoping)
    {
      break;
    }
    if(!Check_fw_Valid(FW)){break;}//2022 0917 remove check HONG BIN
    if(!Change_BaudRate(download_baudRate_)){break;}
    if(!Soft_Reset()){break;}
    if(!UPG_Request()){break;}
       setDownload_state (S_Send_Bootloader);
    if(!Send_File(BFW)){break;}
    QTime start=QTime::currentTime ();
    QTime end=QTime::currentTime ();
    QByteArray read;
    bool sucess=false;
    while (start.msecsTo (end)<2000)
    {
       end=QTime::currentTime ();
      if(downloadstoping)
      {
        break;
      }
      read+=io_->io_Read (MAX_READ_TIMEOUT,1);
      if(read.contains ("boot>"))
      {
        io_->io_Write (QByteArray("2\n"),0,true);
        io_->io_asyncsleep (100);
        io_->io_Read (MAX_READ_TIMEOUT);
        io_->io_Write (QByteArray("2\n"),0,true);
        read= io_->io_Read (MAX_READ_TIMEOUT,2);
        if(read.contains ('C'))
        {
          sucess=true;
        }
        break;
      }
    }
    if(!sucess){break;}
    setDownload_state (S_Send_FW);
    if(!Send_File(FW)){break;}
    start=QTime::currentTime ();
    end=QTime::currentTime ();
    sucess=false;
    while (start.msecsTo (end)<2000)
    {
      end=QTime::currentTime ();
      if(downloadstoping)
      {
        break;
      }
      read+=io_->io_Read (MAX_READ_TIMEOUT,1);
      if(read.contains ("boot>"))
      {
        sucess=true;
        break;
      }
    }
    if(!sucess){break;}
    if(!F_SoftReset()){break;}
    flag=true;
    break;
  }
  fclose (BFW);
  fclose (FW);
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

void Unicorecomm_Upgrade::qGNSS_SetDownLoadBaudRate(int baudrate)
{
  download_baudRate_=baudrate;
  flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
  emit flashInfoChanged ();
}

void Unicorecomm_Upgrade::setDownload_state(const Unicorecomm_Upgrade::Download_State &newDownload_state)
{
  download_state_ = newDownload_state;
  switch (newDownload_state)
  {
    case State_Unstart:
      statekeyvalue()->setKey (tr("Ready to download"));
      break;
    case S_Check_FW_Vaild:
      statekeyvalue()->setKey (tr("Check Firmware Valid..."));
      break;
    case S_Changed_BaudRate:
      statekeyvalue()->setKey (tr("Changed BaudRate..."));
      break;
    case S_UPG_Request:
      statekeyvalue()->setKey (tr("Upgrade Request(wait reset or reboot)..."));
      break;
    case S_Send_Bootloader:
      statekeyvalue()->setKey (tr("Download Bootloader..."));
      break;
    case S_Send_FW:
      statekeyvalue()->setKey (tr("Download Firmware..."));
      break;
    case State_Finished:
      statekeyvalue()->setKey (tr("Finished..."));
      break;
    case State_CMD_Fail:
      statekeyvalue()->setKey (tr("Download Fail"));
      break;
    case Custom_DownLoad_Stoping:
      statekeyvalue()->setKey (tr("Downloading is stopping"));
      break;
    case Custom_DownLoad_Stoped:
      statekeyvalue()->setKey (tr("Downloading is stopped"));
      break;
  }
  statekeyvalue()->setValue ((int)newDownload_state);
  emit statekeyvalueChanged ();
  emit download_stateChanged(download_state_);
}

void Unicorecomm_Upgrade::setDownload_stateStr(QString state)
{
  statekeyvalue()->setKey (state);
  emit statekeyvalueChanged ();
  emit download_stateChanged(download_state_);
}
Unicorecomm_Upgrade::XmodemState Unicorecomm_Upgrade::ReadReciveState()
{
  XmodemState resultCode = XMODEM_NAK;

  do
  {
    if(downloadstoping)
    {
      break;
    }
    char code =io_->io_Read (MAX_READ_TIMEOUT,1).data ()[0];//ReadData();
    if ((XmodemState)code == XMODEM_ACK ||
        (XmodemState)code == XMODEM_NAK ||
        (XmodemState)code == XMODEM_CAN)
    {
      resultCode = (XmodemState)code;
      break;
    }

  } while (true);

  return resultCode;
}

void Unicorecomm_Upgrade::LoadPackage(char startHeader, char packageNumber, char* package, const char* data, int xmodemDataSize)
{
  char* ptr = package;
  char negationPackageNumber = (uint8_t)(XMODEM_PACKET_NUMBER_MAX - packageNumber);
  *ptr++ = startHeader;
  *ptr++ = packageNumber;
  *ptr++ = negationPackageNumber;
  memcpy(ptr, (const char*)data, xmodemDataSize);
  //CRC校验
  uint16_t crc = Calculate_CRC16(data, xmodemDataSize);
  ptr = (ptr + xmodemDataSize);
  char crrHigh = (char)(crc >> 8);
  char crcLow = (char)(crc);
  *ptr++ = crrHigh;
  *ptr = crcLow;
}
bool Unicorecomm_Upgrade::Transmit(FILE* fStream)
{
  char bufferData[XMODEM_DATA_SIZE_DEFINE];
  char package[XMODEM_PACKET_SIZE_DEFINE];

  int remainLength;
  char packageNumber = 1;
  int retryNum = 0;
  fseek(fStream, 0, SEEK_END);
  remainLength = ftell(fStream);
  fseek(fStream, 0, SEEK_SET);
  //发送<SOH>
  int ttsize=remainLength;
  int vaildData;
  while (remainLength > 0)
  {
    if(downloadstoping)
    {
      return false;
    }
     QTime start_t=QTime::currentTime ();
    if (retryNum == 0)
    {
     vaildData = remainLength >= XMODEM_DATA_SIZE_DEFINE ? XMODEM_DATA_SIZE_DEFINE : remainLength;
      fread(bufferData, sizeof(char), vaildData, fStream);
      for (int i = (int)vaildData; i < XMODEM_DATA_SIZE_DEFINE; i++)
      {
        bufferData[i] = 0x1A;
      }
      remainLength -= vaildData;
    }
    LoadPackage((char)XMODEM_SOH, packageNumber, package, bufferData);
    io_->io_Write (QByteArray(package,XMODEM_PACKET_SIZE_DEFINE),0,true);
    XmodemState xState = ReadReciveState();

    setDownload_percentage((ttsize-remainLength)*100/ttsize);
    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (ttsize-remainLength)
                                      .arg (ttsize)
                                      .arg (QString::number (vaildData/ms/1024,'f',1))
        ;
    emit flashInfoChanged ();

    if (XMODEM_ACK == xState)
    {
      retryNum = 0;
      packageNumber++;
    }
    else if (XMODEM_CAN == xState)
    {
      return false;
    }
    else if (XMODEM_NAK == xState)
    {
      retryNum++;
      if (retryNum > XMODEM_MAX_ERR_RETRY_NUM)
      {
        return false;
      }
    }
  }

  //发送<EOT>
  while (true)
  {
    //装填数据
    package[0] = (char)XMODEM_EOT;
    //SendData(package, 1);
    io_->io_Write (QByteArray(package,1),0,true);
    break;
  }
  return true;
}

bool Unicorecomm_Upgrade::Check_fw_Valid(FILE *fwStream)
{
  setDownload_state (S_Check_FW_Vaild);
  setDownload_percentage (0);
  if (FwIsValid(fwStream) != true) //检测Fareware文件
  {
    QERROR (LOGFRONT+ QString(" Firmware not Valid... "));
    return false;
  }
  setDownload_percentage (100);
  return true;
}

bool Unicorecomm_Upgrade::Change_BaudRate(int BaudRate)
{
  setDownload_state (S_Changed_BaudRate);
  setDownload_percentage (0);
  io_->set_BuadRate (BaudRate);
  io_->io_asyncsleep (200);
  QString cmd=QString("$CFGPRT,1,0,%1,3,3\r\n").arg (BaudRate);
  io_->io_Write (cmd.toUtf8 (),0,true);
  setDownload_percentage (100);
  return true;
}

bool Unicorecomm_Upgrade::Soft_Reset()
{
  QString cmd=QString("$reset,hf0,hff\r\n");
  io_->io_Write (cmd.toUtf8 (),0,true);
  return true;
}

bool Unicorecomm_Upgrade::Obtaining_module_information()
{
  QString cmd=QString("$PDTINFO\r\n");
  io_->io_Write (cmd.toUtf8 (),0,true);
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,1024);
  return true;
}

bool Unicorecomm_Upgrade::UPG_Request()
{
  setDownload_state (S_UPG_Request);
  QTime startt=QTime::currentTime ();
  QTime endt;
  QByteArray semd;
   setDownload_percentage (0);
  for(int i=0;i<2048;i++)
  {
    semd.append (QByteArray::fromHex ("4D215400"));
  }
  while (startt.msecsTo (endt)<=3000)
  {
    endt=QTime::currentTime ();
    if(io_->io_Write (semd,0,true))
    {
      QERROR (LOGFRONT+ QString("Write fail  "));
      return false;
    }
  }
  QByteArray read=io_->io_Read (MAX_READ_TIMEOUT);
  if(!read.contains ("YC"))
  {
    QERROR (LOGFRONT+ QString("Read fail : %1... ").arg (QString(read.toHex ()) ));
    return false;
  }
  read=io_->io_Read (MAX_READ_TIMEOUT,2);
  setDownload_percentage (100);

  return true;
}

bool Unicorecomm_Upgrade::Send_File(FILE *Stream)
{
  fseek(Stream, 0, SEEK_SET);
  if (!Transmit(Stream))
  {
    QERROR (LOGFRONT+ QString(" Download Firmware Fail... "));
    return false;
  }
  return true;
}

bool Unicorecomm_Upgrade::F_SoftReset()
{
  QString cmd=QString("5\r\n");
  io_->io_Write (cmd.toUtf8 (),0,true);
  io_->io_asyncsleep (400);
  io_->io_Write (cmd.toUtf8 (),0,true);
  io_->io_asyncsleep (400);
  io_->io_Write (cmd.toUtf8 (),0,true);
  io_->io_asyncsleep (400);
  io_->io_Write (cmd.toUtf8 (),0,true);
  io_->io_asyncsleep (400);
  return true;
}

bool Unicorecomm_Upgrade::Check_NMEAOutPut()
{
  return true;
}
