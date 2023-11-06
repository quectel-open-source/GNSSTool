/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_upgrade.cpp
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-16      victor.gong
* ***************************************************************************/

#include "broadcom_upgrade.h"
#define CUSTOM_NMEA_REGEX "\\$[,.A-Za-z0-9]{1,}\\*"
Broadcom_upgrade::Broadcom_upgrade(QObject *parent)
    : Broadcom_Base(parent)

{

}

Broadcom_upgrade::~Broadcom_upgrade()
{
    if(command_)
    {
        delete command_;
    }
}

void Broadcom_upgrade::broadcom_init_IO(IODvice *io)
{
    if(!io){return;}
    io_=io;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    connect (seril,&IOserial::errorOccurred,this,[&](QString err){
        QMSG<MSGType::bb_Error> ("[IO serialport]"+LOGFRONT+"serialport error:"+err);
    });
    connect (seril,&IOserial::rw_InfoChanged,this,&Broadcom_Base::rw_InfoChanged);
    init_baudRate_=seril->get_baudRate ();
    flashInfo()["PORTNAME"]=seril->portName ();
    emit flashInfoChanged ();
    seril=nullptr;
}

bool Broadcom_upgrade::ConnectModule()
{
    setUpgrade_state (upgrade_flow::ConnectModule);
    QByteArray read;
    bool isreadNMEA=false;
    io_->io_clear ();
    io_->io_asyncsleep (2000);
    for(int i=0;i<40;i++)
    {
        setDownload_percentage ((i+1)*100/40);
        if(downloadstoping)
        {
            return false;
        }
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
        QMSG<MSGType::bb_Warning>("[ConnectModule]"+LOGFRONT+": read NMEA Data :"+QString(read));
        QMSG<MSGType::bb_Warning>("[ConnectModule]"+LOGFRONT+": NO NMEA Data,No need to send :"+QString(command_->Send));
        return false;
    }
    io_->io_asyncsleep (200);
    io_->io_clear ();
    return true;
}

bool Broadcom_upgrade::SendFirmwareUpgradeCommand()
{
    setUpgrade_state (upgrade_flow::SendFirmwareUpgradeCommand);
    QByteArray read;
    io_->io_Write (QByteArray::fromHex ("11"),0,true);
    bool  isResultOk=false;
    for(int i=0;i<10;i++)
    {
       io_->io_Write (command_->Send,0,true);
        setDownload_percentage ((i+1)*100/10);
        read.append (io_->io_Read (500));
        io_->io_clear ();
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
                               );
        return false;
    }
    io_->io_clear ();
    return true;
}

bool Broadcom_upgrade::SynchronizeModule()
{
    setUpgrade_state (upgrade_flow::SynchronizeModule);
    bool receiveSYNC=false;
    QByteArray read;
    int times=trytimes*3;
    for(int i=0;i<times;i++)
    {
         setDownload_percentage ((i+1)*100/times);
        if(downloadstoping)
        {
            return false;
        }
        QTime start=QTime::currentTime ();
        read=io_->syncWriteRead (HEX2BYTES (constants_.sFLASHER_IDENTIFIER),10,4);
        if(read==HEX2BYTES (constants_.sFLASHER_SYNC))
        {
            setDownload_percentage (100);
            receiveSYNC=true;
            break;
        }
        QTime end=QTime::currentTime ();
        int time=start.msecsTo (end);
        if(time<20)
        {
            io_->io_asyncsleep (20-time);
        }
    }
    if(!receiveSYNC)
    {
        QMSG<MSGType::bb_Error>("[Synchronize Module]"+LOGFRONT+QString(":send command:%1 times:%2 unreceive:%3 ")
                                                                        .arg (QString(constants_.sFLASHER_IDENTIFIER))
                                                                        .arg (QString::number (times))
                                                                        .arg (QString(constants_.sFLASHER_SYNC))
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
                                );
        return  false;
    }
    return true;
}

