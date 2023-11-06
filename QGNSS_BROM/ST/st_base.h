/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_base.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-01      victor.gong
* ***************************************************************************/
#ifndef ST_BASE_H
#define ST_BASE_H

#include "QGNSS_BROM_global.h"
#include "brom_base.h"
#include "QVector"
#define CRC32_EVAL_BUF(d,c,b,l)       d = crc32_eval(c,b,l)
#define CRC32_EVAL_STR(d,c,s,l)       d = crc32_eval(c,s,l)
#define CRC32_EVAL_INT(d,c,i)         { uint val = (i); (d) = crc32_eval(c,&val,sizeof(val)); }

//Binary image options
   struct ImageOptions
{
  unsigned char eraseNVM;
  unsigned char programOnly;
  unsigned char reserved;
  unsigned char baudRate;
  int firmwareSize;   //4
  uint firmwareCRC;   //4
  int nvmAddressOffset;      //4
  int nvmSize;      //4
  ImageOptions()
  {
    eraseNVM=1;
    programOnly=0;
    reserved=0;
    baudRate=4;
  }
   } ;
class QGNSS_BROM_EXPORT ST_Base:public BROM_BASE
{
    Q_OBJECT
    Q_PROPERTY(upgrade_flow upgrade_state READ upgrade_state WRITE setUpgrade_state NOTIFY upgrade_stateChanged)
    Q_PROPERTY(QMap_variant flashInfo READ flashInfo WRITE setFlashInfo NOTIFY flashInfoChanged)
public:
    explicit ST_Base(QObject *parent=nullptr);
    virtual ~ST_Base();
    Q_INVOKABLE virtual void init_IO(IODvice *io)=0;/*get io instance from QGNSS*/
    Q_INVOKABLE virtual bool download_stop()=0;
    Q_INVOKABLE virtual void reset_buadRate()=0;
    Q_INVOKABLE virtual void qGNSS_SetDownLoadBaudRate(int baudrate)=0;

    typedef   QMap<QString,QVariant> QMap_variant;
    enum class ST_downmode:unsigned char{
        download,
        upgrade
    };
    Q_ENUM (ST_downmode)

    enum upgrade_flow{
        ConnectModule,
        SendFirmwareUpgradeCommand,
        SynchronizeModule,
        WriteBinaryImageOptions,
        EraseProgramArea ,
        EraseNVM,
        SendFirmwarePackage,
        PerformCRCErrorCheck,
        //***********download bin************ //
        Download_Sync,
        Download_send_baudrate,
        Download_change_baudrate,
        //Download_check_baudrate,
        Download_wait_host_ready,
        Download_check_hostready,
        Download_send_preamble,
        Download_send_bootloader,
        Download_send_bininfo,
        Download_wait_devinit,
        Download_wait_eraseprog,
        Download_wait_erasenvm,
        Download_send_buffer,
        Download_check_crc,
        //************MQSP GNSS&MCU Upgrade********************//
        MQSPConnectModule,
        MQSPSynchronize_Module,
        MQSPQuerytheBootloaderVersion,
        MQSPSendFirmwareAddress,
        MQSPSendFirmwareInformation,
        MQSPSendStartFirmwareUpgradeCommand,
        MQSPSendFirmware,
        //************MQSP GNSS&MCU Upgrade********************//
        MQSPUSART_Boot_Code_Sequence,
        MQSPGet_command,
        //MQSPGetVersion_ReadProtectionStatus,
        MQSPGetID,
        MQSPRead_Memory,
        //        MQSPGo,
        MQSPWrite_Memory,
        //        MQSPErase,
        MQSPExtendedErase,
        //        MQSPWriteProtect,
        MQSPWriteUnprotect,
        //        MQSPReadoutProtect,
        //        ReadoutUnprotect,
        //**********custom********************//
        Unstart,
        Finished,
        Fail,
        Stoping,
        Stoped
    };
    Q_ENUM (upgrade_flow)

    typedef  BROM_BASE::brmsgtype MSGType;

    template<enum  BROM_BASE::brmsgtype > void QMSG(QString msg,QString state="" );

