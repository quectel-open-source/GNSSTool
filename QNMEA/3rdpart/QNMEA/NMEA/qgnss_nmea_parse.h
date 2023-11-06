/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qgnss_nmea_parse.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-08      victor.gong
* ***************************************************************************/

#ifndef QGNSS_NMEA_PARSE_H
#define QGNSS_NMEA_PARSE_H
#include "iostream"
#include "nmea_dt.h"
#include "nmea_base.h"
class QNMEA_EXTERN QGNSS_NMEA_Parse:public NMEA_Base
{

public:
    QGNSS_NMEA_Parse();
    ~QGNSS_NMEA_Parse();
    ///read file stream
    NMEA_DT* NMEA_Parse_Stream(const char *Sentence,size_t SentenceSize);
    void clear();
    size_t RMC_CT=0;
    size_t GGA_CT=0;
    size_t GNS_CT=0;
    size_t GSA_CT=0;
    size_t GSV_CT=0;
    size_t VTG_CT=0;
    size_t GLL_CT=0;
    char * remindbuff=nullptr;

    void print_Ct()
    {
        std::cout<<"RMC_CT "<<RMC_CT<<std::endl;
        std::cout<<"GGA_CT "<<GGA_CT<<std::endl;
        std::cout<<"GNS_CT "<<GNS_CT<<std::endl;
        std::cout<<"GSA_CT "<<GSA_CT<<std::endl;
        std::cout<<"GSV_CT "<<GSV_CT<<std::endl;
        std::cout<<"VTG_CT "<<VTG_CT<<std::endl;
        std::cout<<"GLL_CT "<<GLL_CT<<std::endl;

        std::cout<<"Cout "         <<RMC_CT
                                    +GGA_CT
                                    +GNS_CT
                                    +GSA_CT
                                    +GSV_CT
                                    +VTG_CT
                                    +GLL_CT
                  <<std::endl;

        RMC_CT=0;
        GGA_CT=0;
        GNS_CT=0;
        GSA_CT=0;
        GSV_CT=0;
        VTG_CT=0;
        GLL_CT=0;

    }
private:
        NMEA_DT *NMEADT=nullptr;
};

#endif // QGNSS_NMEA_PARSE_H