bool Broadcom_upgrade::WriteOptionsAndChangeUARTBaudRate()
{
    setUpgrade_state (upgrade_flow::WriteOptionsAndChangeUARTBaudRate);
    char* opt_c=reinterpret_cast<char*>(&opt_);
    QByteArray sendopt=QByteArray(opt_c,20);

    io_->io_Write (sendopt,0,true);
    io_->io_asyncsleep (220);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    /*send different baudRates   seven */
    if(download_baudRate_>115200)
    {
        seril->set_BuadRate(download_baudRate_);
        io_->io_clear ();
        io_->io_asyncsleep (200);
    }
    int times=/*trytimes*/1;//CON'T SEND MULT TIMES
    bool isready=false;
    for(int i=0;i<times;i++)
    {
        io_->io_Write (HEX2BYTES (constants_.sFLASHER_READY),0,true);
        QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,1);
        if(read==HEX2BYTES(constants_.sACK))
        {
            read=io_->io_Read (10,1);
            if(read==HEX2BYTES(constants_.sACK))
            {
                isready=true;
                break;
            }
        }
    }
    if(!isready)
    {
        QMSG<MSGType::bb_Error>("[Write Options And Change UART BaudRate]"+LOGFRONT+QString(":send command:%1 times:%2 unreceive:%3 ")
                                                                                            .arg (QString(constants_.sFLASHER_READY))
                                                                                            .arg (QString(times))
                                                                                            .arg (QString("CCCC"))
                                );
        return false;
    }
    return true;
}

bool Broadcom_upgrade::EraseCode()
{
    setUpgrade_state (upgrade_flow::EraseCode);
    QByteArray read=  io_->io_Read (30*1000,1);
    if(read!=HEX2BYTES (constants_.sACK))
    {
        QMSG<MSGType::bb_Error>("[Erase Code]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                .arg (QString(read))
                                                                .arg (QString(constants_.sACK))
                                );
        return false;
    }
    return true;
}

bool Broadcom_upgrade::EraseNVM()
{
    if(opt_.eraseNVM)
    {
        setUpgrade_state (upgrade_flow::EraseNVM);
        QByteArray read=  io_->io_Read (10*1000,1);
        if(read!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Erase NVM]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                   .arg (QString(read))
                                                                   .arg (QString(constants_.sACK))
                                    );
            return false;
        }
        return true;
    }
    return true;
}

bool Broadcom_upgrade::EraseLTO()
{
    if(opt_.eraseLTO)
    {
        setUpgrade_state (upgrade_flow::EraseLTO);
        QByteArray read=  io_->io_Read (10*1000,1);
        if(read!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Erase LTO]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                   .arg (QString(read))
                                                                   .arg (QString(constants_.sACK))
                                    );
            return false;
        }
        return true;
    }
    return true;
}

bool Broadcom_upgrade::EraseNVMAndLTO()
{
    if(opt_.eraseNVM==1&&opt_.eraseLTO==1)
    {
        setUpgrade_state (upgrade_flow::EraseNVMAndLTO);
        QByteArray read=  io_->io_Read (MAX_READ_TIMEOUT,2);
        if(read!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Erase NVM And LTO]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                           .arg (QString(read))
                                                                           .arg (QString(constants_.sACK))
                                    );
            return false;
        }
        return true;
    }
    return true;
}

