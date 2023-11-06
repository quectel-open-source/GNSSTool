/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         st_mqsp_upgrade.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-06-14      victor.gong
* ***************************************************************************/
#ifndef ST_MQSP_UPGRADE_H
#define ST_MQSP_UPGRADE_H

#include "st_base.h"


/**
      *Doc source AN_STA8100_XLoaderImplementationGuide (H7 serials)
      *Erase_option = 1 consists in erasing firmware area and a second area which usually matches with  navigation data  area
      *(offset  0x180000) or  settings area  (0x170000).  (*) erase_start and erase_size allow to extend this area
      **/
struct GNSS_FW_INFO
{
  unsigned int  GNSS_FW_Size;
  unsigned int  GNSS_FW_CRC32;
  unsigned int  GNSS_DestAddr;
  unsigned int  GNSS_EntryPoint;
  unsigned int  GNSS_NVM_Offset;
  unsigned int  GNSS_NVM_Erase_Size;
  unsigned char GNSS_EraseNVM;
  unsigned char GNSS_EraseOnly;
  unsigned char GNSS_ProgramOnly;
  unsigned char GNSS_Reversed[5];
  GNSS_FW_INFO()
  {
    GNSS_DestAddr          =0x10000000U;
    GNSS_EntryPoint        =0x00000400U;
    GNSS_NVM_Offset        =0x000FF000;//0x000FF000;//0x00170000U;//0x00000000U;//0x00170000U
    GNSS_NVM_Erase_Size    =0x00101000;//0x00101000;//0x00090000U;//0x00000000U;//0x00090000U
    GNSS_EraseNVM          =0x01U;//0x01U;
    GNSS_EraseOnly         =0x00U;
    GNSS_ProgramOnly       =0x00U;
    memset (GNSS_Reversed,0,sizeof (GNSS_Reversed));
  }
  char *RS(const char* data,size_t size)
  {
    char * rdata=new char[size];
    for(uint i=0;i<size;i++)
    {
      rdata[i]=data[size-i-1];
    }
    return  rdata;
  };
  char* Get_chr(uint fwsize,uint fwcrc32,bool eraseNVM=true)
  {
    GNSS_EraseNVM=eraseNVM?0x01U:0x00U;
    char * data= new char[32];
    char * fsize    =RS (reinterpret_cast<char*>(&fwsize),sizeof (fwsize));
    char * fcrc32   =RS (reinterpret_cast<char*>(&fwcrc32),sizeof (fwcrc32));
    char * dstaddr  =RS (reinterpret_cast<char*>(&GNSS_DestAddr),sizeof (GNSS_DestAddr));
    char * etyPot   =RS (reinterpret_cast<char*>(&GNSS_EntryPoint),sizeof (GNSS_EntryPoint));
    char * nvmoffset=RS (reinterpret_cast<char*>(&GNSS_NVM_Offset),sizeof (GNSS_NVM_Offset));
    char * erasesize=RS (reinterpret_cast<char*>(&GNSS_NVM_Erase_Size),sizeof (GNSS_NVM_Erase_Size));
    char *ptr=nullptr;
    memcpy (ptr=data,fsize,sizeof (fwsize));
    memcpy (ptr+=sizeof (fwsize),fcrc32,sizeof (fwcrc32));
    memcpy (ptr+=sizeof (fwcrc32),dstaddr,sizeof (GNSS_DestAddr));
    memcpy (ptr+=sizeof (GNSS_DestAddr),etyPot,sizeof (GNSS_EntryPoint));
    memcpy (ptr+=sizeof (GNSS_EntryPoint),nvmoffset,sizeof (GNSS_NVM_Offset));
    memcpy (ptr+=sizeof (GNSS_NVM_Offset),erasesize,sizeof (GNSS_NVM_Erase_Size));
    memcpy (ptr+=sizeof (GNSS_NVM_Erase_Size),&GNSS_EraseNVM,sizeof (GNSS_EraseNVM));
    memcpy (ptr+=sizeof (GNSS_EraseNVM),&GNSS_EraseOnly,sizeof (GNSS_EraseOnly));
    memcpy (ptr+=sizeof (GNSS_EraseOnly),&GNSS_ProgramOnly,sizeof (GNSS_ProgramOnly));
    memcpy (ptr+=sizeof (GNSS_ProgramOnly),GNSS_Reversed,sizeof (GNSS_Reversed));

    delete  fsize;
    delete  fcrc32;
    delete  dstaddr;
    delete  etyPot;
    delete  nvmoffset;
    delete  erasesize;
    fsize=nullptr;
    fcrc32=nullptr;

    return data;
  }
} ;

