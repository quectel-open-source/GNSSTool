/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_upgrade.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-01      victor.gong
* ***************************************************************************/
#include "st_upgrade.h"
#include "ST_BootLoader.h"

#define TRGT_VARIANT_SECURED         0x8000U
#define TRGT_VARIANT_UNKNOWN         0U
#define TRGT_VARIANT_STA8100_CUT1    1U
#define TRGT_VARIANT_STA8100_CUT2    2U
#define TRGT_VARIANT_STA8100_CUT2S   3U
#define TRGT_VARIANT_STA8100_NUMBER  4U

#define XLDR_TESEO5_FLASHERVER_OFFSET  0x60U

#define ERASE_DISABLE                0
#define ERASE_AREA                   1 /* Erase area : NVM or other */
#define ERASE_CHIPSET                2 /* Erase chipset */
#define ERASE_ALL                    3 /* Erase all     */

#define TESEO5_MAX_ROM_BR            230400U
#define TESEO5_MAX_ROM_BR_IDX        2U

#define CUSTOM_NMEA_REGEX "\\$[,.A-Za-z0-9]{1,}\\*"

ST_Upgrade::ST_Upgrade(QObject *parent)
    :ST_Base(parent)
{

}

ST_Upgrade::~ST_Upgrade()
{

}

bool ST_Upgrade::ConnectModule()
{
    setUpgrade_state (upgrade_flow::ConnectModule);
    QByteArray read;
    bool isreadNMEA=false;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_RTS (true);
    io_->io_clear ();
    io_->io_asyncsleep (100);
    static int tms=40;
    for(int i=0;i<tms;i++)
    {
        if(downloadstoping)
        {
            return false;
        }
        setDownload_percentage ((i+1)*100/tms);
        read.append (io_->io_Read (100));
        if(nmea_help_.isContainsNMEA (read,CUSTOM_NMEA_REGEX))
        {
            setDownload_percentage (100);
            isreadNMEA=true;
            break;
        }
    }

    if(!isreadNMEA)
    {
        QMSG<MSGType::bb_Warning>("[ConnectModule]"+LOGFRONT+": read NMEA Data :"+QString(read.toHex (' ').toUpper ()));
        QMSG<MSGType::bb_Warning>("[ConnectModule]"+LOGFRONT+": NO NMEA Data,No need to send :"+QString(command_->Send));
        return false;
    }
    io_->io_clear ();
    return true;
}

bool ST_Upgrade::SendFirmwareUpgradeCommand()
{
    setUpgrade_state (upgrade_flow::SendFirmwareUpgradeCommand);
    bool flag=true;
    QByteArray read;
    io_->io_Write (command_->Send,0,true);
    bool  isResultOk=false;
    static int tms=50;
    for(int i=0;i<tms;i++)
    {
        if(downloadstoping)
        {
            return false;
        }

        setDownload_percentage ((i+1)*100/tms);
        read.append (io_->io_Read (100));
        if(read.contains (command_->ResultOK))
        {
            setDownload_percentage (100);
            isResultOk= true;
            break;
        }
    }
    if(!isResultOk)
    {
        QMSG<MSGType::bb_Error>("[Send Firmware Upgrade Command]"+LOGFRONT+QString(":send command:%1Result:%2 do not contains:%3")
                                                                                   .arg (QString(command_->Send))
                                                                                   .arg (QString(read))
                                                                                   .arg (QString(command_->ResultOK))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );

        flag=false;
    }
    io_->io_clear ();

    return flag;
}

