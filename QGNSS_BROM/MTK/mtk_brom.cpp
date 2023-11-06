/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_brom.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/
#include "mtk_brom.h"

MTK_BROM::MTK_BROM(MTK_Base *parent)
    : MTK_Base{parent}
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

void MTK_BROM::MTK_init_IO(IODvice *io)
{
    if(!io){return;}
    io_=io;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    connect (seril,&IOserial::errorOccurred,this,[&](QString err){
        QERROR ("[Init IO]"+LOGFRONT+"serialport error:"+err);
    });
    connect (seril,&IOserial::rw_InfoChanged,this,&MTK_BROM::rw_InfoChanged);
    init_baudRate_=seril->get_baudRate ();
    flashInfo()["PORTNAME"]=seril->portName ();
    emit flashInfoChanged ();
    seril=nullptr;
}

     MTK_BROM::~MTK_BROM()
{
    /*
    if(!io_)
    {
        io_->deleteLater ();
        io_=nullptr;
    }
    */
}

bool MTK_BROM::qGNSS_mTK_DownLoad(QString DA_Path, QString ROM_Path, int mtkmodel, int mode)
{
    bool flag=false;
    setCurrent_model ((BROM_BASE::QGNSS_Model)mtkmodel);
    if(startdownload ())
    {
        QLOG(QString("downloading...!"));
        return flag;
    }
    setStartdownload(true);
    DA_Path=DA_Path.trimmed ();
    ROM_Path=ROM_Path.trimmed ();
    QFile bin_file(DA_Path);
    QFile Rom_file(ROM_Path);
    if(!bin_file.exists ()){QERROR(QString("DA  file:%1 not exists!").arg (DA_Path));return flag ;}
    if(!Rom_file.exists ()){QERROR(QString("ROM file:%1 not exists!").arg (ROM_Path));return flag;}
    bin_file.open (QIODevice::ReadOnly);
    Rom_file.open (QIODevice::ReadOnly);
    QByteArray bin_b=bin_file.readAll ();
    QByteArray rom_b=Rom_file.readAll ();

    bin_file_bys=bin_b;
    rom_file_bys=rom_b;
    Rom_file.close ();
    bin_file.close ();
    da_base_addr=QByteArray::fromHex (MT3329ANDLATTER);
    downloadmode=(DownLoadMode)mode;
    QMetaEnum em=QMetaEnum::fromType<DownLoadMode>();
    flashInfo()["DownLoadMode"]=em.valueToKey (downloadmode);
    emit flashInfoChanged ();
    /* time  */
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
    switch (mode) {
    case MTK_BROM::DOWNLOAD:
        if(!QGNSS_StartCmd()){break;}
        if(!QGNSS_WriteBuf()){break;}
        if(!QGNSS_JumpCmd()){break;}
        if(download_baudRate_>high_baudRateforDA)/*switch highbaudrate*/
        {
            if(!QGNSS_CMD_download_baudrate()){break;}
        }
        if(!QGNSS_SetMemBlock()){break;}
        if(!QGNSS_WriteData()){break;}
        if(!QGNSS_Finish()){break;}
        flag=true;
        break;

    case MTK_BROM::REGIONFORMAT:
        if(!QGNSS_StartCmd()){break;}
        if(!QGNSS_WriteBuf()){break;}
        if(!QGNSS_JumpCmd()){break;}
        if(!QGNSS_FormatFlash(REGIONFORMATSIZE)){break;}
        if(!QGNSS_Finish()){break;}

        if(!QGNSS_StartCmd()){break;}
        if(!QGNSS_WriteBuf()){break;}
        if(!QGNSS_JumpCmd()){break;}
        if(download_baudRate_>high_baudRateforDA)/*switch highbaudrate*/
        {
            if(!QGNSS_CMD_download_baudrate()){break;}
        }
        if(!QGNSS_SetMemBlock()){break;}
        if(!QGNSS_WriteData()){break;}
        if(!QGNSS_Finish()){break;}
        flag=true;
        break;
    case MTK_BROM::TOTALFORMAT:
        if(!QGNSS_StartCmd()){break;}
        if(!QGNSS_WriteBuf()){break;}
        if(!QGNSS_JumpCmd()){break;}
        if(!QGNSS_FormatFlash(TOTALFORMATSIZE)){break;}
        if(!QGNSS_Finish()){break;}

        if(!QGNSS_StartCmd()){break;}
        if(!QGNSS_WriteBuf()){break;}
        if(!QGNSS_JumpCmd()){break;}
        if(download_baudRate_>high_baudRateforDA)/*switch highbaudrate*/
        {
            if(!QGNSS_CMD_download_baudrate()){break;}
        }
        if(!QGNSS_SetMemBlock()){break;}
        if(!QGNSS_WriteData()){break;}
        if(!QGNSS_Finish()){break;}
        flag=true;
        break;
    }
    setStartdownload(false);
    reset_buadRate ();
    timer.stop ();
    if(downloadstoping)
    {
        setDownload_state (Download_State::Custom_DownLoad_Stoped);
        downloadstoping=false;
    }
    return flag;
}

