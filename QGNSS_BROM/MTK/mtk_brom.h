/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_brom.h
* Description: MTK 3333
* History:
* Version Date                           Author          Description
*         2022-04-01      victor.gong
* ***************************************************************************/

#ifndef MTK_BROM_H
#define MTK_BROM_H
#include "mtk_base.h"

#define MT3318_ADDR     "00100800"
#define MT3329ANDLATTER "00000C00"

#define TRY_TIMES  300 /*send times until receiver*/
//#define WR_TIMEOUT 2000 /*sync writeread timeout*/


//#define QLOG_INFO(x) qDebug()<<x
/**************************************************************/
#define _CMD_START_
#define _CMD_WRITE_
#define _CMD_JUMP_
#define _CMD_SYNC_
#define _CMD_SETMEMBLOCK_
#define _CMD_WRITEDATA_
#define _CMD_FINISH_
#define _CMD_FORMATFLASH_

#ifdef _CMD_START_
#define START_NMEA_START_CMD "$PMTK180*3B\r\n"
#define START_NMEA_START_CMD_SEND_TIMES 7
#define START_NMEA_START_CMD_TIMEOUT    40
#define START_BOOT_ROM_START_CMD1      "A0"
#define START_BOOT_ROM_START_CMD1_RPLY "5F"
#define START_BOOT_ROM_START_CMD2      "0A"
#define START_BOOT_ROM_START_CMD2_RPLY "F5"
#define START_BOOT_ROM_START_CMD3      "50"
#define START_BOOT_ROM_START_CMD3_RPLY "AF"
#define START_BOOT_ROM_START_CMD4      "05"
#define START_BOOT_ROM_START_CMD4_RPLY "FA"
#endif

#ifdef _CMD_WRITE_
#define WRITE_BOOT_ROM_WRITE_CMD       "A1"
#define WRITE_BOOT_ROM_WRITE_CMD_RPLY  "A1"
//#define WRITE_DA_BASE_ADDR             MTADDR
//#define WRITE_DA_BASE_ADDR_RPLY        MTADDR
#define WRITE_DA_SIZE_NUM_WORD
#define WRITE_DA_SIZE_NUM_WORD_RPLY
#define WRITE_BOOT_ROM_CHECKSUM_CMD        "A4"
#define WRITE_BOOT_ROM_CHECKSUM_CMD_RPLY   "A4"
#define WRITE_DA_CHECKSUM_RPLY              "678D"
#endif

#ifdef _CMD_JUMP_
#define JUMP_BOOT_ROM_JUMP_CMD        "A8"
#define JUMP_BOOT_ROM_JUMP_CMD_RPLY   "A8"
//#define JUMP_DA_BASE_ADDR             MTADDR
#endif

#ifdef _CMD_WRITEDATA_
#define WRITEDATA_DA_WRITE_CMD        "D5"
#define WRITEDATA_DA_WRITE_CMD_RPLY   "5A"
#define WRITEDATA_PACKET_LENGTH       "00000400"      /*1024*/
#define WRITEDATA_PACKET_LENGTH_DEC   1024

#define WRITEDATA_LOWBAUDRATEPACKET_LENGTH       "00000100"      /*256*/
#define WRITEDATA_LOWBAUDRATEPACKET_LENGTH_DEC   256
#define WRITEDATA_CONT_CHAR           "69"
#endif

#ifdef _CMD_FINISH_
#define FINISH_DA_FINISH_CMD          "D9"

#endif

#ifdef _CMD_SYNC_
#define SYNC_SYNC_CHAR               "C0"
#endif

#ifdef _CMD_SETMEMBLOCK_
#define SETMEMBLOCK_DA_MEM_CMD    "D3"
#define SETMEMBLOCK_BLOCK_COUNT   "01"
#define SETMEMBLOCK_BEGIN_ADDRESS "00000000"
#define SETMEMBLOCK_ACK "5A"
#endif

#ifdef _CMD_FORMATFLASH_
#define FORMATFLASH_FORMAT_CMD          "D4"
#define FORMATFLASH_FORMAT_BEGIN_ADDR   "00000000"
#define FORMATFLASH_ACK          "5A"
#define FORMATFLASH_
#endif

#ifdef  _CMD_FINISH_
#define FINISH_DA_FINISH_CMD             "D9"
#endif

