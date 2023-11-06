/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_brom_3335.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-18      victor.gong
* ***************************************************************************/
#include "mtk_brom_3335.h"

MTK_BROM_3335::MTK_BROM_3335(MTK_Base *parent)
    : MTK_Base{parent}
{
  QMap <QString,QVariant> map;
  map.insert ("PORTNAME",QVariant());
  map.insert ("DOWNLOADBAUDRATE",QVariant());
  map.insert ("DownLoadMode",QVariant());

  map.insert ("Flash_manufacturer_ID",QVariant());
  map.insert ("Flash_device_ID1",QVariant());
  map.insert ("Flash_device_ID2",QVariant());
  map.insert ("Flash_mount_status",QVariant());
  map.insert ("FLASH_MANUFACTURE_CODE",QVariant());
  map.insert ("Flash_base_address",QVariant());
  map.insert ("Flash_size",QVariant());
  map.insert ("TooltipTXT",QVariant());
  setFlashInfo (map);

  qRegisterMetaType<CFG>();
  qRegisterMetaType<CFG*>();
}

MTK_BROM_3335::~MTK_BROM_3335()
{

}

const char* MTK_BROM_3335::getFormatTotalLength(BROM_BASE::QGNSS_Model mtkmodel)
{
  return "00400000";
}

void MTK_BROM_3335::MTK_init_IO(IODvice *io)
{
  if(io)
  {
    io_=io;

    IOserial *seril=static_cast<IOserial *>(io_.data ());
    RTS_=seril->get_RTS ();
    connect (seril,&IOserial::errorOccurred,this,[&](QString err){
      QERROR ("serialport error:"+err);
    });
    connect (seril,&IOserial::rw_InfoChanged,this,&MTK_BROM_3335::rw_InfoChanged);
    init_baudRate_=seril->get_baudRate ();
    flashInfo()["PORTNAME"]=seril->portName ();
    emit flashInfoChanged ();
    seril=nullptr;
  }

}

bool MTK_BROM_3335::load_file(QString DA_Path, QString flash_cfg_path)
{
  QFile DAfile(DA_Path);
  if(!DAfile.exists ())
  {
    QERROR (LOGFRONT+ QString("DA File: %1 not exists!").arg (DA_Path));
    return false;
  }
  else
  {
    if(!DAfile.open (QIODevice::ReadOnly))
    {
      QERROR (LOGFRONT+QString("DA File: %1 open fail !").arg (DA_Path));
      return false;
    }
    QByteArray readbytes=DAfile.readAll ();
    cfg_.DA_size =readbytes.size ();
    cfg_.DA_data=readbytes;
    DAfile.close ();
  }
  QFile cfgfile(flash_cfg_path);
  if(!cfgfile.exists ())
  {
    QERROR (LOGFRONT+QString("cfg file: %1 not exists!").arg (flash_cfg_path));
    return false;
  }
  else
  {
    if(!cfgfile.open (QIODevice::ReadOnly))
    {
      QERROR (LOGFRONT+QString("cfg file: %1 open fail !").arg (flash_cfg_path));
      return false;
    }
    flash_downloadcfg_=cfgfile.readAll ().data ();
    cfgfile.close ();
    QFileInfo fileinfo(flash_cfg_path);
    if(!cfg_parse(flash_downloadcfg_,fileinfo.absoluteDir ().path ()))
    {
      QERROR (QString("cfg path:%1 parser fail !").arg (fileinfo.absoluteDir ().path ()));
      return false;
    }
  }
  return true;
}

/**
 * @brief MTK_BROM_3335::handshake_with_Module
 * After the module is powered on, it waits for 150 milliseconds for 0xA0 sent by the host. If it fails to receive
 * the 0xA0, the handshake fails.
 * ***********************************************************************************
 * keeps SEND0 0xA0, and the module is powered on.
 * RECEIVE0    0x5F.
 * SEND1       0x0A.
 * RECEIVE1    0xF5.
 * SEND2       0x50.
 * RECEIVE2    0xAF.
 * SEND3       0x05.
 * RECEIVE3    0xFA.
 * @return
 */
bool MTK_BROM_3335::handshake_with_Module()
{
  setDownload_state (Download_State::State_handshake_with_Module);
  int tms  =ST_HandShakeWithModul.WAIT_TIMES;
  int times=ST_HandShakeWithModul.WAIT_TIMES;
  bool b_RECEIVE0=false;
  int timeout=10/*150*/;
  //[1]
  int i=0;
  while(times--)
  {
    if(downloadstoping)
    {
      return false;
    }
    setDownload_percentage ((i+1)*100/(tms+3));
    QTime start=QTime::currentTime ();
    if(io_->syncWriteRead (HEX2BYTES(ST_HandShakeWithModul.SEND0),timeout,0)==HEX2BYTES (ST_HandShakeWithModul.RECEIVE0))
    {
      b_RECEIVE0=true;

      break;
    }

    io_->io_clear ();
    //2022-06-18 Victor fix bug tools-021  the loop exit fast if the data is read immediately
    QTime end=QTime::currentTime ();
    int time=start.msecsTo (end);
    if(time<timeout)
    {
      io_->io_asyncsleep (timeout-time);
    }
    i++;
  }
  if(!b_RECEIVE0)
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_handshake_with_Module)+LOGFRONT+QString(" send:%1 total:%2 times not RECEIVE:%3")
                                                                                                   .arg (ST_HandShakeWithModul.SEND0)
                                                                                                   .arg (ST_HandShakeWithModul.WAIT_TIMES)
                                                                                                   .arg (ST_HandShakeWithModul.RECEIVE0)
           )
    return false;
  }
  //[2]
  QByteArray syncread;
  syncread=io_->syncWriteRead (HEX2BYTES(ST_HandShakeWithModul.SEND1),MAX_READ_TIMEOUT,1);

  if(syncread!=HEX2BYTES (ST_HandShakeWithModul.RECEIVE1))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_handshake_with_Module)+LOGFRONT+QString(" send:%1 RECEIVE:%2 unequal %3")
                                                                                                   .arg (ST_HandShakeWithModul.SEND1)
                                                                                                   .arg (QString(syncread.toHex ()))
                                                                                                   .arg (ST_HandShakeWithModul.RECEIVE1)
           )
    return false;
  }

  setDownload_percentage ((tms+1)*100/(tms+3));
  //[3]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_HandShakeWithModul.SEND2),MAX_READ_TIMEOUT,1);
  i++;
  setDownload_percentage ((i+1)*100/(tms+3));
  if(syncread!=HEX2BYTES (ST_HandShakeWithModul.RECEIVE2))
  {

    QERROR(DOWNSTATE(Download_State,Download_State::State_handshake_with_Module)+LOGFRONT+QString(" send:%1 RECEIVE:%2 unequal %3")
                                                                                                   .arg (ST_HandShakeWithModul.SEND2)
                                                                                                   .arg (QString(syncread.toHex ()))
                                                                                                   .arg (ST_HandShakeWithModul.RECEIVE2)
           )
    return false;
  }
  setDownload_percentage ((tms+2)*100/(tms+3));
  //[4]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_HandShakeWithModul.SEND3),MAX_READ_TIMEOUT,1);

  if(syncread!=HEX2BYTES (ST_HandShakeWithModul.RECEIVE3))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_handshake_with_Module)+LOGFRONT+QString(" send:%1 RECEIVE:%2 unequal %3")
                                                                                                   .arg (ST_HandShakeWithModul.SEND3)
                                                                                                   .arg (QString(syncread.toHex ()))
                                                                                                   .arg (ST_HandShakeWithModul.RECEIVE3)
           )
    return false;
  }
  setDownload_percentage ((tms+3)*100/(tms+3));
  return true;
}
/**
 * @brief MTK_BROM_3335::disable_Modeule_WDT
 * The following diagram illustrates how to disable module WDT to avoid module reboot.
 * SEND0       0xD2
 * RECEIVE0    0xD2
 * SEND1       0xA2080000  SEND(Module WDT register address, 4 bytes)
 * RECEIVE1    0xA2080000  RECEIVE(Module WDT register address, 4 bytes)
 * SEND2       0x00000001  4 bytes
 * RECEIVE2[0] 0x00000001  4 bytes
 * RECEIVE2[1] 0x0001      RECEIVE(Brom status, 2 bytes)
 * SEND3       0x0010      WDT value, 2 bytes
 * RECEIVE3[0] 0x0010      RECEIVE(WDT value,   2 bytes)
 * RECEIVE3[1] 0x0001      RECEIVE(Brom status, 2 bytes)
 * @return
 **/