bool MTK_BROM::download_stop()
{
    setDownload_state (Download_State::Custom_DownLoad_Stoping);
    downloadstoping=true;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    Q_EMIT seril->send_Stop ();//stop reading func
    seril->io_asyncsleep (500);
    seril->io_clear ();
    return true;
}

void MTK_BROM::reset_buadRate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    /*send different baudRates   seven */
    seril->set_BuadRate(init_baudRate_);
    seril=nullptr;
}

void MTK_BROM::qGNSS_SetDownLoadBaudRate(int baudrate)
{
    download_baudRate_=baudrate;
    // setDownload_state (Download_State::State_setDownLoadBaudRate);
    flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
    emit flashInfoChanged ();
}

/**
 * @brief MTK_BROM::QGNSS_StartCmd
 * 1.NEMA_Sends START_CMD :$PMTK180*3B\r\n   send 7 times
 * 2.BOOT_ROM_START_CMD1  :(0xA0)  until read 0x5F interval 50ms
 * 3.BOOT_ROM_START_CMD2  :(0x0A)  ~BOOT_ROM_START_CMD2(0xF5)
 * 4.BOOT_ROM_START_CMD3  :(0x50)  ~BOOT_ROM_START_CMD3(0xAF)
 * 5.BOOT_ROM_START_CMD4  :(0x05)  ~BOOT_ROM_START_CMD4(0xFA)
 */
bool MTK_BROM::QGNSS_StartCmd()
{
    if(!io_->io_isOpen ())
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_StartCmd)+LOGFRONT+"serial isn't open");
        return false;
    }
    setDownload_state(Download_State::State_StartCmd);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    /*send different baudRates   seven */
    seril->set_BuadRate(download_baudRate_);
    for(int i=0;i<START_NMEA_START_CMD_SEND_TIMES;i++)
    {
        seril->set_BuadRate(seril->baudRate[START_NMEA_START_CMD_SEND_TIMES-i-1]);
        seril->io_clear ();
        seril->io_Write (START_NMEA_START_CMD,0,false);
        seril->io_asyncsleep (100);
    }
    if(download_baudRate_>=high_baudRateforDA)
    {
        seril->set_BuadRate(high_baudRateforDA);
        seril->io_clear ();
        seril->io_asyncsleep (20);
        seril->io_clear ();
    }
    else
    {
        seril->set_BuadRate(download_baudRate_);
        seril->io_clear ();
        seril->io_asyncsleep (20);
        seril->io_clear ();
    }
    int try_times=TRY_TIMES;
    bool cmd1_true=false;
    int i=0;
    while( try_times--/*||!ATOM01_START_CMD1_RPLY*/)
    {
        if(downloadstoping)
        {
            return false;
        }
        setDownload_percentage ((i+1)*100/(TRY_TIMES));

        QTime start=QTime::currentTime ();
        if( io_->syncWriteRead (QByteArray::fromHex (START_BOOT_ROM_START_CMD1),40,0)==QByteArray::fromHex (START_BOOT_ROM_START_CMD1_RPLY))
        {
            cmd1_true=true;
            break;
        }
        QTime end=QTime::currentTime ();
        int tm=start.msecsTo (end);
        if(tm<40)
        {
            io_->io_asyncsleep (40-tm);
        }
        i++;
    }

    if(!cmd1_true)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_StartCmd)+LOGFRONT+ QString("send: %1  total :%2 times ACK unequal :%3")
                                                                                          .arg (START_BOOT_ROM_START_CMD1)
                                                                                          .arg (TRY_TIMES)
                                                                                          .arg (START_BOOT_ROM_START_CMD1_RPLY)
               );
        return false;
    }
    QByteArray read_data=io_->syncWriteRead (QByteArray::fromHex (START_BOOT_ROM_START_CMD2),START_NMEA_START_CMD_TIMEOUT,1);
    if( read_data!=QByteArray::fromHex (START_BOOT_ROM_START_CMD2_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_StartCmd)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (START_BOOT_ROM_START_CMD2_RPLY)
               );
        return false;
    }
    read_data=io_->syncWriteRead (QByteArray::fromHex (START_BOOT_ROM_START_CMD3),START_NMEA_START_CMD_TIMEOUT,1);
    if( read_data!=QByteArray::fromHex (START_BOOT_ROM_START_CMD3_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_StartCmd)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (START_BOOT_ROM_START_CMD3_RPLY)
               );
        return false;
    }
    read_data=io_->syncWriteRead (QByteArray::fromHex (START_BOOT_ROM_START_CMD4),START_NMEA_START_CMD_TIMEOUT,1);
    if( read_data!=QByteArray::fromHex (START_BOOT_ROM_START_CMD4_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_StartCmd)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (START_BOOT_ROM_START_CMD3_RPLY)
               );
        return false;
    }

    seril=nullptr;
    return true;
}

