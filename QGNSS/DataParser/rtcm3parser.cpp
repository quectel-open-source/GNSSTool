#include "rtcm3parser.h"
#include "QtDebug"
#include "QFile"
#include "QDateTime"


//customData_t g_customData;
//char lg96taa_version[64] = {0};
string loggerName;
char g_str[Q_RESULT_BUFFER_SIZE];
static char *p = g_str;
rtcm_t rtcm;
static int ret = 0;
static vector<satFreqSig> GPS_DATA[NFREQ+NEXOBS]; //GPS
static vector<satFreqSig> SBS_DATA[NFREQ+NEXOBS]; //SBSA
static vector<satFreqSig> GLO_DATA[NFREQ+NEXOBS]; //Glonass
static vector<satFreqSig> GAL_DATA[NFREQ+NEXOBS]; //Galileo
static vector<satFreqSig> QZS_DATA[NFREQ+NEXOBS]; //QZSS
static vector<satFreqSig> CMP_DATA[NFREQ+NEXOBS]; //BeiDou
static vector<satFreqSig> IRN_DATA[NFREQ+NEXOBS]; //Indian Regional Navigation Satellite System

void initRTCM3Parser()
{
    init_rtcm(&rtcm);
    memset(&g_customData,0,sizeof(customData_t));
//    traceopen("./logFile/rtklib.log");   //RTKlib  trace switch
//    tracelevel(3);
}

void satFreqSig2nmea_str(vector<satFreqSig> aFS_data[],const char *gsvHead){
    vector<satFreqSig>::iterator sData;
    char *q,*s = p,sum;
    int gsv_n = 0,i = 0,j = 0,freq = 0;
    for(freq = 0; freq < NFREQ+NEXOBS; freq++){
        if(aFS_data[freq].size()>0){
            gsv_n = (aFS_data[0].size()+aFS_data[1].size()+aFS_data[2].size() - 1) / 4 + 1;
            for (sData = aFS_data[freq].begin(),j = 1; sData != aFS_data[freq].end(); ++sData,++j) {
                if(j%4 == 1){
                    s=p;
                    p+=sprintf(p,"$%s,%d,%d,%02d",gsvHead,gsv_n,i+1,aFS_data[0].size()+aFS_data[1].size()+aFS_data[2].size());
                    i++;
                }
                p+=sprintf(p,",%02d,,,%02.0f", sData->sat, sData->SNR);
                if(j%4 == 0)
                {
                    p+=sprintf(p,",%d",sData->sigID);
                    for (q=s+1,sum=0;*q;q++) sum^=*q; /* check-sum */
                    p+=sprintf(p,"*%02X%c%c ",sum,0x0D,0x0A);
                }
            }
            j--;
            for(;j%4 != 0;){
                p+=sprintf(p,",,,,");
                ++j;
                if(j%4 == 0)
                {
                    p+=sprintf(p,",%d",aFS_data[freq].at(0).sigID);
                    for (q=s+1,sum=0;*q;q++) sum^=*q; /* check-sum */
                    p+=sprintf(p,"*%02X%c%c ",sum,0x0D,0x0A);
                    break;
                }
            }
        }
    }
}

