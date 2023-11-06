#ifndef __ST_TESTO5_ST8100_WMCU_FWUPG_H__
#define __ST_TESTO5_ST8100_WMCU_FWUPG_H__

#include <map>
#include <string>
#include <functional>
#include <atomic>
#include <chrono>
#include <filesystem>
#include "QG_Uart.h"
#include "QG_Common.h"
#include "QG_Print.h"

QG_PORTEXTERN class ST8100_wmcu_fwupg:public QG_Uart
{

public:
    ST8100_wmcu_fwupg();
    enum class ClassID :unsigned char
    {
        GNSS = 0x01,
        APP = 0x02,
        SGNSS = 0x03,
        SAPP = 0x04,
        Query = 0xF0
    };

    enum class Query_MsgID :unsigned char
    {
        response = 0x00,
        bootloader_version = 0x01
    };

    enum class MsgID :unsigned char
    {
        response = 0x00,
        send_fw_address = 0x01,
        send_fw_information = 0x02,
        start_fw_upg = 0x03,
        send_fw_pkg = 0x04
    };

    struct gnss_fw_info //32 bytes
    {
        unsigned int  fw_size;
        unsigned int  fw_crc32;
        unsigned int  destaddr;
        unsigned int  entrypoint;
        unsigned int  nvm_offset;
        unsigned int  nvm_erase_size;
        unsigned char erasenvm;
        unsigned char eraseonly;
        unsigned char programonly;
        unsigned char reversed[5];
        /*
        gnss_fw_info()
        {
            destaddr = 0x10000000u;
            entrypoint = 0x00000400u;
            nvm_offset = 0x000ff000;
            nvm_erase_size = 0x00101000;
            erasenvm = 0x01u;
            eraseonly = 0x00u;
            programonly = 0x00u;
            memset(reversed, 0, sizeof(reversed));
        }
        */
        char* Get_chr(uint32_t fwsize, uint32_t fwcrc32)
        {
            char* data = new char[32];
            unsigned int  destaddr_= destaddr;
            unsigned int  entrypoint_= entrypoint;
            unsigned int  nvm_offset_= nvm_offset;
            unsigned int  nvm_erase_size_= nvm_erase_size;

            char* fsize = RS(reinterpret_cast<char*>(&fwsize), sizeof(unsigned int));
            char* fcrc32 = RS(reinterpret_cast<char*>(&fwcrc32), sizeof(unsigned int));
            char* dstaddr = RS(reinterpret_cast<char*>(&destaddr_), sizeof(unsigned int));
            char* etyPot = RS(reinterpret_cast<char*>(&entrypoint_), sizeof(unsigned int));
            char* nvmoffset = RS(reinterpret_cast<char*>(&nvm_offset_), sizeof(unsigned int));
            char* erasesize = RS(reinterpret_cast<char*>(&nvm_erase_size_), sizeof(unsigned int));
            char* ptr = data;
            memcpy(ptr, fsize, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), fcrc32, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), dstaddr, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), etyPot, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), nvmoffset, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), erasesize, sizeof(unsigned int));
            memcpy(ptr += sizeof(unsigned int), &erasenvm, sizeof(unsigned char));
            memcpy(ptr += sizeof(erasenvm), &eraseonly, sizeof(unsigned char));
            memcpy(ptr += sizeof(eraseonly), &programonly, sizeof(unsigned char));
            memcpy(ptr += sizeof(programonly), reversed, 5);
            return data;
        }
    private:
        char* RS( char* data, size_t size)
        {
            if(isLittleEndian())
            {
                std::reverse(data, data + size);
            }
            return  data;
        };
    };
    struct app_fw_info
    {
        unsigned int  fw_size;
        unsigned int  fw_crc32;
        unsigned int  destaddr;
        unsigned char erasenvm;
        unsigned char reversed[3];
        /*
        app_fw_info()
        {
            destaddr = 0x00000000u;
            erasenvm = 0x01u;
            memset(reversed, 0, sizeof(reversed));
        }
        */
        char* Get_chr(uint32_t fwsize, uint32_t fwcrc32)
        {
            char* data = new char[16];
            fw_size = fwsize;
            fw_crc32 = fwcrc32;
            char* fsize = RS(reinterpret_cast<char*>(&fwsize), sizeof(fwsize));
            char* fcrc32 = RS(reinterpret_cast<char*>(&fwcrc32), sizeof(fwcrc32));
            char* dstaddr = RS(reinterpret_cast<char*>(&destaddr), sizeof(destaddr));
            char* ernvm = RS(reinterpret_cast<char*>(&erasenvm), sizeof(erasenvm));
            char* reserv = RS(reinterpret_cast<char*>(&reversed), sizeof(reversed));
            char* ptr = nullptr;
            memcpy(ptr = data, fsize, sizeof(fwsize));
            memcpy(ptr += sizeof(fwsize), fcrc32, sizeof(fwcrc32));
            memcpy(ptr += sizeof(fwcrc32), dstaddr, sizeof(destaddr));
            memcpy(ptr += sizeof(destaddr), ernvm, sizeof(erasenvm));
            memcpy(ptr += sizeof(erasenvm), reserv, sizeof(reversed));

            delete  fsize;
            delete  fcrc32;
            delete  dstaddr;
            delete  ernvm;
            delete  reserv;
            return data;
        }
        /*
        app_fw_info& operator=(const app_fw_info& info)
        {
            fw_size = info.fw_size;
            fw_crc32 = info.fw_crc32;
            destaddr = info.destaddr;
            erasenvm = info.erasenvm;
            memcpy(reversed, info.reversed, 3);
            return *this;
        }
        */
    private:
        char* RS(const char* data, size_t size)
        {
            char* rdata = new char[size];
            for (uint32_t i = 0; i < size; i++)
            {
                rdata[i] = data[size - i - 1];
            }
            return  rdata;
        };
    };

    struct msg_pkg
    {
        unsigned char   header;
        ClassID         class_id;
        MsgID           msg_id;
        unsigned short  payload_length;
        char*           payload;
        unsigned int    crc32;
        unsigned char   tail;

        std::string data()
        {
            int paylen = payload_length;
            int fixsize = 10;
            std::size_t size = fixsize + paylen;
            char* tempdata = new char[size] {0};
            char* data = tempdata;
            memcpy(data, &header, 1);              data = data + 1;
            memcpy(data, &class_id, 1);            data = data + 1;
            memcpy(data, &msg_id, 1);              data = data + 1;
            char* payload_lengthcs =isLittleEndian()? QG_reverse<unsigned short,char>(payload_length):reinterpret_cast<char*>(&payload_length);//to bigend
            memcpy(data, payload_lengthcs, 2);     data = data + 2;
            if(payload)
            {
                memcpy(data, payload, paylen); data = data + paylen;
            }
            //calculate crc32
            int crc_data_size = 4 + paylen;
            char* crcdatatemp = new char[crc_data_size] {0};
            char* crcdata = crcdatatemp;
            memcpy(crcdata, &class_id, 1);            crcdata = crcdata + 1;
            memcpy(crcdata, &msg_id, 1);              crcdata = crcdata + 1;
            memcpy(crcdata, payload_lengthcs, 2);     crcdata = crcdata + 2;
            if (payload)
            { 
                memcpy(crcdata, payload, paylen); crcdata = crcdata + paylen;
            }
            unsigned int crc32_=crc32_eval(0, crcdatatemp, crc_data_size);
            crc32 = crc32_;
            delete[] crcdatatemp;
            crcdatatemp = nullptr;
            char* crc32cs = isLittleEndian() ? QG_reverse<unsigned int, char>(crc32_): reinterpret_cast<char*>(&crc32_);
            memcpy(data, crc32cs, 4); data = data + 4;
            tail = 0x55;//fix 0x55
            memcpy(data, &tail, 1); 
            return std::string{ tempdata, size };
        }
    };
    #pragma pack(1)
    struct msg_pkg_response
    {
        ClassID         class_id;
        MsgID           msg_id;
        unsigned short  status;
        unsigned char   major;
        unsigned char   minor;
        unsigned char   build;
    };
    #pragma pack()

    ///
    void default_fw_info( gnss_fw_info& expert_config);
    void default_fw_info( app_fw_info& expert_config);

    ///upgrade process
    bool connect_module(unsigned int downloadbaudrate);
    bool synchronize_module(unsigned int totaltimeout, unsigned int intervaltime);
    bool query_the_bootloaer_version(ClassID id, unsigned int timeout, msg_pkg_response& pkgresponse);
    bool send_firmware_address(ClassID id, unsigned int timeout);
    bool send_Firmware_Information(ClassID id, char* payload, unsigned short payloadsize, unsigned int timeout);
    bool send_Start_Firmware_Upgrade_Command(ClassID id, unsigned int timeout);
    bool send_Firmware(ClassID id, char* fwdata, unsigned int fwsize, unsigned int timeout, unsigned int pkgsize = 5000/*bytes*/);

