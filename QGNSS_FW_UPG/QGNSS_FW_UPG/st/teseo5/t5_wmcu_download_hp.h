#ifndef T5_WMCU_DOWNLOAD_HP_H
#define T5_WMCU_DOWNLOAD_HP_H

#include <optional>
#include <fstream>
#include <filesystem>
#include <functional>
#include "ST8100_USART_download.h"
#include "QG_Print.h"

QG_PORTEXTERN class t5_wmcu_download_hp
{
public:

    //params
    struct t5_dwd_params
    {
        std::optional <uint32_t> read_start_address= 0x08000000;
        std::optional <uint32_t> read_number_bytes= 0x0B;
        std::optional <uint32_t> write_start_address= 0x08000000;
        std::optional <uint32_t> erease_pages= 0x10;

        std::optional <bool>    quit_mode;
        std::optional < std::string> file_path;
        SerialPortInfo* serialportinfo=nullptr;
    };

    //step1
    bool params_parse(int args, char* arg[]);

    //or step1 
    ///TODO cmd parameter parser
    bool set_params(const t5_dwd_params& params);
    //step2
    bool exec();
        void stop();
    t5_wmcu_download_hp();
    ~t5_wmcu_download_hp();

private:

    unsigned  int        sync_total_time = 10000;
    unsigned  int        sync_interval = 200;
    unsigned  int        max_read_timeout = 60000;
    //default params
    //uint32_t read_start_address_  = 0x08000000;
    //uint32_t read_number_bytes_   = 0x0B;

    //uint32_t write_start_address_ = 0x08000000;
    //uint32_t erease_pages         = 0x10;

    constexpr static unsigned short per_pkg_size = 0x100;

    t5_dwd_params params_;
    ST8100_USART_download st_uart_dwd_;
    std::string fwdata_;

};

#endif
