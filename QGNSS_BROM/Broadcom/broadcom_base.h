/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         broadcom_base.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-18      victor.gong
* ***************************************************************************/
#ifndef BROADCOM_BASE_H
#define BROADCOM_BASE_H
#include "brom_base.h"

class QGNSS_BROM_EXPORT Broadcom_Base : public BROM_BASE
{
  Q_OBJECT
  Q_PROPERTY(upgrade_flow upgrade_state READ upgrade_state WRITE setUpgrade_state NOTIFY upgrade_stateChanged)
  Q_PROPERTY(QMap_variant flashInfo READ flashInfo WRITE setFlashInfo NOTIFY flashInfoChanged)

 public:
  explicit Broadcom_Base(QObject *parent = nullptr);
  virtual ~Broadcom_Base();
  typedef   QMap<QString,QVariant> QMap_variant;
  Q_INVOKABLE virtual void broadcom_init_IO(IODvice *io)=0;
  Q_INVOKABLE virtual bool download_stop()=0;
  Q_INVOKABLE virtual void reset_buadRate()=0;
  Q_INVOKABLE virtual void qGNSS_SetDownLoadBaudRate(int baudrate)=0;
  QMap<QString, QVariant> &flashInfo() ;

  enum class BC_DownMode{
    Download,
    upgrade
  };
  Q_ENUM (BC_DownMode)

  enum upgrade_flow{
    ConnectModule,
    SendFirmwareUpgradeCommand,
    SynchronizeModule,
    WriteOptionsAndChangeUARTBaudRate,
    EraseCode,
    EraseNVM,
    EraseLTO,
    EraseNVMAndLTO,
    SendData,
    CRCErrorCheck,
    //**********download patch************//
    DW_Patch_ConnectModule,
    DW_Patch_Enter_Boot_Mode,
    DW_Patch_Download_FW,
    DW_Patch_Reset,
    DW_Patch_Finish,
    //***********download bin************ //
    DW_Bin_Start,
    DW_Bin_Finish,
    DW_Bin_Fail,
    //**********custom********************//
    Unstart,
    Finished,
    Fail,
    Stoping,
    Stoped
  };
  Q_ENUM (upgrade_flow)

  typedef  BROM_BASE::brmsgtype MSGType;
  // using MSGType=BROM_BASE::brmsgtype;
  template<enum  BROM_BASE::brmsgtype > void QMSG(QString msg);

  template< > void QMSG<MSGType::bb_Fail>(QString msg)
  {
    setBbmsgStr ({MSGType::bb_Fail,msg});
    // setUpgrade_state(upgrade_flow::Fail);
  }

  template< > void QMSG<MSGType::bb_Error>(QString msg)
  {
    setBbmsgStr ({MSGType::bb_Error,msg});
    setUpgrade_state(upgrade_flow::Fail);
  }

  template< > void QMSG<MSGType::bb_Warning>(QString msg)
  {
    setBbmsgStr ({MSGType::bb_Warning,msg});
  }
  template< > void QMSG<MSGType::bb_Info>(QString msg)
  {
    setBbmsgStr ({MSGType::bb_Info,msg});
  }

  typedef   struct upgradeConstants
  {
    char sFLASHER_IDENTIFIER[9];
    char sFLASHER_SYNC[9];
    char sDEVICE_START_COMMUNICATION[3];
    char sFLASHER_READY[3];
    char sACK[3];
    char sNAK[3];
    int schunks;//16KB
    upgradeConstants(){
      strcpy_s (&sFLASHER_IDENTIFIER[0],sizeof (char)*9,"B1557704");
      strcpy_s (&sFLASHER_SYNC[0],sizeof (char)*9,"1C134251");
      strcpy_s (&sDEVICE_START_COMMUNICATION[0],sizeof (char)*3,"A3");
      strcpy_s (&sFLASHER_READY[0],sizeof (char)*3,"4A");
      strcpy_s (&sACK[0],sizeof (char)*3,"CC");
      strcpy_s (&sNAK[0],sizeof (char)*3,"DD");
      schunks=16*1024;
    }
  }UpgradeConstants;


  struct UpgradeCommand{
    QByteArray Send;
    QByteArray ResultOK;
    QByteArray ResultERROR;

