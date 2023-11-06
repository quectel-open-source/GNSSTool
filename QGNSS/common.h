#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QByteArray>

#define SETTINGS_FILE_NAME "./Config.ini"
#define TianDiMapKey "TianDiMapKey"
#define parseNMEA_Key "Parse_NMEA"
#define parseRTCM_Key "Parse_RTCM"
#define parseOther "Parse_Other"
#define parseMTK_Key "Parse_M"
#define parseBroad_Key "Parse_B"
#define parseCASIC_Key "Parse_C"
#define parseP1_Key "Parse_P"
#define LogType_Key "LogType"
#define MAX_Log_Key "MAXLog"
#define DailyTime_Key "DailyTime"

namespace quectel_gnss
{
Q_NAMESPACE

enum  ProtocolType{
  Unknown = 0,
  NMEA_RMC,
  NMEA_GGA,
  NMEA_GNS,
  RTCM
};
Q_ENUM_NS(ProtocolType);

enum RestartType
{
  Cold = 0x0001,
  Warm = 0x0002,
  Hot = 0x0003,
  FullCold = 0x0004
};

enum QGNSS_Model
{
  NONE,
  /*****************MTK AG3331************************************/
  L76_LB,
  L26_LB,
  LC86L,
  LG77L,
  /*****************MTK MT3333************************************/
  L76,
  L76_L,
  L86,
  L26,
  L26B,
  L76B,
  L96,
  L86LIC,
  LG77LIC,
  /*****************MTK MT3337************************************/
  L70_R,
  L80_R,
  /*****************MTK MT3339************************************/
  L70,
  L80,
  /******************MTK AG3352***********************************/
  LC26G,
  LC26GTAA,
  LC76G,
  LC86G,
  /******************MTK AG3335MN*********************************/
  L89HA,
  LC79HBA,
  /******************MTK AG3335M***********************************/
  LC79HAA,
  LC79HAL,
  LC29HAL,
  LC29HAA,
  /******************MTK AG3335AT**********************************/

  LC02HAA,
  LC02HBA,
  LC29HBA,
  LC29HCA,
  LC29HCA_TB,
  LC29HDA,
  /******************MTK AG3335A************************************/
  LC29HEA,
  // Broadcom
  /******************Broadcom BCM47755*******************************/
  LC29DC,
  LC79DA,
  LC79DC,
  LC79DD,
  LC79DE,
  /******************Broadcom BCM47758*******************************/
  LC29DA,
  /******************Broadcom BCM47765*******************************/
  LC29DB,
  LC29DE,
  LC29DF,
  LC29DG,
  LC79DF,
  // ST
  /******************ST STA8090+STA5635******************************/
  L89,
  /******************ST STA8089**************************************/
  L26ADR,
  L26ADRA,
  L26DRAA,
  L26ADRC,
  L26T,
  L26T_RD,
  L26TB,
  L26P,
  L26UDR,
  LC98S,
  /******************ST STA8100*********************************************/
  LG69TAA,
  LG69TAD,
  LG69TAI,
  LG69TAJ,
  LG69TAK,

  LG69TAQ,
  LG69TAM,
  LG69TAP,
  LG69TAS,

  LG69TAH,
  LC29TAA,
  LC99TIA,
  /******************ST STA9100**********************************************/

  LG69TAB,

  // ST
  /******************ST STA8090+STA5635******************************/

  /******************ST STA8089**************************************/

  /******************ST STA8100*********************************************/

  /******************ST STA9100**********************************************/

  /******************Unicorecomm UC6226NIS*************************************/
  L76C,
  L26C,
  //    /******************Unicorecomm UC6226NIK*************************************/
  //    L26CB,
  //    /******************Unicorecomm UC6226CI**************************************/
  //    L26CA,
  //    /******************Unicorecomm UC6228CI*************************************/
  //    L76CC,
  //    L26CC,
  //Allystar
  /*********************** HD8040D ******************************************/
  LC29YIA,
  LC760Z,
  LC260Z,
  //中科微
  /******************Unicorecomm AT6558R**************************************/
  L76KA,
  L26KA,
  LC03K,
  /******************Unicorecomm AT6558R(新版)*********************************/
  L76KB,
  L26KB,
  // Goke
  /******************Goke GK9501**********************************************/
  LC76F


};
Q_ENUM_NS(QGNSS_Model);
}

void qgnss_sleep(int delay_time);
void Delay_MSec_Suspend(int msec);
void utc_to_gpstime(uint32_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, int32_t *wn, double *tow);
int addNMEA_CS(QByteArray &NMEA_Ste);
int addNMEA_CS(QString &NMEA_Ste);
int addNMEA_MC_CS(QByteArray &NMEA_Ste);
int addNMEA_MC_CS(QString &NMEA_Ste);
int add_CS(QByteArray &NMEA_Ste, int start_p);
int addAllystarBinaryCS(QByteArray &msg);
bool intInRange(int min, int max, int n);
// extern unsigned int xcrc32 (const unsigned char *, int, unsigned int init = 0xffffffff);
unsigned int c_crc32(const unsigned char *buffer, unsigned int size, unsigned int crc = 0xffffffff);

#endif // COMMON_H