void ggaData2nmea_str(void)
{
    char *q, *s = p, sum;
    int dd, ddd;
    double mm, mmm, lat, lon;
    if (st_HAP.epvt.GPS_Quality > 0)
    {
        lat = fabs(st_HAP.epvt.Latitude / 1000.0 / 3600);
        lon = fabs(st_HAP.epvt.Longitude / 1000.0 / 3600);
        dd = static_cast<int>(lat);
        ddd = static_cast<int>(lon);
        mm = (lat - dd) * 60;
        mmm = (lon - ddd) * 60;
        gtime_t time = gpst2time((int)st_HAP.epvt.ExtendedWeekNumber, st_HAP.epvt.GNSS_EpochTime / 1000.0);
        time = gpst2utc(time);
        double utc[6];
        time2epoch(time, utc);
        p += sprintf(p, "$GPGGA,%02.0f%02.0f%05.2f,%02d%09.6f,%s,%03d%09.6f,%s,%d,%02d,%.1f,%.1f,M,,M,,",
                     utc[3], utc[4], utc[5],
                     dd, mm, st_HAP.epvt.Latitude >= 0 ? "N" : "S",
                     ddd, mmm, st_HAP.epvt.Longitude >= 0 ? "E" : "W",
                     st_HAP.epvt.GPS_Quality, st_HAP.epvt.satellitesInUse, st_HAP.epvt.HDOP/10.0,
                     st_HAP.epvt.Height/10.0);
    }
    else
    {
        p += sprintf(p, "$GPGGA,,,,,,%d,,,,M,,M,,", st_HAP.epvt.GPS_Quality);
    }
    for (q = s + 1, sum = 0; *q; q++)
        sum ^= *q; /* check-sum */
    p += sprintf(p, "*%02X%c%c ", sum, 0x0D, 0x0A);
}

void generate_nmea_gsv()
{
    satFreqSig2nmea_str(GPS_DATA, "GPGSV");
    satFreqSig2nmea_str(GLO_DATA, "GLGSV");
    satFreqSig2nmea_str(GAL_DATA, "GAGSV");
    satFreqSig2nmea_str(QZS_DATA, "GQGSV");
    satFreqSig2nmea_str(CMP_DATA, "GBGSV");
    satFreqSig2nmea_str(IRN_DATA, "GIGSV");
    ggaData2nmea_str();
}

bool IsParseComplete()
{
    return ret == 1;
}

void clearSatData()
{
    for(int i = 0; i < NFREQ+NEXOBS; i++)
    {
        GPS_DATA[i].clear();
        SBS_DATA[i].clear();
        GLO_DATA[i].clear();
        GAL_DATA[i].clear();
        QZS_DATA[i].clear();
        CMP_DATA[i].clear();
        IRN_DATA[i].clear();
    }
}

//const char * RTCM2NMEA(QQueue<QByteArray> &RTCM_Sets_Q)
//{
//    QByteArray RTCM_Set;
//    while (!RTCM_Sets_Q.empty())
//    {
//        RTCM_Set = RTCM_Sets_Q.takeFirst();
//        foreach (char c, RTCM_Set) {
//            ret = input_rtcm3(&rtcm, static_cast<unsigned char>(c));
//            if (ret == 1)
//            {
//                clearSatData();
//                for (int i = 0; i < rtcm.obs.n; i++)
//                {
//                    for (int j = 0; j < 5; j++)
//                    {
//                        unsigned char code = 0;
//                        int snr = rtcm.obs.data[i].SNR[j];
//                        char id[8] = {0};
//                        char *freq;
//                        int x = 0,prn;
//                        if (snr)
//                        {
//                            satno2id(rtcm.obs.data[i].sat, id);
//                            code  =  rtcm.obs.data[i].code[j];
////                            freq = code2obs(rtcm.obs.data[i].code[j], &x);
//                            freq = code2obs(rtcm.obs.data[i].code[j]);
//                            satsys(rtcm.obs.data[i].sat,&prn);
//                            satFreqSig aFS = {prn, "0", snr * 0.001};
//                            switch (id[0])
//                            {
//                            case 'G':
//                            {
//                                if(code>CODE_NONE&&MAXCODE>=code)
//                                    aFS.freq = gpsgsvcodes[code];
//                                GPS_DATA[j].push_back(aFS);
//                                break;
//                            }
//                            case 'R':
//                            {
//                                if(code>CODE_NONE&&MAXCODE>=code)
//                                    aFS.freq = glogsvcodes[code];
//                                GLO_DATA[j].push_back(aFS);
//                                break;
//                            }
//                            case 'E':
//                            {
//                                if(code>CODE_NONE&&MAXCODE>=code)
//                                    aFS.freq = galgsvcodes[code];
//                                GAL_DATA[j].push_back(aFS);
//                                break;
//                            }
//                            case 'J':
//                            {
//                                if(code>CODE_NONE&&MAXCODE>=code)
//                                    aFS.freq = qzssgsvcodes[code];
//                                QZS_DATA[j].push_back(aFS);
//                                break;
//                            }
//                            case 'C':
//                            {
//                                if(code>CODE_NONE&&MAXCODE>=code)
//                                    aFS.freq = bdsgsvcodes[code];
//                                CMP_DATA[j].push_back(aFS);
//                                break;
//                            }
//                            }
//                        }
//                    }
//                }
//                generate_nmea_gsv();
//                if(convtLog.open(QIODevice::Append)){
//                    convtLog.write(g_str);
//                    convtLog.close();
//                }
//                return g_str;
//            }
//        }
//    //
//    }
//    return "None";
//}