struct APP_FW_INFO
{
  unsigned int  APP_FW_Size;
  unsigned int  APP_FW_CRC32;
  unsigned int  APP_DestAddr;
  unsigned char APP_EraseNVM;
  unsigned char APP_Reversed[3];
  APP_FW_INFO()
  {
    APP_DestAddr        =0x00000000U;
    APP_EraseNVM        =0x01U;
    memset (APP_Reversed,0,sizeof (APP_Reversed));
  }
   char *RS(const char* data,size_t size)
  {
    char * rdata=new char[size];
    for(uint i=0;i<size;i++)
    {
      rdata[i]=data[size-i-1];
    }
    return  rdata;
  };
  char* Get_chr(uint fwsize,uint fwcrc32)
  {
    char * data= new char[16];
    APP_FW_Size=fwsize;
    APP_FW_CRC32=fwcrc32;
    char * fsize    =RS (reinterpret_cast<char*>(&fwsize),sizeof (fwsize));
    char * fcrc32   =RS (reinterpret_cast<char*>(&fwcrc32),sizeof (fwcrc32));
    char * dstaddr  =RS (reinterpret_cast<char*>(&APP_DestAddr),sizeof (APP_DestAddr));
    char * ernvm    =RS (reinterpret_cast<char*>(&APP_EraseNVM),sizeof (APP_EraseNVM));
    char * reserv   =RS (reinterpret_cast<char*>(&APP_Reversed),sizeof (APP_Reversed));
    char *ptr=nullptr;
    memcpy (ptr=data,fsize,sizeof (fwsize));
    memcpy (ptr+=sizeof (fwsize),fcrc32,sizeof (fwcrc32));
    memcpy (ptr+=sizeof (fwcrc32),dstaddr,sizeof (APP_DestAddr));
    memcpy (ptr+=sizeof (APP_DestAddr),ernvm,sizeof (APP_EraseNVM));
    memcpy (ptr+=sizeof (APP_EraseNVM),reserv,sizeof (APP_Reversed));

    delete  fsize;
    delete  fcrc32;
    delete  dstaddr;
    delete  ernvm;
    delete  reserv;
    return data;
  }
} ;

class ST_MQSP_Upgrade:public ST_Base
{
    Q_OBJECT
public:
    ST_MQSP_Upgrade(QObject *parent=nullptr);

    bool qGNSS_ST_DownLoad(QString firmware,int qgnss_model
                               ,int downmode,bool showExpert=false);

    virtual void init_IO(IODvice *io) override;
    virtual bool download_stop() override;
    virtual void reset_buadRate() override;
    virtual void qGNSS_SetDownLoadBaudRate(int baudrate) override;

        enum Bootloader_Command
    {
        Get                             =0x00,
        GetVersion_ReadProtectionStatus =0x01,
        GetID                           =0x02,
        ReadMemory                      =0x11,
        Go                              =0x21,
        WriteMemory                     =0x31,
        Erase                           =0x43,
        ExtendedErase                   =0x44,
        WriteProtect                    =0x63,
        WriteUnprotect                  =0x73,
        ReadoutProtect                  =0x82,
        ReadoutUnprotect                =0x92,
        };
    Q_ENUM (Bootloader_Command)

        enum class Down_Mode:unsigned char
    {
        MCU_Download,
        MCU_Upgrade,
        GNSS_Upgrade,
        };
    Q_ENUM (Down_Mode)

