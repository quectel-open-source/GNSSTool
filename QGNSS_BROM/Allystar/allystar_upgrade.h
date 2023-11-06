#ifndef ALLYSTAR_UPGRADE_H
#define ALLYSTAR_UPGRADE_H
#include <QProcess>
#include "allystar_base.h"
#define QERROR(x) \
    setErroString (x);\
    setDownload_state(Download_State::State_CMD_Fail);

#define QINFO(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Info,x});

class QGNSS_BROM_EXPORT AllyStar_Upgrade :public AllyStar_Base
{
  Q_OBJECT
  /*show each step and download state*/
  Q_PROPERTY(Download_State download_state READ download_state WRITE setDownload_state NOTIFY download_stateChanged)

 public:
   explicit AllyStar_Upgrade(QObject *parent=nullptr);
   ~AllyStar_Upgrade();
   enum Download_State
   {
     State_Unstart,
     BOOT_Unzip_File,
     BOOT_MD5_Check,
     BOOT_Query_Version,
     BOOT_UAHRY_SET,
     BOOT_FLSEARSE_SET,
     CFG_SIMPLERST_,
     BOOT_RFF_SET,
     BOOT_SPIFLSINFO_POLL,
     BOOT_SPIFLS_WRSR_SET,
     BOOT_FWPG_SET,
     BOOT_RESERT,
     USEREnterUpgradeMode,
     USERCFGPRT,
     USERSendSOHPKG,
     USERSendSTXPKG,
     USERSendEOTSOHPKG,
     //**********custom**************//
     State_Finished,
     State_CMD_Fail,
     Custom_DownLoad_Stoping,
     Custom_DownLoad_Stoped
   };
   Q_ENUM (Download_State);

   enum DownLoadMode
   {
     BOOT=0,
     USER
   };
   Q_ENUM (DownLoadMode);

  Q_SIGNALS:
   void download_stateChanged(const Download_State &state);

  public:
   Download_State download_state() const;
   Q_INVOKABLE void init_IO(IODvice *io) override;
   Q_INVOKABLE bool download_stop() override;
   Q_INVOKABLE void reset_buadRate() override;
   Q_INVOKABLE bool qGNSS_DownLoad(QString Path,int module,int mode=DownLoadMode::BOOT) override;
   Q_INVOKABLE void qGNSS_SetDownLoadBaudRate(int baudrate) override;

  public Q_SLOTS:
   void setDownload_state(const Download_State &newDownload_state);
   void setDownload_stateStr(QString state);

  private:
   typedef struct FUR_Binary_MSG_PKG_
   {
     char SYNC[2];
     char MessageID[2];
     ushort Payload_Length;
     char * Payload;
     char CRC[2];
     QByteArray sendbytarry;
     uint16_t cynomsg_checksum(const uint8_t *buf, uint16_t len)
     {
       uint16_t x;
       unsigned char checksum[] = { 0x00, 0x00 };
       for (x = 0; x < len; x++) {
         *checksum += buf[x];
         *(checksum + 1) += *checksum;
       }
       return *(uint16_t*) checksum;
     }
     FUR_Binary_MSG_PKG_(uchar sync[2],uchar messageID[2],ushort Payloadlength,char * payload)
     {
       memcpy (SYNC,sync,2);
       memcpy (MessageID,messageID,2);
       Payload_Length=Payloadlength;
       Payload=payload;


       sendbytarry.append (SYNC,2);
       sendbytarry.append (MessageID,2);

       sendbytarry.append ((char*)&Payload_Length,2);
       if(Payload)
       {
         sendbytarry.append (Payload,Payload_Length);
       }
       uint16_t crc16=cynomsg_checksum((uint8_t*)sendbytarry.mid (2).data (),sendbytarry.length ()-2);
       char * crc=(char*) (&crc16);
       sendbytarry.append (crc,2);
     }
     FUR_Binary_MSG_PKG_()=delete ;
   }FUR_Binary_MSG_PKG;
   int download_baudRate_=460800;        /*default downloadbaudrate*/
   Download_State download_state_=Download_State::State_Unstart;
   QByteArray firmwaredata_;

   bool Unzip_File(QString path);
   bool MD5_Check();
   bool Query_Version();
   bool BOOT3_UAHRY_SET(int baudrate,QByteArray read);
   bool BOOT3_FLSEARSE_SET();
   bool CFG_SIMPLERST();
   bool  BOOT3_RFF_SET(int baudrate);
   bool BOOT2_SPIFLSINFO_POLL ();
   bool BOOT2_SPIFLS_WRSR_SET ();

   bool FUR_CMD_BOOT2_FWPG_SET(int size,uint16_t number,QByteArray data);
 //bool EnterBootMode
   bool Download();
   bool BOOT_Resert();
   int replytimes=20*1000;
   QString filename;
   bool USER_CFG_PRT(int baudrate);
   bool USER_Enter_Upgrade_Mode(char mode=0x10);
   bool USER_Send_SOH_PKG();
   bool USER_Send_STX_PKG();
   bool USER_Send_EOT_SOH_PKG();

   uint start_address=0x00100000;
};

#endif // ALLYSTAR_UPGRADE_H