bool MTK_BROM_3335::disable_Modeule_WDT()
{
  setDownload_state (Download_State::State_disable_Modeule_WDT);
  QByteArray syncread;
  setDownload_percentage (0);
  //[1]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_Disable_Modeule_WDT.SEND0),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_Disable_Modeule_WDT.RECEIVE0))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                 .arg (ST_Disable_Modeule_WDT.SEND0)
                                                                                                 .arg (QString(syncread.toHex ()))
                                                                                                 .arg (ST_Disable_Modeule_WDT.RECEIVE0)
           )
    return false;
  }
  setDownload_percentage (25);
  //[2]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_Disable_Modeule_WDT.SEND1),MAX_READ_TIMEOUT,4);
  if(syncread!=HEX2BYTES (ST_Disable_Modeule_WDT.RECEIVE1))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                 .arg (ST_Disable_Modeule_WDT.SEND1)
                                                                                                 .arg (QString(syncread.toHex ()))
                                                                                                 .arg (ST_Disable_Modeule_WDT.RECEIVE1)
           )
    return false;
  }
  setDownload_percentage (50);
  //[3]
  io_->io_Write (HEX2BYTES(ST_Disable_Modeule_WDT.SEND2),MAX_WRITE_TIMEOUT,true);
  syncread=io_->io_Read (MAX_READ_TIMEOUT,6);
  if(syncread.mid (0,4)!=HEX2BYTES (ST_Disable_Modeule_WDT.RECEIVE2[0]))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                 .arg (ST_Disable_Modeule_WDT.SEND2)
                                                                                                 .arg (QString(syncread.mid (0,4).toHex ()))
                                                                                                 .arg (ST_Disable_Modeule_WDT.RECEIVE2[0])
           )
    return false;
  }
  if(syncread.mid (4,2)==HEX2BYTES (BROM_ERROR_))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# RECEIVE Brom status:%1 == BROM_ERROR:%2")
                                                                                                 .arg (QString(syncread.mid (4,2).toHex ()))
                                                                                                 .arg (BROM_ERROR_)
           )
    return false;
  }
  setDownload_percentage (75);
  //[4]
  io_->io_Write (HEX2BYTES(ST_Disable_Modeule_WDT.SEND3),MAX_WRITE_TIMEOUT,true);
  syncread=io_->io_Read (MAX_READ_TIMEOUT,4);
  if(syncread.mid (0,2)!=HEX2BYTES (ST_Disable_Modeule_WDT.RECEIVE3[0]))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                 .arg (ST_Disable_Modeule_WDT.SEND3)
                                                                                                 .arg (QString(syncread.mid (0,2).toHex ()))
                                                                                                 .arg (ST_Disable_Modeule_WDT.RECEIVE3[0])
           )
    return false;
  }
  if(syncread.mid (2,4)==HEX2BYTES (BROM_ERROR_))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# RECEIVE Brom status:%1 == BROM_ERROR:%2")
                                                                                                 .arg (QString(syncread.mid (2,4).toHex ()))
                                                                                                 .arg (BROM_ERROR_)
           )
    return false;
  }
  setDownload_percentage (100);
  return true;
}
/**
 * @brief MTK_BROM_3335::send_DA_File_to_Module
 * The following diagram illustrates how to send a DA file to the module. You can use either of the two DA
 *files provided: one requires the baud rate of 921600 bps for transmission, and the other requires the baud
 *rate of 115200 bps.
 * SEND0       D7
 * RECEIVE0    D7
 * SEND1       04204000   SEND(DA run/start address, 4 bytes)
 * RECEIVE1    04204000   RECEIVE(DA run/start address, 4 bytes)
 * SEND2                  SEND(DA length, 4 bytes)
 * RECEIVE2               RECEIVE(DA length, 4 bytes)
 * SEND3       00000000
 * RECEIVE3[0] 00000000
 * RECEIVE3[1] 0000       responds with Brom status
 * SEND(DA data, 1024 bytes)
 * .
 * .
 * .
 * SEND(DA data, length<=1024 bytes)
 * RECEIVE(Brom checksum of received DA, 2 bytes )
 * RECEIVE(Brom status, 2 bytes)1)
 * @return
 */
bool MTK_BROM_3335::send_DA_File_to_Module()
{
  setDownload_state (Download_State::State_send_DA_File_to_Module);
  QByteArray syncread;
  setDownload_percentage (0);
  //[1]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_send_DA_File_to_Module.SEND0),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_send_DA_File_to_Module.RECEIVE0))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal RECEIVE:%3")
                                                                                                    .arg (ST_send_DA_File_to_Module.SEND0)
                                                                                                    .arg (QString(syncread.toHex ()))
                                                                                                    .arg (ST_send_DA_File_to_Module.RECEIVE0)
           )
    return false;
  }
  //[2]
  syncread=io_->syncWriteRead (HEX2BYTES(ST_send_DA_File_to_Module.SEND1),MAX_READ_TIMEOUT,4);
  if(syncread!=HEX2BYTES (ST_send_DA_File_to_Module.RECEIVE1))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal RECEIVE:%3")
                                                                                                    .arg (ST_send_DA_File_to_Module.SEND1)
                                                                                                    .arg (QString(syncread.toHex ()))
                                                                                                    .arg (ST_send_DA_File_to_Module.RECEIVE1)
           )
    return false;
  }
  int hexbinsize=cfg_.DA_size;
  char *binsize_c=reinterpret_cast<char*>(&hexbinsize);// ABCD
  QByteArray DA_hex_size;
  //DCBA
  DA_hex_size.append (binsize_c[3]).append (binsize_c[2]).append (binsize_c[1]).append (binsize_c[0]);
  syncread=io_->syncWriteRead (DA_hex_size,MAX_READ_TIMEOUT,4);
  if(syncread!=DA_hex_size)
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal RECEIVE:%3")
                                                                                                    .arg (QString(DA_hex_size.toHex ()))
                                                                                                    .arg (QString(syncread.toHex ()))
                                                                                                    .arg (QString(syncread.toHex ()))
           )
    return false;
  }

  io_->io_Write (HEX2BYTES(ST_send_DA_File_to_Module.SEND3),MAX_WRITE_TIMEOUT,true);
  syncread= io_->io_Read (MAX_READ_TIMEOUT,4);
  if(syncread!=HEX2BYTES (ST_send_DA_File_to_Module.RECEIVE3[0]))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("#  RECEIVE:%1 unequal SEND:%2")
                                                                                                    .arg (QString(syncread.toHex ()))
                                                                                                    .arg (ST_send_DA_File_to_Module.RECEIVE3[0])
           )
    return false;
  }
  syncread= io_->io_Read (MAX_READ_TIMEOUT,2);
  if(syncread==HEX2BYTES (BROM_ERROR_))
  {
    QString read=syncread.toHex ();
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("#  RECEIVE:%1 == BROM_ERROR: %2")
                                                                                                    .arg (QString(syncread.toHex ()))
                                                                                                    .arg (BROM_ERROR_)
           )
    return false;
  }
  int remainder=cfg_.DA_size%1024;
  QByteArray DAbyte=cfg_.DA_data;

  for(int i=0;i<cfg_.DA_size/1024;i++)
  {
    QTime start_t=QTime::currentTime ();
    io_->io_Write (DAbyte.mid (i*1024,1024),MAX_WRITE_TIMEOUT,true);
    if(remainder)
    {
      setDownload_percentage (i*100/(cfg_.DA_size/(1024+1)));
    }
    else
    {
      setDownload_percentage (i*100/(cfg_.DA_size/1024));
    }


    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg ((i+1)*1024).arg (QString::number (1024/ms/1024,'f',1)).arg (cfg_.DA_size);
    emit flashInfoChanged ();
  }
  if(remainder)
  {
    QTime start_t=QTime::currentTime ();
    io_->io_Write (DAbyte.right (remainder),MAX_WRITE_TIMEOUT,true);
    setDownload_percentage (100);

    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg (cfg_.DA_size).arg (QString::number (remainder/ms/1024,'f',1)).arg (cfg_.DA_size);
    emit flashInfoChanged ();
  }
  uint16_t local_check_sum = DA_compute_checksum(reinterpret_cast<uint8_t*>( DAbyte.data ()), DAbyte.size ());
  uchar * checksum=reinterpret_cast<uchar*>(&local_check_sum );
  QByteArray qchecksum;
  qchecksum.append (checksum[1]).append (checksum[0]);
  QByteArray read_checksum= io_->io_Read (MAX_READ_TIMEOUT,2);
  if(qchecksum!=read_checksum)
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("# DA_CHECKSUM RECEIVE:%1 unequal DA_COMPUTE_CHECKSUM %2")
                                                                                                    .arg (QString(read_checksum.toHex (' ').toUpper ()))
                                                                                                    .arg (QString(qchecksum.toHex (' ').toUpper ()))
           )

    return false;
  }
  QByteArray brom_status= io_->io_Read (MAX_READ_TIMEOUT,2);
  if(brom_status==QByteArray::fromHex (BROM_ERROR_))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_send_DA_File_to_Module)+LOGFRONT+QString("brom_status:%1 == BROM_ERROR:%2 ")
                                                                                                    .arg (QString(brom_status.toHex (' ')))
                                                                                                    .arg (BROM_ERROR_)
           );
    return false;
  }
  return true;
}

