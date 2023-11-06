/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_brom_3352.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-28      victor.gong
* ***************************************************************************/
#define RACE_DA_GET_FLASH_ADDRESS (0x210E)
#define RACE_DA_GET_FLASH_SIZE (0x210F)
#define RACE_DA_GET_FLASH_ID (0x2110)
#define RACE_DA_BAUDRATE (0x2115)
#define RACE_DA_FLOW_CTRL (0x2116)

#define RACE_DA_WRITE_BYTES (0x2100)
#define RACE_DA_READ_BYTES (0x2101)
#define RACE_DA_ERASE_BYTES (0x2104)
#define RACE_DA_FINISH (0x2106)

#define DA_S_DONE               0x00

#define DA_SYNC_CHAR            0xC0
#define DA_CONT_CHAR            0x69
#define DA_STOP_CHAR            0x96
#define DA_ACK                  0x5A
#define DA_NACK                 0xA5
#define DA_UNKNOWN_CMD          0xBB
#define DA_FLUSH_CONT           0xE1
#define DA_FLUSH_DONE           0xE2

#define DA_S_IN_PROGRESS                  3021
#define DA_S_UART_GET_DATA_TIMEOUT        3061
#define DA_S_UART_GET_CHKSUM_LSB_TIMEOUT  3062
#define DA_S_UART_GET_CHKSUM_MSB_TIMEOUT  3063
#define DA_S_UART_DATA_CKSUM_ERROR        3064
#define DA_S_UART_RX_BUF_FULL             3065

#define QERROR(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Fail,x});\
    setDownload_state(Download_State::State_CMD_Fail);

#define QWARN(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Warning,x});\

#define QINFO(x) \
    setDownload_stateStr(x);\
//setBbmsgStr ({BROM_BASE::brmsgtype::bb_Info,x});
#define QMSG(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Info,x});\

#include "mtk_brom_3352.h"
#include <qt_windows.h>
MTK_BROM_3352::MTK_BROM_3352(MTK_Base *parent)
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

MTK_BROM_3352::~MTK_BROM_3352()
{

}

const char* MTK_BROM_3352::getFormatTotalLength(BROM_BASE::QGNSS_Model mtkmodel)
{
     return "00400000";
}

void MTK_BROM_3352::MTK_init_IO(IODvice *io)
{
    if(io)
    {
        io_=io;

        IOserial *seril=static_cast<IOserial *>(io_.data ());
        RTS_=seril->get_RTS ();
        connect (seril,&IOserial::errorOccurred,this,[&](QString err){
            QERROR ("serialport error:"+err);
        });
        connect (seril,&IOserial::rw_InfoChanged,this,&MTK_BROM_3352::rw_InfoChanged);
        init_baudRate_=seril->get_baudRate ();
        flashInfo()["PORTNAME"]=seril->portName ();
        emit flashInfoChanged ();
        seril=nullptr;
    }

}

bool MTK_BROM_3352::load_file(QString DA_Path, QString flash_cfg_path)
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
bool MTK_BROM_3352::handshake_with_Module()
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
bool MTK_BROM_3352::disable_Modeule_WDT( )
{
    setDownload_state (Download_State::State_disable_Modeule_WDT);
    setDownload_percentage (0);
    if(!disable_Modeule_WDT_help("A2080000","0010")){return false;}
    setDownload_percentage (50);
    if(!disable_Modeule_WDT_help ("A2080030","0040")){return false;}
    setDownload_percentage (100);
    return true;
}