/**
 * @brief MTK_BROM::QGNSS_WriteBuf
 * 1. BOOT_ROM_WRITE_CMD  :(0xA1) BOOT_ROM_WRITE_CMD(0xA1)
 * 2. DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00)  DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00)
 * 3. DA_SIZE_NUM_WORD     4 bytes                 DA_SIZE_NUM_WORD
 * 4. Send DA with 10 bytes                 ACK with 10 bytes sent
 *       .
 *       .
 *       .
 * 5.Send DA with last bytes                ACK with last bytes sent
 * 6.BOOT_ROM_CHECKSUM_CMD(0xA4)            BOOT_ROM_CHECKSUM_CMD(0xA4)
 * 7.DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00)   DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00)
 * 8.DA_SIZE_NUM_WORD                       DA_SIZE_NUM_WORD  +  DA_CHECKSUM
 */
bool MTK_BROM::QGNSS_WriteBuf()
{
    /*
    if(!io_->io_isOpen ())
    {
        QLOG("serial isn't open");
        return false;
    }
    */
    setDownload_state(Download_State::State_WriteBuf);
    QByteArray read_data= io_->syncWriteRead (QByteArray::fromHex (WRITE_BOOT_ROM_WRITE_CMD),MAX_READ_TIMEOUT,1);
    if(read_data!=QByteArray::fromHex (WRITE_BOOT_ROM_WRITE_CMD_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (WRITE_BOOT_ROM_WRITE_CMD_RPLY)
              );
        return false;
    }
    read_data=io_->syncWriteRead (da_base_addr,MAX_READ_TIMEOUT,4);
    if( read_data!=da_base_addr)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (QString(da_base_addr.toHex ().toUpper ()))
               );
        return false;
    }

    int binsize= bin_file_bys.size ();/*to bigendain*/
    int hexbinsize=binsize/2;
    char *binsize_c=reinterpret_cast<char*>(&hexbinsize);// ABCD
    QByteArray bis;
    //DCBA
    bis.append (binsize_c[3]).append (binsize_c[2]).append (binsize_c[1]).append (binsize_c[0]);
    read_data=io_->syncWriteRead (bis,MAX_READ_TIMEOUT,4);
    if(read_data !=bis)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (QString(bis.toHex (' ').toUpper ()))
               );
        return false;
    }

    int send_ADD=binsize%10;

    int sendsize =binsize/10;
    int i=0;
    setDownload_percentage (0);
    for( i=0;i<sendsize;i++)
    {
        QTime start_t=QTime::currentTime ();
        QByteArray data=    swip_each_16 (bin_file_bys.mid (i*10,10));
        read_data=io_->syncWriteRead (data,MAX_READ_TIMEOUT,10);
        if(read_data !=data)
        {
            QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                              .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                              .arg (QString(data.toHex (' ').toUpper ()))
                   );
            return false;
        }
        if(send_ADD)
        {
            setDownload_percentage(i*100/(sendsize+1)+1);
        }
        else
        {
            setDownload_percentage(i*100/(sendsize)+1);
        }
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 KBps").arg ((i+1)*10).arg (QString::number (10/ms/1024,'f',1));
        emit flashInfoChanged ();
    }
    if(send_ADD)
    {
        QTime start_t=QTime::currentTime ();
        QByteArray data=    swip_each_16 (bin_file_bys.right (send_ADD).leftJustified (10,0));
        read_data=io_->syncWriteRead (data,MAX_READ_TIMEOUT,10);
        if(read_data!=data)
        {
            QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                              .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                              .arg (QString(data.toHex (' ').toUpper ()))
                   );
            return false;
        }
        setDownload_percentage(i*100/(sendsize+1)+1);
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 KBps").arg ((i+1)*10).arg (QString::number (10/ms/1024,'f',1));
        emit flashInfoChanged ();
    }
    read_data=io_->syncWriteRead (QByteArray::fromHex (WRITE_BOOT_ROM_CHECKSUM_CMD),MAX_READ_TIMEOUT,1);
    if(read_data!=QByteArray::fromHex (WRITE_BOOT_ROM_CHECKSUM_CMD))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ')))
                                                                                          .arg (QString(WRITE_BOOT_ROM_CHECKSUM_CMD))
               );
        return false;
    }
    read_data=io_->syncWriteRead (da_base_addr,MAX_READ_TIMEOUT,4);
    if(read_data!=da_base_addr)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (QString(da_base_addr.toHex (' ').toUpper ()))
               );
        return false;
    }
    read_data=io_->syncWriteRead (bis,MAX_READ_TIMEOUT,4);
    if(read_data!=bis)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (QString(bis.toHex (' ').toUpper ()))
               );
        return false;
    }

    /*
    read_data=io_->io_Read (MAX_READ_TIMEOUT,2);
    if(read_data!=QByteArray::fromHex (WRITE_DA_CHECKSUM_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
                                                                                          .arg (QString(WRITE_DA_CHECKSUM_RPLY))
               );
        return false;
    }
    */
    //*********DA_CHECKSUM************//2022 04 18 add
    quint16 q16checksum= DA_CHECKSUM (reinterpret_cast<uint8_t*>( bin_file_bys.data ()),bin_file_bys.size ());
    uchar * checksum=reinterpret_cast<uchar*>(&q16checksum );
    QByteArray qchecksum;
    qchecksum.append (checksum[1]).append (checksum[0]);
      read_data=io_->io_Read (MAX_READ_TIMEOUT,2);
    if(qchecksum!=read_data/*QByteArray::fromHex (WRITE_DA_CHECKSUM_RPLY)*/)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_WriteBuf)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(qchecksum.toHex (' ').toUpper ()))
                                                                                          .arg (QString(read_data.toHex (' ').toUpper ()))
               );
        return false;
    }
    //********************************//
    return true;
}
/**
 * @brief MTK_BROM::QGNSS_JumpCmd
 * 1.BOOT_ROM_JUMP_CMD(0xA8)                BOOT_ROM_JUMP_CMD(0xA8)
 * 2.DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00)   DA_BASE_ADDR(0x00, 0x00, 0x0C, 0x00) + CMD_Sync (Report Flash Information)
 */