/**
 * @brief MTK_BROM_3335::jump_to_DA
 * The following diagram illustrates how to inject the DA and how to execute it.
 * SEND0       D5
 * RECEIVE0    D5
 * SEND1       04204000
 * RECEIVE1[0] 04204000
 * RECEIVE1[1] Brom status must not be BROM_ERROR.
 * RECEIVE1[2] C0
 * @return
 */
bool MTK_BROM_3335::jump_to_DA()
{
  setDownload_state (Download_State::State_jump_to_DA);
  setDownload_percentage (0);
  QByteArray syncread;
  syncread=io_->syncWriteRead (HEX2BYTES(ST_jump_to_DA.SEND0),MAX_READ_TIMEOUT,1);
  setDownload_percentage (25);
  if(syncread!=HEX2BYTES (ST_jump_to_DA.RECEIVE0))
  {

    QERROR(DOWNSTATE(Download_State,Download_State::State_jump_to_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                        .arg (ST_jump_to_DA.SEND0)
                                                                                        .arg (QString(syncread.toHex ()))
                                                                                        .arg (ST_jump_to_DA.RECEIVE0)
           )
    return false;
  }
  syncread=io_->syncWriteRead (HEX2BYTES(ST_jump_to_DA.SEND1),MAX_READ_TIMEOUT,4);
  setDownload_percentage (50);
  if(syncread!=HEX2BYTES (ST_jump_to_DA.RECEIVE1[0]))
  {

    QERROR(DOWNSTATE(Download_State,Download_State::State_jump_to_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                        .arg (ST_jump_to_DA.SEND1)
                                                                                        .arg (QString(syncread.toHex ()))
                                                                                        .arg (ST_jump_to_DA.RECEIVE1[0])
           )
    return false;
  }
  syncread=io_->io_Read (MAX_READ_TIMEOUT,2);
  setDownload_percentage (75);
  if(syncread==HEX2BYTES (BROM_ERROR_))
  {

    QERROR(DOWNSTATE(Download_State,Download_State::State_jump_to_DA)+LOGFRONT+QString("#  RECEIVE:%1 = BROM_ERROR:%2")
                                                                                        .arg (QString(syncread.toHex ()))
                                                                                        .arg (BROM_ERROR_)
           )
    return false;
  }

  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_jump_to_DA.RECEIVE1[2]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_jump_to_DA)+LOGFRONT+QString("#  RECEIVE:%1 != %2")
                                                                                        .arg (QString(syncread.toHex ()))
                                                                                        .arg (ST_jump_to_DA.RECEIVE1[2])
           )
    return false;
  }
  setDownload_percentage (100);
  return true;
}
/**
 * @brief MTK_BROM_3335::sync_with_DA
 * The following diagram illustrates how to sync with DA and get the DA report in detail.
 * SEND0      ="3F";
 * RECEIVE0   ="0C";
 * SEND1      ="F3";
 * RECEIVE1   ="3F";
 * SEND2      ="C0";
 * RECEIVE2   ="F3";
 * SEND3      ="0C00";
 * RECEIVE3   ="5A6969";
 * SEND4      ="5A00";
 * RECEIVE4   ="69";
 * SEND5      ="5A";
 * RECEIVE5   ="69";
 * SEND6[0]   ="5A";
 * SEND6[1]   ="C0";
 * RECEIVE6   ="C0";
 * SEND7      ="5A";
 * RECEIVE7   ="5A69";
 * SEND8      ="5A";
 *
 * RECEIVE(Flash manufacturer ID, 2 bytes)
 * RECEIVE(Flash device ID1, 2 bytes)
 * RECEIVE(Flash device ID2, 2 bytes)
 * RECEIVE(Flash mount status, 0, 4 bytes)
 * RECEIVE(Flash base address, 4 bytes)
 * RECEIVE(Flash size, 4 bytes)
 * RECEIVE(0x5A)
 * SEND(0x5A)
 *
 * @return
 */
