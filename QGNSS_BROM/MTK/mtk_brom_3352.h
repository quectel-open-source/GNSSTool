/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         mtk_brom_3352.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-28      victor.gong
* ***************************************************************************/
// DA CMD
#define DA_FORMAT_CMD           0xD4
#define DA_NOR_WRITE_DATA       0xB2
#define DA_READ_CMD             0xD6
#define DA_RESET_CMD           	0xD9

#define DA_FW_PACKET_LEN      	(0x1000)
#define DA_SEND_PACKET_LEN      (DA_FW_PACKET_LEN)
#define DA_RECV_PACKET_LEN      (DA_FW_PACKET_LEN)

#define LEN_4K (0x1000)
#define LEN_64K (0x10000)

#ifndef MTK_BROM_3352_H
#define MTK_BROM_3352_H

#include "mtk_base.h"


class QGNSS_BROM_EXPORT MTK_BROM_3352 : public MTK_Base
{
    Q_OBJECT
    Q_PROPERTY(Download_State download_state READ download_state WRITE setDownload_state NOTIFY download_stateChanged)
    Q_PROPERTY(CFG *cfg_struct READ cfg_struct WRITE setCfg_struct NOTIFY cfg_structChanged)

public:
    explicit MTK_BROM_3352(MTK_Base *parent = nullptr);
    ~MTK_BROM_3352();

    enum FW_Type{
        PartitionTable,
        BootLoader,
        CM4,
        GNSSConfiguration
    };
    Q_ENUM (FW_Type)

    enum Down_Mode{
        // Download,
        FormatDownload,
        ReadBck
    };
    Q_ENUM (Down_Mode)

    enum Address_Type{
        physical,
        logical
    };
    Q_ENUM (Address_Type)

    enum Format_mode{
        Auto_Format,
        Total_Format,
        Manual_Format_Logical,
        Manual_Format_Physical
    };
    Q_ENUM (Format_mode)

    const char* BROM_ERROR_                 ="1000";

    enum Download_State{
        State_Unstart,
        State_handshake_with_Module,
        State_disable_Modeule_WDT,
        State_send_DA_File_to_Module,
        State_jump_to_DA,
        State_sync_with_DA,
        State_format_Flash,
        State_send_FW_File_to_Module,
        State_send_PartitionTable,
        State_send_BootLoader,
        State_send_CM4,
        State_send_GNSSConfiguration,

        //**********custom**************//
        State_Finished,
        State_CMD_Fail,
        Custom_DownLoad_Stoping,
        Custom_DownLoad_Stoped
    };
    Q_ENUM (Download_State);





    typedef struct _hdl_image_t
    {
        uint32_t        image_slave_flash_addr;
        uint32_t        image_len;
        char            *image_name;
        char            * file;
        QByteArray      data;
        bool            image_is_checked;
                        _hdl_image_t *next;
                        _hdl_image_t():
                            image_slave_flash_addr(0),
                            image_len(0),
                            image_name(nullptr),
                            file(nullptr),
                            data(nullptr),
                            image_is_checked(false),
                            next(nullptr)
          {

          }
    } hdl_image_t;

    typedef struct cfg_general
    {
        char* config_version;
        char* platform;
    } CFG_general;

    /*flash_download.cfg*/
    typedef struct cfg
    {
        qsizetype       DA_size;

        QByteArray  DA_data;
        cfg_general general;
        Address_Type addresstype;

        _hdl_image_t cfgrom;
    } CFG;

    /**
     * send SEND0 while receive Receive0 within wait_time
     **/
    typedef  struct handshakewithModul_
    {
        int   WAIT_TIMES;       /*150 ms*/
        const char* SEND0;
        const char* RECEIVE0;
        const char* SEND1;
        const char* RECEIVE1;
        const char* SEND2;
        const char* RECEIVE2;
        const char* SEND3;
        const char* RECEIVE3;
        handshakewithModul_()
        {
            WAIT_TIMES=2000;

            SEND0   ="A0";
            RECEIVE0="5F";

            SEND1   ="0A";
            RECEIVE1="F5";
            SEND2   ="50";
            RECEIVE2="AF";
            SEND3   ="05";
            RECEIVE3="FA";

        }
    }Struct_HandShakeWithModul;

