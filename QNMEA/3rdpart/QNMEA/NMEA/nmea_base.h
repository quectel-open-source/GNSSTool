/****************************************************************************
 * Copyright(C) 2022-2022 *** All Rights Reserved
 * Name:         nmea_base.h
 * Description:
 * History:
 * Version Date                           Author          Description
 *         2022-07-07      victor.gong
 * ***************************************************************************/
#ifndef NMEA_BASE_H
#define NMEA_BASE_H
#define CSTR(X) (#X)
#include <ctype.h>
#include <array>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include "qnmea.h"
#include "qlog_msg.h"
#define  NULLVALUE -1

    class QNMEA_EXTERN NMEA_Base
{
 public:
  NMEA_Base();
  virtual ~NMEA_Base();
  enum NMEA_Sentence_ID
  {
    NMEA_SENTENCE_NO = -1,
    NMEA_SENTENCE_RMC,
    NMEA_SENTENCE_GGA,
    NMEA_SENTENCE_GNS,
    NMEA_SENTENCE_GSA,
    NMEA_SENTENCE_GSV,
    NMEA_SENTENCE_VTG,
    NMEA_SENTENCE_GLL,
    };
  constexpr static std::array<const char *, 7> sentence_idstrs
      {
          "RMC", "GGA", "GNS", "GSA", "GSV", "VTG", "GLL",
          };
  enum class NMEA_Check_Flag : char
  {
    NMEA_OK = -1,
    NMEACHECKSUMERROR,
    NMEA_NOT_CMP,  // not complete
    NMEA_LEN_TMIN,
    NMEA_LEN_TMAX,
    NMEA_TalkerID_ERROR,
    NMEA_SentenceID_ERROR,
    NMEA_ERROR
  };

      enum NMEA_TalkerID : char
  {
    ID_NOID = -1,
    ID_AI,    // Alarm Indicator, (AIS?)
    ID_GP,    // Global Positioning System receiver
    ID_GL,    // GLONASS, according to IEIC 61162-1
    ID_GA,    // Galileo Positioning System
    ID_GB,    // BeiDou (China)
    ID_QZ,    // QZSS regional GPS augmentation system (Japan)
    ID_GI,    // NavIC, IRNSS (India)
    ID_CD,    // Digital Selective Calling (DSC)
    ID_EC,    // Electronic Chart Display & Information System (ECDIS)
    ID_BD,    // BeiDou (China)
    ID_GN,    // Combination of multiple satellite systems (NMEA 1083)
    ID_GQ,    // QZSS regional GPS augmentation system (Japan)
    ID_HC,    // Heading/Compass
    ID_HE,    // Gyro, north seeking
    ID_II,    // Integrated Instrumentation
    ID_IN,    // Integrated Navigation
    ID_LC,    // Loran-C receiver (obsolete)
    ID_Pxxx,  // Proprietary (Vendor specific)
    ID_PQ,    // QZSS (Quectel Quirk)
    ID_SD,    // Depth Sounder
    ID_ST,    // Skytraq
    ID_TI,    // Turn Indicator
    ID_YX,    // Transducer
    ID_WI,    // Weather Instrument
    };
  constexpr static std::array<const char *, 24> talerid_strs
      {
          "AI", "GP", "GL", "GA", "GB", "QZ",   "GI", "CD", "EC", "BD", "GN", "GQ",
          "HC", "HE", "II", "IN", "LC", "Pxxx", "PQ", "SD", "ST", "TI", "YX", "WI",
      };

  enum class NMEA_System_ID : char
  {
    NO = -1,
    GPS = 1,
    ///俄罗斯
    GLONASS,
    ///欧盟
    Galileo,
    ///北斗
    BeiDou,
    ///日本
    QZSS,
    ///印度
    NAVIC,

    //SBAS