bool MTK_BROM_3335::sync_with_DA()
{
  setDownload_state (Download_State::State_sync_with_DA);
  QByteArray syncread;
  setDownload_percentage (0);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND0),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE0))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND0)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE0)
           )
    return false;
  }
  setDownload_percentage (10);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND1),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE1))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND1)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE1)
           )
    return false;
  }
  setDownload_percentage (20);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND2),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE2))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND2)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE2)
           )
    return false;
  }
  setDownload_percentage (30);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND3),MAX_READ_TIMEOUT,2);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE3[0]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND3)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE3[0])
           )
    return false;
  }
  setDownload_percentage (40);
  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE3[1]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("#  RECEIVE:%2 != %3")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE3[1])
           )
    return false;
  }
  setDownload_percentage (50);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND4),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE4))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND4)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE4)
           )
    return false;
  }
  setDownload_percentage (60);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND5),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE5))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND5)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE5)
           )
    return false;
  }
  setDownload_percentage (70);
  io_->io_Write (HEX2BYTES(ST_sync_with_DA.SEND6[0]),MAX_WRITE_TIMEOUT,true);


  io_->io_Write (HEX2BYTES(ST_sync_with_DA.SEND6[1]),MAX_WRITE_TIMEOUT,true);
  io_->set_BuadRate(downrate_);

  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE6))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("#  RECEIVE:%1 != %2")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE6)
           )
    return false;
  }
  setDownload_percentage (80);
  syncread=io_->syncWriteRead (HEX2BYTES(ST_sync_with_DA.SEND7),MAX_READ_TIMEOUT,2);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE7))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                          .arg (ST_sync_with_DA.SEND7)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE7)
           )
    return false;
  }
  setDownload_percentage (90);
  io_->io_Write (HEX2BYTES(ST_sync_with_DA.SEND8),MAX_WRITE_TIMEOUT,true);
  syncread =io_->io_Read (MAX_READ_TIMEOUT,18);//18  Flash_INFO size
  if(syncread.size ()!=18)
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("# Read:%1 RECEIVE:%2 != DA report size:%3")
                                                                                          .arg (QString(syncread.toHex (' ').toUpper ()))
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (18)
           )
    return false;
  }
  setDownload_percentage (95);
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_manufacturer_ID,syncread.mid (0,2).toHex ().toUpper ()+'\0',sizeof(char)*(2*2+1));
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_device_ID1,syncread.mid (2,2).toHex ().toUpper ()+'\0',sizeof(char)*(2*2+1));
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_device_ID2,syncread.mid (4,2).toHex ().toUpper ()+'\0',sizeof(char)*(2*2+1));
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_mount_status,syncread.mid (6,4).toHex ().toUpper ()+'\0',sizeof(char)*(4*2+1));
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_base_address,syncread.mid (10,4).toHex ().toUpper ()+'\0',sizeof(char)*(4*2+1));
  memcpy (ST_sync_with_DA.Struct_Flash_INFO_.Flash_size,syncread.mid (14,4).toHex ().toUpper ()+'\0',sizeof(char)*(4*2+1));

  flashInfo()["Flash_manufacturer_ID"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_manufacturer_ID;
  flashInfo()["Flash_device_ID1"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_device_ID1;
  flashInfo()["Flash_device_ID2"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_device_ID2;
  flashInfo()["Flash_mount_status"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_mount_status;
  flashInfo()["Flash_base_address"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_base_address;
  flashInfo()["Flash_size"]=ST_sync_with_DA.Struct_Flash_INFO_.Flash_size;

  flashInfo()["TooltipTXT"]=QString(  "Flash_manufacturer_ID:   0x%1\r\n"
                                  "Flash_device_ID1:        0x%2\r\n"
                                  "Flash_device_ID2:        0x%3\r\n"
                                  "Flash_mount_status:      0x%4\r\n"
                                  "Flash_base_address:      0x%5\r\n"
                                  "Flash_size:              0x%6"
                                  ).arg (flashInfo ()["Flash_manufacturer_ID"].toString ())
                                  .arg (flashInfo ()["Flash_device_ID1"].toString ())
                                  .arg (flashInfo ()["Flash_device_ID2"].toString ())
                                  .arg (flashInfo ()["Flash_mount_status"].toString ())
                                  .arg (flashInfo ()["Flash_base_address"].toString ())
                                  .arg (flashInfo ()["Flash_size"].toString ())
      ;
  emit flashInfoChanged ();
  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_sync_with_DA.RECEIVE9))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("#  RECEIVE:%1 != %2")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_sync_with_DA.RECEIVE9)
           )
    return false;
  }
  io_->io_Write (HEX2BYTES(ST_sync_with_DA.SEND9),MAX_WRITE_TIMEOUT,true);
  setDownload_percentage (100);
  return true;
}
/**
 * @brief MTK_BROM_3335::format_Flash
 * The following diagram illustrates how to format the module’s flash
 * SEND0[0]    D4
 * SEND0[1]    08000000
 * SEND0[2]    00400000
 * RECEIVE0[0] 5A5A
 * RECEIVE0[1] 00000BCD
 * progress    0x01
 * SEND1       5A
 * RECEIVE1    00000BCD
 * progress    0x02
 * .
 * .
 * .
 * SEND        5A
 * RECEIVE    00000BCD
 * progress    0x63
 *
 * SEND2        5A
 * RECEIVE2[0]    00000000
 * progress[1]    0x64
 *
 * SEND3        5A
 * RECEIVE3     5A
 * @return
 */
bool MTK_BROM_3335::format_Flash(Format_mode mode,QString hexbeginaddress,QString hexlength)
{
  QString beginaddress=ST_format_Flash.SEND0[1];//08000000
  QString length      =ST_format_Flash.SEND0[2];//00400000 or 003EF000

  int interbegin=beginaddress.toInt (nullptr,16);
  int totallength=length.toInt (nullptr,16);
  switch (mode)
  {
    case MTK_BROM_3335::Auto_Format:
    {
      switch (current_model ()) {
        //reserve ROM_QUECTEL 128kB
        case QGNSS_Model::LC29HAA:
        case QGNSS_Model::LC29HBA:
        case QGNSS_Model::LC29HCA:
        case QGNSS_Model::LC29HDA:
        case QGNSS_Model::LC29HEA:
        case QGNSS_Model::LC79HAL:
        case QGNSS_Model::LC79HBA:
        case QGNSS_Model::LC02HAA:
        case QGNSS_Model::LC02HBA:
          beginaddress="08000000";
          length="003E0000";
          break;
        //reserve ROM_GNSS_CONFIG 4kB
        case QGNSS_Model::LC29HCA_TB:
          beginaddress="08000000";
          length="003EF000";
          if( !format_help(beginaddress,length)){return false;}
          beginaddress="083FF000";
          length="00001000";
          if( !format_help(beginaddress,length)){return false;}
          return true;
          break;
        default:
          beginaddress="08000000";
          length="003E0000";
          break;
      }
    }
    break;
    case MTK_BROM_3335::Total_Format:
    {
      beginaddress=ST_format_Flash.SEND0[1];//08000000
      length      =ST_format_Flash.SEND0[2];//00400000
    }
    break;
    case MTK_BROM_3335::Manual_Format_Logical: //begin 00000000
    {
      bool tryparse=false;
      int hexinterbegin=hexbeginaddress.toInt (&tryparse,16);
      if(!tryparse)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 tryparse to int false!")
                                                                                              .arg (hexbeginaddress)
               )
        return false;
      }
      tryparse=false;
      int hexinterlength=hexlength.toInt (&tryparse,16);
      if(!tryparse)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Length:%1 tryparse to int false!")
                                                                                              .arg (hexlength)
               )
        return false;
      }

      if(hexinterlength>totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Length:%1 greater than Total Length:%2")
                                                                                              .arg (hexlength)
                                                                                              .arg (length)
               )
        return false;
      }
      if(hexinterbegin<0||hexinterbegin>=totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 less than 0 or begin:%2 greater than or equal Total Length:%3")
                                                                                              .arg (hexbeginaddress)
                                                                                              .arg (hexbeginaddress)
                                                                                              .arg (length)
               )
        return false;
      }
      if(hexinterbegin+hexinterlength>totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 add Length:%2 greater than Total Length:%3!")
                                                                                              .arg (hexbeginaddress)
                                                                                              .arg (hexlength)
                                                                                              .arg (length)
               )
        return false;
      }
      beginaddress=QString::number ((interbegin+hexinterbegin),16);
      length      =QString::number (hexinterlength,16);
    }
    break;
    case MTK_BROM_3335::Manual_Format_Physical://begin 08000000
    {
      bool tryparse=false;
      int hexinterbegin=hexbeginaddress.toInt (&tryparse,16)-interbegin;
      if(!tryparse)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 tryparse to int false!")
                                                                                              .arg (hexbeginaddress)
               )
        return false;
      }
      tryparse=false;
      int hexinterlength=hexlength.toInt (&tryparse,16);
      if(!tryparse)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Length:%1 tryparse to int false!")
                                                                                              .arg (hexlength)
               )
        return false;
      }


      if(hexinterlength>totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Length:%1 greater than Total Length:%2")
                                                                                              .arg (hexlength)
                                                                                              .arg (length)
               )
        return false;
      }
      if(hexinterbegin<0||hexinterbegin>=totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 less than 0||begin:%2 greater than or equal Total Length:%3")
                                                                                              .arg (hexinterbegin)
                                                                                              .arg (hexinterbegin)
                                                                                              .arg (totallength)
               )
        return false;
      }
      if(hexinterbegin+hexinterlength>totallength)
      {
        QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("Begin Address:%1 add Length:%2 greater than Total Length:%3!")
                                                                                              .arg (hexbeginaddress)
                                                                                              .arg (hexlength)
                                                                                              .arg (length)
               )
        return false;
      }
      beginaddress=QString::number ((interbegin+hexinterbegin),16);
      length      =QString::number (hexinterlength,16);
    }
    break;
  }

  return format_help(beginaddress,length);
}