    typedef struct  disable_Modeule_WDT_
    {
        const char* SEND0;
        const char* RECEIVE0;
        const char* SEND1;
        const char* RECEIVE1;
        const char* SEND2;
        const char* RECEIVE2[2];
        const char* SEND3;
        const char* RECEIVE3[2];
        disable_Modeule_WDT_()
        {


            SEND0   ="D2";
            RECEIVE0="D2";

            SEND1       ="A2080000";
            RECEIVE1    ="A2080000";
            SEND2       ="00000001";
            RECEIVE2[0] ="00000001";
            RECEIVE2[1] ="0001";    /*Brom status*/
            SEND3       ="0010";
            RECEIVE3[0] ="0010";
            RECEIVE3[1] ="0001";
        }
    }Struct_Disable_Modeule_WDT;

    typedef struct  send_DA_File_to_Module_
    {
        const char* SEND0;
        const char* RECEIVE0;
        const char* SEND1;
        const char* RECEIVE1;
        const char* SEND2;
        const char* RECEIVE2;
        const char* SEND3;
        const char* RECEIVE3[2];

        send_DA_File_to_Module_()
        {
            SEND0   ="D7";
            RECEIVE0="D7";

            SEND1   ="04200000";
            RECEIVE1="04200000";
            SEND2   ="000098EC";
            RECEIVE2="000098EC";
            SEND3   ="00000000";
            RECEIVE3[0]="00000000";
            RECEIVE3[1]="0000";
            /*send da*/
        }
    }Struct_send_DA_File_to_Module;

    typedef struct jump_to_DA_
    {
        const char* SEND0;
        const char* RECEIVE0;
        const char* SEND1;
        const char* RECEIVE1[3];
        jump_to_DA_()
        {
            SEND0   ="D5";
            RECEIVE0="D5";
            SEND1   ="04200000";
            RECEIVE1[0]="04200000";
            RECEIVE1[1]="0000";
            RECEIVE1[2]="C0";
        }
    } Struct_jump_to_DA;

    typedef struct {
        uint16_t        hw_code;
        uint16_t        hw_subcode;
        uint16_t        flash_manufacturer_id;
        uint16_t        flash_id1;
        uint16_t        flash_id2;
        uint32_t        flash_base_addr;
        uint32_t        flash_size;
    } hdl_da_report_t;

    typedef struct format_Flash_
    {
        const char* SEND0[3];
        const char* RECEIVE0[3];
        const char* SEND1;
        const char* RECEIVE1;
        const char* SEND2;
        const char* RECEIVE2[2];
        const char* SEND3;
        const char* RECEIVE3;
        int progress;
        format_Flash_()
        {
            progress=0;

            SEND0[0]="D400";
            SEND0[1]="08000000";
            SEND0[2]="00400000";//  or LC29H (BA, CA), the Format Length must be set to 0x003EF000.<20220517 changed>
            RECEIVE0[0]="5A";
            RECEIVE0[1]="5A";
            RECEIVE0[2]="00000BCD";
            /*receive 0x01 progress*/
            SEND1="5A";
            RECEIVE1   ="00000BCD";
            /*receive 0x02 progress*/
            /*.*/
            /*receive 0x63 progress*/
            SEND2="5A";
            RECEIVE2[0]="00000000";
            RECEIVE2[1]="0x64";
            SEND3="5A";
            RECEIVE3="5A";
        }
    }Struct_format_Flash;

