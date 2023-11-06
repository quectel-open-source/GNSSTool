#ifndef RTCM3PARSER_H
#define RTCM3PARSER_H
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include "RTKLib/rtklib.h"
#include "DataParser/extended_rtklib.h"
#include "vector"
#include "queue"
#include "string"
#include "QQueue"

#define Q_FREQ_OFFSET  100
#define Q_RESULT_BUFFER_SIZE 1024*40

using namespace std;

typedef struct
{
    int sat;
    const char * freq;
    double SNR;
    int sigID;
} satFreqSig;

extern char g_str[Q_RESULT_BUFFER_SIZE];
extern rtcm_t rtcm;

void initRTCM3Parser();
const char * RTCM2NMEA(QQueue<QByteArray> &RTCM_Sets_Q);
bool IsParseComplete();
void createRTCM2NMEA_LogFile(QString portName = "None");

void clearSatData();

#endif // RTCM3PARSER_H