bool MTK_BROM::QGNSS_JumpCmd()
{

    setDownload_state(Download_State::State_JumpCmd);
    if(io_->syncWriteRead (QByteArray::fromHex (JUMP_BOOT_ROM_JUMP_CMD),MAX_READ_TIMEOUT,1)!=QByteArray::fromHex (JUMP_BOOT_ROM_JUMP_CMD))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_JumpCmd)+LOGFRONT+"JUMP_BOOT_ROM_JUMP_CMD is different from ACK ");
        return false;
    }

    if(io_->syncWriteRead (da_base_addr,MAX_READ_TIMEOUT,4)!=da_base_addr)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_JumpCmd)+LOGFRONT+"JUMP_da_base_addr is different from ACK ");
        return false;
    }

    QByteArray da_base_addr_reply=io_->io_Read (MAX_READ_TIMEOUT,20);
    //  qDebug()<<da_base_addr_reply.size ()<<da_base_addr_reply.toHex ();
    if(da_base_addr_reply.size ()<20)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_JumpCmd)+LOGFRONT+"JUMP_da_base_addr_read_size < 20 ");
        return false;
    }
    return  QGNSS_CMD_Sync(da_base_addr_reply);

}
/**
 * @brief MTK_BROM::QGNSS_SetMemBlock
 *
 * 1. DA_MEM_CMD(0xD3)+MEM_BLOCK_COUNT(0x01)+BLOCK:BEGIN_ADDRESS(0x00000000)+BLOCK: END_ADDRESS         ACK 5A + UNCHANGED_BLOCK_COUNT
 * @return
 */