    enum class Response_Status:unsigned short
    {
        successfully,                   //Receive message and execution successfully.
        Unknow_error_occurs,            //Unknow error occurs.
        CRC32_checksum_error,
        Timeout,
        Nonsupport_message,
        Message_packet_error,
        GNSS_sync_error=10,
        GNSS_change_baud_rate_error,
        GNSS_Bootloader_error,
        GNSS_firmware_information_error,
        GNSS_firmware_upgrade_error,
        APP_firmware_area_erase_error=20,
        APP_firmware_write_to_flash_error
    };
    Q_ENUM (Response_Status)

    typedef struct mqsp_constant
    {
        uint32_t sSYNC_WORD1;
        uint32_t sRSP_WORD1;
        uint32_t SYNC_WORD2;
        uint32_t RSP2_WORD2;
        mqsp_constant()
        {
            sSYNC_WORD1=0x514C1309;
            sRSP_WORD1 =0xAAFC3A4D;
            SYNC_WORD2 =0x1203A504;
            RSP2_WORD2 =0x55FD5BA0;
        }
    }MQSP_Constant;
    MQSP_Constant MQSPConstant_;

public Q_SLOTS:
Q_SIGNALS:
 void showGNSSFWINFO_UI(GNSS_FW_INFO*opt);
 void showAPPFWINFO_UI(APP_FW_INFO*opt);
private:
    const int trytimes=100;
    QByteArray firmwaredata_;
    int download_baudRate_=921600;
    Down_Mode downmode_;
    unsigned char classID_=0x01U;
    const char* ACK="79";
    char startAddress[4]{0x08,0x00,0x00,0x00};
    QByteArray qsenddata;
    const int  max_read_time=20000;
    bool RTS_=false;
    QSerialPort::StopBits stpbs;

    QSerialPort::Parity   prty;
    char getxor(const char * data,size_t size)
    {
        char rdata=0;
        for(uint i=0;i<size;i++)
        {
            rdata^=data[i];
        }
        return  rdata;
    }
public:

    QByteArray getProtocol(unsigned char header,unsigned char  classID,
                           unsigned char msgID,unsigned short pLength,
                           QByteArray  pfAddress);

    // Upgrade Process
    bool ConnectModule();
    bool Synchronize_Module();
    bool Query_the_Bootloader_Version();
    bool Send_Firmware_Address();
    bool Send_Firmware_Information (bool showExpert);
    bool Send_Start_Firmware_Upgrade_Command ();
    bool Send_Firmware ();


    bool USART_Boot_Code_Sequence();//0x7F

    // MCU Download
    template<Bootloader_Command Command>
    bool BootloaderCommand();