    template< > void QMSG<MSGType::bb_Fail>(QString msg,QString state)
    {
        setBbmsgStr ({MSGType::bb_Fail,msg});
#ifdef DEBUG_MSG
        qDebug()<<msg;
#endif
    }

    template< > void QMSG<MSGType::bb_Error>(QString msg,QString state)
    {
        setBbmsgStr ({MSGType::bb_Error,msg});
        if(!state.isEmpty ())
        {
            setUpgrade_stateStr(state);
        }
#ifdef DEBUG_MSG
        qDebug()<<msg;
#endif
    }

    template< > void QMSG<MSGType::bb_Warning>(QString msg,QString state)
    {
        setBbmsgStr ({MSGType::bb_Warning,msg});
        if(!state.isEmpty ())
        {
            setUpgrade_stateStr(state);
        }
    }
    template< > void QMSG<MSGType::bb_Info>(QString msg,QString state)
    {
        setBbmsgStr ({MSGType::bb_Info,msg});
        if(!state.isEmpty ())
        {
            setUpgrade_stateStr(state);
        }
    }

    typedef   struct upgradeConstants
    {
        char sHOST_READY[3];
        char sFLASHER_IDENTIFIER[9];
        char sFLASHER_SYNC[9];
        char sDEVICE_START_COMMUNICATION[3];
        char sFLASHER_READY[3];
        char sACK[3];
        char sNAK[3];
        int  schunks;//16KB
        char sNVM_FLASH_OFFSET[9];
        char sNVM_FLASH_ERASE_SIZE[9];
        upgradeConstants(){
            strcpy_s (sHOST_READY,sizeof (char)*9,"5A");
            strcpy_s (sFLASHER_IDENTIFIER,sizeof (char)*9,"BCD501F4");
            strcpy_s (sFLASHER_SYNC,sizeof (char)*9,"83984073");
            strcpy_s (sDEVICE_START_COMMUNICATION,sizeof (char)*3,"A3");
            strcpy_s (sFLASHER_READY,sizeof (char)*3,"4A");
            strcpy_s (sACK,sizeof (char)*3,"CC");
            strcpy_s (sNAK,sizeof (char)*3,"DD");
            strcpy_s (sNVM_FLASH_OFFSET,sizeof (char)*9,    "00180000");
            strcpy_s (sNVM_FLASH_ERASE_SIZE,sizeof (char)*9,"00080000");
            schunks=16384;//16*1024
        }
    }UpgradeConstants;

    enum class ST_Platform:unsigned char{
        STA8090_90, //STA8090/90
        STA8100,    //AA,AD,AI,AJ,AK
        STA8100_MQSP,//AM,AQ,AS,AP
        STA9100
    };
    Q_ENUM (ST_Platform)

        enum class output_mode:unsigned  char{
            OUTPUT_SRAM                  ,
            OUTPUT_SQI                   ,
            OUTPUT_NOR                   ,
            OUTPUT_ITCM                  ,
            };
    Q_ENUM (output_mode)

