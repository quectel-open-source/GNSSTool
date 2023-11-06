#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <QObject>
#include <QQueue>
#include <QDateTime>
#include <QSet>
#include <QMetaEnum>
#include <optional>
#include <QVector>

#define KT2KMH         (1.852)          /* deg to rad */
#define GNSS_NUM_OF_SYSTEMS (6)

//typedef struct{
//    double			Latitude;											///< Latitude (Decimal degrees, S < 0 > N)
//    double			Longitude;											///< Longitude (Decimal degrees, W < 0 > E)
//    double			AltitudeMSL;
//}SolutionLLA_t;

// RMC Status field
enum RMC_STATUS_E {
    Empty = 0,
    RMC_STATUS_VOID = 'V',
    RMC_STATUS_ACTIVE = 'A',
    RMC_STATUS_D = 'D'
};
struct SavedData_t{
    double			Latitude;											///< Latitude (Decimal degrees, S < 0 > N)
    double			Longitude;											///< Longitude (Decimal degrees, W < 0 > E)
    double			AltitudeMSL;
    int             GPSQuality;
};

//enum RMC_STATUS_E {
//    RMC_STATUS_ACTIVE = 'A',
//    RMC_STATUS_VOID = 'V',
//};

enum  SatSys{
    All = 0,
    GPS = 1,
    GLONASS = 2,
    Galileo = 3,
    BeiDou = 4,
    QZSS = 5,
    IRNSS = 6,
    SBAS
};


struct SignalInfo_t{
    SatSys Sys;
    uint32_t PRN;
    uint32_t Azimuth;
    uint32_t Elevation;
    uint32_t SNR;
    uint32_t Channel;
    bool usedInSolution;
};

struct Solution_t{
    std::optional<int> Year;
    std::optional<int> Month;
    std::optional<int> Day;
    std::optional<int> Hour;
    std::optional<int> Minute;
    std::optional<int> Second;
    std::optional<double> dSecond;
    std::optional<double> SpeedKnots;
    std::optional<int> FixMode;
    std::optional<double> PDOP;
    std::optional<double> HDOP;
    std::optional<double> VDOP;
    std::optional<double> AgeOfDiff;//Age of Differential
    QList<SavedData_t> savedData_L;
    QList<SignalInfo_t> SatInfo;
    std::optional<double> ACC_3D;
    std::optional<double> ACC_2D;
    std::optional<int> DR_Type;//PQTMINS
};

struct GGA_t{
    uint32_t                _n,_error;
    uint32_t                _Fixed,_RTK,_FloatRTK;
    char                    GGA_String[256];
    int                     m_nHour;												///< hour
    int                     m_nMinute;												///< Minute
    double                  m_dSecond;												///< Second
    double                  m_dLatitude;											///< Latitude (Decimal degrees, S < 0 > N)
    double                  m_dLongitude;											///< Longitude (Decimal degrees, W < 0 > E)
    double                  AltitudeMSL;											///< Altitude (Meters)
    int                     m_nGPSQuality;											///< GPS Quality
    int                     m_nSatsInView;											///< Number of satellites in view
    char                    HDOP[8];												///< Horizontal Dilution of Precision
    char                    GeoidalSep[8];											///< Geoidal separation, the difference between the WGS-84 earth ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid (meters)
    double                  m_dDifferentialAge;										///< Age of differential GPS data, time in seconds since last SC104 type 1 or 9 update, null field when DGPS is not used
    int                     m_nDifferentialID;										///< Differential reference station ID, 0000-1023
    double                  m_dVertSpeed;											///< Derived vertical speed (THIS IS NOT PART OF THE NMEA SPECIFICATION. It is derived during parsing)
};

typedef struct{

}GLL_t;

typedef struct{
    double		dAzimuth;						///< Satellite Azimuth
    double		dElevation;						///< Satellite Elevation
    int			nPRN;							///< Satellite Psudo Random Number (THis is the ID and not the satellite vehicle number)
    int			nSNR;							///< Signal to Noise Ration - this is the signal quality
    int 		nChannel;
    int         usedInSolution;
}SAT_Info_t;


