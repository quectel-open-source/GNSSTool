/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_upgrade.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-16      victor.gong
* ***************************************************************************/
#ifndef BROADCOM_UPGRADE_H
#define BROADCOM_UPGRADE_H

#include "broadcom_base.h"
#include "nmea_0813v14.h"
#include "QProcess"
#include "QApplication"
#include <QEventLoop>

class QGNSS_BROM_EXPORT Broadcom_upgrade : public Broadcom_Base
{
    Q_OBJECT
public:
    explicit Broadcom_upgrade(QObject *parent = nullptr);
    virtual~Broadcom_upgrade();
    //upgrade
    bool ConnectModule();
    bool SendFirmwareUpgradeCommand();
    bool SynchronizeModule();
    bool WriteOptionsAndChangeUARTBaudRate();
    bool EraseCode();
    bool EraseNVM();
    bool EraseLTO();
    bool EraseNVMAndLTO();
    bool SendData();
    bool CRCErrorCheck();
    bool Finished();
    //download patch
    IOserial::Serial_INFO serialInfo;/*retain initial informatione*/

    //1 byte: The Type of the Data Block for details.3 bytes, the length (in bytes) of payload with little endian mode.The block content.
    enum class DataBlockType{
        Transmit=0x01,//Transmit, need to transmit the payload to module.
        Receive =0x02,//Receive, the payload needs to compare with received data from module, and they should be equal.
        String  =0x03,//String, the payload no need to send to module.
        Delay   =0x04,//Delay, the length of payload should 4 bytes (little-endian mode), the host should delay the time which describe in payload, and the unit is milliseconds.
    };
    Q_ENUM (DataBlockType)
    template<DataBlockType type>
    bool DW_DownloadPatch_help(const QByteArray &senddata);

    template<>
    bool DW_DownloadPatch_help<DataBlockType::Transmit>(const QByteArray &senddata){
       // io_->waitwritte ();
        int writesize=io_->io_Write (senddata,0,true);
        if(  writesize!=senddata.size ())
        {
            QMSG<MSGType::bb_Error>("[DownloadPatch]"+LOGFRONT+QString(" Write size:%1 unequal :%2 ")
                                                           .arg (QString(writesize))
                                                           .arg (QString(senddata.size ()))
                        );
            return false;
        }
        return true;
    }
    template<>
    bool DW_DownloadPatch_help<DataBlockType::Receive>(const QByteArray &readdata){
        QByteArray read=io_->io_Read (MAX_READ_TIMEOUT,readdata.size ());

        if( read!=readdata)
        {
            QMSG<MSGType::bb_Error>("[DownloadPatch]"+LOGFRONT+QString("read data:%1 unequal :%2 ")
                                                           .arg (QString(read.toHex ()))
                                                           .arg (QString(readdata.toHex ()))
                        );
            return false;
        }
        return true;
    }
    template<>
    bool DW_DownloadPatch_help<DataBlockType::String>(const QByteArray &stringdata)
    {
        QString str=stringdata.toHex ('0');
        return true;
    }

    template<>
    bool DW_DownloadPatch_help<DataBlockType::Delay>(const QByteArray &delay)
    {
        int time= reverse_4_bys(delay).toHex ().toInt (nullptr,16);
        io_->io_asyncsleep (time);
        return true;
    }

    bool DW_Choose_Module(QGNSS_Model model);
    bool DW_ConnectModule();
    bool DW_EnterBootMode();
    bool DW_DownloadPatch();
    bool DW_Finished();
    bool DW_Reset();

Q_SIGNALS:
    void patchVsersionChanged(const QString &version);

public Q_SLOTS:

private:
    UpgradeCommand *command_=nullptr;
    NMEA_0813V14 nmea_help_;
    UpgradeConstants constants_;
    ImageOptions opt_;
    const int trytimes=100;
    QByteArray firmwaredata_;
    int download_baudRate_=921600;
    //int nmearate_=115200;
public:
    void qGNSS_SetDownLoadBaudRate(int baudrate) override;
    bool qGNSS_broadcom_DownLoad(QString firmware,int qgnss_model,int downmode
                                 ,const ImageOptions &p=ImageOptions());
    void broadcom_init_IO(IODvice *io) override;
    bool download_stop() override;
    void reset_buadRate() override;

    template<enum upgrade_flow> bool broadcom_upgrade_step() ;
    //upgrade
    template<> bool broadcom_upgrade_step<upgrade_flow::ConnectModule>(){return ConnectModule();}
    template<> bool broadcom_upgrade_step<upgrade_flow::SendFirmwareUpgradeCommand>() {return SendFirmwareUpgradeCommand();}
    template<> bool broadcom_upgrade_step<upgrade_flow::SynchronizeModule>(){return SynchronizeModule();} ;
    template<> bool broadcom_upgrade_step<upgrade_flow::WriteOptionsAndChangeUARTBaudRate>() {return WriteOptionsAndChangeUARTBaudRate();}
    template<> bool broadcom_upgrade_step<upgrade_flow::EraseCode>() {return EraseCode();}
    template<> bool broadcom_upgrade_step<upgrade_flow::EraseNVM>(){return EraseNVM();}
    template<> bool broadcom_upgrade_step<upgrade_flow::EraseLTO>() {return EraseLTO();}
    template<> bool broadcom_upgrade_step<upgrade_flow::EraseNVMAndLTO>() {return EraseNVMAndLTO();}
    template<> bool broadcom_upgrade_step<upgrade_flow::SendData>() {return SendData();}
    template<> bool broadcom_upgrade_step<upgrade_flow::CRCErrorCheck>() {return CRCErrorCheck();}
    template<> bool broadcom_upgrade_step<upgrade_flow::Finished>() {return Finished();}
    //download patch
    template<> bool broadcom_upgrade_step<upgrade_flow::DW_Patch_ConnectModule>() {return DW_ConnectModule ();}
    template<> bool broadcom_upgrade_step<upgrade_flow::DW_Patch_Enter_Boot_Mode>() {return DW_EnterBootMode ();}
    template<> bool broadcom_upgrade_step<upgrade_flow::DW_Patch_Download_FW>() {return DW_DownloadPatch ();}
    template<> bool broadcom_upgrade_step<upgrade_flow::DW_Patch_Reset>() {return DW_Reset ();}
    template<> bool broadcom_upgrade_step<upgrade_flow::DW_Patch_Finish>() {return DW_Finished ();}
};
#endif // BROADCOM_UPGRADE_H