    ///GPS SBAS https://en.wikipedia.org/wiki/Wide_Area_Augmentation_System
    /// Atlantic Ocean Region-West 122 35
    /// Pacific Ocean Region (POR) 134 47
    /// Galaxy 15                  135 48
    /// Anik F1R                   138 51
    /// Inmarsat-4 F3              133 46
    /// Eutelsat 117 West B        131 44
    /// SES 15                     133 46
    /// Galaxy 30                  135 48
    /* Satellite name and details	PRN	NMEA	Designator	Location	Active Period (Not in Test Mode)	Status	Signal Capability
        Atlantic Ocean Region-West	122	35	AORW	54°W, later moved to 142°W[18]	July 10, 2003 – July 31, 2017	Ceased operational WAAS transmissions on July 31, 2017	L1 Narrowband
        Pacific Ocean Region (POR)	134	47	POR	178°E	July 10, 2003 – July 31, 2017	Ceased operational WAAS transmissions on July 31, 2017	L1
        Galaxy 15                   	135	48	CRW	133°W	November 2006 – July 25, 2019	Ceased operational WAAS transmissions on July 25, 2019.	L1, L5 (Test Mode)
        Anik F1R                  	138	51	CRE	107.3°W	July 2007 – May 17, 2022	Ceased operational WAAS transmissions on May 17, 2022.[17]	L1, L5 (Test Mode)
        Inmarsat-4 F3             	133	46	AMR	98°W	November 2010 – November 9, 2017	Ceased operational WAAS transmissions as of November 9, 2017.[19]	L1 Narrowband, L5 (Test Mode)
        Eutelsat 117 West B       	131	44	SM9	117°W	March 2018 – Present	Operational	L1, L5 (Test Mode)
        SES 15                    	133	46	S15	129°W	July 15, 2019 – Present	Operational	L1, L5 (Test Mode)
        Galaxy 30               	135	48	G30	125°W	April 26, 2022 – Present	Operational	L1, L5 (Test Mode)
    */
    WAAS,

    ///GLONASS SBAS
    /// https://sdcm.ru/%D1%81%D0%BF%D1%83%D1%82%D0%BD%D0%B8%D0%BA%D0%B8-sbas/?uid=2
    /// https://www.gpsworld.com/prn-codes-assigned-to-russian-sbas-satellites/
    /// 125 38
    /// 140 53
    /// 141 54
    SDCM,

    /**Galileo SBAS 123-136
    * https://egnos-user-support.essp-sas.eu/new_egnos_ops/documents/monthly-performance-report/137-monthly-performance-report-september-2022
    * https://gssc.esa.int/navipedia/index.php/EGNOS_Space_Segment
    * 120 33    2019 退役
    * 123 36
    * 126 39
    * 136 49
    **/
    EGNOS,

    /**BeiDou SBAS
      *  http://www.csno-tarc.cn/system/constellation
      *  130 43
      *  143 56
      *  144 57
      **/
    BDSBAS,

    /**QZSS SBAS
    * 129 42
    * 137 50
    **/
    MSAS,

    /**NAVIC SBAS
    * 127 40
    * 128 41
    *132 45
    **/
    GAGAN
  };

  enum class NMEA_Status : unsigned char
  {
    A,  // Data valid
    V,  // Invalid
    D   // Differential
  };

  enum class NMEA_Direction : unsigned char
  {
    N,  // North
    S,  // South
    E,  // East
    W   // West
  };

  enum class NMEA_FAA_Mode : unsigned char
  {
    A,  // Autonomous (non-differential)
    D,  // Differential mode
    E,  // Estimated (dead reckoning) Mode
    M,  // Manual Input Mode
    S,  // Simulator Mode
    N,  // Data not valid
  };

  enum class NMEA_Mode : unsigned char
  {
    A,  // Autonomous (non-differential)
    D,  // Differential mode
    E,  // Estimated (dead reckoning) Mode
    F,  // RTK Float
    M,  // Manual Input Mode
    N,  // Constellation not in use, or no valid fix
    P,  // Precise (no degradation, like Selective Availability, and hires)
    R,  // RTK Integer
    S,  // Simulator Mode
  };

  enum class NMEA_Sele_Mode : unsigned char
  {
    M,  // Manual, forced to operate in 2D or 3D
    A,  // Automatic, 2D/3D
  };

  enum class NMEA_Fix_Mode : unsigned char
  {
    FixNotAvailable = 1,
    Fix2D,
    Fix3D,
  };