typedef struct{
    int                 SystemID;
    int					nTotalSentences;			///< Total number of GSV sentences to build up satellites monitored
    int					nSentenceNumber;					///< Current sentence number
    int					nSatsInView;						///< Number of satellites in view
    QList<SignalInfo_t> signal_L;
}GSV_Data_t;

typedef struct{
    uint32_t        _n,_error;
//    QTime           timer;
//    uint32_t        received;
    GSV_Data_t      gpGSV_Data;
    GSV_Data_t      glGSV_Data;
    GSV_Data_t      gaGSV_Data;
    GSV_Data_t      gbGSV_Data;
    GSV_Data_t      gqGSV_Data;
    GSV_Data_t      giGSV_Data;
    GSV_Data_t      gnGSV_Data;
    GSV_Data_t      gsGSV_Data;
}GSV_t;

typedef struct{
    uint32_t        _n,_error;
    char            cAutoMode;
    char            cFixMode;
    double			PDOP;
    double          HDOP;
    double			VDOP;
//    QVector<int> gpInSol;
//    QVector<int> glInSol;
//    QVector<int> gaInSol;
//    QVector<int> gbInSol;
//    QVector<int> giInSol;
//    QVector<int> gqInSol;
//    QVector<int> gnInSol;
//    QVector<int> gsInSol;

    QVector<int>    UsedInSolution[GNSS_NUM_OF_SYSTEMS + 1]; //GPS:1  GLONASS:2  GALILEO:3 ...Table 19
}GSA_t;

// RMC Recommended minimum data for GPS
typedef struct{
    uint32_t        _n,_error;
//    QDateTime       *m_qTime;
    int				m_nHour;												///< hour
    int				m_nMinute;												///< Minute
    int				m_nSecond;												///< Second
    double			m_dSecond;												///< Fractional second
    double			m_dLatitude;											///< Latitude (Decimal degrees, S < 0 > N)
    double			m_dLongitude;											///< Longitude (Decimal degrees, W < 0 > E)
    RMC_STATUS_E	m_nStatus;												///< Status
    double			m_dSpeedKnots;											///< Speed over the ground in knots
    double			m_dTrackAngle;											///< Track angle in degrees True North
    int				m_nMonth;												///< Month
    int				m_nDay;													///< Day
    int				m_nYear;												///< Year
    double			m_dMagneticVariation;									///< Magnetic Variation
    char            m_cMode;                                                ///< Positioning system mode
    char            m_cNavStatus;                                           ///< Navigational status
}RMC_t;

typedef struct{
    uint32_t        _n,_error;
    double          dTrackTrue;
    double          dTrackMag;
    double          dSpeedKn;
    double          dSpeedKm;
    char            cModeIndicator;
}VTG_t;

typedef struct{
    uint32_t        _n,_error;
    int             nHour;
    int             nMinute;
    double          dSecond;
    double          dLatitude;
    double          dLongitude;
    char            AltitudeMSL[8];
    char            cPosMode[8];
    int             nSatsInView;
    char            HDOP[8];
    char            GeoidalSep[8];
    double          dDifferentialAge;
    int             nDifferentialID;
    char            m_cNavStatus;
}GNS_t;

typedef struct{
    uint32_t _L26DR;
    uint32_t _MTK;
    uint32_t _nST_LTODOWNLOAD;
}ACK_t;

typedef struct{
    uint32_t dr_is_calib;
    uint32_t odo_is_calib;
    uint32_t gyro_sensitivity_is_calib;
    uint32_t gyro_bias_is_calib;
    uint32_t NavModeInfo;//UDR
}DR_t;