bool MTK_BROM::QGNSS_SetMemBlock()
{
    setDownload_state(Download_State::State_CMD_SetMemBlock);
    int int_END_ADDRESS=rom_file_bys.size ()-1;
    char * end_address=reinterpret_cast<char*>(&int_END_ADDRESS);
    QByteArray big_end_address;
    big_end_address.append (end_address[3]).append (end_address[2]).append (end_address[1]).append (end_address[0]);

    io_->io_Write (IODvice::cchrAppend ({SETMEMBLOCK_DA_MEM_CMD,SETMEMBLOCK_BLOCK_COUNT}),0,true);
    io_->io_Write (QByteArray::fromHex (SETMEMBLOCK_BEGIN_ADDRESS),0,true);
    io_->io_Write (big_end_address,0,true);
    QByteArray read_io= io_->io_Read (MAX_READ_TIMEOUT,2);
    if(read_io!=QByteArray::fromHex ("5A00"))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_SetMemBlock)+LOGFRONT+QString("%1 is different from ACK  %2")
                                                                                                 .arg(QString(read_io.toHex (' ').toUpper ()))
                                                                                                 .arg (MAX_READ_TIMEOUT)
               );
        return false;
    }
    return true;
}
/**
 * @brief MTK_BROM::QGNSS_WRiteData
 * 1.DA_WRITE_CMD(0xD5)+PACKET_LENGTH(0x00000400)         ACK SAVE_UNCHANGEDDATA_ACK(0x5A)+ERASE_1st_SECTOR_ACK(0x5A)
 * 2. Download BIN PACKET[1024+2]=DATA[1024]+CHECKSUM[2]  ACK CONT_CHAR[0x69]
 *    .
 *    .
 *    .
 * 3. Wait for DA to perform flash checksum
 * 4 .ACK(0x5A)
 * 5. ACK(0x5A)
 * @return
 */