    UpgradeCommand(QGNSS_Model model){
      Send=nullptr;
      ResultOK=nullptr;
      ResultERROR=nullptr;

      switch (model)
      {
        case QGNSS_Model::LC29DA:
        case QGNSS_Model::LC29DB:
        case QGNSS_Model::LC29DC:
        case QGNSS_Model::LC29DE:
        case QGNSS_Model::LC29DF:
        case QGNSS_Model::LC79DD:
        case QGNSS_Model::LC29DG:
        case QGNSS_Model::LC79DF:
        {
          Send=QString("$PQTMFWUPGRADE*59%1").arg (NMEA_CRLF).toLocal8Bit ();
          ResultOK=QString("$PQTMFWUPGRADEOK*5D%1").arg (NMEA_CRLF).toLocal8Bit ();
          ResultERROR=QString("$PQTMFWUPGRADEERROR*01%1").arg (NMEA_CRLF).toLocal8Bit ();
        }
        break;

        case QGNSS_Model::LC79DA:
        case QGNSS_Model::LC79DC:

          {
            Send=QString("$PQFWUPGRADE*40%1").arg (NMEA_CRLF).toLocal8Bit ();
            ResultOK=QString("$PQFWUPGRADEOK*44%1").arg (NMEA_CRLF).toLocal8Bit ();
            ResultERROR=QString("$PQFWUPGRADEERROR*18%1").arg (NMEA_CRLF).toLocal8Bit ();
          }
          break;
        default:
        {
          Send=QString("$PQFWUPGRADE*40%1").arg (NMEA_CRLF).toLocal8Bit ();
          ResultOK=QString("$PQFWUPGRADEOK*44%1").arg (NMEA_CRLF).toLocal8Bit ();
          ResultERROR=QString("$PQFWUPGRADEERROR*18%1").arg (NMEA_CRLF).toLocal8Bit ();
          break;
        }
      }
    }
    ~UpgradeCommand()
    {

    }
  };

  enum DownloadBaudrate{
    Rate57600,
    Rate115200,
    Rate230400,
    Rate460800,
    Rate921600
  };
  Q_ENUM (DownloadBaudrate)



  //Binary image options
  typedef   struct imageoptions
  {
    unsigned char eraseNVM;
    unsigned char eraseLTO/*other module is reserved*/;
    unsigned char reserved;
    unsigned char baudRate;
    int firmwareSize;   //4
    uint firmwareCRC;   //4
    int reserved0;      //4
    int reserved1;      //4
    imageoptions()
    {
      eraseNVM=1;
      eraseLTO=0;
      reserved=0;
      baudRate=4;
      reserved0=0x00100000;
      reserved1=0x00100000;
    }
    imageoptions(int fSize,uint fCRC,bool iseraseNVM=true,bool iseraseLTO=false,
                 DownloadBaudrate downloadrate=DownloadBaudrate::Rate921600/*,int rsd0=0x00100000,int rsd1=0x00100000*/)
    {
      eraseNVM=iseraseNVM?1:0;
      eraseLTO=iseraseLTO?1:0;
      reserved=0;
      baudRate=(int)downloadrate;
      firmwareSize=fSize;
      firmwareCRC=fCRC;
      reserved0=0x00100000;
      reserved1=0x00100000;
    }

  }ImageOptions;

  upgrade_flow upgrade_state() const;

  unsigned int calculate_ether_crc32(unsigned int crc32val, unsigned int integer);
  unsigned int calculate_ether2_crc32(unsigned int crc32val, char * str, int len);
 Q_SIGNALS:
  void upgrade_stateChanged(upgrade_flow download_state);
  void flashInfoChanged();
 public Q_SLOTS:
  void setUpgrade_state(upgrade_flow download_state);
  void setFlashInfo(const QMap<QString, QVariant> &newFlashInfo);

 private:
  upgrade_flow m_download_state;
  QMap<QString, QVariant> flashInfo_;

};
Q_DECLARE_METATYPE (Broadcom_Base::UpgradeConstants)
Q_DECLARE_METATYPE (Broadcom_Base::UpgradeConstants*)
Q_DECLARE_METATYPE (Broadcom_Base::ImageOptions)
Q_DECLARE_METATYPE (Broadcom_Base::ImageOptions*)
#endif // BROADCOM_BASE_H
