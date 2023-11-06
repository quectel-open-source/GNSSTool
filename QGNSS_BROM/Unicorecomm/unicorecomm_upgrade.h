#ifndef UNICORECOMM_UPGRADE_H
#define UNICORECOMM_UPGRADE_H

#include "unicorecomm_base.h"
#include "QApplication"

#define QERROR(x) \
    setErroString (x);\
    setDownload_state(Download_State::State_CMD_Fail);

#define QINFO(x) \
    setBbmsgStr ({BROM_BASE::brmsgtype::bb_Info,x});

class QGNSS_BROM_EXPORT Unicorecomm_Upgrade:public Unicorecomm_Base
{
  Q_OBJECT
  Q_PROPERTY(Download_State download_state READ download_state WRITE setDownload_state NOTIFY download_stateChanged)

 public:
 explicit Unicorecomm_Upgrade(QObject *parent=nullptr);

  enum Download_State
  {
    State_Unstart,
    S_Check_FW_Vaild,
    S_Changed_BaudRate,
    S_UPG_Request,
    S_Send_Bootloader,
    S_Send_FW,
    //**********custom**************//
    State_Finished,
    State_CMD_Fail,
    Custom_DownLoad_Stoping,
    Custom_DownLoad_Stoped
  };
  Q_ENUM (Download_State);

 Q_SIGNALS:
  void download_stateChanged(const Download_State &state);

 public:
  Download_State download_state() const;
  Q_INVOKABLE void init_IO(IODvice *io) override;
  Q_INVOKABLE bool download_stop() override;
  Q_INVOKABLE void reset_buadRate() override;
  Q_INVOKABLE bool qGNSS_DownLoad(QString Path,int module) override;
  Q_INVOKABLE void qGNSS_SetDownLoadBaudRate(int baudrate) override;

  typedef struct
 {
    uint32_t MagicNumber;  // Image Header Magic Number
    uint32_t PkgChecksum;  // Image Data vertical accumulation check
    uint32_t TimeStamp;    // Image Creation Timestamp
    uint32_t PkgSize;      // Image Data Size
    uint32_t Reserved;     // Image Header Reserved Area
    uint32_t PkgFlashAddr; // flash memory offset for package
    uint32_t PkgRunAddr;   // Run time address for this package
    uint32_t HeaderCRC;    // Image Header CRC checksum
  }Ql_Image_Header_Typedef;

  uint32_t CovertArrayToUint32(const char* src, uint16_t offset)
  {
    uint32_t number = 0;
    number |= (src[offset] & 0xff);
    number |= (uint32_t)((src[offset + 1] & 0xff) << 8);
    number |= (uint32_t)((src[offset + 2] & 0xff) << 16);
    number |= (uint32_t)((src[offset + 3] & 0xff) << 24);
    return number;
  }

  enum XmodemState
  {
    XMODEM_SOH = 0x02, //1024字节数据标志
    XMODEM_EOT = 0x04, //发送结束标志
    XMODEM_ACK = 0x06, //响应
    XMODEM_NAK = 0x15, //错误，重发
    XMODEM_CAN = 0x18, //结束
  };

  uint16_t Calculate_CRC16(const char* source, uint16_t count)
  {
    char* ptr = (char*)source;
    uint16_t crc = 0;
    while (count--)
    {
      crc = crc ^ (int)(*ptr++) << 8;
      for (int i = 0; i < 8; i++)
      {
        if (crc & 0x8000)
        {
          crc = (crc << 1) ^ 0x1021;
        }
        else
        {
          crc = crc << 1;
        }
      }
    }
    return (crc & 0xFFFF);
  }

  XmodemState ReadReciveState();


  void LoadPackage(char startHeader, char packageNumber, char* package, const char* data, int xmodemDataSize = 1024);
  bool Transmit(FILE* fStream);
 public Q_SLOTS:
  void setDownload_state(const Download_State &newDownload_state);
  void setDownload_stateStr(QString state);

 private:
  int download_baudRate_=115200;        /*default downloadbaudrate*/
  Download_State download_state_=Download_State::State_Unstart;
  QByteArray firmwaredata_;
  QString Bootloader_pth=QApplication::applicationDirPath ()+"/bootloader.pkg";
  bool VerifyImageInfo(FILE* fStream, uint32_t packetageType);
  bool FwIsValid(FILE* fStream);

  bool Check_fw_Valid(FILE* fwStream);

  bool Change_BaudRate(int BaudRate);

  bool Obtaining_module_information();
  bool Soft_Reset();

  bool UPG_Request();
  bool Send_File(FILE* Stream);
  bool F_SoftReset();
  bool Check_NMEAOutPut();
};

#endif // UNICORECOMM_UPGRADE_H