typedef struct{
    char*      commandName;
    uint8_t    cSleepMode;
    uint8_t    cGLPModeStatus;
    uint8_t    c_B0:1;//B0
    uint8_t    c_B1:1;//B1
    uint8_t    c_B2:1;//B2
    uint8_t    c_B3:1;//B3
    uint8_t    c_B4:1;//B4
    uint8_t    c_B5:1;//B5
    uint8_t    c_B6:1;//B6
    uint8_t    c_B7:1;//B7
    uint32_t   nAuto_Start;
    uint32_t   nMSG_Mode;
    double     dOdo_Val;
    double     dL5L5Bias;
    double     dInitVal;
    uint32_t   nGeoID;
    uint32_t   nGeoMode;
    uint32_t   nGeoShape;
    uint32_t   nGeoEn;
    float      fLat0;
    float      fLat1;
    float      fLat2;
    float      fLat3;
    float      fLon0;
    float      fLon1;
    float      fLon2;
    float      fLon3;
    uint32_t   nNMEA_GGA;
    uint32_t   nNMEA_RMC;
    uint32_t   nNMEA_GSV;
    uint32_t   nNMEA_GSA;
    uint32_t   nNMEA_GLL;
    uint32_t   nNMEA_VTG;
    uint32_t   nEA_Mask;
    uint32_t   nEnable;

}LC79DA_t;
//
typedef struct{
    char*      commandName;
    uint32_t   nMode;
    uint8_t    c_B0:1;//B0
    uint8_t    c_B1:1;//B1
    uint8_t    c_B2:1;//B2
    uint8_t    c_B3:1;//B3
    uint8_t    c_B4:1;//B4
    uint8_t    c_B5:1;//B5
    uint8_t    c_B6:1;//B6
    uint8_t    c_B7:1;//B7
    double     dL5Bias;
    uint32_t   nAuto_Start;
    uint32_t   nMSG_Mode;
    double     dInitVal;
    double     dOdo_Val;
    uint32_t   nGeoID;
    uint32_t   nGeoMode;
    uint32_t   nGeoShape;
    float      fLat0;
    float      fLat1;
    float      fLat2;
    float      fLat3;
    float      fLon0;
    float      fLon1;
    float      fLon2;
    float      fLon3;
    uint32_t   nGeoEn;
    uint32_t   nGeoStatus;
    uint32_t   nNMEA_GGA;
    uint32_t   nNMEA_RMC;
    uint32_t   nNMEA_GSV;
    uint32_t   nNMEA_GSA;
    uint32_t   nNMEA_GLL;
    uint32_t   nNMEA_VTG;
    uint32_t   nEA_Mask;
    uint32_t   nEnable;
}LC79DC_t;

typedef struct{
    char*      commandName;
    char*      mainVersion;
    uint32_t   nPortType;
    uint32_t   nProtocolType;
    uint32_t   nBaudRate;
    uint32_t   nMask;
    uint32_t   nMode;
    uint32_t   nDuration;
    double     dAntHeight;
    double     dECEF_X;
    double     dECEF_Y;
    double     dECEF_Z;
}LG69TAS_t;

typedef struct{
    char*      commandName;
    char*      mainVersion;
    uint32_t   nPortType;
    uint32_t   nProtocolType;
    uint32_t   nBaudRate;
    uint32_t   MsgMask;
    uint32_t   nSpeedMode;
    uint32_t   nMode;
    uint32_t   nInvert;
    uint32_t   nPull;
    uint32_t   nEdgeType;
    double     dMPT;
    uint32_t   nPortID;
    uint32_t   nEnable;
    uint32_t   nFrameFormat;
    uint32_t   nDataBaudrate;
    uint32_t   nIndex;
    uint32_t   nFilterType;
    uint32_t   nID_Type;
    char*      ID1;
    char*      ID2;
    char*      MsgID;
    uint32_t   nStartBit;
    uint32_t   nBitSize;
    uint32_t   nByteOrder;
    uint32_t   nValueType;
    double     dFactor;
    double     dOffset;
    double     dMin;
    double     dMax;
    uint32_t   nRVal;
}LG69TAP_t;

typedef struct{

    uint32_t nIndex;
    uint32_t nMsgVer;
    double   dUTC;
    uint32_t nQuality;
    double   dLength;
    double   dPitch;
    double   dRoll;
    double   dHeading;
    double   dAcc_Pitch;
    double   dAcc_Roll;
    double   dAcc_Heading;
    uint32_t nUsedSV;
}LC02H_t;


