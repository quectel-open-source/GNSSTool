#ifndef EXTENDED_RTKLIB_H
#define EXTENDED_RTKLIB_H
#include <stdint.h>
#include "RTKLib/rtklib.h"

//typedef struct{
//    int type;
//    QByteArray content;
//}RTCM_Info_t;

#ifdef __cplusplus
extern "C"
{
#endif

//Receiver Status and Safety
typedef struct{
    uint32_t _n;
    uint32_t TOW;
    uint16_t weekNum;
    uint8_t LeapSeconds;
    uint8_t safelyInfo;
    uint8_t protocolVersionFlags;
    uint32_t FirmwareVersion;
    uint8_t ppsStatus;
    uint8_t TimeValidity;
    uint32_t constellationAlarmMask;
    uint32_t GNSS_ConstellationMask;
    uint32_t Multi_FrequencyConstellationMask;
    uint32_t NCO_clockDrift;
} st_rss_t;

//Receiver Configuration and Control
typedef struct{
    uint32_t _n;
    uint16_t ResponseID;
    uint8_t ConfigPageNumber;
    uint8_t ContinueOnNextMessage;
    uint8_t CDB_WriteFlag;
    uint16_t ConfigPageMask;
    uint32_t word[16];
} st_rcc_t;

//Receiver PVT
typedef struct{
    uint32_t _n;
    uint16_t ReferenceStationID;
    uint8_t ReservedForITRF_RealizationYear;
    uint8_t GPS_Quality;
    uint8_t satellitesInUse;
    uint8_t satellitesInView;
    uint8_t HDOP;
    uint8_t VDOP;
    uint8_t PDOP;
    uint16_t GeoidalSeparation;
    uint32_t AgeOfDifferentials;
    uint16_t DifferentialReferenceStationID;
    uint8_t TimeID;
    uint32_t GNSS_EpochTime;
    uint16_t ExtendedWeekNumber;
    uint16_t LeapSeconds;
    int64_t AP_ECEF_X;
    int64_t AP_ECEF_Y;
    int64_t AP_ECEF_Z;
    int32_t AV_ECEF_X;
    int32_t AV_ECEF_Y;
    int32_t AV_ECEF_Z;
} st_pvt_t;

//Extended PVT
typedef struct{
    uint32_t _n;
    uint16_t ReferenceStationID;
    uint8_t ReservedForITRF_RealizationYear;
    uint8_t GPS_Quality;
    uint8_t DataStatus;
    uint8_t FixFrequencyMode;
    uint8_t FixIntegrity;
    uint8_t RFU;
    uint8_t satellitesInUse;
    uint8_t satellitesInView;
    uint8_t HDOP;
    uint8_t VDOP;
    uint8_t PDOP;
    int16_t GeoidalSeparation;
    uint32_t AgeOfDifferentials;
    uint16_t DifferentialReferenceStationID;
    uint8_t TimeID;
    uint8_t TimeValidity;
    uint32_t GNSS_EpochTime;
    uint16_t ExtendedWeekNumber;
    uint16_t LeapSeconds;
    int Latitude;
    int Longitude;
    int Height;
    int VelocityHorizontal;
    int VelocityVertical;
    uint16_t CourseAngle;
    uint16_t ProtectionLevelH;
    uint16_t ProtectionLevelV;
    uint16_t ProtectionLevelA;
    uint32_t ReceiverClockBias;
    uint32_t ReceiverClockDrift;
} st_epvt_t;

//Position Quality Metrics
typedef struct{
    uint32_t _n;
    uint32_t GNSS_EpochTime;
    uint8_t Time_ID;
    uint16_t Mask;
    uint32_t Value;
    uint8_t MultipleMessageIndicator;
    int32_t MetricsValue;
} st_posqm_t;

//Observable Quality Metrics
typedef struct{
    uint32_t _n;
    uint32_t TOW;
    uint8_t GNSS_ID;
    uint64_t GNSS_SatelliteMask;
    uint32_t MetricsMask;
    uint8_t MultipleMessageIndicator;
    int32_t MetricsValue;
} st_obsqm_t;



//Host Application Protocol
typedef struct{
    uint32_t _n;
    st_rss_t rss;
    st_rcc_t rcc;
    st_pvt_t pvt;
    st_epvt_t epvt;
} st_HAP_t;

extern st_HAP_t st_HAP;



typedef struct {
    uint32_t _n2,_n_resp,_n21;
    double utc[6];
    double lat;
    double lon;
    uint32_t quality;
    uint32_t u_sats;
    double hdop;
    double alt;
    double undulation;
    char lg96taa_version[64];
    uint32_t resp[1000][2];
} customData_t;

extern customData_t g_customData;
extern char lg96taa_version[64];

int decode_type999(rtcm_t *rtcm);
void SaveInfoToQueue(uint8_t *buff, int size);

#ifdef __cplusplus
}
#endif

#endif // EXTENDED_RTKLIB_H