    typedef struct send_FW_File_
    {
        const char* SEND0[4];
        const char* RECEIVE0[2];

        const char* RECEIVE1[2];
        const char* SEND2;

        const char* RECEIVE3;
        const char* SEND4;
        const char* RECEIVE4;
        send_FW_File_()
        {
            SEND0[0]="B2";
            SEND0[1]="08000000";  /*FW flash address ,  4 bytes*/
            SEND0[2]="00001000"; /*FW total length  ,  4 bytes*/
            SEND0[3]="00001000"; /*FW packet length ,  4 bytes*/
            RECEIVE0[0]="5A";
            RECEIVE0[1]="5A";    /*If check fails,module will return0x5A and errorcode.*/
            /*SEND(FW packet, 4096 bytes)*/

            RECEIVE1[0]="69";
            /*SEND(FW packet, 4096 bytes)*/
            /*RECEIVE(0xC0)*/
            /*RECEIVE(0x69)*/
            //.
            //.
            //.
            /*SEND(FW packet, 4096 bytes)*/

            /*RECEIVE(0xC0)*/
            /*RECEIVE(0x69)*/
            /*RECEIVE(0x5A)*/
            RECEIVE1[1]="5A";
            /*SEND(Total FW checksum, 4 bytes)*/
            RECEIVE3="5A";
            SEND4   ="";/*If this FW is bootloader, send0x5A, other wises end 0xA5.*/
            RECEIVE4="5A";
        }
    }STruct_Send_FW_File;/*If the last FW packet is less than 4096 bytes, add “0xFF” at the end of the last FW packet.*/

    typedef  struct Read_Back_
    {
        QString FileDir;

        QString Name;

        Address_Type AddressType;
        int  BeginAddress;
        int  ReadSize;
        bool Select;
    }Struct_ReadBack;



#pragma pack(1)
    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
    }RACE_SIZE_SEND;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t status_;
        uint32_t size_;
    }RACE_SIZE_RES;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
    }RACE_ADDR_SEND;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t status_;
        uint32_t addr_;
    }RACE_ADDR_RES;


    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
    }RACE_ID_SEND;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t status_;
        uint8_t flash_id_[3];
    }RACE_ID_RES;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint32_t addr_;
        uint32_t size_;
        uint32_t crc_;
    }RACE_FM_SEND;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t status_;
        uint32_t addr_;
    }RACE_FM_RES;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint32_t addr_;
        uint16_t size_;
        uint8_t buf_[DA_SEND_PACKET_LEN];
        uint32_t crc_;
    }RACE_DL_SEND;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t status_;
        uint32_t addr_;
    }RACE_DL_RES;

    typedef struct
    {
        uint8_t head_;
        uint8_t type_;
        uint16_t len_;
        uint16_t id_;
        uint8_t pld;
    }RACE_rebot;

    #pragma pack()
    Q_INVOKABLE const char* getFormatTotalLength( BROM_BASE::QGNSS_Model mtkmodel);
public:

    const Download_State &download_state() const;

    bool cfg_parse(const QByteArray &data,QString cfg_dir);
Q_SIGNALS:

    void download_stateChanged(const Download_State &state);

    void cfg_structChanged(CFG * cfg_struct);

public Q_SLOTS:

    void setDownload_state(const Download_State &newDownload_state);

    void setDownload_stateStr(QString state);

    void setCfg_struct(CFG * cfg_struct);