#define READ1CHAR  1
#define READ2CHAR  2

#define TOTALFORMATSIZE 0x00100000
#define REGIONFORMATSIZE 0x000F7FFF
/**
 * @brief The MTK_BROM class AG3331 MTK3333 MTK3337  MTK3339
 * provide implement download func class =>base download func
 */
#define QERROR(x) \
    setErroString (x);\
    qCritical()<<x;   \
    setDownload_state(Download_State::State_CMD_Fail);

class QGNSS_BROM_EXPORT MTK_BROM : public MTK_Base
{
    Q_OBJECT
    /*show each step and download state*/
    Q_PROPERTY(Download_State download_state READ download_state WRITE setDownload_state NOTIFY download_stateChanged)

public:
    explicit MTK_BROM(MTK_Base *parent = nullptr);
             ~MTK_BROM();

    enum Download_State{
        State_Unstart,
        State_StartCmd,
        State_WriteBuf,
        State_JumpCmd,
        State_CMD_SetMemBlock,
        State_CMD_WriteData,
        State_CMD_FormatFlash,
        State_CMD_Sync,
        State_setDownLoadBaudRate,
        State_CMD_Finish,
        State_CMD_Fail,
        //**********custom**************//
        Custom_DownLoad_Stoping,
        Custom_DownLoad_Stoped
    };
    Q_ENUM (Download_State);

    Q_SIGNALS:
    void download_stateChanged(const Download_State &state);

public:
    enum DownLoadMode
    {
        DOWNLOAD=0,
        TOTALFORMAT,
        REGIONFORMAT
    };
    Q_ENUM (DownLoadMode);

    typedef struct write_Command
    {
        char *   CMD_Write;
        qint32   CMD_Write_Size;
        char *   CMD_Reply;
        bool     CMD_isReply;
        qint32   CMD_Reply_Size;
        int      timeout;
        int      Write_times;
        int      one_time_Cout;
    }Write_Command;

    Download_State download_state() const;
    Q_INVOKABLE void MTK_init_IO(IODvice *io) override;
    Q_INVOKABLE bool download_stop() override;
    Q_INVOKABLE void reset_buadRate() override;
    Q_INVOKABLE bool qGNSS_mTK_DownLoad(QString DA_Path,QString ROM_Path,int mtkmodel,int mode=DownLoadMode::DOWNLOAD) override;
    Q_INVOKABLE void qGNSS_SetDownLoadBaudRate(int baudrate) override;

public Q_SLOTS:
    //*******property set*********************************//
    void setDownload_state(Download_State newDownload_state);

private:
    QByteArray da_base_addr;

    QByteArray bin_file_bys;
    QByteArray rom_file_bys;
    DownLoadMode downloadmode;
    char da_size_num_word[4]={0};

    int download_baudRate_=460800;        /*default downloadbaudrate*/
    const int high_baudRateforDA=115200;  /*downRatebaudrate>=115200 startcmd baudrate set 115200*/

private:
    bool QGNSS_StartCmd();
    bool QGNSS_WriteBuf();
    bool QGNSS_JumpCmd();
    bool QGNSS_SetMemBlock();
    bool QGNSS_WriteData();
    bool QGNSS_FormatFlash(qint32 formatlength);
    bool QGNSS_Finish();
    bool QGNSS_CMD_Sync (const QByteArray &bytes);
    bool QGNSS_CMD_download_baudrate();

    uint16_t DA_CHECKSUM (uint8_t *buf, uint32_t buf_len)
    {
        uint16_t checksum = 0;
        if (buf == NULL || buf_len == 0)
        {
            return 0;
        }
        int i = 0;
        for (i = 0; i < static_cast<int>(buf_len / 2) ; i++)
        {
            checksum ^= *(uint16_t *) (buf + i * 2);
        }
        if ((buf_len % 2) == 1)
        {
            checksum ^= buf[i * 2];
        }
        return checksum;
    }

    quint16  DATA_CHECKSUM(uint8_t *buf, uint32_t buf_len)
    {
        quint16 checksum = 0;
        int i;
        for(i=0; i< static_cast<int>(buf_len);i++)
        {
            checksum += *(buf + i);
        }

        return checksum;
    }

    Download_State download_state_=Download_State::State_Unstart;

};

#endif // MTK_BROM_H
