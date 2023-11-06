#include "extended_rtklib.h"
#include "QQueue"

st_HAP_t st_HAP = st_HAP_t();
customData_t g_customData;
char lg96taa_version[64] = {0};
QQueue<QByteArray> g_rtcm3_queue;

int decode_type999(rtcm_t *rtcm)
{
    gtime_t time;
    unsigned int   secondsOfWeek,week;
    int Latitude,longitude,height;
    unsigned int subType = getbitu(rtcm->buff,36,8);
    st_HAP._n++;
    switch (subType)
    {
    case 1: {
        st_HAP.rss._n++;
        st_HAP.rss.TOW = getbitu(rtcm->buff,24+20, 30);
        st_HAP.rss.weekNum = getbitu(rtcm->buff,24+50, 16);
        st_HAP.rss.LeapSeconds = getbitu(rtcm->buff,24+66, 8);
        st_HAP.rss.safelyInfo = getbitu(rtcm->buff,24+74, 1);
        st_HAP.rss.protocolVersionFlags = getbitu(rtcm->buff,24+75, 7);
        st_HAP.rss.FirmwareVersion = getbitu(rtcm->buff,24+82, 24);
        st_HAP.rss.ppsStatus = getbitu(rtcm->buff,24+130, 8);
        st_HAP.rss.TimeValidity = getbitu(rtcm->buff,24+130, 8);
        st_HAP.rss.constellationAlarmMask = getbitu(rtcm->buff,24+142, 32);
        st_HAP.rss.GNSS_ConstellationMask = getbitu(rtcm->buff,24+206, 32);
        st_HAP.rss.Multi_FrequencyConstellationMask = getbitu(rtcm->buff,24+206, 32);
        st_HAP.rss.NCO_clockDrift = getbitu(rtcm->buff,24+206, 32);
        break;
    }
    case 2: {
        st_HAP.rcc._n++;
        st_HAP.rcc.ResponseID = getbitu(rtcm->buff,24+20, 10);
        st_HAP.rcc.ConfigPageNumber = getbitu(rtcm->buff,24+32, 8);
        st_HAP.rcc.ContinueOnNextMessage = getbitu(rtcm->buff,24+40, 1);
        st_HAP.rcc.CDB_WriteFlag = getbitu(rtcm->buff,24+41, 1);
        st_HAP.rcc.ConfigPageMask = getbitu(rtcm->buff,24+42, 16);
        uint16_t mask = st_HAP.rcc.ConfigPageMask;
        for(int i=0,j=0; i<16; i++){
            if(mask>>i & 1u){
                st_HAP.rcc.word[i] = getbitu(rtcm->buff,24+58+j*32, 32);
                j++;
            }
            else{
                st_HAP.rcc.word[i] = 0;
            }
        }

        if(st_HAP.rcc.ConfigPageNumber == 63){
            unsigned int page,index = 0;
            for(int i = 0; i<16; i++){
                page = getbitu(rtcm->buff,24+58+i*32, 32);
                if(page){
                    for(int j = 0;j<4;j++){
                        g_customData.lg96taa_version[index++] = page&0xff;
                        page >>= 8;
                    }
                }
                else{
                    break;
                }
            }
        }
        break;
    }
    case 4: {
        st_HAP.pvt._n++;
        st_HAP.pvt.ReferenceStationID = getbitu(rtcm->buff,24+20, 12);
        st_HAP.pvt.ReservedForITRF_RealizationYear = getbitu(rtcm->buff,24+32, 6);
        st_HAP.pvt.GPS_Quality = getbitu(rtcm->buff,24+38, 4);
        st_HAP.pvt.satellitesInUse = getbitu(rtcm->buff,24+42, 8);
        st_HAP.pvt.satellitesInView = getbitu(rtcm->buff,24+50, 8);
        st_HAP.pvt.HDOP = getbitu(rtcm->buff,24+58, 8);
        st_HAP.pvt.VDOP = getbitu(rtcm->buff,24+66, 8);
        st_HAP.pvt.PDOP = getbitu(rtcm->buff,24+74, 8);
        st_HAP.pvt.GeoidalSeparation = getbitu(rtcm->buff,24+82, 15);
        st_HAP.pvt.AgeOfDifferentials = getbitu(rtcm->buff,24+97, 24);
        st_HAP.pvt.DifferentialReferenceStationID = getbitu(rtcm->buff,24+121, 12);
        st_HAP.pvt.TimeID = getbitu(rtcm->buff,24+133, 4);
        st_HAP.pvt.GNSS_EpochTime = getbitu(rtcm->buff,24+137, 30);
        st_HAP.pvt.ExtendedWeekNumber = getbitu(rtcm->buff,24+167, 16);
        st_HAP.pvt.LeapSeconds = getbitu(rtcm->buff,24+183, 8);
//        st_HAP.pvt.AP_ECEF_X = getbitu(rtcm->buff,24+191, 38);
//        st_HAP.pvt.AP_ECEF_Y = getbitu(rtcm->buff,24+229, 38);
//        st_HAP.pvt.AP_ECEF_Z = getbitu(rtcm->buff,24+267, 38);
        st_HAP.pvt.AV_ECEF_X = getbits(rtcm->buff,24+305, 32);
        st_HAP.pvt.AV_ECEF_Y = getbits(rtcm->buff,24+337, 32);
        st_HAP.pvt.AV_ECEF_Z = getbits(rtcm->buff,24+369, 32);
        break;
    }
    case 21: {
        st_HAP.epvt._n++;
        st_HAP.epvt.ReferenceStationID = getbitu(rtcm->buff,24+20, 12);
        st_HAP.epvt.ReservedForITRF_RealizationYear = getbitu(rtcm->buff,24+32, 6);
        st_HAP.epvt.GPS_Quality = getbitu(rtcm->buff,24+38, 4);
        st_HAP.epvt.DataStatus = getbitu(rtcm->buff,24+42, 1);
        st_HAP.epvt.FixFrequencyMode = getbitu(rtcm->buff,24+43, 1);
        st_HAP.epvt.FixIntegrity = getbitu(rtcm->buff,24+44, 1);
        st_HAP.epvt.RFU = getbitu(rtcm->buff,24+45, 1);
        st_HAP.epvt.satellitesInUse = getbitu(rtcm->buff,24+46, 8);
        st_HAP.epvt.satellitesInView = getbitu(rtcm->buff,24+54, 8);
        st_HAP.epvt.HDOP = getbitu(rtcm->buff,24+62, 8);
        st_HAP.epvt.VDOP = getbitu(rtcm->buff,24+70, 8);
        st_HAP.epvt.PDOP = getbitu(rtcm->buff,24+78, 8);
        st_HAP.epvt.GeoidalSeparation = getbits(rtcm->buff,24+86, 15);
        st_HAP.epvt.AgeOfDifferentials = getbitu(rtcm->buff,24+101, 24);
        st_HAP.epvt.DifferentialReferenceStationID = getbitu(rtcm->buff,24+125, 12);
        st_HAP.epvt.TimeID = getbitu(rtcm->buff,24+137, 4);
        st_HAP.epvt.TimeValidity = getbitu(rtcm->buff,24+141, 4);
        st_HAP.epvt.GNSS_EpochTime = getbitu(rtcm->buff,24+145, 30);
        st_HAP.epvt.ExtendedWeekNumber = getbitu(rtcm->buff,24+175, 16);
        st_HAP.epvt.LeapSeconds = getbitu(rtcm->buff,24+191, 8);
        st_HAP.epvt.Latitude = getbits(rtcm->buff,24+199, 32);
        st_HAP.epvt.Longitude = getbits(rtcm->buff,24+231, 32);
        st_HAP.epvt.Height = getbits(rtcm->buff,24+263, 20);
        st_HAP.epvt.VelocityHorizontal = getbits(rtcm->buff,24+283, 20);
        st_HAP.epvt.VelocityVertical = getbits(rtcm->buff,24+303, 20);
        st_HAP.epvt.CourseAngle = getbitu(rtcm->buff,24+263, 16);
        st_HAP.epvt.ProtectionLevelH = getbitu(rtcm->buff,24+263, 16);
        st_HAP.epvt.ProtectionLevelV = getbitu(rtcm->buff,24+355, 16);
        st_HAP.epvt.ProtectionLevelA = getbitu(rtcm->buff,24+371, 16);
        st_HAP.epvt.ReceiverClockBias = getbitu(rtcm->buff,24+387, 32);
        st_HAP.epvt.ReceiverClockDrift = getbitu(rtcm->buff,24+419, 32);
        break;
    }
    case 19: {
        unsigned int  index = g_customData._n_resp%1000;
        if(index == 0) memset(g_customData.resp,0,sizeof (g_customData.resp));
        g_customData.resp[index][0] = getbitu(rtcm->buff,24+20, 10);
        g_customData.resp[index][1] = getbitu(rtcm->buff,24+30, 16);
        if(g_customData.resp[index][1] > 2) g_customData.resp[index][1] = 2;
        g_customData._n_resp++;
        break;
    }
//    case 21: {
//        g_customData._n21++;
//        g_customData.quality = getbitu(rtcm->buff,24+12+8+12+6, 4);
//        week = getbitu(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 +15+24+12+4+4+30,16);
//        secondsOfWeek = getbitu(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 +15+24+12+4+4,30);
//        time = gpst2time((int)week,secondsOfWeek/1000.0);
//        time2epoch(time,g_customData.utc);
//        Latitude = getbits(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 +15+24+12+4+4+30+16+8,32);
//        g_customData.lat = Latitude/1000.0/3600;
//        longitude = getbits(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 +15+24+12+4+4+30+16+8+32,32);
//        g_customData.lon = longitude/1000.0/3600;
//        height = getbits(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 +15+24+12+4+4+30+16+8+32+32,20);
//        g_customData.alt = height/10.0;
//        g_customData.hdop = getbits(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8,8)/10.0;
//        g_customData.u_sats = getbitu(rtcm->buff,24+12+8+12+6 +4+1+1+1+1,8);
//        g_customData.undulation = getbits(rtcm->buff,24+12+8+12+6 +4+1+1+1+1 +8+8+8+8+8 ,15)/100.0;
//        break;
//    }
    }
    return 0;
}

void SaveInfoToQueue(uint8_t *buff, int size){
    QByteArray sentence((char *)buff,size);
    if(g_rtcm3_queue.size() <  1000){

        g_rtcm3_queue.append(sentence);
    }else{

    }
}