bool ST_Upgrade::SynchronizeModule()
{
    setUpgrade_state (upgrade_flow::SynchronizeModule);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_BuadRate (115200);
    io_->io_asyncsleep (100);
    bool receiveSYNC=false;
    QByteArray read;
    auto sync=[&]()
    {
        static int tms=512;
        for(int i=0;i<tms;i++)
        {
            if(downloadstoping)
            {
                return false;
            }
            setDownload_percentage ((i+1)*100/tms);
            io_->io_Write (reverse_4_bys (HEX2BYTES  (constants_.sFLASHER_IDENTIFIER)),0,true);
            read+=io_->io_Read (1);
            if(read.contains (reverse_4_bys (HEX2BYTES (constants_.sFLASHER_SYNC))))
            {
                setDownload_percentage (100);
                receiveSYNC=true;
                return  true;
            }
        }
        return  false;
    };
    QTime start=QTime::currentTime ();
    QTime end  =QTime::currentTime ();
    bool synced=false;

    while(start.msecsTo (end)<10000)//5->10S
    {
        if(sync())
        {
            synced=true;
            break;
        }
        end=QTime::currentTime ();
    }
    if(!synced)
    {
        QMSG<MSGType::bb_Error>("[Synchronize Module]"+LOGFRONT+QString("Synchronize fail!")
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }
    read=io_->syncWriteRead (HEX2BYTES (constants_.sDEVICE_START_COMMUNICATION),MAX_READ_TIMEOUT,1);
    if(read!=HEX2BYTES (constants_.sACK))
    {
        QMSG<MSGType::bb_Error>("[Synchronize Module]"+LOGFRONT+QString(":send command:%1 Result:%2 unequal:%3 ")
                                                                        .arg (QString(constants_.sDEVICE_START_COMMUNICATION))
                                                                        .arg (QString(read))
                                                                        .arg (QString(constants_.sACK))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return  false;
    }
    return true;
}

bool ST_Upgrade::WriteBinaryImageOptions(bool showExpert)
{
    setUpgrade_state (upgrade_flow::WriteBinaryImageOptions);
    if(showExpert)
    {
      Q_EMIT showOPTCFG(&opt_);
    }
    char* opt_c=reinterpret_cast<char*>(&opt_);
    QByteArray sendopt=QByteArray(opt_c,20);

    io_->io_Write (sendopt,0,true);

    IOserial *seril=static_cast<IOserial *>(io_.data ());
    io_->io_asyncsleep (100);
    if(download_baudRate_>115200)
    {
        seril->set_BuadRate(download_baudRate_);
    }
    io_->io_clear ();
    io_->io_asyncsleep (100);
    int tms=50;//CON'T SEND MULT TIMES
    bool isready=false;
    for(int i=0;i<tms;i++)
    {
      if(downloadstoping)
      {
        return false;
      }
        setDownload_percentage ((i+1)*100/tms);
        io_->io_clear ();
        io_->io_Write (HEX2BYTES (constants_.sFLASHER_READY),0,true);
        QByteArray read=io_->io_Read (500,1);
        if(read==HEX2BYTES(constants_.sACK))
        {
            read=io_->io_Read (1000,1);
            if(read==HEX2BYTES(constants_.sACK))
            {
                setDownload_percentage (100);
                isready=true;
                break;
            }
        }
    }
    if(!isready)
    {
        QMSG<MSGType::bb_Error>("[Write Options And Change UART BaudRate]"+LOGFRONT+QString(":send command:%1 times:%2 unreceive:%3 ")
                                                                                            .arg (QString(constants_.sFLASHER_READY))
                                                                                            .arg (QString::number (tms))
                                                                                            .arg (QString("CCCC"))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_Upgrade::EraseProgramArea()
{
    setUpgrade_state (upgrade_flow::EraseProgramArea);
    setDownload_percentage (0);
    QByteArray read=  io_->io_Read (30*1000,1);
    setDownload_percentage (100);
    if(read!=HEX2BYTES (constants_.sACK))
    {
        QMSG<MSGType::bb_Error>("[Erase Program Area]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                        .arg (QString(read))
                                                                        .arg (QString(constants_.sACK))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_Upgrade::EraseNVM()
{
    if(opt_.eraseNVM)
    {
        setUpgrade_state (upgrade_flow::EraseNVM);
        setDownload_percentage (0);
        QByteArray read=  io_->io_Read (10*1000,1);
        setDownload_percentage (100);
        if(read!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Erase NVM]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                   .arg (QString(read))
                                                                   .arg (QString(constants_.sACK))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            return false;
        }
        return true;
    }
    return true;
}

bool ST_Upgrade::SendFirmwarePackage()
{
    setUpgrade_state (upgrade_flow::SendFirmwarePackage);
    int remainddata=firmwaredata_.size ()% constants_.schunks;
    int times=firmwaredata_.size ()/constants_.schunks;
    QByteArray data;
    for(int i=0;i<times;i++)
    {
        QTime start_t=QTime::currentTime ();
        data= io_->syncWriteRead (firmwaredata_.mid (i*constants_.schunks,constants_.schunks),MAX_READ_TIMEOUT*10,1);
        if(data!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Send Firmware Package]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                               .arg (QString(data))
                                                                               .arg (QString(constants_.sACK))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            return false;
        }
        if(remainddata)
        {
            setDownload_percentage((i+1)*100/(times+1));
        }
        else
        {
            setDownload_percentage((i+1)*100/(times));
        }
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg ((i+1)*constants_.schunks)
                                          .arg (firmwaredata_.size ())
                                          .arg (QString::number (constants_.schunks/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }
    if(remainddata)
    {
        QTime start_t=QTime::currentTime ();
        io_->io_Write (firmwaredata_.right (remainddata)/*.trimmed ()*/,0,true);
        data=io_->io_Read (MAX_READ_TIMEOUT*10);
        if(data!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Send Data]"+LOGFRONT+QString(" Receive:%1unequal:%2")
                                                                   .arg (QString(data))
                                                                   .arg (QString(constants_.sACK))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1").arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            return false;
        }
        setDownload_percentage(100);

        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (firmwaredata_.size ())
                                          .arg (firmwaredata_.size ())
                                          .arg (QString::number (remainddata/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }
    return true;
}

bool ST_Upgrade::PerformCRCErrorCheck()
{
    setUpgrade_state (upgrade_flow::PerformCRCErrorCheck);
    setDownload_percentage (0);
    QByteArray read=  io_->io_Read (60*1000,1);
    setDownload_percentage (100);
    if(read!=HEX2BYTES (constants_.sACK))
    {
        QMSG<MSGType::bb_Error>("[CRC Error Check]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                     .arg (QString(read))
                                                                     .arg (QString(constants_.sACK))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }

    return true;
}

bool ST_Upgrade::Finished()
{
    //IOserial *seril=static_cast<IOserial *>(io_.data ());
    //seril->set_RTS (false);
    io_->io_clear ();
    setUpgrade_state(upgrade_flow::Finished);
    return true;
}


bool ST_Upgrade::xldr_teseo_init(bool showExpert)
{
    uint file_crc32 = 0;
    CRC32_EVAL_INT(file_crc32, file_crc32, firmwaredata_.size ());
    CRC32_EVAL_BUF(file_crc32, file_crc32, firmwaredata_.data (), firmwaredata_.size ());

    binimg_info_t_.fileSize = firmwaredata_.size ();
    binimg_info_t_.crc32 = file_crc32;
    choose_param((int)DownloadConstants_.OUTPUTMode,false,false, opt_.eraseNVM>0,false,false);
    if(showExpert)
    {
      Q_EMIT showDWNOPTCFG (&binimg_info_t_);
    }
    /* Prepare preamble */
    preamble_.destinationAddress =  DownloadConstants_.sTP_DEF_DEST_ADDR;
    preamble_.identifier_msp =  DownloadConstants_.sTP_ID_MSP;
    preamble_.identifier_lsp =  DownloadConstants_.sTP_ID_LSP;
    preamble_.options = (0xFFU << 16) | (1U << 8) | (2U << 1);

    uint crc32 = 0;
    if(DownloadConstants_.platform_==ST_Platform::STA8100
        ||DownloadConstants_.platform_==ST_Platform::STA9100)
    {
        bootloader_.buf_ptr=xldr_teseo5_bootloader_Bx_buffer;
        bootloader_.size=xldr_teseo5_bootloader_Bx_len;
    }
    else
    {
        bootloader_.buf_ptr=xldr_teseo3_bootloader;
        bootloader_.size=xldr_teseo3_bootloader_len;
    }
    bootloader_.entry = 0;
    /* Calculate CRC32 on bootloader size */
    CRC32_EVAL_INT(crc32, crc32,bootloader_.size);
    /* Calculate CRC32 using the *new* "entry point" */
    CRC32_EVAL_INT(crc32, crc32, bootloader_.entry);
    /* Calculate CRC32 on bootloader code */
    CRC32_EVAL_BUF(crc32, crc32, bootloader_.buf_ptr, bootloader_.size);
    bootloader_.crc32 = crc32;
    return true;
}

bool ST_Upgrade::xldr_teseo_sync(int times)
{
    setUpgrade_state (upgrade_flow::Download_Sync);
    io_->io_clear ();
    io_->io_asyncsleep (250);
    bool flag=true;
    uint t3id;
    t3id = DownloadConstants_.sFLASHER_IDENTIFIER;
    /* Send data */
    for(int i=0;i<times;i++)
    {
        if(downloadstoping)
        {
            return false;
        }
        setDownload_percentage ((i+1)*100/times);
        QTime start=QTime::currentTime ();
        io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&t3id),4),0,true);
        /* Receive data */
        QByteArray readdata;
        readdata+=io_->io_Read (DownloadConstants_.sWAIT_READ_DRIVER);
        QByteArray   read_chars =QByteArray::fromRawData (readdata.data (),readdata.size ());
        QByteArray   ack_chars=QByteArray::fromRawData (reinterpret_cast<char*>(& DownloadConstants_.sFLASHER_SYNC),4);
        if (read_chars.contains (ack_chars) )
        {
            setDownload_percentage (100);
            return true;
        }
        QTime end=QTime::currentTime ();
        int totaltime=start.msecsTo (end);
        if(totaltime<20)
        {
            io_->io_asyncsleep (20-totaltime);
        }
    }
    QMSG<MSGType::bb_Error>("[sync]"+LOGFRONT+QString(":send command:%1 times:%2 unreceive:%3 ")
                                                            .arg (QString::number ( DownloadConstants_.sFLASHER_IDENTIFIER,16))
                                                            .arg (QString::number (times))
                                                            .arg (QString::number ( DownloadConstants_.sFLASHER_SYNC,16))
                                ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                   .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                            );
    flag=false;

    return flag;
}

bool ST_Upgrade::xldr_teseo_send_baudrate(uint baudrate)
{
    setUpgrade_state (upgrade_flow::Download_send_baudrate);
    setDownload_percentage (0);
    bool flag=true;
    uint8_t data_out;
    /* Send baud rate command, baud rate and changed byte */
    data_out = DownloadConstants_.sCHANGE_BAUD_RATE;
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&data_out),1),0,true);
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&/*download_baudRate_*/baudrate),sizeof (baudrate)),0,true);
    QByteArray ack= io_->io_Read (DownloadConstants_.sWAIT_ACK,1).toHex ()/*.toUInt (nullptr,16)*/;
    setDownload_percentage (100);
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[send_baudrate]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                         .arg (QString (ack))
                                                                         .arg (QString::number (DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_check_baudrate()
{
    //setUpgrade_state (upgrade_flow::Download_check_baudrate);
    bool flag=true;
    QByteArray data_in, data_out;
    data_out.append (DownloadConstants_.sBAUD_RATE_CHANGED);
    io_->io_Write (data_out,0,true);
    data_in= io_->io_Read (DownloadConstants_.sWAIT_READ_DRIVER,1).toHex ()/*.toUInt (nullptr,16)*/;
    if (!data_in  .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_check_hostready()
{
    setUpgrade_state (upgrade_flow::Download_check_hostready);
    bool flag=true;
    QByteArray data_out;
    data_out.append ((DownloadConstants_.sHOST_READY));
    io_->io_Write (data_out,0,true);
    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_SHORT_ACK*3,1).toHex ()/*.toUInt (nullptr,16)*/;
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[check_hostready]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                           .arg (QString(ack))
                                                                           .arg (QString::number(DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_send_preamble()
{
    setUpgrade_state (upgrade_flow::Download_send_preamble);
    bool flag=true;
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&preamble_),sizeof (preamble_)),0,true);
    return flag;
}

bool ST_Upgrade::xldr_teseo_send_bootloader()
{
    setUpgrade_state (upgrade_flow::Download_send_bootloader);
    uint chunk_size = 0;
    uint8_t*  src_ptr = (uint8_t*)bootloader_.buf_ptr;
    uint remaining_bytes = bootloader_.size;

    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&bootloader_.crc32),4),0,true);
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&bootloader_.size),4),0,true);
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&bootloader_.entry),4),0,true);

    /* Write bootloader binary image */
    while ( 0 < remaining_bytes)
    {
        QTime start_t=QTime::currentTime ();
        /* Check bytes to sent */
        if (DownloadConstants_.sFLASHER_CHUNKSIZE < remaining_bytes)
        {
            chunk_size = DownloadConstants_.sFLASHER_CHUNKSIZE;
            remaining_bytes -= DownloadConstants_.sFLASHER_CHUNKSIZE;
        }
        else
        {
            chunk_size = remaining_bytes;
            remaining_bytes = 0;
        }
        io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(src_ptr),chunk_size),0,true);
        src_ptr += chunk_size;
        setDownload_percentage(( bootloader_.size-remaining_bytes)*100/(bootloader_.size));
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg ( bootloader_.size-remaining_bytes)
                                          .arg (bootloader_.size)
                                          .arg (QString::number (chunk_size/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }

    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_ACK).toHex ()/*.toUInt (nullptr,16)*/;
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[send_bootloader]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                           .arg (QString(ack))
                                                                           .arg (QString::number (DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return  false;
    }
    uint8_t data_out=DownloadConstants_.sFLASHER_READY;
    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&data_out),sizeof (data_out)),0,true);
    ack=io_->io_Read (DownloadConstants_.sWAIT_ACK*10).toHex ()/*.toUInt (nullptr,16)*/;
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[send_bootloader]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                           .arg (QString (ack))
                                                                           .arg (QString::number (DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_Upgrade::xldr_teseo_send_bininfo()
{
    setUpgrade_state (upgrade_flow::Download_send_bininfo);
    setDownload_percentage (0);
    bool flag=true;

    io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&binimg_info_t_),sizeof (binimg_info_t_)),0,true);
    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_SHORT_ACK).toHex ();

    setDownload_percentage (100);
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[send_bininfo]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                        .arg (QString (ack))
                                                                        .arg (QString::number(DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_wait_devinit()
{
    setUpgrade_state (upgrade_flow::Download_wait_devinit);
    setDownload_percentage (0);
    bool flag=true;
    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_SHORT_ACK).toHex ();
    setDownload_percentage (100);
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[wait_devinit]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                        .arg (QString(ack))
                                                                        .arg (QString::number(DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_wait_eraseprog()
{
    setUpgrade_state (upgrade_flow::Download_wait_eraseprog);
    bool flag=true;
    uint timeout = DownloadConstants_.sWAIT_ERASING;

    timeout *= DownloadConstants_.sWAIT_ERASING_CHIPSET_FACTOR;

    setDownload_percentage (0);
    QByteArray ack=io_->io_Read (timeout).toHex ()/*.toUInt (nullptr,16)*/;
    setDownload_percentage (100);
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[wait_eraseprog]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                        .arg (QString(ack))
                                                                        .arg (QString::number(DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }

    return flag;
}

bool ST_Upgrade::xldr_teseo_wait_erasenvm()
{
    setUpgrade_state (upgrade_flow::Download_wait_erasenvm);
    bool flag=true;
    setDownload_percentage (0);
    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_ERASING).toHex ()/*.toUInt (nullptr,16)*/;
    setDownload_percentage (100);
    if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
    {
        QMSG<MSGType::bb_Error>("[wait_erasenvm]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                        .arg (QString (ack))
                                                                        .arg (QString::number(DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_send_buffer()
{
    setUpgrade_state (upgrade_flow::Download_send_buffer);
    bool flag=true;
    uint chunk_size = 0;
    uint8_t*  src_ptr = (uint8_t*)firmwaredata_.data ();
    uint remaining_bytes = firmwaredata_.size();

    while ((0 < remaining_bytes))
    {
        QTime start_t=QTime::currentTime ();
        /* Check bytes to sent */
        if (DownloadConstants_.sFLASHER_CHUNKSIZE < remaining_bytes)
        {
            chunk_size = DownloadConstants_.sFLASHER_CHUNKSIZE;
            remaining_bytes -= DownloadConstants_.sFLASHER_CHUNKSIZE;
        }
        else
        {
            chunk_size = remaining_bytes;
            remaining_bytes = 0;
        }

        /* Send buffer */
        io_->io_Write (QByteArray::fromRawData ((char*)src_ptr,chunk_size),0,true);
        QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_SHORT_ACK*10).toHex ()/*.toUInt (nullptr,16)*/;
        if (!ack .contains (QByteArray::number (DownloadConstants_.sDEVICE_ACK,16)))
        {
            QMSG<MSGType::bb_Error>("[send_buffer]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                           .arg (QString (ack))
                                                                           .arg (QString::number (DownloadConstants_.sDEVICE_ACK,16))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            flag=false;
            break;
        }
        src_ptr += chunk_size;
        setDownload_percentage(( firmwaredata_.size()-remaining_bytes)*100/(firmwaredata_.size()));
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (firmwaredata_.size()-remaining_bytes)
                                          .arg (firmwaredata_.size())
                                          .arg (QString::number (chunk_size/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }
    return flag;
}

bool ST_Upgrade::xldr_teseo_check_crc()
{
    setUpgrade_state (upgrade_flow::Download_check_crc);
    bool flag=true;
    setDownload_percentage (0);
    QByteArray ack=io_->io_Read (DownloadConstants_.sWAIT_CRC);
    setDownload_percentage (100);
    if (!ack.contains ( DownloadConstants_.sDEVICE_ACK))
    {
        QMSG<MSGType::bb_Error>("[check_crc]"+LOGFRONT+QString("Result:%1 do not contains:%2")
                                                                    .arg (QString(ack.toHex ()))
                                                                    .arg (QString::number (DownloadConstants_.sDEVICE_ACK,16))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        flag=false;
    }
    return flag;
}

bool ST_Upgrade::tp_teseo_preamble()
{
    /* Using UART, preamble must be sent at 115200 */
    if(init_baudRate_!=115200)
    {
        IOserial *seril=static_cast<IOserial *>(io_.data ());
        seril->set_BuadRate (115200);
    }
    if(!xldr_teseo_sync(300)){return false;}
    if(!tp_teseo_change_baudrate()){return false;}
    if(!xldr_teseo_check_hostready ()){return  false;}
    if(!xldr_teseo_send_preamble ()){return  false;}
    if(!xldr_teseo_send_bootloader ()){return  false;}
    return true;
}

bool ST_Upgrade::tp_teseo_change_baudrate()
{
    setUpgrade_state (upgrade_flow::Download_change_baudrate);
    if(!xldr_teseo_send_baudrate (download_baudRate_))
    {
        io_->io_asyncsleep (250);
        io_->io_clear ();
        io_->io_asyncsleep (250);
        if(!xldr_teseo_sync(1)){return false;}
        if(!xldr_teseo_send_baudrate ((uint8_t)baudratelist.indexOf (download_baudRate_))){return false;}
    }

    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_BuadRate (download_baudRate_);
    if(!tp_teseo_wait_host_ready()){return false;}
    return true;
}

bool ST_Upgrade::tp_teseo_wait_host_ready()
{
    setUpgrade_state (upgrade_flow::Download_wait_host_ready);
    if(DownloadConstants_.platform_==ST_Platform::STA8100)
    {
        io_->io_Close ();
        io_->io_asyncsleep (500);
        io_->io_Open ();
        io_->io_clear ();
    }
    static int tms=10;
    for(int i=0;i<tms;i++)
    {
        setDownload_percentage ((i+1)*100/tms);
        QTime start=QTime::currentTime ();
//        if(DownloadConstants_.platform_!=ST_Platform::STA8100)
//        {
//            io_->io_Close ();
//            io_->io_asyncsleep (500);
//            io_->io_Open ();
//            io_->io_clear ();
//        }

        if(xldr_teseo_check_baudrate ())
        {
            setDownload_percentage (100);
            return true;
        }

        QTime end=QTime::currentTime ();
        int totaltime=start.msecsTo (end);
        if(totaltime<1000)
        {
            io_->io_asyncsleep (1000-totaltime);
        }
    }
    QMSG<MSGType::bb_Error>("[check_baudrate]"+LOGFRONT+QString("Host not ready!")
                                ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                   .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                            );

    return false;
}

bool ST_Upgrade::tp_teseo_erase_flash()
{
    if(!xldr_teseo_wait_eraseprog ()){return  false;}
    if(binimg_info_t_.eraseNVM==1)
    {
        if(!xldr_teseo_wait_erasenvm ()) {return  false;}
    }
    return true;
}

bool ST_Upgrade::tp_teseo_send_buffer()
{
    if(!xldr_teseo_send_buffer ()) {return  false;}
    if(!xldr_teseo_check_crc ())   {return  false;}
    return  true;
}


void ST_Upgrade::choose_param(int bootmode, bool bProgramOnly, bool bEraseOnly,
                              bool eraseOption, bool bSubSectorErase, bool bSTA8089FG)
{
    if(DownloadConstants_.platform_==ST_Platform::STA8100
      ||DownloadConstants_.platform_==ST_Platform::STA9100
      )
  {
    binimg_info_t_.eraseNVM = (eraseOption ? ERASE_CHIPSET : 0);
   }
   else
    {
     binimg_info_t_.eraseNVM = (eraseOption ? ERASE_AREA : 0);
   }
    binimg_info_t_.eraseOnly = (bEraseOnly ? 1 : 0);
    binimg_info_t_.programOnly = (bProgramOnly ? 1 : 0);
    binimg_info_t_.subSector = (bSubSectorErase ? 1 : 0);
    binimg_info_t_.sta8090fg = (bSTA8089FG ? 1 : 0);
    binimg_info_t_.bootMode = (uint)bootmode;

    if ( binimg_info_t_.bootMode != (int)output_mode::OUTPUT_SQI)
    {
        if (bootmode == (int)output_mode::OUTPUT_NOR)
        {
            binimg_info_t_.destinationAddress = 0x20000000U;//536870912U;
        }
    }
    else
    {
        binimg_info_t_.destinationAddress = 0x10000000U;

    }

    if(DownloadConstants_.platform_==ST_Platform::STA8100
        ||DownloadConstants_.platform_==ST_Platform::STA9100
        )
    {
        binimg_info_t_.entryPoint  = 0x400U;//1024U;
        //binimg_info_t_.NVMOffset = ( 1024 + 512) * 1024;;//1507328U
        //binimg_info_t_.NVMSize   =   512 * 1024;// 0x90000U ;//589824U

        binimg_info_t_.NVMOffset = 0;
        binimg_info_t_.NVMSize   = 0;

    }
    else
    {
        binimg_info_t_.entryPoint         = 0x0U;
        binimg_info_t_.NVMOffset = 0x100000U;
        binimg_info_t_.NVMSize   = 0x100000U;
    }
    binimg_info_t_.debug        = 0U;
    binimg_info_t_.debugAction  = 0U;
    binimg_info_t_.debugAddress = 0U;
    binimg_info_t_.debugData    = 0U;
    binimg_info_t_.debugSize    = 0U;
}

bool ST_Upgrade::qGNSS_ST_DownLoad(QString firmware, int qgnss_model, int downmode,
                                   const ImageOptions &imgopt,bool showExpert)
{
    QFile file(firmware);
    if(!file.exists ())
    {
        QMSG<MSGType::bb_Error>("[ERROR]"+LOGFRONT+QString("firmware file not exists! path: ")
                                                           .arg (QString(firmware))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                );
        return false;
    }
    file.open (QIODevice::ReadOnly);
    firmwaredata_=file.readAll ();
    file.close ();

    setCurrent_model ((QGNSS_Model)qgnss_model);
    switch (current_model ())
    {
    case QGNSS_Model::LG69TAA:
    case QGNSS_Model::LG69TAD:
    case QGNSS_Model::LG69TAI:
    case QGNSS_Model::LG69TAJ:
    case QGNSS_Model::LG69TAK:
    case QGNSS_Model::LG69TAH:
    case QGNSS_Model::LC29TAA:
    case QGNSS_Model::LC99TIA:
    {
        DownloadConstants_.choose_Platform (ST_Platform::STA8100);
        opt_.nvmAddressOffset= imgopt.nvmAddressOffset;
        opt_.nvmSize         = imgopt.nvmSize;
        break;
    }
    case QGNSS_Model::LG69TAB:
    {
      DownloadConstants_.choose_Platform (ST_Platform::STA9100);
      strcpy_s (constants_.sNVM_FLASH_OFFSET,sizeof (char)*9,    "00180000");
      strcpy_s (constants_.sNVM_FLASH_ERASE_SIZE,sizeof (char)*9,"00080000");
      opt_.nvmAddressOffset= (QByteArray::fromHex (constants_.sNVM_FLASH_OFFSET)).toHex ().toInt (nullptr,16);
      opt_.nvmSize         = (QByteArray::fromHex (constants_.sNVM_FLASH_ERASE_SIZE)).toHex ().toInt (nullptr,16);
      break;
    }
    case QGNSS_Model::LG69TAM:
    case QGNSS_Model::LG69TAQ:
    case QGNSS_Model::LG69TAS:
    case QGNSS_Model::LG69TAP:
    {
        DownloadConstants_.choose_Platform (ST_Platform::STA8100_MQSP);
        break;
    }
    case QGNSS_Model::L89:
    case QGNSS_Model::L26ADR:
    case QGNSS_Model::L26ADRA:
    case QGNSS_Model::L26ADRC:
    case QGNSS_Model::L26T:
    case QGNSS_Model::L26T_RD:
    case QGNSS_Model::L26TB:
    case QGNSS_Model::L26P:
    case QGNSS_Model::L26UDR:
    case QGNSS_Model::LC98S:
    {
        DownloadConstants_.choose_Platform (ST_Platform::STA8090_90);
        strcpy_s (constants_.sNVM_FLASH_OFFSET,    sizeof (char)*9,"00100000");
        strcpy_s (constants_.sNVM_FLASH_ERASE_SIZE,sizeof (char)*9,"00100000");
        opt_.nvmAddressOffset= (QByteArray::fromHex (constants_.sNVM_FLASH_OFFSET)).toHex ().toInt (nullptr,16);
        opt_.nvmSize         = (QByteArray::fromHex (constants_.sNVM_FLASH_ERASE_SIZE)).toHex ().toInt (nullptr,16);
        break;
    }

    }
    command_=new UpgradeCommand(current_model ());

    opt_.eraseNVM=imgopt.eraseNVM>0?1:0;
    opt_.reserved=imgopt.reserved;
    opt_.baudRate=imgopt.baudRate;
    opt_.firmwareSize=firmwaredata_.size ();

    uint crc32num=calculate_ether_crc32(0U,opt_.firmwareSize);
    crc32num=calculate_ether2_crc32(crc32num,firmwaredata_.data (),opt_.firmwareSize);
    opt_.firmwareCRC=crc32num;


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
    md=(ST_downmode)downmode;
    flashInfo()["DownLoadMode"]=IODvice::Enum_Value2Key<ST_downmode>(md);
    emit flashInfoChanged ();
    bool flag=false;
    switch ((ST_downmode)downmode)
    {
    case ST_downmode::download:
    {
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(!filename.contains ("_BOOT",Qt::CaseInsensitive))
        {
            QMSG<MSGType::bb_Error>("[Download ]"+LOGFRONT+QString("firmware file name do not contains:%1")
                                                                   .arg (QString("_BOOT"))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            break;
        }
        if(!xldr_teseo_init(showExpert)){break;}
        if(!tp_teseo_preamble ()){break;}
        if(!xldr_teseo_send_bininfo ()){break;}
        if(!xldr_teseo_wait_devinit ()){break;}
        if(!tp_teseo_erase_flash ()){break;}
        if(!tp_teseo_send_buffer ()){break;}
        flag=true;
        break;
    }
        flag=true;
        break;
    case ST_downmode::upgrade:
    {
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(!filename.contains ("_UPG",Qt::CaseInsensitive))
        {
            QMSG<MSGType::bb_Error>("[Download ]"+LOGFRONT+QString("firmware file name do not contains:%1")
                                                                   .arg (QString("_UPG"))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<ST_Upgrade::ST_downmode>((ST_Upgrade::ST_downmode)md))+": Fail!"
                                    );
            break;
        }

        if(extension.compare ("bin",Qt::CaseInsensitive)!=0)
        {
            QMSG<MSGType::bb_Warning>("[Upgrade]"+LOGFRONT+QString(" file extension is not bin:%1")
                                                                   .arg (QString(extension))
                                      );
            break;
        }
        if(recovery_){goto Nonmea_jump;}
        if(!ConnectModule()){goto Nonmea_jump;}
        if(!SendFirmwareUpgradeCommand()){break;}
    Nonmea_jump:
        if(!SynchronizeModule()){break;}
        if(!WriteBinaryImageOptions(showExpert)){break;}
        if(!EraseProgramArea()){break;}
        if(!EraseNVM()){break;}
        if(!SendFirmwarePackage()){break;}
        if(!PerformCRCErrorCheck()){break;}
        if(!Finished()){break;}
        flag=true;
        break;
    }
    }
    setStartdownload(false);
    reset_buadRate ();
    timer.stop ();
    if(downloadstoping)
    {
        setUpgrade_state (upgrade_flow::Stoped);
        downloadstoping=false;
    }
    return flag;
}

void ST_Upgrade::init_IO(IODvice *io)
{
    if(!io){return;}
    io_=io;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    RTS_=seril->get_RTS ();
    connect (seril,&IOserial::errorOccurred,this,[&](QString err){
        QMSG<MSGType::bb_Error> ("[IO serialport]"+LOGFRONT+"serialport error:"+err);

    });
    init_baudRate_=seril->get_baudRate ();
    flashInfo()["PORTNAME"]=seril->portName ();
    emit flashInfoChanged ();
    seril=nullptr;
}

bool ST_Upgrade::download_stop()
{
    setUpgrade_state (upgrade_flow::Stoping);
    downloadstoping=true;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    Q_EMIT seril->send_Stop ();//stop reading func
    seril->io_asyncsleep (500);
    seril->io_clear ();
    return true;
}

void ST_Upgrade::reset_buadRate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_BuadRate(init_baudRate_);
    seril->set_RTS (RTS_);
    seril=nullptr;
}

void ST_Upgrade::qGNSS_SetDownLoadBaudRate(int baudrate)
{
    download_baudRate_=baudrate;
    flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
    emit flashInfoChanged ();
}