bool MTK_BROM_3335::format_help(QString hexbeginaddress, QString hexlength)
{
  setDownload_state (Download_State::State_format_Flash);
  QByteArray syncread;
  io_->io_Write (HEX2BYTES(ST_format_Flash.SEND0[0]),MAX_WRITE_TIMEOUT,true);
  io_->io_Write (HEX2BYTES(hexbeginaddress.rightJustified (8,QChar('0')).toLocal8Bit ()),MAX_WRITE_TIMEOUT,true);
  io_->io_Write (HEX2BYTES(hexlength.rightJustified (8,QChar('0')).toLocal8Bit ()),MAX_WRITE_TIMEOUT,true);

  syncread =io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE0[0]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  RECEIVE:%1 unequal RECEIVE:%2")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_format_Flash.RECEIVE0[0])
           )
    return false;
  }
  syncread =io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE0[1]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  RECEIVE:%1 unequal RECEIVE:%2")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_format_Flash.RECEIVE0[1])
           )
    return false;
  }

  syncread =io_->io_Read (MAX_READ_TIMEOUT,4);
  if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE0[2])&&syncread!=HEX2BYTES (ST_format_Flash.RECEIVE2[0]))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  RECEIVE:%1 unequal RECEIVE:%2 and unequal RECEIVE:%3")
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_format_Flash.RECEIVE0[2])
                                                                                          .arg (ST_format_Flash.RECEIVE2[0])
           )
    return false;
  }
  ST_format_Flash.progress =io_->io_Read (MAX_READ_TIMEOUT,1).toHex ().toInt (nullptr,16);
  setDownload_percentage (ST_format_Flash.progress);

  int times=100;
  while(times--&& ST_format_Flash.progress<99)
  {

    io_->io_Write (HEX2BYTES(ST_format_Flash.SEND1),MAX_WRITE_TIMEOUT,true);
    syncread =io_->io_Read (MAX_READ_TIMEOUT,4);

    if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE1)&&syncread!=HEX2BYTES (ST_format_Flash.RECEIVE2[0]))
    {
      QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  RECEIVE:%1 unequal %2 and RECEIVE:%1 unequal %3")
                                                                                            .arg (QString(syncread.toHex ()))
                                                                                            .arg (ST_format_Flash.RECEIVE1)
                                                                                            .arg (ST_format_Flash.RECEIVE2[0])
             )
      return false;
    }

    ST_format_Flash.progress =io_->io_Read (MAX_READ_TIMEOUT,1).toHex ().toInt (nullptr,16);
    setDownload_percentage (ST_format_Flash.progress);

    if(ST_format_Flash.progress==0)
    {
      QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  progress==0 true") );
      return false;
    }
  }

  if(ST_format_Flash.progress!=100)
  {
    io_->io_Write (HEX2BYTES(ST_format_Flash.SEND2),MAX_WRITE_TIMEOUT,true);
    syncread =io_->io_Read (MAX_READ_TIMEOUT,4);
    if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE2[0]))
    {
      QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  RECEIVE:%1 unequal RECEIVE:%2")
                                                                                            .arg (QString(syncread.toHex ()))
                                                                                            .arg (ST_format_Flash.RECEIVE2[0])
             )
      return false;
    }

    ST_format_Flash.progress =io_->io_Read (MAX_READ_TIMEOUT,1).toHex ().toInt (nullptr,16);
    setDownload_percentage (ST_format_Flash.progress);
  }

  if(ST_format_Flash.progress!=100)
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("#  progress:%1 unequal100 true")
                                                                                          .arg (ST_format_Flash.progress));
    return false;
  }

  syncread=io_->syncWriteRead (HEX2BYTES(ST_format_Flash.SEND3),MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_format_Flash.RECEIVE3))
  {
    QERROR(DOWNSTATE(Download_State,Download_State::State_format_Flash)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                          .arg (ST_format_Flash.SEND3)
                                                                                          .arg (QString(syncread.toHex ()))
                                                                                          .arg (ST_format_Flash.RECEIVE3)
           )
    return false;
  }

  return true;
}

/**
 * @brief MTK_BROM_3335::send_FW_File_to_Module
 * The following diagram illustrates how to send FW files to the module.
 * SEND0[0]      B2       SEND(FW flash address, 4 bytes)
 * SEND0[1]               SEND(FW total length 1) , 4 bytes)
 * SEND0[2]               SEND(FW packet length 2) , 4 bytes)
 * RECEIVE0[0]   5A
 * RECEIVE0[1]   5A
 * @return
 */
bool MTK_BROM_3335::send_FW_File_to_Module()
{
  for (auto i=cfg_.cfg_roms.begin ();i!=cfg_.cfg_roms.end ();++i)
  {
    if(!send_fw_file_help(*i)){return false;}
  }
  return true;
}

bool MTK_BROM_3335::read_back(const MTK_BROM_3335::Struct_ReadBack *ReadBack, int size)
{

  for(int i=0;i<size;i++)
  {
    setDownload_stateStr (QString("Read Back %1...").arg (i+1));
    if(ReadBack[i].Select)
    {
      io_->io_asyncsleep (20);
      if(!read_back_help(ReadBack[i]))
      {
        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  ReadBack :%1 Fial!")
                                                                            .arg (ReadBack[i].Name)
               )
        continue;
      }
    }
  }
  return true;
}