  enum class NMEA_Quality : unsigned char
  {
    FixNotAvailable,
    GPSFix,
    DifferentialGPSFix,
    PPSFix,
    RealTimeKinematic,
    FloatRTK,
    Estimated,
    ManualInputmode,
    SimulationMode,
  };

  typedef struct NMEA_Data_
  {
    char yy;
    char mm;
    char dd;
    NMEA_Data_()
    {
      yy = -1;
      mm = -1;
      dd = -1;
    }

    //@bug
    //NMEA_Data_(NMEA_Data_ &data) {
    //  yy = data.yy;
    //  mm = data.mm;
    //  dd = data.dd;
    //}
    void cclear()
    {
      yy = -1;
      mm = -1;
      dd = -1;
    }

    bool operator==(const NMEA_Data_ &data)
    {
      if (data.yy == yy && data.mm == mm && data.dd == dd)
      {
        return true;
      }
      return false;
    }

    void operator=(const NMEA_Data_ &data)
    {
      yy = data.yy;
      mm = data.mm;
      dd = data.dd;
    }
  } NMEA_Data;

  typedef struct NMEA_Time_
  {
    char hh;
    char mm;
    char ss;
    short sss;
    NMEA_Time_()
    {
      hh = -1;
      mm =  -1;
      ss =  -1;
      sss =  -1;
    }
    NMEA_Time_(const NMEA_Time_ &data)
    {
      hh = data.hh;
      mm = data.mm;
      ss = data.ss;
      sss = data.sss;
    }

    void cclear()
    {
      hh = -1;
      mm =  -1;
      ss =  -1;
      sss =  -1;
    }


    bool operator==(const NMEA_Time_ &data)
    {
      if (data.hh == hh && data.mm == mm && data.ss == ss && data.sss == sss)
      {
        return true;
      }
      return false;
    }
    bool operator!=(const NMEA_Time_ &data)
    {
      if (data.hh == hh && data.mm == mm && data.ss == ss && data.sss == sss)
      {
        return false;
      }
      return true;
    }
  } NMEA_Time;

  typedef struct NMEA_64_
  {
    unsigned short dd;
    unsigned char mm;
    unsigned int mmmmmm;

  } NMEA_64;

  typedef struct NMEA_SBASE_
  {
    NMEA_TalkerID TalkerID;
    // unsigned char          CheckSum;
    NMEA_SBASE_() { TalkerID = NMEA_TalkerID::ID_NOID; }
    virtual ~NMEA_SBASE_() {}
  } NMEA_SBASE;

  /**RMC
   *Recommended Minimum Specific GNSS Data. Time, date, position, course, and
   *speed data provided by a GNSS receiver
   *
   * */
  typedef struct NMEA_RMC_ : public NMEA_SBASE {
    // NMEA_TalkerID  TalkerID;
    NMEA_Time UTC;
    NMEA_Status Status;
    NMEA_Direction LAT_DIRC;
    NMEA_Direction LON_DIRC;
    char NavStatus;  // Navigational status. Not supported.Always “V” (Not
        // valid).
    char FAAMode;
    NMEA_Data Date;
    NMEA_64 LAT;
    NMEA_64 LON;
    float SOG;  // Speed over ground. Variable length.Note that this field is
        // empty in case of an invalid value.
    float COG;  // Speed over ground. Variable length. Note that this field is
        // empty in case of an invalid value
    // MagVar
    // MagVarDir E or W
    /* NMEA_FAA_Mode*/
    // char          CheckSum;
    void dpCopy(NMEA_RMC_ &data)
    {
      TalkerID = data.TalkerID;
      UTC = data.UTC;
      Status = data.Status;
      LAT_DIRC = data.LAT_DIRC;
      LON_DIRC = data.LON_DIRC;
      NavStatus = data.NavStatus;
      FAAMode = data.FAAMode;
      Date = data.Date;
      LAT = data.LAT;
      LON = data.LON;
      SOG = data.SOG;
      COG = data.COG;
    }
  } NMEA_RMC;