bool MTK_BROM::QGNSS_WriteData()
{
    setDownload_state(Download_State::State_CMD_WriteData);
    const char* packet_length_hex=WRITEDATA_PACKET_LENGTH;
    if(download_baudRate_>high_baudRateforDA)
    {
        packet_length_hex=WRITEDATA_PACKET_LENGTH;
    }
    else
    {
        packet_length_hex=WRITEDATA_LOWBAUDRATEPACKET_LENGTH ;
    }

    QByteArray send_cmd  = IODvice::cchrAppend ({WRITEDATA_DA_WRITE_CMD,packet_length_hex});

    io_->io_Write (send_cmd,0,true);
    QByteArray read =io_->io_Read (MAX_READ_TIMEOUT,1);
    if(read!=QByteArray::fromHex (WRITEDATA_DA_WRITE_CMD_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                               .arg (QString(read.toHex (' ').toUpper ()))
                                                                                               .arg (QString(WRITEDATA_DA_WRITE_CMD_RPLY))
               );
        return false;
    }
    read =io_->io_Read (MAX_READ_TIMEOUT,1);
    if(read!=QByteArray::fromHex (WRITEDATA_DA_WRITE_CMD_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                               .arg (QString(read.toHex (' ').toUpper ()))
                                                                                               .arg (QString(WRITEDATA_DA_WRITE_CMD_RPLY))
               );
        return false;
    }
    int packet_length=WRITEDATA_PACKET_LENGTH_DEC;

    if(download_baudRate_>high_baudRateforDA)
    {
        packet_length=WRITEDATA_PACKET_LENGTH_DEC;
    }
    else
    {
        packet_length=WRITEDATA_LOWBAUDRATEPACKET_LENGTH_DEC;
    }
    int sendadd=rom_file_bys.size ()%packet_length;
    int sendtimes=rom_file_bys.size ()/packet_length;
    QByteArray send_bin_byts;
    int i=0;
    setDownload_percentage(0);
    for( i=0;i<sendtimes;i++)
    {
        QTime start_t=QTime::currentTime ();
        send_bin_byts=rom_file_bys.mid (i*packet_length,packet_length);
        quint16 q16checksum= DATA_CHECKSUM(reinterpret_cast<uint8_t*>( send_bin_byts.data ()),send_bin_byts.size ());
        uchar * checksum=reinterpret_cast<uchar*>(&q16checksum );
        QByteArray qchecksum;
        qchecksum.append (checksum[1]).append (checksum[0]);
        io_->io_Write (send_bin_byts,0,false);
        io_->io_Write (qchecksum,0,false);

        QByteArray read_=io_->io_Read (MAX_READ_TIMEOUT,1);

        if(read_!=QByteArray::fromHex (WRITEDATA_CONT_CHAR))
        {
            QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                                   .arg (QString(read_.toHex (' ').toUpper ()))
                                                                                                   .arg (QString(WRITEDATA_CONT_CHAR))
                   );
            return false;
        }

        if(sendadd)
        {
            setDownload_percentage(i*100/(sendtimes+1)+1);
        }
        else
        {
            setDownload_percentage(i*100/(sendtimes)+1);
        }

        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 KBps").arg ((i+1)*packet_length).arg (QString::number (packet_length/ms/1024,'f',1));
        emit flashInfoChanged ();
    }

    if(sendadd)
    {
        QTime start_t=QTime::currentTime ();
        send_bin_byts=rom_file_bys.right (sendadd);
        quint16 q16checksum= DATA_CHECKSUM(reinterpret_cast<uint8_t*>(send_bin_byts.data ()),send_bin_byts.size ());
        char * checksum=reinterpret_cast<char*>(&q16checksum);
        QByteArray qchecksum;
        qchecksum.append (checksum[1]).append (checksum[0]);
        io_->io_Write ( send_bin_byts,0,true);
        io_->io_Write (qchecksum,0,true);
        QByteArray read_=io_->io_Read (MAX_READ_TIMEOUT,1);

        if(read_!=QByteArray::fromHex (WRITEDATA_CONT_CHAR))
        {
            QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                                   .arg (QString(read_.toHex (' ').toUpper ()))
                                                                                                   .arg (QString(WRITEDATA_DA_WRITE_CMD_RPLY))
                   );
            return false;
        }
        setDownload_percentage(i*100/(sendtimes+1)+1);
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 KBps").arg ((i)*packet_length+sendadd).arg (QString::number (sendadd/ms/1024,'f',1));
        emit flashInfoChanged ();
    }
    QByteArray read_=io_->io_Read (MAX_READ_TIMEOUT,1);
    if(read_!=QByteArray::fromHex (WRITEDATA_DA_WRITE_CMD_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                               .arg (QString(read_.toHex (' ').toUpper ()))
                                                                                               .arg (QString(WRITEDATA_DA_WRITE_CMD_RPLY))
               );
        return false;
    }

    read_=io_->io_Read (MAX_READ_TIMEOUT,1);
    if(read_!=QByteArray::fromHex (WRITEDATA_DA_WRITE_CMD_RPLY))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_WriteData)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                               .arg (QString(read_.toHex (' ').toUpper ()))
                                                                                               .arg (QString(WRITEDATA_DA_WRITE_CMD_RPLY))
               );
        return false;
    }
    return true;
}

/**
 * @brief MTK_BROM::QGNSS_FormatFlash
 * @param formatlength
 * 1.send 0xD4
 * 2.send 0x00700000
 * 3.send hex lenght
 * 4 read format_percentage(1)    0x04 0x01
 * .
 * .
 * .
 *    read format_percentage(100) 0x04 0x64
 *    ACK(0x5A)
 * @return
 */
