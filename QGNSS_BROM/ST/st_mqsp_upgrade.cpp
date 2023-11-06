/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_mqsp_upgrade.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-14      victor.gong
* ***************************************************************************/
#include "st_mqsp_upgrade.h"

ST_MQSP_Upgrade::ST_MQSP_Upgrade(QObject *parent)
    :ST_Base(parent)
{

}

bool ST_MQSP_Upgrade::qGNSS_ST_DownLoad(QString firmware,
                                        int qgnss_model,
                                        int downmode
                                        ,bool showExpert
                                        )
{
    QFile file(firmware);
    if(!file.exists ())
    {
        QMSG<MSGType::bb_Error>("[ERROR]"+LOGFRONT+QString("firmware file not exists! path: ")
                                                           .arg (QString(firmware))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    file.open (QIODevice::ReadOnly);
    firmwaredata_=file.readAll ();
    file.close ();

    setCurrent_model ((QGNSS_Model)qgnss_model);
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
    Down_Mode md=(Down_Mode)downmode;
    flashInfo()["DownLoadMode"]=IODvice::Enum_Value2Key<Down_Mode>(md);
    emit flashInfoChanged ();
    bool flag=false;

    downmode_=(Down_Mode)downmode;
    switch (downmode_)
    {
    case Down_Mode::GNSS_Upgrade:
    case Down_Mode::MCU_Upgrade :
    {
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(downmode_==Down_Mode::MCU_Upgrade)
        {
            if(!filename.contains ("_UPG",Qt::CaseInsensitive))
            {
                QMSG<MSGType::bb_Error>("[Upgrade ]"+QString("firmware file name do not contains:%1")
                                                           .arg (QString("_UPG"))
                                            ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                               .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                        );
                break;
            }
        }

        classID_=downmode_==Down_Mode::GNSS_Upgrade?0x01U:0x02U;
        if(!ConnectModule()){break;}
        if(!Synchronize_Module()){break;}
        //if(!Query_the_Bootloader_Version()){break;}
        if(!Send_Firmware_Address()){break;}
        if(!Send_Firmware_Information(showExpert)){break;}
        if(!Send_Start_Firmware_Upgrade_Command()){break;}
        if(!Send_Firmware()){break;}
        flag=true;
        break;
    }
    case Down_Mode::MCU_Download:
    {
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(!filename.contains ("_BOOT",Qt::CaseInsensitive))
        {
            QMSG<MSGType::bb_Error>("[Download ]"+QString("firmware file name do not contains:%1")
                                                        .arg (QString("_BOOT"))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                    );
            break;
        }
        io_->set_BuadRate (download_baudRate_);
        IOserial *seril=static_cast<IOserial *>(io_.data ());
        seril->set_disable_waitWT ();
        seril->set_RTS (true);
        //seril->set_stopbits (QSerialPort::StopBits::TwoStop);//@bug GNSS-ToolsGNSSTOOL-87
        seril->set_Parity (QSerialPort::Parity::EvenParity);
        io_->io_asyncsleep (200);
        io_->io_clear ();

        if(!USART_Boot_Code_Sequence()){break;}
        if(!BootloaderCommand<Get>()){break;}
        if(!BootloaderCommand<GetID>()){break;}
        if(!BootloaderCommand<ReadMemory>()){break;}
        if(!BootloaderCommand<WriteUnprotect>()){break;}
        if(!BootloaderCommand<ExtendedErase>()){break;}
        if(!BootloaderCommand<ReadMemory>()){break;}
        int remainsize=firmwaredata_.size ();
        int sendsize=256;
        char *ptr=firmwaredata_.data ();
        int totalsend=0;
        int address=0x08000000;

        setUpgrade_state (upgrade_flow::MQSPWrite_Memory);

        while(remainsize>0)
        {
            QTime start_t=QTime::currentTime ();
            if(remainsize<256)
            {
                sendsize=remainsize;
            }

            char  * cadres=RS(reinterpret_cast<char*>(&address),sizeof (address));
            memcpy (startAddress,cadres,sizeof (address));
            qsenddata=QByteArray::fromRawData (ptr,sendsize);
            if(!BootloaderCommand<WriteMemory>()){goto end_S;}
            address+=sendsize;
            ptr+=sendsize;
            remainsize-=sendsize;
            totalsend+=sendsize;

            setDownload_percentage(( firmwaredata_.size()-remainsize)*100/(firmwaredata_.size()));
            QTime end_t=QTime::currentTime ();
            float ms=start_t.msecsTo (end_t)/1000.0;
            flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (firmwaredata_.size()-remainsize)
                                              .arg (firmwaredata_.size())
                                              .arg (QString::number (sendsize/ms/1024,'f',1))
                ;
            emit flashInfoChanged ();
        }
        startAddress[0]=0x08;
        startAddress[1]=0x00;
        startAddress[2]=0x00;
        startAddress[3]=0x00;

        flag=true;
        break;
    }
    }
    end_S:
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_enable_waitWT ();
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

bool ST_MQSP_Upgrade::ConnectModule()
{
    setUpgrade_state (upgrade_flow::MQSPConnectModule);
    io_->set_BuadRate (download_baudRate_);//io_->set_BuadRate (460800);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_RTS (true);
    return true;
}

bool ST_MQSP_Upgrade::Synchronize_Module()
{
    setUpgrade_state (upgrade_flow::MQSPSynchronize_Module);
    bool flag=false;
    QByteArray RSP=QByteArray::fromRawData (reinterpret_cast<char*>(&MQSPConstant_.sRSP_WORD1),sizeof (MQSPConstant_.sRSP_WORD1));
    static int tms=1000;
    for(int i=0;i<tms;i++)
    {
        if(downloadstoping)
        {
            return false;
        }
        setDownload_percentage ((i+1)*100/tms);
        QTime start=QTime::currentTime ();
        io_->io_Write ( QByteArray::fromRawData (reinterpret_cast<char*>(&MQSPConstant_.sSYNC_WORD1),sizeof (MQSPConstant_.sSYNC_WORD1)),0,true);
        QByteArray read=io_->io_Read (20);

        if(read.contains (RSP))
        {
            setDownload_percentage (100);
            flag=true;
            break;
        }
        QTime end=QTime::currentTime ();
        int time=start.msecsTo (end);
        if(time<20)
        {
            io_->io_asyncsleep (20-time);
        }
    }
    if(!flag)
    {
        QMSG<MSGType::bb_Error>("[Synchronize_Module]"+LOGFRONT+QString("Synchronize with Module Fail(SYNC1)")
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    flag=false;
    RSP=QByteArray::fromRawData (reinterpret_cast<char*>(&MQSPConstant_.RSP2_WORD2),sizeof (MQSPConstant_.RSP2_WORD2));
    for(int i=0;i<tms;i++)
    {
        setDownload_percentage ((i+1)*100/tms);
        io_->io_Write ( QByteArray::fromRawData (reinterpret_cast<char*>(&MQSPConstant_.SYNC_WORD2),sizeof (MQSPConstant_.SYNC_WORD2)),0,true);
        QByteArray read=io_->io_Read (20);
        if(read.contains (RSP))
        {
            setDownload_percentage (100);
            flag=true;
            break;
        }
    }
    if(!flag)
    {
        QMSG<MSGType::bb_Error>("[Synchronize_Module]"+LOGFRONT+QString("Synchronize with Module Fail(SYNC2)")
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    return flag;
}

bool ST_MQSP_Upgrade::Query_the_Bootloader_Version()
{
    setUpgrade_state (upgrade_flow::MQSPQuerytheBootloaderVersion);
    setDownload_percentage (0);
    QByteArray pro=getProtocol(0xAA,0xF0,0x01,0x0000,QByteArray()/*QByteArray::fromRawData (reinterpret_cast<char*>(&payloadFWAddress),sizeof (payloadFWAddress))*/);//0x00000000
    io_->io_Write (pro,0,true);
    QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,14);
    setDownload_percentage (100);
    if(read.size ()!=14)
    {
        QMSG<MSGType::bb_Error>("[Query Bootloader Version]"+LOGFRONT+QString("Query Bootloader Version Fail")

                                );
        return false;
    }
    Response_Status state=(Response_Status)read.mid(7,1).toHex ().toUInt (nullptr,16);
    if(state!=Response_Status::successfully)
    {
        QMSG<MSGType::bb_Error>("[Query Bootloader Version]"+LOGFRONT+QString("Status:%1")
                                                                              .arg (IODvice::Enum_Value2Key<Response_Status>(state))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    int Major= read.mid(9,1).toHex ().toUInt (nullptr,16);
    int Minor= read.mid(10,1).toHex ().toUInt (nullptr,16);
    int Build= read.mid(11,1).toHex ().toUInt (nullptr,16);
    QMSG<MSGType::bb_Info>("[Query Bootloader Version]"+QString("Query Bootloader Version:Major:%1 Minor: %2 Build:%3")
                                                              .arg (Major)
                                                              .arg (Minor)
                                                              .arg (Build)
                           );
    return true;
}

bool ST_MQSP_Upgrade::Send_Firmware_Address()
{
    setUpgrade_state (upgrade_flow::MQSPSendFirmwareAddress);
    setDownload_percentage (0);
    QByteArray pro=getProtocol(0xAA,classID_,0x01,0x0004,QByteArray::fromHex ("00000000"));
    io_->io_Write (pro,0,true);
    QByteArray read= io_->io_Read (MAX_READ_TIMEOUT*5,14);
    setDownload_percentage (100);
    if(read.size ()!=14)
    {
        QMSG<MSGType::bb_Error>("[Send_Firmware_Address]"+LOGFRONT+QString("response:%3 size:%1 unequal :%2")
                                                                           .arg (QString::number(read.size ()))
                                                                           .arg (QString::number (14))
                                                                           .arg(QString(read.toHex ()))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    Response_Status state=(Response_Status)read.mid(7,1).toHex ().toUInt (nullptr,16);
    if(state!=Response_Status::successfully)
    {
        QMSG<MSGType::bb_Error>("[Send_Firmware_Address]"+LOGFRONT+QString("Status:%1")
                                                                           .arg (IODvice::Enum_Value2Key<Response_Status>(state))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_MQSP_Upgrade::Send_Firmware_Information(bool showExpert)
{
    setUpgrade_state (upgrade_flow::MQSPSendFirmwareInformation);
    setDownload_percentage (0);
    uint size=firmwaredata_.size ();
    uint crc32num=calculate_ether_crc32(0U,size);
    crc32num=calculate_ether2_crc32(crc32num,firmwaredata_.data (),size);
    QByteArray pro;

    if(downmode_==Down_Mode::GNSS_Upgrade)
    {
        GNSS_FW_INFO GNSSFWINFO;
        if(showExpert)
        {
          GNSSFWINFO.GNSS_FW_Size=size;
          GNSSFWINFO.GNSS_FW_CRC32=crc32num;
          Q_EMIT showGNSSFWINFO_UI (&GNSSFWINFO);
        }
        char * cGNSSFWINFO= GNSSFWINFO.Get_chr (size,crc32num);
        pro=getProtocol(0xAA,classID_,0x02,0x0020,QByteArray::fromRawData (cGNSSFWINFO,sizeof (GNSSFWINFO)));
    }
    else
    {
        APP_FW_INFO APPFWINFO;
        if(showExpert)
        {
          APPFWINFO.APP_FW_Size=size;
          APPFWINFO.APP_FW_CRC32=crc32num;
          Q_EMIT showAPPFWINFO_UI (&APPFWINFO);
        }
        char * cAPPFWINFO= APPFWINFO.Get_chr (size,crc32num);

        pro=getProtocol(0xAA,classID_,0x02,0x0010,QByteArray::fromRawData (cAPPFWINFO,sizeof (APPFWINFO)));
    }
    io_->io_Write (pro,0,true);
    QByteArray read= io_->io_Read (MAX_READ_TIMEOUT,14);
    setDownload_percentage (100);
    if(read.size ()!=14)
    {
        QMSG<MSGType::bb_Error>("[Send Firmware Information]"+LOGFRONT+QString("response:%3 size:%1 unequal :%2")
                                                                               .arg (QString::number(read.size ()))
                                                                               .arg (QString::number (14))
                                                                               .arg(QString(read.toHex ()))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    Response_Status state=(Response_Status)read.mid(7,1).toHex ().toUInt (nullptr,16);
    if(state!=Response_Status::successfully)
    {
        QMSG<MSGType::bb_Error>("[Send Firmware Information]"+LOGFRONT+QString(" Status:%1")
                                                                               .arg (IODvice::Enum_Value2Key<Response_Status>(state))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_MQSP_Upgrade::Send_Start_Firmware_Upgrade_Command()
{
    setUpgrade_state (upgrade_flow::MQSPSendStartFirmwareUpgradeCommand);
    setDownload_percentage (0);
    QByteArray pro=getProtocol(0xAA,classID_,0x03,0x0000,QByteArray());
    io_->io_Write (pro,0,true);
    QByteArray read= io_->io_Read (MAX_READ_TIMEOUT*20,14);//con't stop bug 2022 07 01 @jehh
    setDownload_percentage (100);
    if(read.size ()!=14)
    {
        QMSG<MSGType::bb_Error>("[Send Upgrade Command]"+LOGFRONT+QString("response:%3 size:%1 unequal :%2")
                                                                          .arg (QString::number(read.size ()))
                                                                          .arg (QString::number (14))
                                                                          .arg(QString(read.toHex ()))
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    Response_Status state=(Response_Status)read.mid(7,1).toHex ().toUInt (nullptr,16);
    if(state!=Response_Status::successfully)
    {
        QMSG<MSGType::bb_Error>("[Send Upgrade Command]"+LOGFRONT+QString(" Status:Fail %1")
                                                                          .arg ((int)state)
                                    ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                       .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                );
        return false;
    }
    return true;
}

bool ST_MQSP_Upgrade::Send_Firmware()
{
    setUpgrade_state (upgrade_flow::MQSPSendFirmware);
    int totalsize=firmwaredata_.size ();
    int remainSize=totalsize;
    int sequence=0;
    char* ptr=firmwaredata_.data ();
    while(remainSize>0)
    {
        QTime start_t=QTime::currentTime ();
        int sendsize=0;
        if(remainSize<0x1400)
        {
            sendsize=remainSize;
        }
        else
        {
            sendsize=0x1400;
        }
        QByteArray payloaddata;
        payloaddata.append (reverse_4_bys (QByteArray::fromRawData (reinterpret_cast<char*>(&sequence), sizeof (sequence))));
        payloaddata.append (QByteArray::fromRawData (ptr,sendsize));
        QByteArray pro=getProtocol(0xAA,classID_,0x04,sendsize+4,payloaddata);
        io_->io_Write (pro,0,true);
        QByteArray read= io_->io_Read (1000*60,14);
        if(read.size ()!=14)
        {
            QMSG<MSGType::bb_Error>("[Send Firmware]"+LOGFRONT+QString("response:%3 size:%1 unequal :%2")
                                                                       .arg (QString::number(read.size ()))
                                                                       .arg (QString::number (14))
                                                                       .arg(QString(read.toHex ()))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                    );
            return false;
        }
        Response_Status state=(Response_Status)read.mid(7,1).toHex ().toUInt (nullptr,16);
        if(state!=Response_Status::successfully)
        {
            QMSG<MSGType::bb_Error>("[Send Firmware]"+LOGFRONT+QString(" Status:%1")
                                                                       .arg (IODvice::Enum_Value2Key<Response_Status>(state))
                                        ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                           .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                                    );
            return false;
        }
        ptr+=sendsize;
        remainSize-=sendsize;
        sequence++;
        setDownload_percentage(( firmwaredata_.size()-remainSize)*100/(firmwaredata_.size()));
        QTime end_t=QTime::currentTime ();
        float ms=start_t.msecsTo (end_t)/1000.0;
        flashInfo ()["DownloadRate"]=QString("%1 Bytes/%2 Bytes    %3 KBps").arg (firmwaredata_.size()-remainSize)
                                          .arg (firmwaredata_.size())
                                          .arg (QString::number (sendsize/ms/1024,'f',1))
            ;
        emit flashInfoChanged ();
    }
    return true;
}

void ST_MQSP_Upgrade::init_IO(IODvice *io)
{
    if(!io){return;}
    io_=io;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    connect (seril,&IOserial::errorOccurred,this,[&](QString err){
        QMSG<MSGType::bb_Error> ("[IO serialport]"+LOGFRONT+"serialport error:"+err);
    });
    init_baudRate_=seril->get_baudRate ();
    flashInfo()["PORTNAME"]=seril->portName ();
    emit flashInfoChanged ();
    RTS_=seril->get_RTS ();
    stpbs=seril->get_StopBits ();
    prty=seril->get_parity ();
    seril=nullptr;
}

bool ST_MQSP_Upgrade::download_stop()
{
    setUpgrade_state (upgrade_flow::Stoping);
    downloadstoping=true;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    Q_EMIT seril->send_Stop ();//stop reading func
    seril->io_asyncsleep (500);
    seril->io_clear ();
    return true;
}

void ST_MQSP_Upgrade::reset_buadRate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_BuadRate(init_baudRate_);
    seril->set_RTS (RTS_);
    //seril->set_stopbits (stpbs);
    seril->set_Parity (prty);
    seril=nullptr;
}

void ST_MQSP_Upgrade::qGNSS_SetDownLoadBaudRate(int baudrate)
{
    download_baudRate_=baudrate;
    flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
    emit flashInfoChanged ();
}

QByteArray ST_MQSP_Upgrade::getProtocol(unsigned char header, unsigned char classID,
                                        unsigned char msgID, unsigned short pLength, QByteArray pfAddress)
{
    QByteArray data;
    QByteArray length= QByteArray::fromRawData (reinterpret_cast<char*>(&pLength),sizeof (pLength));
    QByteArray Slength;
    Slength.append (length[1]);
    Slength.append (length[0]);
    data.append (header);
    data.append (classID);
    data.append (msgID);
    data.append (Slength);
    if(!pfAddress.isNull ())
    {
        data.append (pfAddress);
    }
    QByteArray CRCRange;
    CRCRange.append (classID);
    CRCRange.append (msgID);
    CRCRange.append (Slength);
    if(!pfAddress.isNull ())
    {
        CRCRange.append (pfAddress);
    }
    uint crc32=calculate_ether2_crc32 (0U,CRCRange.data (),CRCRange.size ());
    QByteArray Qcrc32= QByteArray::fromRawData (reinterpret_cast<char*>(&crc32),sizeof (crc32));
    Qcrc32=reverse_4_bys (Qcrc32);
    data.append (Qcrc32);
    unsigned char endc=0x55;
    QByteArray Qendc=QByteArray::fromRawData (reinterpret_cast<char*>(&endc),sizeof (endc));
    data.append (Qendc);
    return data;
}
/**
 *Once the system memory boot mode is entered and the STM32 microcontroller (based on
 *on Arm®(a) cores) has been configured (for more details refer to AN2606) the bootloader code begins to scan the USARTx_RX line pin, waiting to receive the 0x7F data frame: a
 *start bit, 0x7F data bits, even parity bit and a stop bit.The duration of this data frame is measured using the Systick timer. The count value of the
 *timer is then used to calculate the corresponding baud rate factor with respect to the current
 *system clock.Next, the code initializes the serial interface accordingly. Using this calculated baud rate, an
 *acknowledge byte (0x79) is returned to the host, which signals that the STM32 is ready to
 *receive commands.
 * @brief ST_MQSP_Upgrade::USART_Boot_Code_Sequence
 * @return
 */
bool ST_MQSP_Upgrade::USART_Boot_Code_Sequence()
{
    setUpgrade_state (upgrade_flow::MQSPUSART_Boot_Code_Sequence);
    unsigned char cmd1=0x7FU;
    static int tms=10;
    for(int i=0;i<tms;i++)
    {
        if(downloadstoping)
        {
            return false;
        }
        setDownload_percentage ((i+1)*100/tms);
        QTime start=QTime::currentTime ();
        io_->io_Write (QByteArray::fromRawData (reinterpret_cast<char*>(&cmd1),sizeof (cmd1)),0,true);
        //IOserial *seril=static_cast<IOserial *>(io_.data ());
       // qDebug()<<seril->get_bytesAvailable ();
        QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,0);
        if(read==QByteArray::fromHex (ACK))
        {
            setDownload_percentage (100);
            return true;
        }
        QTime end=QTime::currentTime ();
        int msc=start.msecsTo (end);
        if(msc<MAX_READ_TIMEOUT)
        {
            io_->io_asyncsleep (1000);
        }
    }
    QMSG<MSGType::bb_Error>("[USART_Boot_Code_Sequence]"+LOGFRONT+QString("STM32 send %1 not received commands ")
                                                                          .arg (QString(QByteArray::fromRawData (reinterpret_cast<char*>(&cmd1),sizeof (cmd1)).toHex ()))
                                                                           .arg (ACK)
                                ,IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model> (current_model ())+QString(":%1")
                                                                                                   .arg (IODvice::Enum_Value2Key<Down_Mode>((Down_Mode)downmode_))+": Fail!"
                            );
    return false;
}
