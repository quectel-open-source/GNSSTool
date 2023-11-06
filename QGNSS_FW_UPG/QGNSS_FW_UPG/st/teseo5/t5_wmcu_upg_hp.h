#ifndef __ST_TESEO5_T5_WMCU_UPG_UP__
#define __ST_TESEO5_T5_MCU_UPG_UP__

#include <optional>
#include <fstream>
#include "ST8100_wmcu_fwupg.h"

/*
usage  desc                                           choice                          sample

-t     ClassID                                       GNSS|APP|SGNSS|SAPP
-q     enable quiet mode                             TRUE|FALSE                       FALSE
-f     binary file                                   filename
-c     COM port name                                  
-b     COM port baud rate          
-l     Execute a reset on                            RTS|DTR|RTSDTR                   RTS=500
       RTS and/or DTR 
       before operate in milliseconds

-d     absolute destination address  Hex format.
-e     Entry point       Hex format.
-o     NVM Offset        Hex format.
-s     NVM Erase Size    Hex format.
-r     EraseOnly         Hex format.
-p     ProgramOnly       Hex format.
*/

QG_PORTEXTERN class t5_wmcu_upg
{
public:
    //params
    struct t5_upg_params
    {
        std::optional< unsigned int>  destaddr;
        std::optional< unsigned int>  entrypoint;
        std::optional< unsigned int>  nvm_offset;
        std::optional< unsigned int>  nvm_erase_size;
        std::optional< unsigned int>  erasenvm;
        std::optional< unsigned int>  eraseonly;
        std::optional< unsigned int>  programonly;
        std::optional < ST8100_wmcu_fwupg::ClassID> classid_;
        std::optional <bool>    quit_mode;
        std::optional < std::string> file_path;
         SerialPortInfo* serialportinfo=nullptr;
    };
    //step1
    bool params_parse(int args, char* arg[]);
    //or step1 
    bool set_params(const t5_upg_params &params);
    //step2
    bool exec();
   //
    void stop();
    t5_wmcu_upg();
    ~t5_wmcu_upg();

private:

    bool get_fw_infor_payload(ST8100_wmcu_fwupg::ClassID id, std::vector<char>& payload);

private:
    t5_upg_params params_;
    ST8100_wmcu_fwupg t5_upg_;
    std::string firmware_data_;

    unsigned  int        sync_total_time     = 10000;
    unsigned  int        sync_interval       = 500;
    unsigned  int        max_read_timeout    = 20000;
    unsigned  int        upgcmd_read_timeout = 60000*2;
    constexpr static int per_pkg_size        = 0x1400;
};  

#endif // !__ST_TESEO5_T5_WMCU_UPG_UP__
