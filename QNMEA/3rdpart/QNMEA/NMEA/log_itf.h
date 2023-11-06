/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         log_itf.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-14      victor.gong
* ***************************************************************************/
#ifndef LOG_ITF_H
#define LOG_ITF_H

#include "qgnss_nmea_parse.h"

class QNMEA_EXTERN LOG_ITF
{
public:
    LOG_ITF();
    ~LOG_ITF();

    //############### Stream Read#############################
    void STR_NMEA_Fliter_ini();
   // bool STR_NMEA_Fliter(const char *Sentence, size_t SentenceSize,  NMEA_DT &NMEA_DT);
    //#########################################################
    void print_Ct();
private:


    void RD_Clear();

    QGNSS_NMEA_Parse *nmea_parse_=nullptr;

    char *logbuf=nullptr;
    int streamsize=1024*1024*10;//100m perRead
};

#endif // LOG_ITF_H