const char * RTCM2NMEA(QQueue<QByteArray> &RTCM_Sets_Q)
{
    QByteArray RTCM_Set;
    p = g_str;
    memset(g_str,0,sizeof (g_str));
    while (!RTCM_Sets_Q.empty())
    {
        RTCM_Set = RTCM_Sets_Q.takeFirst();
        foreach (char c, RTCM_Set) {
            ret = input_rtcm3(&rtcm, static_cast<unsigned char>(c));
            if (ret == 1)
            {
                clearSatData();
                for (int i = 0; i < rtcm.obs.n; i++)
                {
                    for (int j = 0; j < NFREQ+NEXOBS; j++)
                    {
                        unsigned char code = 0;
                        int snr = rtcm.obs.data[i].SNR[j];
                        char id[8] = {0};
                        if (snr)
                        {
                            satno2id(rtcm.obs.data[i].sat, id);
                            code  =  rtcm.obs.data[i].code[j];
                            QString num(&id[1]);
                            satFreqSig aFS = {num.toInt(), "0", snr * 0.001, code + Q_FREQ_OFFSET};
                            switch (id[0])
                            {
                            case 'G':
                            {
                                GPS_DATA[j].push_back(aFS);
                                break;
                            }
                            case 'R':
                            {
                                GLO_DATA[j].push_back(aFS);
                                break;
                            }
                            case 'E':
                            {
                                GAL_DATA[j].push_back(aFS);
                                break;
                            }
                            case 'J':
                            {
                                QZS_DATA[j].push_back(aFS);
                                break;
                            }
                            case 'C':
                            {
                                CMP_DATA[j].push_back(aFS);
                                break;
                            }
                            case 'I':
                            {
                                IRN_DATA[j].push_back(aFS);
                                break;
                            }
                            }
                        }
                    }
                }
                generate_nmea_gsv();
//                if(convtLog.open(QIODevice::Append)){
//                    QDateTime curTime = QDateTime::currentDateTime();
//                    convtLog.write(curTime.toString("[MM-dd hh:mm:ss]").toLocal8Bit());
//                    convtLog.write(g_str);
//                    convtLog.close();
//                }
                spdlog::get(loggerName)->info(g_str);
                if(strlen(g_str) > Q_RESULT_BUFFER_SIZE - 1024){
                    qDebug()<<"Insufficient cache for RTCM2NMEA:" << strlen(g_str);
                    return "None";
                }
            }
        }
    }
    if(strlen(g_str)){
        return g_str;
    }
    return "None";
}
void createRTCM2NMEA_LogFile(QString portName)
{
    QString fileName = QDateTime::currentDateTime().toString("_MMdd_hhmmss_") + portName;
    loggerName = fileName.toStdString();
    fileName = "./logFile/RTCM2NMEA"+fileName+".log";
    auto some_logger = spdlog::basic_logger_mt(loggerName, fileName.toStdString());
    some_logger->set_pattern("[%d  %H:%M:%S.%e]\r\n %v");
//    convtLog.setFileName("./logFile/RTCM2NMEA"+time+".log");
}