bool MTK_BROM::QGNSS_FormatFlash(qint32 formatlength)
{
    setDownload_state(Download_State::State_CMD_FormatFlash);
    io_->io_asyncsleep (56);
    io_->io_Write (QByteArray::fromHex (FORMATFLASH_FORMAT_CMD),0,true);
    io_->io_Write (QByteArray::fromHex (FORMATFLASH_FORMAT_BEGIN_ADDR),0,true);
    QByteArray b_lenght=QByteArray::number (formatlength,16);
    io_->io_Write (QByteArray::fromHex (b_lenght).rightJustified(4,0),0,true);
    io_->io_asyncsleep (8);
    int i=100;
    bool read_percent=false;
    setDownload_percentage(0);
    while (i--)
    {
        QByteArray readbits;
        if(download_percentage ()==100)
        {
         readbits= io_->io_Read (MAX_READ_TIMEOUT,1);
        }
        else
        {
         readbits= io_->io_Read (MAX_READ_TIMEOUT,2);
        }
        if(readbits==QByteArray::fromHex (FORMATFLASH_ACK))
        {
            read_percent=true;
            break;
        }
        else
        {
            if(readbits.size ()>=2)
            {
                QByteArray percent;
                percent .append (readbits[1]);
                int pct=percent.toHex ().toInt (nullptr,16);
                setDownload_percentage(pct);
            }
        }


    }
    if(!read_percent)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_FormatFlash)+LOGFRONT+ QString("ACK != %1").arg (FORMATFLASH_ACK));
        return false;
    }

    return true;
}

bool MTK_BROM::QGNSS_Finish()
{
    io_->io_Write (QByteArray::fromHex (FINISH_DA_FINISH_CMD),1,false);
    io_->io_Write (QByteArray::fromHex (FINISH_DA_FINISH_CMD),1,false);
    setDownload_state(Download_State::State_CMD_Finish);
    return true;
}

bool MTK_BROM::QGNSS_CMD_Sync(const QByteArray &bytes)
{
    setDownload_state(Download_State::State_CMD_Sync);

    if(bytes.mid (0,1)!=QByteArray::fromHex (SYNC_SYNC_CHAR))
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_CMD_Sync)+LOGFRONT+ QString("ACK %1  different from %2")
                                                                                          .arg (QString(bytes.mid (0,1).toHex (' ').toUpper ()))
                                                                                          .arg (QString(SYNC_SYNC_CHAR))
               );
        return false;
    }

    flashInfo()["SYNC_CHAR"]=QVariant(QString(bytes.mid (0,1).toHex ().toUpper ()));              //1
    flashInfo()["DA_VERSION"]=QVariant(QString(bytes.mid (1,2).toHex ().toUpper ()));             //2
    flashInfo()["FLASH_DEVICE_ID"]=QVariant(QString(bytes.mid (3,1).toHex ().toUpper ()));        //1
    flashInfo()["FLASH_SIZE"]=QVariant(QString(bytes.mid (4,4).toHex ().toUpper ()));             //4
    flashInfo()["FLASH_MANUFACTURE_CODE"]=QVariant(QString(bytes.mid (8,2).toHex ().toUpper ())); //2
    flashInfo()["FLASH_DEVICE_CODE"]=QVariant(QString(bytes.mid (10,2).toHex ().toUpper ()));     //2
    flashInfo()["FLASH_EXT_DEVICE_CODE1"]=QVariant(QString(bytes.mid (12,2).toHex ().toUpper ()));//2
    flashInfo()["FLASH_EXT_DEVICE_CODE2"]=QVariant(QString(bytes.mid (14,2).toHex ().toUpper ()));//2
    flashInfo()["EXT_SRAM_SIZE"]=QVariant(QString(bytes.mid (16,4).toHex ().toUpper ()));         //4

    flashInfo()["TooltipTXT"]=QString( "FLASH_DEVICE_ID:   0x%1\r\n"
                                        "FLASH_DEVICE_CODE: 0x%2\r\n"
                                        "FLASH_SIZE:        0x%3\r\n"
                                        "EXT_SRAM_SIZE:     0x%4"
                                        ).arg (flashInfo ()["FLASH_DEVICE_ID"].toString ())
                                    .arg (flashInfo ()["FLASH_DEVICE_CODE"].toString ())
                                    .arg (flashInfo ()["FLASH_SIZE"].toString ())
                                    .arg (flashInfo ()["EXT_SRAM_SIZE"].toString ());
    emit flashInfoChanged ();
    return true;
}