private:
    bool load_file(QString DA_Path,QString flash_cfg_path);

    uint16_t DA_compute_checksum (uint8_t *buf, uint32_t buf_len)
    {
        uint16_t checksum = 0;
        if (buf == NULL || buf_len == 0) {
            return 0;
        }
        int i = 0;
        for (i = 0; i < (int32_t)buf_len / 2; i++) {
            checksum ^= *(uint16_t *) (buf + i * 2);
        }
        if ((buf_len % 2) == 1) {
            checksum ^= buf[i * 2];
        }
        return checksum;
    }

    uint32_t CRC_compute_simple_checksum (uint8_t *buf, uint32_t buf_len) {
        uint32_t checksum = 0;
        if (buf == NULL || buf_len == 0) {
            return 0;
        }
        for (int i = 0; i < (int32_t)buf_len; i++) {
            checksum += *(buf + i);
        }
        return checksum;
    }

    Struct_HandShakeWithModul     ST_HandShakeWithModul;
    Struct_Disable_Modeule_WDT    ST_Disable_Modeule_WDT;
    Struct_send_DA_File_to_Module ST_send_DA_File_to_Module;
    Struct_jump_to_DA             ST_jump_to_DA;
    hdl_da_report_t               da_report;

    Struct_format_Flash           ST_format_Flash;
    STruct_Send_FW_File           ST_Send_FW_File;

    CFG cfg_;
    QByteArray flash_downloadcfg_;

    //********************************************************download func************************//
    bool handshake_with_Module();
    bool disable_Modeule_WDT();
    bool send_DA_File_to_Module();
    bool jump_to_DA();
    bool sync_with_DA();
    bool format_Flash(Format_mode mode=Format_mode::Auto_Format,QString hexbeginaddress="08000000",QString hexlength="001F8000");
    bool format_help(QString hexbeginaddress/*8 bits*/,QString hexlength/*8bits*/);
    bool send_FW_File_to_Module();
    bool autoReboot();
    bool read_back(const Struct_ReadBack *ReadBack,int size);
    bool read_back_help(const Struct_ReadBack &ReadBack);
    bool finish();

    bool disable_Modeule_WDT_help(QString Hexaddr, QString hexdata);
    bool hdl_da_get_flash_address(uint32_t *pData);
    bool hdl_da_get_flash_size(uint32_t *pData);
    bool hdl_da_get_flash_id(uint8_t *pManufacturerId, uint8_t *pDeviceId1, uint8_t *pDeviceId2);
    bool hdl_format_race(uint32_t addr, uint32_t len);

    bool check_address_range(uint32_t address, uint32_t len, const hdl_da_report_t *da_report);
    bool hdl_download_race(uint32_t addr, const uint8_t *data);
    bool hdl_da_download(hdl_image_t *image);

    bool hdl_brom_set_baudrate(uint32_t bd);
    //********************************************************download func************************//
    /**
     *  PartitionTable,
     *  BootLoader,
     *  CM4,
     *  GNSSConfiguration
     * */
    bool send_fw_file_help(FW_Type fwtype);

    Download_State download_state_=Download_State::State_Unstart;
    QGNSS_Model      mtkmodel_;

    int downrate_=921600;
    bool RTS_=false;
    // MTK_Base interface
    CFG * m_cfg_struct;

public:
    Q_INVOKABLE  virtual bool qGNSS_mTK_DownLoad(QString DA_Path, QString CFG_Path, int mtkmodel, int mode) override;

    Q_INVOKABLE  virtual bool qGNSS_mTK_Format(QString DA_Path, QString CFG_Path, int mtkmodel
                                              ,Format_mode mode,QString hexbegin_address=0,QString hexLength=0);

    Q_INVOKABLE  bool qGNSS_mTK_ReadBack(QString DA_Path, QString CFG_Path, int mtkmodel, int mode,const Struct_ReadBack *STRread,int readLen);

    Q_INVOKABLE  virtual void qGNSS_SetDownLoadBaudRate(int baudrate) override;
    Q_INVOKABLE void MTK_init_IO(IODvice *io) override;
    Q_INVOKABLE bool download_stop()          override;
    Q_INVOKABLE void reset_buadRate() override;
    bool downbegin(QString DA_Path, QString CFG_Path, int mtkmodel, int mode);
    CFG * cfg_struct() const;

};

Q_DECLARE_METATYPE (MTK_BROM_3352::Struct_HandShakeWithModul)
Q_DECLARE_METATYPE (MTK_BROM_3352::Struct_Disable_Modeule_WDT)
Q_DECLARE_METATYPE (MTK_BROM_3352::Struct_send_DA_File_to_Module)
Q_DECLARE_METATYPE (MTK_BROM_3352::Struct_jump_to_DA)

Q_DECLARE_METATYPE (MTK_BROM_3352::Struct_format_Flash)
Q_DECLARE_METATYPE (MTK_BROM_3352::STruct_Send_FW_File)
Q_DECLARE_METATYPE (MTK_BROM_3352::CFG)
Q_DECLARE_METATYPE (MTK_BROM_3352::CFG*)
Q_DECLARE_METATYPE (MTK_BROM_3352::CFG_general)


#endif // MTK_BROM_3352_H