bool Broadcom_upgrade::SendData()
{
    setUpgrade_state (upgrade_flow::SendData);
    int remainddata=firmwaredata_.size ()% constants_.schunks;
    int times=firmwaredata_.size ()/constants_.schunks;
    QByteArray data;
    for(int i=0;i<times;i++)
    {
        QTime start_t=QTime::currentTime ();
        data= io_->syncWriteRead (firmwaredata_.mid (i*constants_.schunks,constants_.schunks),MAX_READ_TIMEOUT,1);
        if(data!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Send Data]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                   .arg (QString(data))
                                                                   .arg (QString(constants_.sACK))
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
        // data= io_->syncWriteRead (firmwaredata_.right (remainddata),30*1000,1);
        io_->io_Write (firmwaredata_.right (remainddata).trimmed (),0,true);
        data=io_->io_Read (MAX_READ_TIMEOUT);
        if(data!=HEX2BYTES (constants_.sACK))
        {
            QMSG<MSGType::bb_Error>("[Send Data]"+LOGFRONT+QString(" Receive:%1unequal:%2")
                                                                   .arg (QString(data))
                                                                   .arg (QString(constants_.sACK))
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

bool Broadcom_upgrade::CRCErrorCheck()
{
    setUpgrade_state (upgrade_flow::CRCErrorCheck);
    QByteArray read=  io_->io_Read (20*1000,1);
    if(read!=HEX2BYTES (constants_.sACK))
    {
        QMSG<MSGType::bb_Error>("[CRC Error Check]"+LOGFRONT+QString(" Receive:%1 unequal:%2 ")
                                                                     .arg (QString(read))
                                                                     .arg (QString(constants_.sACK))
                                );
        return false;
    }

    return true;
}

bool Broadcom_upgrade::Finished()
{
    io_->io_clear ();
    setUpgrade_state(upgrade_flow::Finished);
    return true;
}

bool Broadcom_upgrade::download_stop()
{
    setUpgrade_state (upgrade_flow::Stoping);
    downloadstoping=true;
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    Q_EMIT seril->send_Stop ();//stop reading func
    seril->io_asyncsleep (500);
    seril->io_clear ();
    return true;
}

void Broadcom_upgrade::reset_buadRate()
{
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    seril->set_BuadRate(init_baudRate_);
    seril=nullptr;
}

void Broadcom_upgrade::qGNSS_SetDownLoadBaudRate(int baudrate)
{
    download_baudRate_=baudrate;
    flashInfo()["DOWNLOADBAUDRATE"]=baudrate;
    emit flashInfoChanged ();
}

bool Broadcom_upgrade::qGNSS_broadcom_DownLoad(QString firmware, int qgnss_model, int downmode,/*int nmearate,*/const ImageOptions &imgopt)
{
    QFile file(firmware);
    if(!file.exists ())
    {
        QMSG<MSGType::bb_Error>("[Upgrade]"+LOGFRONT+QString("firmware file not exists! path: ")
                                                             .arg (QString(firmware))
                                );
        return false;
    }
    file.open (QIODevice::ReadOnly);
    firmwaredata_=file.readAll ();
    file.close ();

    setCurrent_model ((QGNSS_Model)qgnss_model);
    switch (current_model ())
    {
    case QGNSS_Model::LC79DA:
        opt_.eraseLTO=imgopt.eraseLTO;
        break;
    default:
        opt_.eraseLTO=0/*other module reserve 0*/;
    }
    command_=new UpgradeCommand(current_model ());

    opt_.eraseNVM=imgopt.eraseNVM;
    opt_.reserved=imgopt.reserved;
    opt_.baudRate=imgopt.baudRate;
    opt_.firmwareSize=firmwaredata_.size ();

    uint crc32num=calculate_ether_crc32(0U,opt_.firmwareSize);
    crc32num=calculate_ether2_crc32(crc32num,firmwaredata_.data (),opt_.firmwareSize);
    opt_.firmwareCRC=crc32num;
    opt_.reserved0=imgopt.reserved0;
    opt_.reserved1=imgopt.reserved1;

    //nmearate_=nmearate;

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
    BC_DownMode md=(BC_DownMode)downmode;
    flashInfo()["DownLoadMode"]=IODvice::Enum_Value2Key<BC_DownMode>(md);
    emit flashInfoChanged ();
    bool flag=false;
    switch ((BC_DownMode)downmode)
    {
    case BC_DownMode::Download:
    {
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(!filename.contains ("_BOOT",Qt::CaseInsensitive))
        {
            QMSG<MSGType::bb_Error>("[Download ]"+LOGFRONT+QString("firmware file name do not contains:%1")
                                                                   .arg (QString("_BOOT"))
                                    );
            break;
        }

        if(extension.compare ("patch",Qt::CaseInsensitive)==0)
        {
            if(  !broadcom_upgrade_step<upgrade_flow::DW_Patch_ConnectModule>() ){break;}
            if(  !broadcom_upgrade_step<upgrade_flow::DW_Patch_Enter_Boot_Mode>()){break;}
            if(  !broadcom_upgrade_step<upgrade_flow::DW_Patch_Download_FW>()) {break;}
            if(  !broadcom_upgrade_step<upgrade_flow::DW_Patch_Reset>()) {break;}
            if(  !broadcom_upgrade_step<upgrade_flow::DW_Patch_Finish>()){break;}
        }
        else if(extension.compare ("bin",Qt::CaseInsensitive)==0)
        {

            QProcess process(this);
            setUpgrade_state (upgrade_flow::DW_Bin_Start);
            IOserial *seril=static_cast<IOserial *>(io_.data ());
            auto seRInfo=seril->getSerial_INFO ();
            file.setFileName (firmware);
            QStringList cmd;
            cmd<<QString("HALcom=%1").arg (seRInfo->portname_)
                <<QString("HALbaud=%2").arg (download_baudRate_)
                <<QString("skipBackup=true")
                <<QString("flashOffset=0")
                <<QString("%3").arg (QFileInfo(file).fileName ())
                ;

            if(!file.exists (firmware))
            {
                QMSG<MSGType::bb_Warning>("[Download Bin]"+LOGFRONT+QString(" file not exists:%1")
                                                                            .arg (QString(firmware))
                                          );
                break;
            }
            QString desfile=QString(QApplication::applicationDirPath ()+"/%1").arg (QFileInfo(file).fileName ());
            file.copy (desfile);
            QString program(QApplication::applicationDirPath ()+"/FlashUpdater4775_CL389797_r.exe");//BoTexe/UserFile/
            if(!file.exists (program))
            {
                QMSG<MSGType::bb_Warning>("[Download Bin]"+LOGFRONT+QString(" program not exists:%1")
                                                                            .arg (QString(program))
                                          );
                break;
            }

            seril->io_Close ();
            QEventLoop loop;
            bool correct=true;
            connect (&process,&QProcess::errorOccurred,this,[&](QProcess::ProcessError error)
                    {
                        QString err=IODvice::Enum_Value2Key<QProcess::ProcessError>(error);
                        QMSG<MSGType::bb_Error>("[FlashUpdater Error]"+LOGFRONT+QString(" FlashUpdater:%1")
                                                                                        .arg (QString(err))
                                                );
                        correct=false;
                    });

            connect (&process,&QProcess::readyReadStandardOutput,this,[&]()
                    {
//                        QString read=process.readAllStandardOutput ();
//                        read.replace ("\r\n","<br/>");
//                        QMSG<MSGType::bb_Info>("[FlashUpdater]"+QString("%1")
//                                                                      .arg (read)
//                                               );
                    });
            connect (&process,&QProcess::readyRead,this,[&]()
                    {
                      QString read=process.readAll ();
                      read.replace ("\r\n","<br/>");
                      QMSG<MSGType::bb_Info>("[FlashUpdater]"+QString("%1")
                                                                    .arg (read)
                                             );
                    });

            connect (&process,&QProcess::readyReadStandardError ,this,[&]()
                    {
                        QString read=process.readAllStandardError ();
//                        QMSG<MSGType::bb_Warning>("[FlashUpdater]"+QString("%1")
//                                                                              .arg (read)
//                                                 );
                    });

            connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
                        Q_UNUSED (exitCode)
                        Q_UNUSED (exitStatus)
                        process.close ();
                        seril->io_Open ();
                        loop.exit();
                    });
            process.start (program,cmd);
            file.setFileName (desfile);
            loop.exec ();
            file.remove ();
            if(! correct)
            {
                break;
            }
        }
    }
        flag=true;
        break;
    case BC_DownMode::upgrade:
        QFileInfo fileinfo(file);
        QString extension= fileinfo.suffix ();
        QString filename=fileinfo.fileName ();
        if(!filename.contains ("_UPG",Qt::CaseInsensitive))
        {
            QMSG<MSGType::bb_Error>("[Download ]"+LOGFRONT+QString("firmware file name do not contains:%1")
                                                                   .arg (QString("_UPG"))
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

        if(!broadcom_upgrade_step<upgrade_flow::ConnectModule>()){goto Nonmea_jump;}
        if(!broadcom_upgrade_step<upgrade_flow::SendFirmwareUpgradeCommand>()){break;}
    Nonmea_jump:
        if(!broadcom_upgrade_step<upgrade_flow::SynchronizeModule>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::WriteOptionsAndChangeUARTBaudRate>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::EraseCode>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::EraseNVM>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::EraseLTO>()){break;}
        // if(!broadcom_upgrade_step<upgrade_flow::EraseNVMAndLTO>()){return false;}
        if(!broadcom_upgrade_step<upgrade_flow::SendData>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::CRCErrorCheck>()){break;}
        if(!broadcom_upgrade_step<upgrade_flow::Finished>()){break;}
        flag=true;
        break;
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

//download patch
bool Broadcom_upgrade::DW_Choose_Module(BROM_BASE::QGNSS_Model model)
{
    Q_UNUSED (model)
    return  true;
}

bool Broadcom_upgrade::DW_ConnectModule()
{
    setUpgrade_state (upgrade_flow::DW_Patch_ConnectModule);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    auto seRInfo=seril->getSerial_INFO ();
    serialInfo.Parity_=seRInfo->Parity_;
    serialInfo.DataBits_=seRInfo->DataBits_;
    serialInfo.StopBits_=seRInfo->StopBits_;
    serialInfo.baudRate_=seRInfo->baudRate_;
    serialInfo.portname_=seRInfo->portname_;
    serialInfo.FlowControl_=seRInfo->FlowControl_;
    seril->set_BuadRate (download_baudRate_);
    // seril->set_Flow (QSerialPort::HardwareControl);
    seril->set_RTS (true);
    io_->io_clear ();
    io_->io_asyncsleep (100);
    return true;
}

bool Broadcom_upgrade::DW_EnterBootMode()
{
    setUpgrade_state (upgrade_flow::DW_Patch_Enter_Boot_Mode);
    return true;
}

bool Broadcom_upgrade::DW_DownloadPatch()
{
    QString PF_Signature=firmwaredata_.mid (0,11).toHex ();
    int format=16;
    QByteArray PF_Version_B  =firmwaredata_.mid (12,4);
    QByteArray PF_V=reverse_4_bys(PF_Version_B);

    int v1=PF_V.mid(0,2).toHex ().toInt (nullptr,format);
    int v2=PF_V.mid(3,2).toHex ().toInt (nullptr,format);
    QString ver=QString("V%1.%2").arg (v1).arg (v2);
    statekeyvalue()->setKey (tr("Version: ")+ver);
    emit statekeyvalueChanged ();
    setUpgrade_state (upgrade_flow::DW_Patch_Download_FW);

    int datablocksize=reverse_4_bys (firmwaredata_.mid (16,4)).toHex ().toInt (nullptr,format);

    QByteArray databloack=firmwaredata_.mid (20);

    int i=0;
    //io_->io_clear ();
    for(int j=0;j<datablocksize;j++)
    {
        if(downloadstoping)
        {
            return false;
        }
        int type=databloack.mid (i,1).toHex ().toInt (nullptr,format);
        int length =reverse_4_bys (databloack.mid (i,4)).mid (0,3).toHex ().toInt (nullptr,format);
        QByteArray data=databloack.mid (i+4,length);
        switch (type) {
        case(int) DataBlockType::Transmit:
            if(!DW_DownloadPatch_help<DataBlockType::Transmit>(data)){return false;}
            break;
        case(int) DataBlockType::Receive:
            if(!DW_DownloadPatch_help<DataBlockType::Receive>(data)){return false;}
            break;
        case(int) DataBlockType::String:
            if(!DW_DownloadPatch_help<DataBlockType::String>(data)){return false;}
            break;
        case(int) DataBlockType::Delay:
            if(!DW_DownloadPatch_help<DataBlockType::Delay>(data)){return false;}
            break;
        default:
            QMSG<MSGType::bb_Error>("[DownloadPatch]"+LOGFRONT+QString(" DataType:%1 not belong to :[1,2,3,4] ")
                                                                       .arg (QString(type))
                                    );
            return  false;
        }
        i=i+4+length;
        setDownload_percentage(j*100/(datablocksize)+1);
        flashInfo ()["DownloadRate"]=QString(" %1 / %2 DataBlocks").arg ((j+1))
                                          .arg (datablocksize)

            ;
        emit flashInfoChanged ();
    }
    return true;
}

bool Broadcom_upgrade::DW_Finished()
{
    setUpgrade_state(upgrade_flow::DW_Patch_Finish);
    return true;
}

bool Broadcom_upgrade::DW_Reset()
{
    setUpgrade_state(upgrade_flow::DW_Patch_Reset);
    IOserial *seril=static_cast<IOserial *>(io_.data ());
    //seril->set_Flow (serialInfo.FlowControl_);
    seril->set_RTS (false);
    seril->set_BuadRate (init_baudRate_);
    return true;
}