bool MTK_BROM_3352::disable_Modeule_WDT_help(QString Hexaddr, QString hexdata )
{
    QByteArray syncread;
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
    //[2]
    syncread=io_->syncWriteRead (HEX2BYTES(Hexaddr.toUtf8 ()),MAX_READ_TIMEOUT,4);
    if(syncread!=HEX2BYTES (Hexaddr.toUtf8 ()))
    {
        QString read=syncread.toHex ();
        QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                     .arg (Hexaddr)
                                                                                                     .arg (QString(syncread.toHex ()))
                                                                                                     .arg (Hexaddr)
               )
        return false;
    }
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
    //[4]
    io_->io_Write (HEX2BYTES(hexdata.toUtf8 ()),MAX_WRITE_TIMEOUT,true);
    syncread=io_->io_Read (MAX_READ_TIMEOUT,4);
    if(syncread.mid (0,2)!=HEX2BYTES (hexdata.toUtf8 ()))
    {
        QString read=syncread.toHex ();
        QERROR(DOWNSTATE(Download_State,Download_State::State_disable_Modeule_WDT)+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal %3")
                                                                                                     .arg (hexdata)
                                                                                                     .arg (QString(syncread.mid (0,2).toHex ()))
                                                                                                     .arg (hexdata)
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
    return true;
}

bool MTK_BROM_3352::hdl_da_get_flash_address(uint32_t *pData)
{
    // send
    RACE_ADDR_SEND send;
    send.head_ = 0x05;
    send.type_ = 0x5A;
    send.len_ = sizeof(send.id_);
    send.id_ = RACE_DA_GET_FLASH_ADDRESS;
    io_->io_Write (QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
    // response
    RACE_ADDR_RES res;
    QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,sizeof(res));
    res=*reinterpret_cast<RACE_ADDR_RES*>(read.data ());
    if (res.head_ == 0x05 &&
        res.type_ == 0x5B &&
        res.len_ == (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.addr_)) &&
        res.id_ == RACE_DA_GET_FLASH_ADDRESS &&
        res.status_ == DA_S_DONE)
    {
        *pData = res.addr_;
        return true;
    }
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("get_flash_address error"))
            return false;
}

bool MTK_BROM_3352::hdl_da_get_flash_size(uint32_t *pData)
{
    // send
    RACE_SIZE_SEND send;
    send.head_ = 0x05;
    send.type_ = 0x5A;
    send.len_ = sizeof(send.id_);
    send.id_ = RACE_DA_GET_FLASH_SIZE;
  //HDL_COM_PutByte_Buffer((uint8_t *)&send, sizeof(send));
    io_->io_Write (QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
  // response
    RACE_SIZE_RES res;
  //HDL_COM_GetByte_Buffer((uint8_t *)&res, sizeof(res));
    QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,sizeof(res));
    res=*reinterpret_cast<RACE_SIZE_RES*>(read.data ());

    if (res.head_ == 0x05 &&
        res.type_ == 0x5B &&
        res.len_ == (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.size_)) &&
        res.id_ == RACE_DA_GET_FLASH_SIZE &&
        res.status_ == DA_S_DONE)
    {

        *pData = res.size_;
        return true;
    }

    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("da_get_flash_size error"))
    return false;
}

