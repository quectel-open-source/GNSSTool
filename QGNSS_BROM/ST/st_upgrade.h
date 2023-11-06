/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_upgrade.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-01      victor.gong
* ***************************************************************************/
#ifndef ST_UPGRADE_H
#define ST_UPGRADE_H

#include "QGNSS_BROM_global.h"
#include "st_base.h"
 struct xldr_binimg_info_t
{
  int  fileSize;
  uint32_t  bootMode;
  uint32_t  crc32;
  int  destinationAddress;
  int  entryPoint;
  uint8_t   eraseNVM;//eraseOption
  uint8_t   eraseOnly;
  uint8_t   programOnly;
  uint8_t   subSector;
  uint8_t   sta8090fg;
  uint8_t   res1;
  uint8_t   res2;
  uint8_t   res3;
  int       NVMOffset;
  int       NVMSize;
  uint32_t  debug;
  uint32_t  debugAction;
  uint32_t  debugAddress;
  int  debugSize;
  uint32_t  debugData;
  xldr_binimg_info_t()
  {
    res1=0;
    res2=0;
    res3=0;
    debug=0;
    debugAction=0;
    debugAddress=0;
    debugSize=0;
    debugData=0;
  }

} ;

class QGNSS_BROM_EXPORT ST_Upgrade:public ST_Base
{
    Q_OBJECT
public:
    explicit ST_Upgrade(QObject *parent=nullptr);
    ~ST_Upgrade();

    bool qGNSS_ST_DownLoad(QString firmware,int qgnss_model
                           ,int downmode,const ImageOptions &p=ImageOptions(),bool showExpert=false);
    //upgrade
    bool ConnectModule();
    bool SendFirmwareUpgradeCommand();
    bool SynchronizeModule();
    bool WriteBinaryImageOptions(bool showExpert);
    bool EraseProgramArea();
    bool EraseNVM();
    bool SendFirmwarePackage();
    bool PerformCRCErrorCheck();
    bool Finished();

    //download
    DownloadConstants   DownloadConstants_;

    typedef struct xldr_teseo_preamble_s
    {
        uint32_t identifier_msp;
        uint32_t identifier_lsp;
        uint32_t options;
        uint32_t destinationAddress;
    } xldr_teseo_preamble_t;
    xldr_teseo_preamble_t  preamble_;

    typedef struct xldr_teseo_bootloader_s
    {
        unsigned char  *buf_ptr;
        uint32_t   size;
        uint32_t   entry;
        uint32_t   crc32;
    } xldr_teseo_bootloader_t;
    xldr_teseo_bootloader_t bootloader_;


    void choose_param(int bootmode, bool bProgramOnly, bool bEraseOnly, bool eraseOption, bool bSubSectorErase, bool bSTA8089FG);

    xldr_binimg_info_t binimg_info_t_;

    bool xldr_teseo_init            (bool showExpert);
    bool xldr_teseo_sync            (int times=500);
    bool xldr_teseo_send_baudrate   (uint baudrate);
    bool xldr_teseo_check_baudrate  ();
    bool xldr_teseo_check_hostready ();
    bool xldr_teseo_send_preamble   ();
    bool xldr_teseo_send_bootloader ();
    bool xldr_teseo_send_bininfo    ();
    bool xldr_teseo_wait_devinit    ();
    bool xldr_teseo_wait_eraseprog  ();
    bool xldr_teseo_wait_erasenvm   ();

    bool xldr_teseo_send_buffer     ();
    bool xldr_teseo_check_crc       ();

    bool  tp_teseo_preamble();
    bool  tp_teseo_change_baudrate();
    bool  tp_teseo_wait_host_ready();
    bool  tp_teseo_erase_flash();
    bool  tp_teseo_send_buffer();

Q_SIGNALS:
 void showOPTCFG(ImageOptions*opt);
 void showDWNOPTCFG(xldr_binimg_info_t*opt);
public Q_SLOTS:

private:
    UpgradeCommand *command_=nullptr;
    NMEA_0813V14 nmea_help_;
    UpgradeConstants constants_;

    const int trytimes=100;
    QByteArray firmwaredata_;
    int download_baudRate_=921600;
    bool RTS_=false;
    ST_downmode md;

public:
     ImageOptions opt_;
    virtual void init_IO(IODvice *io) override;
    virtual bool download_stop() override;
    virtual void reset_buadRate() override;
    virtual void qGNSS_SetDownLoadBaudRate(int baudrate) override;
};
#endif // ST_UPGRADE_H