bool MTK_BROM::QGNSS_CMD_download_baudrate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());

    /*test send D2 02 01 ACK 5A*/
    const char* switch_cmd="0201";
    if(download_baudRate_==230400)
    {
        switch_cmd="0301";
    }
    if(download_baudRate_==460800)
    {
        switch_cmd="0201";
    }
    if(download_baudRate_==921600)
    {
        switch_cmd="0101";
    }
    QByteArray swh_cmd=io_->cchrAppend ({"D2",switch_cmd});
    io_->io_Write (swh_cmd,0,true);
    QByteArray start_data= io_->io_Read (MAX_READ_TIMEOUT,1);
    if(start_data!=QByteArray::fromHex (SETMEMBLOCK_ACK))
    {
        QERROR(LOGFRONT+ QString("ACK %1  different from %2")
                              .arg (QString(start_data.toHex (' ')))
                              .arg (QString(SETMEMBLOCK_ACK))
               );

        return false;
    }
    io_->io_asyncsleep (38);

    seril->set_BuadRate(download_baudRate_);
    seril->io_clear ();
    io_->io_asyncsleep (100);
    seril->io_clear ();

    QByteArray senddata;
    bool matching_=false;
    for(int i=0;i<10;i++)
    {

        io_->io_Write (QByteArray::fromHex ("C0"),0,true);
        QByteArray senddata= io_->io_Read (MAX_READ_TIMEOUT,1);

        if(senddata==QByteArray::fromHex ("C0"))
        {
            io_->io_Write (QByteArray::fromHex ("5A"),0,true);
            QByteArray senddata= io_->io_Read (MAX_READ_TIMEOUT,1);
            if(senddata==QByteArray::fromHex ("5A"))
            {
                matching_=true;
            }
            break;
        }
    }
    if(!matching_)
    {
        QERROR(DOWNSTATE(Download_State,Download_State::State_setDownLoadBaudRate)+LOGFRONT+QString("Switch baud rate:%1  Fail ").arg(download_baudRate_));
        return false;
    }

    for(int i=0;i<=255;i++)
    {
        QByteArray re=QByteArray::number (i,16);

        io_->io_Write (QByteArray::fromHex (re),0,true);
        senddata= io_->io_Read (MAX_READ_TIMEOUT,1);
        if(senddata!=QByteArray::fromHex (re))
        {
            QERROR(DOWNSTATE(Download_State,Download_State::State_setDownLoadBaudRate)+LOGFRONT+ QString("ACK: %1  different from write: %2")
                                  .arg (QString(senddata.toHex (' ')))
                                  .arg (QString(re))
                   );
            return false;
        }

    }
    return true;
}

//*********************class property**********************//

MTK_BROM::Download_State MTK_BROM::download_state() const
{
    return download_state_;
}

void MTK_BROM::setDownload_state(Download_State newDownload_state)
{
    /*
    if (download_state_ == newDownload_state)
        return;
    */
    download_state_ = newDownload_state;
    switch (newDownload_state) {
    case MTK_BROM::State_Unstart:
        statekeyvalue()->setKey (tr("Ready to download"));
        break;
    case MTK_BROM::State_StartCmd:
        statekeyvalue()->setKey (tr("Start Upgrade Procedure..."));
        break;
    case MTK_BROM::State_WriteBuf:
        statekeyvalue()->setKey (tr("Download DA..."));
        break;
    case MTK_BROM::State_JumpCmd:
        statekeyvalue()->setKey (tr("Jump to DA..."));
        break;
    case MTK_BROM::State_CMD_SetMemBlock:
        statekeyvalue()->setKey (tr("Set Memory Block..."));
        break;
    case MTK_BROM::State_CMD_WriteData:
        statekeyvalue()->setKey (tr("Download Firmware..."));
        break;
    case MTK_BROM::State_CMD_FormatFlash:
        statekeyvalue()->setKey (tr("Format..."));
        break;
    case MTK_BROM::State_CMD_Sync:
        statekeyvalue()->setKey (tr("Sync with DA..."));
        break;
    case MTK_BROM::State_setDownLoadBaudRate:
        statekeyvalue()->setKey (tr("Set DownLoad BaudRate"));
        break;
    case MTK_BROM::State_CMD_Finish:
        statekeyvalue()->setKey (tr("Download Finish"));
        break;
    case MTK_BROM::State_CMD_Fail:
        statekeyvalue()->setKey (tr("Download Fail"));
        break;
    case MTK_BROM::Custom_DownLoad_Stoping:
        statekeyvalue()->setKey (tr("Downloading is stopping"));
        break;
    case MTK_BROM::Custom_DownLoad_Stoped:
        statekeyvalue()->setKey (tr("Downloading is stopped"));
        break;
    }

    statekeyvalue()->setValue ((int)newDownload_state);
    emit statekeyvalueChanged();
    emit download_stateChanged(download_state_);
}