  /**GGA
   *Global Positioning System Fix Data. Time, position, and fix-related data for
   *a GNSS receiver.
   * */
  typedef struct NMEA_GGA_ : public NMEA_SBASE
  {
    // NMEA_TalkerID  TalkerID;
    NMEA_Time UTC;
    NMEA_Direction LAT_DIRC;
    NMEA_Direction LON_DIRC;
    NMEA_Quality Quality;
    char NumSatUsed;  // Number of satellites in use, 00 - 12
    char M2;          //
    char M;           // Units of antenna altitude, meters

    NMEA_64 LAT;
    NMEA_64 LON;
    float HDOP;  // Horizontal Dilution of precision (meters)
    float
        ALT;  // Antenna Altitude above/below mean-sea-level (geoid) (in meters)
    float Sep;  // Geoidal separation (the difference between the WGS84 earth
        // ellipsoid surface and the mean-sea-level surface)
    // DiddAge                 //Age of differential GPS data. Not supported.
    // DiffStation             //Differential reference station ID. Not
    // supported. char          CheckSum;
    void dpCopy(NMEA_GGA_ &data)
    {
      TalkerID = data.TalkerID;
      UTC = data.UTC;
      LAT_DIRC = data.LAT_DIRC;
      LON_DIRC = data.LON_DIRC;
      Quality = data.Quality;
      NumSatUsed = data.NumSatUsed;
      M2 = data.M2;
      M = data.M;
      LAT = data.LAT;
      LON = data.LON;
      HDOP = data.HDOP;
      ALT = data.ALT;
      Sep = data.Sep;
    }
  } NMEA_GGA;

  /**
   * GNS
   * GNSS Fix Data. Fix data for single or combined satellite navigation systems
   * (GNSS).
   * */
  typedef struct NMEA_GNS_ : public NMEA_SBASE
  {
    // NMEA_TalkerID  TalkerID;
    NMEA_Time UTC;
    NMEA_Direction LAT_DIRC;
    NMEA_Direction LON_DIRC;
    char Navlnd;  // Navigational status indicator. Not supported.Always “V”
        // (Not valid).
    unsigned char NumSatUsed;  // Total number of satellites in use.Range: 00–99
    /*NMEA_Mode*/ char
        Model[6];  // 1GPS 2GLONASS 3Galileo 4BeiDou 5QZSS 6NAVIC(IRNSS)
    NMEA_64 LAT;
    NMEA_64 LON;
    float HDOP;  // Horizontal dilution of precision. The maximum value is 99.0.
    float ALT;   // Antenna altitude above the mean sea level (geoid).
    float Sep;   // Geoid separation (the difference between the earth ellipsoid
        // surface and the mean-sea-level (geoid) surface defined by the
        // reference datum used in the position solution).
    // DiffAge                 //Age of differential GPS data. Not supported.
    // DiffStation             //Differential reference station ID. Not
    // supported.

    //  char           CheckSum;
    void dpCopy(NMEA_GNS_ &data)
    {
      TalkerID = data.TalkerID;
      UTC = data.UTC;
      LAT_DIRC = data.LAT_DIRC;
      LON_DIRC = data.LON_DIRC;
      Navlnd = data.Navlnd;
      NumSatUsed = data.NumSatUsed;
      for (int i = 0; i < 6; i++)
      {
        Model[i] = data.Model[i];
      }
      LAT = data.LAT;
      LON = data.LON;
      HDOP = data.HDOP;
      ALT = data.ALT;
      Sep = data.Sep;
    }
  } NMEA_GNS;

  /**
   * GSA
   * GNSS DOP and Active Satellites. GNSS receiver operating mode,
   * satellites used in the navigation solution reported by the GGA or GNS
   *sentence, and DOP values.
   **/