bool MTK_BROM_3335::read_back_help(const MTK_BROM_3335::Struct_ReadBack &ReadBack)
{
  QByteArray FW_RW;

  const char* sDA_ReadBack_CMD="D6";
  const char* sDA_ACK ="5A";
  int packetLen=4096;
  char * sPacketLength=RS(reinterpret_cast<char*>(&packetLen),sizeof (packetLen));

  QString readpath=ReadBack.FileDir;
  QString fileName=ReadBack.Name;
  QString FilePath=readpath+"/"+fileName;

  int baddress=ReadBack.BeginAddress;
  int endaddress=ReadBack.ReadSize;
  char *cbeginaddress=reinterpret_cast<char*>(&baddress);
  char *cendADR=reinterpret_cast<char*>(&endaddress);
  QByteArray hexBeginADR=QByteArray::fromRawData (RS(cbeginaddress ,4),4);
  QByteArray endADR     =QByteArray::fromRawData (RS(cendADR     ,4),4);
  QByteArray qPacketLength=QByteArray::fromRawData (sPacketLength,sizeof (packetLen));
  io_->io_clear ();
  io_->io_Write (QByteArray::fromHex (sDA_ReadBack_CMD),0,true);
  io_->io_Write (hexBeginADR,0,true);
  io_->io_Write (endADR,0,true);
  io_->io_Write (qPacketLength,0,true);
  int remainSize=ReadBack.ReadSize;
  QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,1);
  if(read!=QByteArray::fromHex (sDA_ACK))
  {
    QERROR("Read Back ERROR: "+LOGFRONT+QString("#  RECEIVE:%1 unequal 5A")
                                                .arg (QString(read.toHex ()))
           )
    return false;
  }

  while(remainSize>0)
  {
    QTime start_t=QTime::currentTime ();
    QByteArray qreadPacket;
    while(qreadPacket.size ()<packetLen)
    {
      QByteArray readtemp=io_->io_Read (MAX_READ_TIMEOUT,packetLen);
      //             if(readtemp.isEmpty ())
      //             {
      //                 break;
      //             }
      qreadPacket+=readtemp;
    }
    uint crc32=CRC_compute_simple_checksum (reinterpret_cast<uint8_t*>(qreadPacket.mid (0,packetLen).data ()),packetLen);
    char * c_crc32=reinterpret_cast<char*>(&crc32);
    QByteArray qcrc=QByteArray::fromRawData (RS(c_crc32,4),sizeof (crc32));
    QByteArray readcrc;
    if(qreadPacket.size ()!=4100)
    {
      readcrc=io_->io_Read (MAX_READ_TIMEOUT,4);
    }
    else
    {
      readcrc=qreadPacket.right (4);
    }
    if(readcrc.toHex ().toInt (nullptr,16)!=qcrc.toHex ().toInt (nullptr,16))
    {
      QERROR("Read Back ERROR: "+LOGFRONT+QString("Current address:%3#  RECEIVE CRC:%1 unequal %2")
                                                  .arg (QString(readcrc.toHex ()))
                                                  .arg (QString(qcrc.toHex ()))
                                                  .arg (QString::number (baddress+(FW_RW.size ()+packetLen),16))
             )
    }
    io_->io_Write (QByteArray::fromHex (sDA_ACK),0,true);
    FW_RW+=qreadPacket.mid (0,packetLen);
    remainSize-=packetLen;
    setDownload_percentage((ReadBack.ReadSize-remainSize)*100/ReadBack.ReadSize);
    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (ReadBack.ReadSize-remainSize)
                                      .arg (ReadBack.ReadSize)
                                      .arg (QString::number (packetLen/ms/1024,'f',1));
    emit flashInfoChanged ();
  }
  QFile file(FilePath);
  file.remove ();
  if(!file.open (QIODevice::WriteOnly))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("# Failed to create File  :%1 ")
                                                                        .arg (FilePath)
           )
    return false;
  }

  file.write (FW_RW);
  file.close ();
  return true;
}

bool MTK_BROM_3335::finish()
{
  setDownload_state(Download_State::State_Finished);
  return true;
}
//************************send fw file  func******************************
bool MTK_BROM_3335::send_fw_file_help(cfg_rom* rom)
{
  int rom_size=0;
  QByteArray begin_address;
  QByteArray data;
  //char *name =rom->name;
  QString name=QString(rom->name);

  if(!memcmp (rom->name,"BootLoader",10))
  {
    ST_Send_FW_File.SEND4="5A";
  }
  else
  {
    ST_Send_FW_File.SEND4="A5";
  }
  if(!rom->selected){return true;}
  setDownload_stateStr (QString("Download %1...").arg (name));//Download
  rom_size=rom->size;
  begin_address=HEX2BYTES(QByteArray::number ( rom->begin_address,16).rightJustified (8,'0'));
  data=rom->data;

  bool less_4096=(rom_size/4096)==0;
  int package_len=less_4096?1:rom_size/4096;
  int remainder  =rom_size%4096;
  int totallen=(remainder&&!less_4096)?(package_len+1)*4096:package_len*4096;
  QByteArray syncread;
  io_->io_Write (HEX2BYTES(ST_Send_FW_File.SEND0[0]),MAX_WRITE_TIMEOUT ,true);
  io_->io_Write (begin_address,MAX_WRITE_TIMEOUT ,true);
  io_->io_Write (HEX2BYTES(QByteArray::number (totallen,16).rightJustified (8,'0')),MAX_WRITE_TIMEOUT ,true);
  io_->io_Write (HEX2BYTES(ST_Send_FW_File.SEND0[3]),MAX_WRITE_TIMEOUT ,true);
  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE0[0]))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                        .arg (QString(syncread.toHex ()))
                                                                        .arg (ST_Send_FW_File.RECEIVE0[0])
           )
    return false;
  }
  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE0[1]))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                        .arg (QString(syncread.toHex ()))
                                                                        .arg (ST_Send_FW_File.RECEIVE0[1])
           )
    return false;
  }
  uint32_t total_check_sum=0;
  setDownload_percentage (0);
  if(less_4096)
  {
    QTime start_t=QTime::currentTime ();
    QByteArray send=data.leftJustified (4095).insert (4095,HEX2BYTES ("FF"));
    io_->io_Write(send,MAX_WRITE_TIMEOUT,true);

    uint32_t local_check_sum = CRC_compute_simple_checksum(reinterpret_cast<uint8_t*>( send.data ()), send.size ());
    total_check_sum+=local_check_sum;
    uchar * checksum=reinterpret_cast<uchar*>(&total_check_sum );
    QByteArray qchecksum;
    qchecksum.append (checksum[3]).append (checksum[2]).append (checksum[1]).append (checksum[0]);
    io_->io_Write(qchecksum,MAX_WRITE_TIMEOUT,true);
    syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
    if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE1[0]))
    {
      QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                          .arg (QString(syncread.toHex ()))
                                                                          .arg (ST_Send_FW_File.RECEIVE1[0])
             )
      return false;
    }
    setDownload_percentage (100);

    QTime end_t=QTime::currentTime ();
    float ms=start_t.msecsTo (end_t)/1000.0;
    flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg ((1)*4096).arg (QString::number (4096/ms/1024,'f',1)).arg (4096);
    emit flashInfoChanged ();
  }
  else
  {
    int i=0;

    for( i=0;i<package_len;i++)
    {
      QTime start_t=QTime::currentTime ();
      QByteArray send=data.mid (i*4096,4096);
      io_->io_Write(send,MAX_WRITE_TIMEOUT,true);

      uint32_t local_check_sum = CRC_compute_simple_checksum(reinterpret_cast<uint8_t*>( send.data ()), send.size ());
      total_check_sum+=local_check_sum;
      uchar * checksum=reinterpret_cast<uchar*>(&local_check_sum );
      QByteArray qchecksum;
      qchecksum.append (checksum[3]).append (checksum[2]).append (checksum[1]).append (checksum[0]);
      io_->io_Write(qchecksum,MAX_WRITE_TIMEOUT,true);
      syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
      if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE1[0]))
      {
        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                            .arg (QString(syncread.toHex ()))
                                                                            .arg (ST_Send_FW_File.RECEIVE1[0])
               )
        return false;
      }
      if(remainder)
      {
        setDownload_percentage (i*100/(package_len+1));
      }
      else
      {
        setDownload_percentage (i*100/(package_len));
      }

      QTime end_t=QTime::currentTime ();
      float ms=start_t.msecsTo (end_t)/1000.0;
      int totalsize=remainder?(package_len+1)*4096:(package_len)*4096;
      flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg ((i+1)*4096).arg (QString::number (4096/ms/1024,'f',1)).arg (totalsize);
      emit flashInfoChanged ();

    }
    if(remainder)
    {
      QTime start_t=QTime::currentTime ();
      QByteArray send=data.right (remainder).leftJustified (4095).insert (4095,HEX2BYTES ("FF"));
      io_->io_Write(send,MAX_WRITE_TIMEOUT,true);

      uint32_t local_check_sum = CRC_compute_simple_checksum(reinterpret_cast<uint8_t*>( send.data ()), send.size ());
      total_check_sum+=local_check_sum;
      uchar * checksum=reinterpret_cast<uchar*>(&local_check_sum );
      QByteArray qchecksum;
      qchecksum.append (checksum[3]).append (checksum[2]).append (checksum[1]).append (checksum[0]);
      io_->io_Write(qchecksum,MAX_WRITE_TIMEOUT,true);

      syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
      if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE1[0]))
      {

        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                            .arg (QString(syncread.toHex ()))
                                                                            .arg (ST_Send_FW_File.RECEIVE1[0])
               )
        return false;
      }
      setDownload_percentage (100);

      QTime end_t=QTime::currentTime ();
      float ms=start_t.msecsTo (end_t)/1000.0;
      int totalsize=remainder?(package_len+1)*4096:(package_len)*4096;
      flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg ((i+1)*4096).arg (QString::number (4096/ms/1024,'f',1)).arg (totalsize);
      emit flashInfoChanged ();

    }
  }
  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);

  if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE1[1]))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                        .arg (QString(syncread.toHex ()))
                                                                        .arg (ST_Send_FW_File.RECEIVE1[1])
           )
    return false;
  }
  uchar * checksum=reinterpret_cast<uchar*>(&total_check_sum );

  QByteArray qchecksum;
  qchecksum.append (checksum[3]).append (checksum[2]).append (checksum[1]).append (checksum[0]);
  io_->io_Write(qchecksum,MAX_WRITE_TIMEOUT,true);

  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE3))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                        .arg (QString(syncread.toHex ()))
                                                                        .arg (ST_Send_FW_File.RECEIVE3)
           )
    return false;
  }
  io_->io_Write(HEX2BYTES (ST_Send_FW_File.SEND4),MAX_WRITE_TIMEOUT,true);

  syncread=io_->io_Read (MAX_READ_TIMEOUT,1);
  if(syncread!=HEX2BYTES (ST_Send_FW_File.RECEIVE4))
  {
    QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal %2")
                                                                        .arg (QString(syncread.toHex ()))
                                                                        .arg (ST_Send_FW_File.RECEIVE4)
           )
    return false;
  }
  return true;
}

