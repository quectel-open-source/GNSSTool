#pragma once

namespace qgnss_allstar_msg {
#pragma pack(push, 1)

struct AidingUTC{
    unsigned char leap_sec = 0;
    unsigned short year = 0;
    unsigned char month = 0;
    unsigned char day = 0;
    unsigned char hour = 0;
    unsigned char minute = 0;
    unsigned char second = 0;
    unsigned int sec_ns = 0;
    unsigned short Tacc_s = 0;
    unsigned int Tacc_ns = 0;
};

struct AidingPos{
    int lat = 0;
    int lon = 0;
    int alt = 0;
    unsigned int pos_acc = 0;
};


struct alignas(4) test{
    static constexpr int a = 0;
    int b = 0;
    char c;
};




#pragma pack(pop)

}