    /**
     *Get command
     *The Get command allows the user to get the version of the bootloader and the supported
     *commands. When the bootloader receives the Get command, it transmits the bootloader
     *version and the supported command codes to the hos
     * Byte 1:  ACK
     * Byte 2:  N = 11 = the number of bytes to follow – 1 except current and ACKs.
     * Byte 3:  Bootloader version (0 < Version < 255), example: 0x10 = Version 1.0
     * Byte 4:  0x00 – Get command
     * Byte 5:  0x01 – Get Version and Read Protection Status
     * Byte 6:  0x02 – Get ID
     * Byte 7:  0x11 – Read Memory command
     * Byte 8:  0x21 – Go command
     * Byte 9:  0x31 – Write Memory command
     * Byte 10: 0x43 or 0x44 – Erase command or Extended Erase command(exclusive commands)
     * Byte 11: 0x63 – Write Protect command
     * Byte 12: 0x73 – Write Unprotect command
     * Byte 13: 0x82 – Readout Protect command
     * Byte 14: 0x92 – Readout Unprotect command
     * Last byte (15): ACK
     */
    template<> inline bool BootloaderCommand<Get>()
    {
        setUpgrade_state (upgrade_flow::MQSPGet_command);
        setDownload_percentage (0);
        QString cmd="00FF";
        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        QByteArray read=io_->io_Read (max_read_time,15);
        setDownload_percentage (100);
        if(read.length ()!=15)
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: Get Command]"+LOGFRONT+
                                    QString("STM32 Send Data Size:%1 ")
                                        .arg (QString::number (read.size ()))
                                    );
            return false;
        }

        if(read.mid (0,1)!=QByteArray::fromHex (ACK)&&read.mid (read.size ()-1,1)!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: Get Command]"+LOGFRONT+
                                    QString("STM32 Send:%1 NACK")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        QByteArray version=read.mid (2,1).toHex();
        version=version.insert (1,'.');
        QString qvr=version;
        QMSG<MSGType::bb_Info>("[BootloaderCommand: Get Command]"+
                               QString("Bootloader version: %1 Command: %2")
                                   .arg (QString(qvr))
                                   .arg (QString(read.mid (3,11).toHex (' ').toUpper ()))
                               );
        return true;
    };

    /**
     *Get Version & Read Protection Status command
     *The Get Version & Read Protection Status command is used to get the bootloader version
     *and the read protection status. After receiving the command the bootloader transmits the
     *version, the read protection and number of times it was enabled and disabled to the host
     */
    template<> inline bool BootloaderCommand<GetVersion_ReadProtectionStatus>()
    {

        return true;
    };

    /**
     * Get ID command
     * The Get ID command is used to get the version of the chip ID (identification). When the
     * bootloader receives the command, it transmits the product ID to the host
     * Get ID command
     * The Get ID command is used to get the version of the chip ID (identification). When the
     * bootloader receives the command, it transmits the product ID to the host.
     * The STM32 device sends the bytes as follows:
     * Byte 1:    ACK
     * Byte 2:    N = the number of bytes – 1 (N = 1 for STM32), except for current byte and ACKs.
     * Bytes 3-4: PID(1) byte 3 = 0x04, byte 4 = 0xXX
     * Byte 5:    ACK
     */
    template<> inline bool BootloaderCommand<GetID>()
    {
        setUpgrade_state (upgrade_flow::MQSPGetID);
        setDownload_percentage (0);
        QString cmd="02FD";
        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        QByteArray read=io_->io_Read (max_read_time,5);
        setDownload_percentage (100);
        if(read.size ()!=5)
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: GetID]"+LOGFRONT+
                                    QString("STM32 Send Data Size:%1 unequal :5 ")
                                        .arg (QString(read.size ()))
                                    );
            return false;
        }

        if(read.mid (0,1)!=QByteArray::fromHex (ACK)&&read.mid (read.size ()-1,1)!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: GetID]"+LOGFRONT+
                                    QString("STM32 Send:%1 NACK")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        QMSG<MSGType::bb_Info>("[BootloaderCommand: GetID]"+
                               QString(" Product ID: %1")
                                   .arg (QString(read.mid (2,2).toHex (' ').toUpper ()))
                               );
        return true;
    };

    /**
*Read Memory command
*The Read Memory command is used to read data from any valid memory address (refer to
*the product datasheets and to AN2606 for more details) in RAM, Flash memory and the
*information block (system memory or option byte areas).
*When the bootloader receives the Read Memory command, it transmits the ACK byte to the
*application. After the transmission of the ACK byte, the bootloader waits for an address (four
*bytes, byte 1 is the address MSB and byte 4 is the LSB) and a checksum byte, then it
*checks the received address. If the address is valid and the checksum is correct, the
*bootloader transmits an ACK byte, otherwise it transmits a NACK byte and aborts the
*command.
*When the address is valid and the checksum is correct, the bootloader waits for the number
*of bytes to be transmitted – 1 (N bytes) and for its complemented byte (checksum). If the
*checksum is correct it then transmits the needed data ((N + 1) bytes) to the application,
*starting from the received address. If the checksum is not correct, it sends a NACK before
*aborting the command.
* The host sends bytes to the STM32 as follows:
* Bytes 1-2: 0x11 + 0xEEWait for ACK
* Bytes 3 to 6 Start address byte 3: MSB, byte 6: LSB
* Byte 7: Checksum: XOR (byte 3, byte 4, byte 5, byte 6)Wait for ACK
* Byte 8: The number of bytes to be read – 1 (0 < N ≤ 255);
* Byte 9: Checksum: XOR byte 8 (complement of byte 8)
*/
    template<> inline bool BootloaderCommand<ReadMemory>()
    {
        setUpgrade_state (upgrade_flow::MQSPRead_Memory);
        setDownload_percentage (0);
        QString cmd="11EE";
        QString cmdread="0BF4";

        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        QByteArray read=io_->io_Read (max_read_time,1);
        setDownload_percentage (20);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: ReadMemory]"+LOGFRONT+
                                    QString("STM32 Send Data :%1  NACK ")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        char senddata[5]{0};
        memcpy (senddata,startAddress,sizeof (startAddress));
        char cxor=getxor (startAddress,sizeof (startAddress));
        memcpy (senddata+sizeof (startAddress),&cxor,sizeof (cxor));
        io_->io_Write (QByteArray::fromRawData (senddata,sizeof (senddata)),0,true);
        read=io_->io_Read (max_read_time,1);
        setDownload_percentage (40);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: ReadMemory]"+LOGFRONT+
                                    QString("STM32 Send Data :%1  NACK ")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }

        io_->io_Write (QByteArray::fromHex (cmdread.toUtf8 ()),0,true);
        read=io_->io_Read (max_read_time,1);
        setDownload_percentage (60);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: ReadMemory]"+LOGFRONT+
                                    QString("STM32 Send Data :%1  NACK ")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        read=io_->io_Read (max_read_time,12);
        setDownload_percentage (100);
        QMSG<MSGType::bb_Info>("[BootloaderCommand: ReadMemory]"+
                               QString("ReadMemory: %1   ")
                                   .arg (QString(read.toHex (' ').toUpper ()))
                               );
        return true;
    };

    template<> inline bool BootloaderCommand<Go>()
    {

        return true;
    };

    template<> inline bool BootloaderCommand<WriteMemory>()
    {
        QString cmd="31CE";
        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        IOserial *seril=static_cast<IOserial *>(io_.data ());

        QByteArray read=io_->io_Read (max_read_time,1);
         if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: WriteMemory]"+LOGFRONT+
                                    QString("STM32 Send Data  %1 :%2  NACK ")
                                        .arg (cmd)
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        char senddata[5]{0};
        memcpy (senddata,startAddress,sizeof (startAddress));
        char cxor=getxor (startAddress,sizeof (startAddress));
        memcpy (senddata+sizeof (startAddress),&cxor,sizeof (cxor));
        io_->io_Write (QByteArray::fromRawData (senddata,sizeof (senddata)),0,true);
        read=io_->io_Read (max_read_time,1);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: WriteMemory]"+LOGFRONT+
                                    QString("STM32 Send Data %1 :%2  NACK ")
                                        .arg (QString(QByteArray::fromRawData (senddata,sizeof (senddata)).toHex ()))
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        qsenddata.insert (0,qsenddata.size ()-1);
        cxor=getxor (qsenddata.data (),qsenddata.size ());
        qsenddata.append (cxor);
        io_->io_Write (qsenddata,0,true);
        io_->io_asyncsleep (10);
        read=io_->io_Read (max_read_time,1);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: WriteMemory]"+LOGFRONT+
                                    QString("STM32 Send Data :%1  NACK ")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        return true;
    };

    template<> inline bool BootloaderCommand<Erase>()
    {

        return true;
    };

    /**
     *Extended Erase Memory command
     *The Extended Erase Memory command allows the host to erase Flash memory pages using
     *two bytes addressing mode. When the bootloader receives the Extended Erase Memory
*command, it transmits the ACK byte to the host. After the transmission of the ACK byte, the
*bootloader receives two bytes (number of pages to be erased), the Flash memory page
*codes (each one coded on two bytes, MSB first) and a checksum byte (XOR of the sent
*bytes); if the checksum is correct, the bootloader erases the memory and sends an ACK
*byte to the host. Otherwise it sends a NACK byte to the host and the command is aborted.Extended Erase Memory command specifications:1. The bootloader receives one half-word (two bytes) that contains N, the number of
*pages to be erased: a) For N = 0xFFFY (where Y is from 0 to F) special erase is performed:
*- 0xFFFF for global mass erase
*- 0xFFFE for bank 1 mass erase
*- 0xFFFD for bank 2 mass erase
*- Codes from 0xFFFC to 0xFFF0 are reserved
*b) For other values where 0 ≤  N <  maximum number of pages: N + 1 pages are
*erased.2. The bootloader receives: a) In the case of a special erase, one byte: checksum of the previous bytes:
*- 0x00 for 0xFFFF
*- 0x01 for 0xFFFE
*- 0x02 for 0xFFFD
*a) In the case of N+1 page erase, the bootloader receives (2 x (N + 1)) bytes, each
*half-word containing a page number (coded on two bytes, MSB first). Then all
*previous byte checksums (in one byte).
*Note: No error is returned when performing erase operations on write-protected sectors.The maximum number of pages is relative to the product and must be respected.
*Byte 3: 0xFF or number of pages to be erased – 1 (0 ≤  N ≤ maximum number of pages)Byte 4: 0x00 (in case of global erase) or ((N + 1 bytes (page numbers) and then checksum
*XOR (N, N+1 bytes))
*
     **/
    template<> inline bool BootloaderCommand<ExtendedErase>()
    {
        setUpgrade_state (upgrade_flow::MQSPExtendedErase);
        setDownload_percentage (0);
        QString cmd="44BB";
        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        QByteArray read=io_->io_Read (max_read_time,1);
        setDownload_percentage (50);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: ExtendedErase]"+LOGFRONT+
                                    QString("STM32 Send %1: %2 NACK")
                                        .arg (cmd)
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        char numberpages[2]={0x00,0x0F};
        char pages[35]{0};
        pages[0]=numberpages[0];
        pages[1]=numberpages[1];
        for(int i=0;i<16;i++)
        {
            unsigned short page=i;
            char * data=reinterpret_cast<char*>(&page);
            pages[2*i+2]=data[1];
            pages[2*i+1+2]=data[0];
        }
        char cxor= getxor(pages,34);
        pages[34]=cxor;

        io_->io_Write (QByteArray::fromRawData (pages,sizeof (pages)),0,true);
        read=io_->io_Read (max_read_time,1);
        setDownload_percentage (100);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: ExtendedErase]"+LOGFRONT+
                                    QString("STM32 Send %1: %2 NACK")
                                        .arg (QString(QByteArray::fromRawData (pages,sizeof (pages)).toHex ().toUpper ()))
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }

        return true;
    };
    template<> inline bool BootloaderCommand<WriteProtect>()
    {
        return true;
    };
    /**
     *Write Unprotect command
     *The Write Unprotect command is used to disable the write protection of all the Flash
     *memory sectors. When the bootloader receives the Write Unprotect command, it transmits
     *the ACK byte to the host. After the transmission of the ACK byte, the bootloader disables
     *the write protection of all the Flash memory sectors. After the unprotection operation the
     *bootloader transmits the ACK byte.At the end of the Write Unprotect command, the bootloader transmits the ACK byte and
     *generates a system reset to take into account the new configuration of the option byte
     **/
    template<> inline bool BootloaderCommand<WriteUnprotect>()
    {
        setUpgrade_state (upgrade_flow::MQSPWriteUnprotect);
        setDownload_percentage (0);
        QString cmd="738C";
        io_->io_Write (QByteArray::fromHex (cmd.toUtf8 ()),0,true);
        QByteArray read=io_->io_Read (max_read_time,1);
        setDownload_percentage (50);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: WriteUnprotect]"+LOGFRONT+
                                    QString("STM32 Send1:%1 NACK")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        read=io_->io_Read (max_read_time,1);
        setDownload_percentage (100);
        if(read!=QByteArray::fromHex (ACK))
        {
            QMSG<MSGType::bb_Error>("[BootloaderCommand: WriteUnprotect]"+LOGFRONT+
                                    QString("STM32 Send2:%1 NACK")
                                        .arg (QString(read.toHex (' ').toUpper ()))
                                    );
            return false;
        }
        return true;
    };

    template<> inline bool BootloaderCommand<ReadoutProtect>()
    {

        return true;
    };

    template<> inline bool BootloaderCommand<ReadoutUnprotect>()
    {
        return true;
    };
};

#include <st_mqsp_upgrade-inl.h>

#endif // ST_MQSP_UPGRADE_H