bool MTK_BROM_3335::qGNSS_mTK_DownLoad(QString DA_Path, QString CFG_Path, int mtkmodel, int mode)
{
  bool flag=false;
  if(!downbegin(DA_Path,CFG_Path,mtkmodel,mode)) {return false;}

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

  bool havecheck=false;
  for(auto i=cfg_.cfg_roms.begin ();i!=cfg_.cfg_roms.end ();++i)
  {
    auto image=*i;
    if(image->selected)
    {
      havecheck=true;
    }
  }

  if(!havecheck)
  {
    flag=false;
  }
  if(havecheck)
  {
    switch (mode)
    {
      /*
    case MTK_BROM_3335::Download:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!send_FW_File_to_Module()){break;}
        flag=true;
        break;
    */
      case MTK_BROM_3335::Auto_Format:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(MTK_BROM_3335::Auto_Format)){break;}
        if(!send_FW_File_to_Module()){break;}
        if(!finish ()){break;}
        flag=true;
        break;
    }
  }
  else
  {
    QERROR (QString(" The all download fill is not enable ! "));
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

bool MTK_BROM_3335::qGNSS_mTK_Format(QString DA_Path, QString CFG_Path, int mtkmodel, MTK_BROM_3335::Format_mode mode
                                     , QString hexbegin_address
                                     , QString hexLength)
{
  bool flag=false;
  downbegin(DA_Path,CFG_Path,mtkmodel,mode);
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
  switch (mode)
  {
    case MTK_BROM_3335::Auto_Format:
      if(!handshake_with_Module()){break;}
      if(!disable_Modeule_WDT()){break;}
      if(!send_DA_File_to_Module()){break;}
      if(!jump_to_DA()){break;}
      if(!sync_with_DA()){break;}
      if(!format_Flash(mode)){break;}
      //if(!finish ()){break;}
      flag=true;
      break;
    case MTK_BROM_3335::Total_Format:
      if(!handshake_with_Module()){break;}
      if(!disable_Modeule_WDT()){break;}
      if(!send_DA_File_to_Module()){break;}
      if(!jump_to_DA()){break;}
      if(!sync_with_DA()){break;}
      if(!format_Flash(mode)){break;}
      //if(!finish ()){break;}
      flag=true;
      break;
    case MTK_BROM_3335::Manual_Format_Logical:
      if(!handshake_with_Module()){break;}
      if(!disable_Modeule_WDT()){break;}
      if(!send_DA_File_to_Module()){break;}
      if(!jump_to_DA()){break;}
      if(!sync_with_DA()){break;}
      if(!format_Flash(MTK_BROM_3335::Manual_Format_Logical,hexbegin_address,hexLength)){break;}
      //if(!finish ()){break;}
      flag=true;
      break;
    case MTK_BROM_3335::Manual_Format_Physical:
      if(!handshake_with_Module()){break;}
      if(!disable_Modeule_WDT()){break;}
      if(!send_DA_File_to_Module()){break;}
      if(!jump_to_DA()){break;}
      if(!sync_with_DA()){break;}
      if(!format_Flash(MTK_BROM_3335::Manual_Format_Physical,hexbegin_address,hexLength)){break;}
      //if(!finish ()){break;}
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

bool MTK_BROM_3335::qGNSS_mTK_ReadBack(QString DA_Path, QString CFG_Path, int mtkmodel, int mode,const Struct_ReadBack *STRread,int readLen)
{
  bool flag=false;
  if(!downbegin(DA_Path,CFG_Path,mtkmodel,mode)) {return false;}
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
  switch (mode)
  {
    case ReadBck:
      IOserial *seril=static_cast<IOserial *>(io_.data ());
      seril->set_RTS (true);
      io_->io_clear ();
      if(!handshake_with_Module()){break;}
      if(!disable_Modeule_WDT()){break;}
      if(!send_DA_File_to_Module()){break;}
      if(!jump_to_DA()){break;}
      if(!sync_with_DA()){break;}
      if(!read_back (STRread,readLen)){break;}
      if(!finish ()){break;}
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

void MTK_BROM_3335::qGNSS_SetDownLoadBaudRate(int baudrate)
{
  downrate_=baudrate;
  flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
  emit flashInfoChanged ();
}
//*************************************************************
bool MTK_BROM_3335::download_stop()
{
  setDownload_state (Download_State::Custom_DownLoad_Stoping);
  downloadstoping=true;
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  Q_EMIT seril->send_Stop ();//stop reading func
  seril->io_asyncsleep (500);
  seril->io_clear ();
  return true;
}

void MTK_BROM_3335::reset_buadRate()
{
  IOserial *seril=static_cast<IOserial *>(io_.data ());
  /*send different baudRates   seven */
  seril->set_BuadRate(init_baudRate_);
  seril->set_RTS (RTS_);
  seril=nullptr;
}

bool MTK_BROM_3335::downbegin(QString DA_Path, QString CFG_Path, int mtkmodel, int mode)
{
  setCurrent_model ((BROM_BASE::QGNSS_Model)mtkmodel);
  if(init_baudRate_!=115200)
  {
    io_->set_BuadRate(115200);
  }

  if(!load_file(DA_Path,CFG_Path))
  {
    return false;
  }
  if(startdownload ())
  {
    QERROR(LOGFRONT+ QString("downloading...!")) ;
    return false;
  }
  setStartdownload(true);
  QMetaEnum em=QMetaEnum::fromType<Down_Mode>();
  flashInfo()["DownLoadMode"]=em.valueToKey (mode);
  emit flashInfoChanged ();
  return true;
}

/*
############################################################################################################
#
#  General Setting
#
############################################################################################################

general:
    config_version : v2.0
                     platform: AG3335

                                ############################################################################################################
#
#  Main Region Setting
#
                                ############################################################################################################

                                main_region:
    address_type: physical
                       rom_list:
    - rom:
    file: partition_table.bin
               name: PartitionTable
                          begin_address: 0x08000000
    - rom:
    file: ag3335_bootloader.bin
               name: BootLoader
                          begin_address: 0x08003000
    - rom:
    file: LC29HAANR01A02S.bin
               name: CM4
                          begin_address: 0x08013000
    - rom:
    file: gnss_config.bin
               name: GNSSConfiguration
                          begin_address: 0x083FF000

          */

          /**
 * old     : index of line then  read get
 * improve : read all line then parse avoid line number change
 *
 ******************************************cfg ***************
 * config_version:
 * platform:
 * address_type
 *
 * partition_table.bin
 * file
 * name
 * begin_address
 *
 * ag3335_bootloader.bin
 * file
 * name
 * begin_address
 *
 * LC29HAANR01A02S.bin
 * file
 * name
 * begin_address
 *
 * gnss_config.bin
 * file
 * name
 * begin_address
 ******************************************cfg***************
 **/
          bool MTK_BROM_3335::cfg_parse(const QByteArray &data,QString cfg_dir)
{
  QList<QByteArray>lines= data.split ('\n');
  QList<QByteArray>reads;

  int size=0;
  while(size<lines.size ())
  {
    QByteArray  line= lines[size].trimmed ();
    if(!line.contains (':'))
    {
      size++;
      continue;
    }
    int valuesize= line.size ()-line.indexOf (':')-1;

    if(valuesize<1)
    {
      size++;
      continue;
    }
    QByteArray read=line.right (valuesize).trimmed ();
    reads.append (read);

    size++;
  }
  if(reads.size ()<3)//1 2 3
  {
    QERROR (QString(" read values size:%1 less then 3! ").arg (reads.size ()));
    return false;
  }
  cfg_.general.config_version=(char*)malloc (sizeof(char)*(reads[0].size ()+1));
  strcpy (cfg_.general.config_version,reads[0].constData ());

  cfg_.general.platform=(char*)malloc (sizeof(char)*(reads[1].size ()+1));
  strcpy (cfg_.general.platform,reads[1].constData ());

  //QMetaEnum metaenum=QMetaEnum::fromType<Address_Type>();
  cfg_.addresstype =reads[2].data();
  int tsize=reads.size ()-3;
  int tms=tsize/3;//3 info
  //cfg_rom *rom=&cfg_.cfgrom;
  if(!cfg_.cfg_roms.empty ())
  {
    while(!cfg_.cfg_roms.empty ())
    {
      delete cfg_.cfg_roms.front ();
      cfg_.cfg_roms.pop_front ();
    }
  }
  for(int i=1;i<=tms;i++)
  {
    cfg_rom* temprom=new cfg_rom;
    temprom->file=(char*)malloc (sizeof(char)*(reads[3*i].size ()+1));
    strcpy (temprom->file,reads[3*i].constData ());

    temprom->name=(char*)malloc (sizeof(char)*(reads[3*i+1].size ()+1));
    strcpy ( temprom->name,reads[3*i+1].constData ());

    QByteArray temp=reads[3*i+2].right (reads[3*i+2].size ()-2);
    temprom->begin_address=temp.toUInt (nullptr,16);
    cfg_.cfg_roms.push_back (temprom);
  }

  for(auto i=cfg_.cfg_roms.begin ();i!=cfg_.cfg_roms.end ();++i)
  {
    auto rom2=*i;
    QString rom_partition_table_path=QString("%1%2%3").arg (cfg_dir).arg ('/').arg (rom2->file);

    QFile rom_partition_tablefile(rom_partition_table_path);
    if(!rom_partition_tablefile.exists ())
    {
      QERROR (QString("[partition_table_path] file:%1  not exists").arg (rom_partition_table_path));
      return false;
    }
    rom_partition_tablefile.open (QIODevice::ReadOnly);
    QByteArray readbytes=rom_partition_tablefile.readAll ();
    rom2->size=readbytes.size ();
    rom2->data=readbytes;
    rom_partition_tablefile.close ();
  }
  setCfg_struct(&cfg_);
  return true;
}

const MTK_BROM_3335::Download_State &MTK_BROM_3335::download_state() const
{
  return download_state_;
}

void MTK_BROM_3335::setDownload_state(const Download_State &newDownload_state)
{
  //    if (download_state_ == newDownload_state)
  //        return;
  download_state_ = newDownload_state;

  switch(download_state_)
  {
    case MTK_BROM_3335::State_Unstart:
      statekeyvalue()->setKey (tr("Ready to download"));
      break;
    case MTK_BROM_3335::State_handshake_with_Module:
      statekeyvalue()->setKey (tr("Handshake... (Press the module reset button!)"));
      break;
    case MTK_BROM_3335::State_disable_Modeule_WDT:
      statekeyvalue()->setKey (tr("Disable WDT..."));
      break;
    case MTK_BROM_3335::State_send_DA_File_to_Module:
      statekeyvalue()->setKey (tr("Download DA..."));
      break;
    case MTK_BROM_3335::State_jump_to_DA:
      statekeyvalue()->setKey (tr("Jump to DA..."));
      break;
    case MTK_BROM_3335::State_sync_with_DA:
      statekeyvalue()->setKey (tr("Sync with DA..."));
      break;
    case MTK_BROM_3335::State_format_Flash:
      statekeyvalue()->setKey (tr("Format..."));
      break;
    case MTK_BROM_3335::State_send_PartitionTable:
      statekeyvalue()->setKey (tr("Download PartitionTable..."));
      break;
    case MTK_BROM_3335::State_send_BootLoader:
      statekeyvalue()->setKey (tr("Download BootLoader..."));
      break;
    case MTK_BROM_3335::State_send_CM4:
      statekeyvalue()->setKey (tr("Download Firmware..."));
      break;
    case MTK_BROM_3335::State_send_GNSSConfiguration:
      statekeyvalue()->setKey (tr("Download GNSSConfiguration..."));
      break;
    case MTK_BROM_3335::State_Finished:
      statekeyvalue()->setKey (tr("Download Finish"));
      break;
    case MTK_BROM_3335::State_CMD_Fail:
      statekeyvalue()->setKey (tr("Download Fail"));
      break;
    case MTK_BROM_3335::Custom_DownLoad_Stoping:
      statekeyvalue()->setKey (tr("Downloading is stopping"));
      break;
    case MTK_BROM_3335::Custom_DownLoad_Stoped:
      statekeyvalue()->setKey (tr("Downloading is stopped"));
      break;

  }

  statekeyvalue()->setValue ((int)newDownload_state);
  emit statekeyvalueChanged();
  emit download_stateChanged(download_state_);
}

MTK_BROM_3335::CFG * MTK_BROM_3335::cfg_struct() const
{
  return m_cfg_struct;
}

void MTK_BROM_3335::setCfg_struct(CFG * cfg_struct)
{
  if (m_cfg_struct == cfg_struct)
    return;
  m_cfg_struct = cfg_struct;
  emit cfg_structChanged(m_cfg_struct);
}
void MTK_BROM_3335::setDownload_stateStr(QString state)
{
  if(state.isEmpty ())
  {
    return;
  }
  statekeyvalue()->setKey (state);
  emit statekeyvalueChanged ();
  emit download_stateChanged(download_state_);
}