  typedef struct NMEA_GSA_ : public NMEA_SBASE
  {
    // NMEA_TalkerID       TalkerID;
    NMEA_Sele_Mode Selection_Mode;
    NMEA_Fix_Mode Fix_Mode;
    NMEA_System_ID SystemID;
    unsigned char SatID_Size;
    short *SatID;
    float PDOP;
    float HDOP;
    float VDOP;
    // char                CheckSum;
    NMEA_GSA_()
    {
      SystemID = NMEA_System_ID::NO;
      SatID = nullptr;
      SatID_Size = 0;
      PDOP = 0;
      HDOP = 0;
      VDOP = 0;
    }
    void dpCopy(NMEA_GSA_ &data)
    {
      TalkerID = data.TalkerID;
      Selection_Mode = data.Selection_Mode;
      Fix_Mode = data.Fix_Mode;
      SystemID = data.SystemID;
      SatID_Size = data.SatID_Size;
      SatID = nullptr;
      if (SatID_Size > 0)
      {
        SatID = new short[SatID_Size]{0};
        for (int i = 0; i < SatID_Size; i++)
        {
          SatID[i] = data.SatID[i];
        }
      }

      PDOP = data.PDOP;
      HDOP = data.HDOP;
      VDOP = data.VDOP;
    }

    ~NMEA_GSA_()
    {
      if (SatID)
      {
        delete[] SatID;
        SatID = nullptr;
      }
    }
  } NMEA_GSA;

  // GSV
  typedef struct NMEA_Sat_Info_
  {
    char SatElev;  // Satellite elevation. Range: 0–90.
    char SatSNR;  // Satellite SNR (C/N0). Range 00–99.Null when not tracking.
    short SatID;  // Satellite ID
    short SatAz;  // Satellite azimuth, with true north as the
        // reference plane.Range: 0–359.

    NMEA_Sat_Info_ ()
    {
      SatElev = NULLVALUE;
      SatSNR = NULLVALUE;
      SatID = NULLVALUE;
      SatAz = NULLVALUE;
    }
    void dpcopy(const NMEA_Sat_Info_ &data)
    {
      SatElev = data.SatElev;
      SatSNR = data.SatSNR;
      SatID = data.SatID;
      SatAz = data.SatAz;
    }
  }
  /**
    * char SatElev // Satellite elevation. Range: 0–90.
    *  char SatSNR  // Satellite SNR (C/N0). Range 00–99.Null when not tracking.
    *  short SatID  // Satellite ID
    *  short SatAz  // Satellite azimuth, with true north as the
    **/
  NMEA_Sat_Info;

  /**
   *GNSS Satellites in View. The GSV sentence provides the number of satellites
   *(SV) in view, satellite ID numbers, elevation, azimuth, and SNR value. The
   *GSV sentence contains maximum four satellites per transmission. The total
   *number of sentences being transmitted and the sentence number are indicated
   *in the first two fields.
   * */
  typedef struct NMEA_GSV_ : public NMEA_SBASE
  {
    // NMEA_TalkerID  TalkerID;
    unsigned char TotalNumSen;  // Total number of sentences.Range: 1–9.
    unsigned char SenNum;       // Sentence number.'
    unsigned char SignalID;     // SignalID
    unsigned char TotalNumSat;  // Total number of satellites in view.
    unsigned char Sat_Info_size;
    NMEA_Sat_Info *Sat_Info;  // repeat block. Repeat times: 1–4

    NMEA_GSV_()
    {
      TotalNumSen = 0;
      SenNum = 0;
      SignalID = 0;
      TotalNumSat = 0;
      Sat_Info = nullptr;
      Sat_Info_size = 0;
    }
    void dpCopy(NMEA_GSV_ &data)
    {
      TalkerID = data.TalkerID;
      TotalNumSen = data.TotalNumSen;
      SenNum = data.SenNum;
      SignalID = data.SignalID;
      TotalNumSat = data.TotalNumSat;
      Sat_Info_size = data.Sat_Info_size;
      if (Sat_Info_size > 0)
      {
        Sat_Info = new NMEA_Sat_Info[Sat_Info_size];
      }
      for (int i = 0; i < Sat_Info_size; i++)
      {
        Sat_Info[i].dpcopy(data.Sat_Info[i]);
      }
    }

    ~NMEA_GSV_()
    {
      if (Sat_Info)
      {
        delete[] Sat_Info;
        Sat_Info = nullptr;
      }
    }
  } NMEA_GSV;