bool MTK_BROM_3352::hdl_da_get_flash_id(uint8_t *pManufacturerId, uint8_t *pDeviceId1, uint8_t *pDeviceId2)
{
    // send
    RACE_ID_SEND send;
    send.head_ = 0x05;
    send.type_ = 0x5A;
    send.len_ = sizeof(send.id_);
    send.id_ = RACE_DA_GET_FLASH_ID;
    //HDL_COM_PutByte_Buffer((uint8_t *)&send, sizeof(send));
    io_->io_Write (QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
    // response
    RACE_ID_RES res;
    //HDL_COM_GetByte_Buffer((uint8_t *)&res, sizeof(res));
    QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,sizeof(res));
    res=*reinterpret_cast<RACE_ID_RES*>(read.data ());
    if (res.head_ == 0x05 &&
        res.type_ == 0x5B &&
        res.len_ == (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.flash_id_)) &&
        res.id_ == RACE_DA_GET_FLASH_ID &&
        res.status_ == DA_S_DONE)
    {
        *pManufacturerId = res.flash_id_[0];
        *pDeviceId1 = res.flash_id_[1];
        *pDeviceId2 = res.flash_id_[2];
        return true;
    }
    QERROR(DOWNSTATE(Download_State,Download_State::State_sync_with_DA)+LOGFRONT+QString("da_get_flash_id error"))
    return false;
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
bool MTK_BROM_3352::send_DA_File_to_Module()
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
    int remainder=hexbinsize%1024;
    QByteArray DAbyte=cfg_.DA_data;

    for(int i=0;i<hexbinsize/1024;i++)
    {
        QTime start_t=QTime::currentTime ();
        io_->io_Write (DAbyte.mid (i*1024,1024),MAX_WRITE_TIMEOUT,true);
        if(remainder)
        {
            setDownload_percentage (i*100/(hexbinsize/(1024+1)));
        }
        else
        {
            setDownload_percentage (i*100/(hexbinsize/1024));
        }


        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg ((i+1)*1024).arg (QString::number (1024/ms/1024,'f',1)).arg (hexbinsize);
        emit flashInfoChanged ();
    }
    if(remainder)
    {
        QTime start_t=QTime::currentTime ();
        io_->io_Write (DAbyte.right (remainder),MAX_WRITE_TIMEOUT,true);
        setDownload_percentage (100);

        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%3 Bytes    %2 KBps").arg (hexbinsize).arg (QString::number (remainder/ms/1024,'f',1)).arg (hexbinsize);
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
bool MTK_BROM_3352::jump_to_DA()
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
bool MTK_BROM_3352::sync_with_DA()
{
    // Need DA init done
    io_->io_asyncsleep (100);

    uint32_t flash_base_addr = 0;
    if(!hdl_da_get_flash_address(&flash_base_addr)){ return false;}

    uint32_t flash_size = 0;
    if(!hdl_da_get_flash_size(&flash_size)){return false;}

    uint8_t manufacturer_id = 0;
    uint8_t device_id1 = 0;
    uint8_t device_id2 = 0;
    if(!hdl_da_get_flash_id(&manufacturer_id, &device_id1, &device_id2)){return false;}
    da_report.flash_manufacturer_id = manufacturer_id;
    da_report.flash_id1 = device_id1;
    da_report.flash_id2 = device_id2;
    da_report.flash_base_addr = flash_base_addr;
    da_report.flash_size = flash_size;

    // DA Report
    flashInfo ()["Flash_manufacturer_ID"] =QString::number (manufacturer_id,16);
    flashInfo ()["Flash_device_ID1"] = QString::number (device_id1,16);
    flashInfo ()["Flash_device_ID2"] = QString::number (device_id2,16);
    flashInfo ()["Flash_base_address"] = QString::number (flash_base_addr,16);
    flashInfo ()["Flash_size"] = QString::number (flash_size,16);
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
bool MTK_BROM_3352::format_Flash(Format_mode mode,QString hexbeginaddress,QString hexlength)
{
    QString beginaddress=hexbeginaddress;
    QString length      =hexlength;
    return format_help(hexbeginaddress,hexlength);
}

bool MTK_BROM_3352::format_help(QString hexbeginaddress, QString hexlength)
{
    setDownload_state (Download_State::State_format_Flash);
    const uint32_t format_begin_address =hexbeginaddress.toUInt (nullptr,16);
    const uint32_t format_size = hexlength.toUInt (nullptr,16);
    const uint32_t format_end_address = format_begin_address+format_size;
    uint32_t format_address = format_begin_address;
    while (format_address < format_end_address)
    {
        uint32_t block_size = 0;
        if (format_size >= LEN_64K && (format_address%LEN_64K) == 0)
        {
            if((format_end_address-format_address)<LEN_64K)
            {
                block_size=LEN_4K;
            }
            else
            {
            block_size = LEN_64K;
            }
        }
        else
        {
            block_size = LEN_4K;
        }
        if (!hdl_format_race(format_address, block_size))
        {
            return false;
        }
        format_address+=block_size;
        uint8_t progress = (format_address-format_begin_address)*100/format_size;
        progress = (progress > 100) ? 100: progress;
        setDownload_percentage (progress);
    }
    return true;
}

bool MTK_BROM_3352::hdl_format_race(uint32_t addr, uint32_t len)
{
    for (int i=0; i<3; i++)
    {
        // send
        RACE_FM_SEND send;
        send.head_ = 0x05;
        send.type_ = 0x5A;
        send.len_ = sizeof(send.id_)+sizeof(send.addr_)+sizeof(send.size_)+sizeof(send.crc_);
        send.id_ = RACE_DA_ERASE_BYTES;
        send.addr_ = addr;
        send.size_ = len;
        send.crc_ = CRC32((uint8_t*)&send, sizeof(send)-sizeof(send.crc_));
        io_->io_Write(QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
        // response
        RACE_FM_RES res;
        QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,sizeof(res));

         res=*reinterpret_cast<RACE_FM_RES*>(read.data ());
        if (res.head_ == 0x05 &&
            res.type_ == 0x5B &&
            res.len_ == (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.addr_)) &&
            res.id_ == RACE_DA_ERASE_BYTES &&
            res.status_ == DA_S_DONE &&
            res.addr_ == addr)
        {
            return true;
        }
        if(res.head_ != 0x05)
        {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.head_:%1 unequal 0x05").arg (QString::number (res.head_ ,16)))}
        if(res.type_ != 0x5B)
         {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.type_:%1 unequal 0x5B").arg (QString::number (res.type_ ,16)))}
        if(res.len_ != (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.addr_)))
         {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.head_:%1 unequal 7").arg (QString::number (res.len_ ,16)))}
        if( res.id_ != RACE_DA_ERASE_BYTES)
         {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.id_:%1 unequal 0x2104").arg (QString::number (res.id_ ,16)))}
        if(res.status_!= DA_S_DONE)
         {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.status_:%1 unequal 0x00").arg (QString::number (res.status_ ,16)))}
        if(res.addr_ != addr)
         {QWARN (DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("res.addr_:%1 unequal %2").arg (QString::number (res.addr_ ,16)).arg (QString::number (addr,16)))}
    }
    QERROR(DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("format error"))
    return false;
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
bool MTK_BROM_3352::send_FW_File_to_Module()
{
    _hdl_image_t *image=&cfg_.cfgrom;

    while (image != NULL)
    {
        if(!image->image_is_checked)
        {
            goto goto_next1;
        }
        if (!check_address_range(image->image_slave_flash_addr, image->image_len,&da_report))
        {

            return false;
        }

        if (image->image_slave_flash_addr % DA_FW_PACKET_LEN != 0)
        {
            QERROR(DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("DA_FW_PACKET_LEN "))
            return false;
        }
//        _hdl_image_t *next = image->next;
//        if (next != NULL && image->image_slave_flash_addr+image->image_len > next->image_slave_flash_addr) {
//            QERROR(DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("image "))
//            return false;
//        }
        goto_next1:
        image = image->next;
    }

    // Download Every Image
    image =&cfg_.cfgrom ;//download_arg->download_images;
    while (image )
    {
        if(!image->image_is_checked)
        {
            goto goto_next_;
        }
        if(!hdl_da_download(image))
        {
            QERROR(DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString(image->image_name));
            return false;
        }
        goto_next_:
        image = image->next;
    }
    return true;
}

bool MTK_BROM_3352::autoReboot()
{
    // send
    RACE_rebot send;
    send.head_ = 0x05;
    send.type_ = 0x5A;
    send.len_ = sizeof(send.id_)+sizeof (send.pld);
    send.id_ = RACE_DA_FINISH;
    send.pld=1;
    io_->io_Write (QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
    // response
    RACE_rebot res;
    QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,sizeof(res));
    res=*reinterpret_cast<RACE_rebot*>(read.data ());
    if (res.head_ == 0x05 &&
        res.type_ == 0x5B &&
        res.len_ == (sizeof(res.id_)+sizeof(res.pld)) &&
        res.id_ == RACE_DA_FINISH &&
        res.pld == DA_S_DONE)
    {

        return true;
    }
    QERROR(LOGFRONT+QString("autoReboot error"))
    return false;
}

bool MTK_BROM_3352::read_back(const MTK_BROM_3352::Struct_ReadBack *ReadBack, int size)
{
    for(int i=0;i<size;i++)
    {
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

bool MTK_BROM_3352::read_back_help(const MTK_BROM_3352::Struct_ReadBack &ReadBack)
{
    QByteArray FW_RW;

    const char* sDA_ReadBack_CMD="D6";
    const char* sDA_ACK ="5A";
    int packetLen=4096;
    char * sPacketLength=RS(reinterpret_cast<char*>(&packetLen),sizeof (packetLen));
    /*
    QString readpath=QString::fromStdString (std::string(ReadBack.FilePath,ReadBack.FilePathSize));
    qDebug()<<readpath;
    QString filepth=QString("%1-%2-%3");//module address time
    QString time=QDateTime::currentDateTime ().toString ("yyMMdd#hh#mm#ss");
    QString moduleName=IODvice::Enum_Value2Key<quectel_gnss::QGNSS_Model>(current_model ());
    QByteArray hexBeginADR=QByteArray::fromRawData (RS(reinterpret_cast<char*>(ReadBack.BeginAddress) ,4),4);
    QByteArray endADR     =QByteArray::fromRawData (RS(reinterpret_cast<char*>(ReadBack.ReadSize)     ,4),4);
    QString Address_=QString("Begin:0x%1 Size:0x%2")
                           .arg (QString(hexBeginADR.toHex ()))
                           .arg (QString(endADR.toHex ()));

    QString FilePath=readpath+"/"+filepth
                           .arg (moduleName)
                           .arg (Address_)
                           .arg (time);
    */
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
        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE:%1 unequal 5A")
                                                                            .arg (QString(read.toHex ()))
               )
        return false;
    }

    while(remainSize>0)
    {
    static int tms=0;
    QTime start=QTime::currentTime ();

    QByteArray qreadPacket=io_->io_Read (MAX_READ_TIMEOUT,packetLen);
    uint crc32=CRC_compute_simple_checksum (reinterpret_cast<uint8_t*>(qreadPacket.data ()),qreadPacket.size ());
    char * c_crc32=reinterpret_cast<char*>(&crc32);
    QByteArray qcrc=QByteArray::fromRawData (RS(c_crc32,4),sizeof (crc32));
    QTime end=QTime::currentTime ();
    QByteArray readcrc=io_->io_Read (MAX_READ_TIMEOUT,4);
    if(readcrc!=qcrc)
    {
        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("#  RECEIVE CRC:%1 unequal %2")
                                                                            .arg (QString(readcrc.toHex ()))
                                                                            .arg (QString(qcrc.toHex ()))
              )
        //return false;
    }
    io_->io_Write (QByteArray::fromHex (sDA_ACK),0,true);
    //io_->io_clear ();
    FW_RW+=qreadPacket;
    remainSize-=qreadPacket.size ();
    }
    QFile file(FilePath);

    if(!file.open (QIODevice::WriteOnly))
    {
        QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("# Failed to create File  :%1 ")
                                                                            .arg (FilePath)
               )
    }
    file.close ();
    return true;
}

bool MTK_BROM_3352::finish()
{
    /*
    io_->io_Write (QByteArray::fromHex ("D9"),1,false);
    io_->io_Write (QByteArray::fromHex ("00"),1,false);
    QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,1);
    if(read!=QByteArray::fromHex ("5A"))
    {
        QERROR("finish "+LOGFRONT+QString("#  RECEIVE:%1 !=%2")
                                  .arg (QString(read.toHex ()))
                                   .arg ("5A")
               )
        return false;
    }
    */

    setDownload_state(Download_State::State_Finished);

    return true;
}
//************************send fw file  func******************************

bool MTK_BROM_3352::qGNSS_mTK_DownLoad(QString DA_Path, QString CFG_Path, int mtkmodel, int mode)
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

   _hdl_image_t *image =&cfg_.cfgrom ;
   bool havecheck=false;
    while (image )
    {
      if(image->image_is_checked)
      {
        havecheck=true;
      }
      image = image->next;
    }

    if(!havecheck)
    {
      flag=false;
    }
  if(havecheck)
  {
    switch (mode)
    {
    case MTK_BROM_3352::Auto_Format:
        if(!handshake_with_Module()){break;}
//      if(!hdl_brom_set_baudrate(downrate_)){break;}
//      if(!disable_Modeule_WDT()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!hdl_brom_set_baudrate(downrate_)){break;}//20220715 change
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(MTK_BROM_3352::Auto_Format)){break;}
        if(!send_FW_File_to_Module()){break;}
        if(!autoReboot()){break;}
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

bool MTK_BROM_3352::qGNSS_mTK_Format(QString DA_Path, QString CFG_Path, int mtkmodel, MTK_BROM_3352::Format_mode mode
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
    case MTK_BROM_3352::Auto_Format:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(mode)){break;}
        //if(!finish ()){break;}
        flag=true;
        break;
    case MTK_BROM_3352::Total_Format:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(mode)){break;}
        //if(!finish ()){break;}
        flag=true;
        break;
    case MTK_BROM_3352::Manual_Format_Logical:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(MTK_BROM_3352::Manual_Format_Logical,hexbegin_address,hexLength)){break;}
        //if(!finish ()){break;}
        flag=true;
        break;
    case MTK_BROM_3352::Manual_Format_Physical:
        if(!handshake_with_Module()){break;}
        if(!disable_Modeule_WDT()){break;}
        if(!send_DA_File_to_Module()){break;}
        if(!jump_to_DA()){break;}
        if(!sync_with_DA()){break;}
        if(!format_Flash(MTK_BROM_3352::Manual_Format_Physical,hexbegin_address,hexLength)){break;}
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

bool MTK_BROM_3352::qGNSS_mTK_ReadBack(QString DA_Path, QString CFG_Path, int mtkmodel, int mode,const Struct_ReadBack *STRread,int readLen)
{
    bool flag=false;
    if(!downbegin(DA_Path,CFG_Path,mtkmodel,mode)) {return false;}
    //time
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

void MTK_BROM_3352::qGNSS_SetDownLoadBaudRate(int baudrate)
{
    downrate_=baudrate;
    flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
    emit flashInfoChanged ();
}
//*************************************************************
bool MTK_BROM_3352::download_stop()
{
    setDownload_state (Download_State::Custom_DownLoad_Stoping);
    downloadstoping=true;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    Q_EMIT seril->send_Stop ();//stop reading func
    seril->io_asyncsleep (500);
    seril->io_clear ();
    return true;
}

void MTK_BROM_3352::reset_buadRate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    /*send different baudRates   seven */
    seril->set_BuadRate(init_baudRate_);
    seril->set_RTS (RTS_);
    seril=nullptr;
}

bool MTK_BROM_3352::downbegin(QString DA_Path, QString CFG_Path, int mtkmodel, int mode)
{
    setCurrent_model ((BROM_BASE::QGNSS_Model)mtkmodel);
    if(init_baudRate_!=115200)
    {
        io_->set_BuadRate(115200);
    }

    //IOserial *seril=static_cast<IOserial *>(io_.data ());
   // SetupComm((HANDLE)seril->get_handle (),4096,4096);
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

/**
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

**/
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
bool MTK_BROM_3352::cfg_parse(const QByteArray &data,QString cfg_dir)
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

    QMetaEnum metaenum=QMetaEnum::fromType<Address_Type>();
    cfg_.addresstype =(Address_Type)metaenum.keyToValue (reads[2].data());
    int tsize=reads.size ()-3;
    int tms=tsize/3;//3 info
    hdl_image_t *rom=&cfg_.cfgrom;
    for(int i=1;i<=tms;i++)
    {
    rom->file=(char*)malloc (sizeof(char)*(reads[3*i].size ()+1));
    strcpy (rom->file,reads[3*i].constData ());

    rom->image_name=(char*)malloc (sizeof(char)*(reads[3*i+1].size ()+1));
    strcpy ( rom->image_name,reads[3*i+1].constData ());

    QByteArray temp=reads[3*i+2].right (reads[3*i+2].size ()-2);
    rom->image_slave_flash_addr=temp.toUInt (nullptr,16);

    if(i!=tms)
    {
        if(!rom->next)
        {
            rom->next=new _hdl_image_t;
        }
        rom=rom->next;
    }
    }

    hdl_image_t *rom2=&cfg_.cfgrom;
    while (rom2)
    {
        QString rom_partition_table_path=QString("%1%2%3").arg (cfg_dir).arg ('/').arg (rom2->file);

        QFile rom_partition_tablefile(rom_partition_table_path);
        if(!rom_partition_tablefile.exists ())
        {
            QERROR (QString("[partition_table_path] file:%1  not exists").arg (rom_partition_table_path));
            return false;
        }
        rom_partition_tablefile.open (QIODevice::ReadOnly);
        QByteArray readbytes=rom_partition_tablefile.readAll ();
        rom2->image_len=readbytes.size ();
        rom2->data=readbytes;
        rom_partition_tablefile.close ();

        rom2=rom2->next;
    }
    setCfg_struct(&cfg_);
    return true;
}

const MTK_BROM_3352::Download_State &MTK_BROM_3352::download_state() const
{
    return download_state_;
}

void MTK_BROM_3352::setDownload_state(const Download_State &newDownload_state)
{
    download_state_ = newDownload_state;
    switch(download_state_)
    {
    case MTK_BROM_3352::State_Unstart:
        statekeyvalue()->setKey (tr("Ready to download"));
        break;
    case MTK_BROM_3352::State_handshake_with_Module:
        statekeyvalue()->setKey (tr("Handshake... (Press the module reset button!)"));
        break;
    case MTK_BROM_3352::State_disable_Modeule_WDT:
        statekeyvalue()->setKey (tr("Disable WDT..."));
        break;
    case MTK_BROM_3352::State_send_DA_File_to_Module:
        statekeyvalue()->setKey (tr("Download DA..."));
        break;
    case MTK_BROM_3352::State_jump_to_DA:
        statekeyvalue()->setKey (tr("Jump to DA..."));
        break;
    case MTK_BROM_3352::State_sync_with_DA:
        statekeyvalue()->setKey (tr("Sync with DA..."));
        break;
    case MTK_BROM_3352::State_format_Flash:
        statekeyvalue()->setKey (tr("Format..."));
        break;
    case MTK_BROM_3352::State_send_PartitionTable:
        statekeyvalue()->setKey (tr("Download PartitionTable..."));
        break;
    case MTK_BROM_3352::State_send_BootLoader:
        statekeyvalue()->setKey (tr("Download BootLoader..."));
        break;
    case MTK_BROM_3352::State_send_CM4:
        statekeyvalue()->setKey (tr("Download Firmware..."));
        break;
    case MTK_BROM_3352::State_send_GNSSConfiguration:
        statekeyvalue()->setKey (tr("Download GNSSConfiguration..."));
        break;
    case MTK_BROM_3352::State_Finished:
        statekeyvalue()->setKey (tr("Download Finish"));
        break;
    case MTK_BROM_3352::State_CMD_Fail:
        statekeyvalue()->setKey (tr("Download Fail"));
        break;
    case MTK_BROM_3352::Custom_DownLoad_Stoping:
        statekeyvalue()->setKey (tr("Trying to Stop"));
        break;
    case MTK_BROM_3352::Custom_DownLoad_Stoped:
        statekeyvalue()->setKey (tr("Stopped"));
        break;

    }
    statekeyvalue()->setValue ((int)newDownload_state);
    emit statekeyvalueChanged();
    emit download_stateChanged(download_state_);
}

void MTK_BROM_3352::setDownload_stateStr(QString state)
{
    if(state.isEmpty ())
    {
        return;
    }
    statekeyvalue()->setKey (state);
    emit statekeyvalueChanged ();
    emit download_stateChanged(download_state_);
}

MTK_BROM_3352::CFG * MTK_BROM_3352::cfg_struct() const
{
    return m_cfg_struct;
}

void MTK_BROM_3352::setCfg_struct(CFG * cfg_struct)
{
    if (m_cfg_struct == cfg_struct)
        return;
    m_cfg_struct = cfg_struct;
    emit cfg_structChanged(m_cfg_struct);
}

bool  MTK_BROM_3352::check_address_range(uint32_t address, uint32_t len, const hdl_da_report_t *da_report)
{
    const uint32_t base_addr = da_report->flash_base_addr;
    const uint32_t flash_size = da_report->flash_size;

    if(base_addr > address || (base_addr + flash_size) < address + len)
    {

        QERROR(DOWNSTATE(Download_State,Download_State())+LOGFRONT+QString("check_address_range Error"))
        return false;
    }
    return true;
}

bool MTK_BROM_3352::hdl_da_download(hdl_image_t *image)
{
    const uint32_t start_addr = image->image_slave_flash_addr;
    const uint32_t image_len = image->image_len;
    const uint32_t packet_num = ((image_len - 1) / DA_SEND_PACKET_LEN) + 1;

    uint32_t packet_sent_num = 0;
     char packet_buf[DA_SEND_PACKET_LEN]={0};
     QINFO(QString("Download %1...").arg(QString(image->image_name)))
    while (packet_sent_num < packet_num)
    {
         QTime start_t=QTime::currentTime ();
        const bool is_last_packet = (packet_sent_num == (packet_num-1));
        const uint32_t start_offset = packet_sent_num * DA_SEND_PACKET_LEN;
        const uint32_t cur_packet_len = is_last_packet ? (image_len-start_offset) : DA_SEND_PACKET_LEN;
        const uint32_t slave_addr = start_addr+start_offset;

        QByteArray pack= image->data.mid (start_offset,cur_packet_len);
         memcpy (packet_buf,pack,cur_packet_len);

            if (is_last_packet && cur_packet_len < DA_SEND_PACKET_LEN)
            {
                memset(packet_buf+cur_packet_len, 0xFF, DA_SEND_PACKET_LEN-cur_packet_len);
            }

            if (!hdl_download_race(slave_addr, (uint8_t*)&packet_buf))
            {
               return false;
            }
        packet_sent_num++;
        setDownload_percentage((packet_sent_num)*DA_SEND_PACKET_LEN*100/(packet_num*DA_SEND_PACKET_LEN));
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg ((packet_sent_num)*DA_SEND_PACKET_LEN)
                                          .arg (packet_num*DA_SEND_PACKET_LEN)
                                          .arg (QString::number (DA_SEND_PACKET_LEN/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }

    return true;
}

bool MTK_BROM_3352::hdl_download_race(uint32_t addr, const uint8_t *data)
{
    for (int i=0; i<3; i++)
    {
        RACE_DL_SEND send;
        send.head_ = 0x05;
        send.type_ = 0x5A;
        send.len_  = sizeof(send.id_)+sizeof(send.addr_)+sizeof(send.size_)+DA_SEND_PACKET_LEN+sizeof(send.crc_);
        send.id_   = RACE_DA_WRITE_BYTES;
        send.addr_ = addr;
        send.size_ = DA_SEND_PACKET_LEN;
        memcpy(send.buf_, data, send.size_);
        send.crc_ = CRC32 ((uint8_t*)&send, sizeof(send)-sizeof(send.crc_));
        io_->io_Write (QByteArray::fromRawData ((char *)&send, sizeof(send)),0,true);
        RACE_DL_RES res;
        QByteArray read=io_->io_Read (MAX_READ_TIMEOUT*5,sizeof (res));
        memcpy (&res,read,sizeof (res));
        if (res.head_ == 0x05 &&
            res.type_ == 0x5B &&
            res.len_ == (sizeof(res.id_)+sizeof(res.status_)+sizeof(res.addr_)) &&
            res.id_ == RACE_DA_WRITE_BYTES &&
            res.status_ == DA_S_DONE &&
            res.addr_ == addr)
        {
            return true;
        }
    QMSG(QString("try to download %1/%2").arg (i+1).arg (3))
    }
    QERROR(QString("Download Fail!"))
    return false;
}

bool MTK_BROM_3352::hdl_brom_set_baudrate(uint32_t bd)
{
    const char* BROM_CMD_SET_BAUD="DC";
    QINFO (QString("Set Baudrate %1 ...").arg (bd))
   QByteArray read= io_->syncWriteRead (QByteArray::fromHex (BROM_CMD_SET_BAUD),MAX_READ_TIMEOUT,1);
   if(read!=QByteArray::fromHex (BROM_CMD_SET_BAUD))
   {
       QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("# send:%1 RECEIVE:%2 != %3")
                                                                                             .arg (BROM_CMD_SET_BAUD)
                                                                                             .arg (QString(read.toHex ()))
                                                                                             .arg (BROM_CMD_SET_BAUD)
              )
       return false;
   }
   char *binsize_c=RS(reinterpret_cast<char*>(&bd),sizeof (bd));
   read=io_->syncWriteRead (QByteArray::fromRawData (binsize_c,sizeof (bd)),MAX_READ_TIMEOUT,4);
   if(read!=QByteArray::fromRawData (binsize_c,sizeof (bd)))
   {

       QERROR(DOWNSTATE(Download_State,download_state ())+LOGFRONT+QString("# send:%1 RECEIVE:%2 unequal RECEIVE:%3")
                                                                                                       .arg (QString(QByteArray::fromRawData (binsize_c,sizeof (bd)).toHex ()))
                                                                                                       .arg (QString(read.toHex ()))
                                                                                                       .arg (QString(read.toHex ()))
              )
       return false;
   }
   read=io_->io_Read (MAX_READ_TIMEOUT,2);
   setDownload_percentage (75);
   if(read==HEX2BYTES (BROM_ERROR_))
   {

       QERROR(DOWNSTATE(Download_State,Download_State::State_jump_to_DA)+LOGFRONT+QString("#  RECEIVE:%1 = BROM_ERROR:%2")
                                                                                           .arg (QString(read.toHex ()))
                                                                                           .arg (BROM_ERROR_)
              )
       return false;
   }
   io_->set_BuadRate (bd);
 //io_->io_asyncsleep (100);
 //io_->io_asyncsleep (200);//2022 0913  mould bug Send D7 uart ack D7 but Qserial not receieve   add 100->200
   io_->io_asyncsleep (500);//2023 0413  mould bug Send D7 uart ack D7 but Qserial not receieve
   return true;
}