struct SurveyIn_t
{
    int valid;
    int obs;
    int CfgDur;
    double MeanX;
    double MeanY;
    double MeanZ;
};

struct NMEA_t{
    uint32_t _n,_error;
    std::optional<int> lastTTFF;
    Solution_t Solution_Data;
    SurveyIn_t SurveyInStatus;
//    QTime timer;
    RMC_t RMC_Data;
    GGA_t GGA_Data;
    GNS_t GNS_Data;
    GSA_t GSA_Data;
    GSV_t GSV_Data;
    VTG_t VTG_Data;
    ACK_t ACK;
    DR_t DR_Data;
    LC79DA_t Lc79DA;
    LC79DC_t Lc79DC;
    LG69TAS_t LG69TAS;
    LG69TAP_t LG69TAP;
    LC02H_t LC02H;
};

class NMEA_Parser : public QObject
{
    Q_OBJECT
public:
    explicit NMEA_Parser(QObject *parent = nullptr,NMEA_t * Nt = nullptr);
    ~NMEA_Parser();
    void ResetNMEA_Data();
    int parseNMEA(QQueue<QByteArray> &NQ);
    bool NMEAcheckSUM(QByteArray NMEA_Ste);
    int fillSatData(const QList<QByteArray> &sFields,GSV_Data_t &GSV_data_t);
    int fillTeseoSatData(const QList<QByteArray> &sFields);
    GSV_Data_t *teseoPRN_GetGSV_DATA(int n);
    int processOther(QByteArray &NMEA_Ste);
    int NMEA_ProcessRMC(QByteArray &NMEA_Ste);
    int NMEA_ProcessGGA(QByteArray &NMEA_Ste);
    int NMEA_ProcessGSA(QByteArray &NMEA_Ste);
    int NMEA_ProcessGSV(QByteArray &NMEA_Ste);
    int NMEA_ProcessVTG(QByteArray &NMEA_Ste);
    int NMEA_ProcessGNS(QByteArray &NMEA_Ste);
    int NMEA_ProcessDRCAL(QByteArray &NMEA_Ste);
    int NMEA_ProcessUDR(QByteArray &NMEA_Ste);
    int NMEA_ProcessPQEPE(QByteArray &NMEA_Ste);
    int NMEA_ProcessPQTMEPE(QByteArray &NMEA_Ste);
    int NMEA_ProcessPQTMSVINSTATUS(QByteArray &NMEA_Ste);
    void clearGSV_Data();

    NMEA_t * Nt = nullptr;
    NMEA_t lco2Nt;
    QTime timeCounter;
    std::function<void()> surveyUpdate;

    enum NMEA_Type{
        Other,
        RMC,
        GGA,
        GLL,
        GNS,
        GSV,
        GSA,
        VTG
    };
    Q_ENUM(NMEA_Type)

    enum Other_Type{
        PQEPE,
        PQTMEPE,
        PQTMSVINSTATUS,
        PQTMINS,

    };
    Q_ENUM(Other_Type)

signals:
    void displayTextData(QByteArray);
    void parseData_L26DR(QByteArray);
    void parseData_LC79DA(NMEA_t*);
    void parseData_LC79DC(NMEA_t*);
    void parseData_LG69TAS(NMEA_t*);
    void parseData_LG69TAP(NMEA_t*);
    void parseDataLC02H(NMEA_t*);
    void parseDataLC02H(NMEA_t);


public slots:

private:
    QSet<char> EpochHasNMEA_Type;//
    QMetaEnum NMEA_Type_ME;
    QMetaEnum Other_Type_ME;
    void ResetRMC_t();
    void ResetGGA_t();
    void ResetGNS_t();
    void ResetGSA_t();
    void ResetGSV_t();
    void ResetVTG_t();
    void CreateEpoch(void);
    void UpdateSatelliteInfo(void);

};

#endif // NMEA_PARSER_H