  /**VTG
   *Track made good and Ground speed
   * */
  typedef struct NMEA_VTG_ : public NMEA_SBASE
  {
    // NMEA_TalkerID  TalkerID;
    char T;
    char M;
    char N;
    char K;
    float TCOG;            // True course over ground (degrees) 000  to 359
    float MCOG;            // Magnetic course over ground       000  to 359
    float SOG_KNOTS;       // Speed over ground (knots)         00.0 to 99.9
    float SOG_kilometers;  // Speed over ground (knots)         00.0 to 99.9
    /* NMEA_FAA_Mode*/ char FAAMode;
    // char           CheckSum;
    void dpCopy(NMEA_VTG_ &data)
    {
      TalkerID = data.TalkerID;
      T = data.T;
      M = data.M;
      N = data.N;
      K = data.K;
      TCOG = data.TCOG;
      MCOG = data.MCOG;
      SOG_KNOTS = data.SOG_KNOTS;
      SOG_kilometers = data.SOG_kilometers;
      FAAMode = data.FAAMode;
    }
  } NMEA_VTG;

  /**
   *Geographic Position - Latitude/Longitude
   * */
  typedef struct NMEA_GLL_ : public NMEA_SBASE
  {
    NMEA_Time UTC;
    NMEA_Status Status;
    char FAAMode;
    NMEA_Direction LAT_DIRC;
    NMEA_Direction LON_DIRC;
    NMEA_64 LAT;
    NMEA_64 LON;

    void dpCopy(NMEA_GLL_ &data)
    {
      TalkerID = data.TalkerID;
      UTC = data.UTC;
      Status = data.Status;
      FAAMode = data.FAAMode;
      LAT_DIRC = data.LAT_DIRC;
      LON_DIRC = data.LON_DIRC;
      LAT = data.LAT;
      LON = data.LON;
    }
    ~NMEA_GLL_() {}
  } NMEA_GLL;

 public:
  /**
   * @brief CheckNMEA
   * @param Sentence
   * @param StartIndex $ start index
   * @param EndIndex   * end   index
   * @return
   */
  static NMEA_Check_Flag CheckNMEA(const char *Sentence, size_t SentenceSize,
                                   size_t &StartIndex, size_t &EndIndex,
                                   NMEA_TalkerID &TalkerID,
                                   NMEA_Sentence_ID &SentenceID);
  static NMEA_Check_Flag CheckNMEA(std::string NMEAStr, NMEA_TalkerID &TalkerID,
                                   NMEA_Sentence_ID &SentenceID);
  static NMEA_Check_Flag CheckNMEA(const char *Sentence, size_t SentenceSize,
                                   NMEA_TalkerID &TalkerID,
                                   NMEA_Sentence_ID &SentenceID);

  static bool NmeaStr2List(const char *Sentence, size_t NMEASIZE,
                           std::list<const char *> &rt);

  static const char *GetLine(const char *Sentence, const char *SentenceEndchar,
                             size_t &LineOFF, size_t &NMEASize,
                             const char *&NMEA_Start);

 public:
  inline static bool IsField(char c)
  {
    return isprint(c) && c != ',' && c != '*';
  }

  // checksum
  inline static unsigned char NMEA_CheckSum(const char *Sentence,
                                            int checksize)
  {
    unsigned char CheckSum = 0;
    if (*Sentence == NMEASTARTC)
    {
      Sentence++;
    }

    while (*Sentence && *Sentence != NMEAENDC && checksize--)
    {
      CheckSum ^= *Sentence++;
    }
    return CheckSum;
  }

  static char hex2char(char c)
  {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
  }

  static std::unordered_map<NMEA_Sentence_ID, const char *> NMEA_MAP;
  static std::unordered_map<NMEA_System_ID, const char *> NMEA_System_ID_MAP;
  static constexpr  int TALKERID_LEN = 2;
  static constexpr  int SENTENCEID_LEN = 3;
  static constexpr  int CHECKSUM_LEN = 2;
  static constexpr  int RF_LEN = 2;
  static constexpr  int MIN_NMEA_LEN = 12;  // 1+2+3+1+1+2+2
  static constexpr  int MAX_NMEA_LEN = 82;
  static constexpr  char NMEASTARTC = '$';

  static constexpr  char NMEAENDC = '*';
  static constexpr  char NMEADOT = ',';
  static constexpr  char NMEARF[3] = "\r\n";
  static constexpr  char NMEAF = '\n';
};

#endif  // NMEA_BASE_H