private:

    //Synchronize Module constexpr value
    unsigned int sync_word[2]{ 0x514C1309,0x1203A504 };
    unsigned int rsp_word [2]{ 0xAAFC3A4D,0x55FD5BA0 };
    constexpr static int response_size = 14;
    std::map<unsigned short, const char*> Response_Status
    { 
        {0x0000,"Message received and executed successfully"}, 
        {0x0001,"Unknow error"},
        {0x0002,"CRC32 checksum error"},
        {0x0003,"Timeout"},
        {0x0004,"Unsupported message."},
        {0x0005,"Message package error"},
        {0x0010,"GNSS synchronization error"},
        {0x0011,"GNSS change baud rate error."},
        {0x0012,"GNSS Bootloader error"},
        {0x0013,"GNSS firmware information error"},
        {0x0014,"GNSS firmware upgrade error"},
        {0x0020,"APP firmware area erase error"},
        {0x0021,"APP firmware write to Flash error"},
    };

 private:

     ///process name
     const char* connect_module_name = "Connect..."  ;
     const char* synchronize_module_name = "Synchronize..."  ;
     const char* query_the_bootloaer_version_name = "Query Bootloader Version..."  ;
     const char* send_firmware_address_name = "Send Firmware Address..."  ;
     const char* Send_Firmware_Information_name = "Send Firmware Information..."  ;
     const char* Send_Start_Firmware_Upgrade_Command_name = "Send Firmware Upgrade Command..."  ;
     const char* Send_Firmware_name = "Send Firmware..."  ;
};  
#endif