    typedef  struct downloadConstants_{

        uint32_t sFLASHER_IDENTIFIER           ;
        uint32_t sFLASHER_SYNC                 ;

        output_mode OUTPUTMode                ;

        uint8_t sDEVICE_NOT_FLASHED           ;
        uint8_t sDEVICE_FLASHED               ;
        uint8_t sDEVICE_DUMPED                ;
        uint8_t sDEVICE_READY                 ;
        uint8_t sDEVICE_START_COMMUNICATION   ;
        uint8_t sFLASHER_READY                ;
        uint8_t sCHANGE_BAUD_RATE             ;
        uint8_t sBAUD_RATE_CHANGED            ;
        uint8_t sHOST_READY                   ;
        uint8_t sHOST_NOT_READY               ;
        uint8_t sDEVICE_ACK                   ;
        uint8_t sDEVICE_NACK                  ;
        uint8_t sDEBUG_DUMP                   ;
        uint8_t sDEBUG_SET                    ;

        uint32_t sTP_DEF_ENTRY_POINT           ;
        uint32_t sTP_DEF_DEST_ADDR             ;
        uint32_t sTP_ID_MSP                    ;
        uint32_t sTP_ID_LSP                    ;
        uint32_t sTP_ID_SIZE                   ;

        uint32_t sWAIT_READ_DRIVER             ;
        uint32_t sWAIT_SHORT_ACK               ;
        uint32_t sWAIT_ACK                     ;
        uint32_t sWAIT_ERASING                 ;
        uint32_t sWAIT_ERASING_CHIPSET_FACTOR  ;
        uint32_t sWAIT_CRC                     ;
        uint32_t sWAIT_DUMP_CHUNK              ;
        uint32_t sFLASHER_CHUNKSIZE            ;
        uint32_t sUART_MAX_REC_BUFFER          ;

        ST_Platform platform_;
        downloadConstants_()
        {
            //FLASHER_IDENTIFIER
            //FLASHER_SYNC

            OUTPUTMode=output_mode::OUTPUT_SQI ;
            sDEVICE_NOT_FLASHED           =0x0BU;
            sDEVICE_FLASHED               =0x0AU;
            sDEVICE_DUMPED                =0x0BU;
            sDEVICE_READY                 =0xC7U;
            sDEVICE_START_COMMUNICATION   =0xA3U;
            sFLASHER_READY                =0x4AU;
            sCHANGE_BAUD_RATE             =0x71U;
            sBAUD_RATE_CHANGED            =0x38U;
            sHOST_READY                   =0x5AU;
            sHOST_NOT_READY               =0x5BU;
            sDEVICE_ACK                   =0xCCU;
            sDEVICE_NACK                  =0xE6U;

            sDEBUG_DUMP                   =0x0U;
            sDEBUG_SET                    =0x0U;

            sTP_DEF_ENTRY_POINT           =0x00000000U;
            sTP_DEF_DEST_ADDR             =0x00000000U;
            sTP_ID_MSP                    =0xBCD501F4U;
            sTP_ID_LSP                    =0x83984073U;
            sTP_ID_SIZE                   =0x8U;

            sWAIT_READ_DRIVER             =100;
            sWAIT_SHORT_ACK               =1000;
            sWAIT_ACK                     =3500;
            sWAIT_ERASING                 =60000;
            sWAIT_ERASING_CHIPSET_FACTOR  =4;
            // WAIT_CRC                     =25000;
            sWAIT_DUMP_CHUNK              =3500;

            sFLASHER_CHUNKSIZE           =(5*1024);
            sUART_MAX_REC_BUFFER         =32;

        }
        void choose_Platform(ST_Platform palt);


    }DownloadConstants;

    struct UpgradeCommand{
        QByteArray Send;
        QByteArray ResultOK;
        QByteArray ResultERROR;
        UpgradeCommand(QGNSS_Model model)
        {
            Send=       QString("$PSTMFWUPGRADE%1").arg (NMEA_CRLF).toLocal8Bit ();
            ResultOK=   QString("$PSTMFWUPGRADEOK%1").arg (NMEA_CRLF).toLocal8Bit ();
            ResultERROR=QString("$PSTMFWUPGRADEERROR%1").arg (NMEA_CRLF).toLocal8Bit ();
        }
    };



    QMap<QString, QVariant> &flashInfo() ;

    upgrade_flow upgrade_state() const;

    static unsigned int calculate_ether_crc32(unsigned int crc32val, unsigned int integer);
    static unsigned int calculate_ether2_crc32(unsigned int crc32val, char * str, int len);
    uint32_t crc32_eval(uint32_t crc32val, const void *void_ptr, const size_t len);

    QVector<int> baudratelist{57600,115200,230400,460800,921600};
Q_SIGNALS:
    void upgrade_stateChanged(upgrade_flow download_state);
    void flashInfoChanged();

public Q_SLOTS:
    void setUpgrade_state(upgrade_flow download_state);
    void setUpgrade_stateStr(QString state);
    void setFlashInfo(const QMap<QString, QVariant> &newFlashInfo);
    void set_Recovery(bool recovery);
protected:
     bool recovery_=true;
private:
    upgrade_flow m_download_state;
    QMap<QString, QVariant> flashInfo_;


};

Q_DECLARE_METATYPE (ImageOptions);
#endif // ST_BASE_H
