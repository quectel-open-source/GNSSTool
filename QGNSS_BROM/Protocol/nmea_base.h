/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         nmea_base.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-18      victor.gong
* ***************************************************************************/
/**
*
*  1B           2B                                         1B          2B
*              <address>
*  $    (<TalkerID><Sentence formatter>)    [,<Data>]  *<Checksum>  <CR><LF>
*       |<--the range for checksum calculation  --> |
*
* */
#ifndef NMEA_BASE_H
#define NMEA_BASE_H

#define NMEA_CRLF "\r\n"
#define NMEA_COMMA ","
#define NMEA_START "$"
#define NMEA_REGEX "\\$[GB]{1}[PLDN]{1}(RMC|GGA|GSV|GSA|VTG|GLL)[,]{1}[,.A-Za-z0-9]{1,}\\*"

#include <QObject>
/**
 * @brief The NMEA_Base class
 */
class NMEA_Base : public QObject
{
    Q_OBJECT
public:
    explicit NMEA_Base(QObject *parent = nullptr);
    virtual ~NMEA_Base();
    typedef  float  NMEA_PTYPE;
    /*standard messages*/
    enum class NMEA_Sentence:unsigned char{
        RMC,/*Recommended Minimum Specific GNSS Data.*/
        GGA,/*Global Positioning System Fix Data. */
        GSV,/*GNSS Satellites in View. */
        GSA,/*GNSS DOP and Active Satellites.*/
        VTG,/*Course Over Ground & Ground Speed.*/
        GLL,/*Geographic Position-Latitude/Longitude.*/
    };
    Q_ENUM (NMEA_Sentence)

    enum class NMEA_TalkerID:unsigned char{
        GP,/*GPS QZSS*/
        GL,/*GLONASS*/
        BD,/*BDS*/
        GN,/*Combination of Multiple Satellite Systems*/
    };
    Q_ENUM  (NMEA_TalkerID)

    /*Positioning system status:*/
    enum class NMEA_Status:unsigned char{
        A,//Data valid
        V,//Invalid
        D,//Differential
    };
    Q_ENUM (NMEA_Status)

    enum class NMEA_Direction:unsigned char{
        N,//North
        S,//South
        E,//East
        W,//West
    };
    Q_ENUM (NMEA_Direction)

    enum class NMEA_Mode_Indicator:unsigned char{
        A,//Autonomous mode. Satellite system used in non-differential mode in position fix
        D,//Differential mode. Satellite system used in differential mode in position fix. Corrections from ground stations or Satellite Based Augmentation System (SBAS).
        E,//Estimated (dead reckoning) mode.
        M,//Estimated (dead reckoning) mode.
        N,//No fix. Satellite system not used in position fix, or fix not valid.
        S //Simulator mode
    };
    Q_ENUM (NMEA_Mode_Indicator)

    enum class NMEA_FixMode_Indicator:unsigned char{
        Fix_not_available=1,
        Fix_2D,
        Fix_3D
    };
    Q_ENUM (NMEA_FixMode_Indicator)



    //*************Standard Messages******************
    /*hhmmss.sss  052457.000*/
    typedef  struct {
        char hh[2]; // Decimal fraction of seconds
        char mm[2]; // Minutes (00–59)
        char ss[2]; //  Seconds (00–59)
        char dot;   // dot
        char sss[3];// Decimal fraction of seconds
    } NMEA_UTC;

    typedef  struct {
        char dd[2];    // Degrees (00–90)
        char mm[2];    // Minutes (00–59)
        char dot;      // dot
        char mmmmmm[6];//Decimal fraction of minutes
    } NMEA_Degree;

    typedef  struct {
        char dd[2];    // Day of month
        char mm[2];    // Month
        char yy[2];    // Year
    } NMEA_Date;

    typedef  struct NMEABASE_{
        virtual  bool set(const char *)=0;
        virtual  const char *get()=0;
    }NMEABASE;

    /**
     * Recommended Minimum Specific GNSS Data. Time, date, position, course, and speed data provided by a GNSS receiver.
     *
     * $<TalkerID>RMC,<UTC>,<Status>,<Lat>,<N/S>,<Lon>,<E/W>,<SOG>,<COG>,<Date>,<MagVar>,<MagVarDir>,<ModeInd>,<NavStatus>*<Checksum><CR><LF>
     **/
    typedef  struct NMEARMC_
    {
        NMEA_TalkerID TalkerID;
        NMEA_Sentence Sentence;//fix RMC
        NMEA_UTC      UTC;
        NMEA_Status   Status;
        NMEA_Degree   Latitude;
        NMEA_Direction NS;
        NMEA_Degree   Longitude;
        NMEA_Direction EW;
        NMEA_PTYPE     SOG;
        NMEA_PTYPE     COG;
        NMEA_Date      Data;
        char           MagVar_NS;
        char           MagVarDir_NS;
        NMEA_Mode_Indicator ModeInd;
        char           NavStatus_NS;//fix V
        char           Checksum[2];//
        NMEARMC_()
        {
            Sentence=NMEA_Sentence::RMC;
            NavStatus_NS='V';
        }
    } NMEARMC;

    /**
    *  Global Positioning System Fix Data. Time, position, and fix-related data for a GNSS receiver.
    *
    *  $<TalkerID>GGA,<UTC>,<Lat>,<N/S>,<Lon>,<E/W>,<Quality>,<NumSatUsed>,<HDOP>,<Alt>,M,<Sep>,M,<DiffAge>,<DiffStation>*<Checksum><CR><LF>
    * */
    typedef  struct NMEAGGA_
    {
        NMEA_TalkerID TalkerID;
        NMEA_Sentence Sentence;//fixed GGA
        NMEA_UTC      UTC;
        NMEA_Degree   Latitude;
        NMEA_Direction NS;
        NMEA_Degree   Longitude;
        NMEA_Direction EW;
        char           Quality;//1digit
        char           NumSatUsed[2];//2digit
        NMEA_PTYPE          HDOP;
        NMEA_PTYPE          Alt;
        char           M1;
        NMEA_PTYPE          Sep;
        char           M2;
        char           DiffAge_NS;
        char           DiffStation_NS;
        char           Checksum[2];//
        NMEAGGA_()
        {
            Sentence=NMEA_Sentence::GGA;
        }
    } NMEAGGA;

    /**
    *
    *GNSS Satellites in View. The GSV sentence provides the number of satellites in view (SV), satellite ID numbers, elevation, azimuth, and SNR value,
    *and contains maximum four satellites per transmission. Therefore, it may take several sentences to get complete information.
    *The total number of sentences being transmitted, and the sentence number are indicated in the first two data fields.
    *
    *  $<TalkerID>GSV,<TotalNumSen>,<SenNum>,<TotalNumSat>{,<SatID>,<SatElev>,<SatAz>,<SatCN0>},<SignalID>*<Checksum><CR><LF>
    * */

    //
    typedef  struct NMEAGSV_SatelliteInfo_
    {
        uint8_t      SatID;      //Satellite ID
        uint8_t      SatElev;    //Satellite elevation. Range: 00–90.
        uint16_t     SatAz;      //Satellite azimuth, with true north as the reference plane. Range: 000–359.
        uint8_t      SatCN0;     //Satellite C/N0.Range: 00–99.Null when not tracking.
    }NMEAGSV_SatelliteInfo;

    typedef  struct NMEAGSV_
    {
        NMEA_TalkerID TalkerID;
        NMEA_Sentence Sentence;
        uint8_t      TotalNumSen;//Total number of sentences.Range: 1–9.
        uint8_t      SenNum;     //Sentence number. Range: 1–TotalNumSen.
        uint8_t      TotalNumSat;//Total number of satellites in view.
        //Start of repeat block. Repeat times: 1–4.
        NMEAGSV_SatelliteInfo *Satellite;
        uint8_t Repeat_times;
        //End of repeat block.

        int      SignalID;   //GNSS signal ID.
        char     Checksum[2];//Checksum..
        NMEAGSV_()
        {
            Sentence=NMEA_Sentence::GSV;
        }
    } NMEAGSV;

    /**
*  GNSS DOP and Active Satellites. GNSS receiver operating mode,
*  satellites used in the navigation solution reported by the GGA or GNS sentence,
*  and DOP values
*  $<TalkerID>GSA,<Mode>,<FixMode>{,<SatID>},<PDOP>,<HDOP>,<VDOP><SystemID>*<Checksum><CR><LF>
* */
    typedef  struct NMEAGSA_
    {
        NMEA_TalkerID          TalkerID;
        NMEA_Sentence          Sentence;
        NMEA_Mode_Indicator    Mode;
        NMEA_FixMode_Indicator FixMode;
        //Start of repeat block. Repeat times: 12.
        uint16_t               SatID[12];//ID numbers of satellites used in solution.Note that this field is empty in case of an invalid value.
        //End of repeat block.
        NMEA_PTYPE             PDOP;    //Position dilution of precision.Maximum value: 99.0.Note that this field is empty in case of an invalid value.
        NMEA_PTYPE             HDOP;    //Horizontal dilution of precision.Maximum value: 99.0.Note that this field is empty in case of an invalid value
        NMEA_PTYPE             VDOP;    //Vertical dilution of precision.Maximum value: 99.0.Note that this field is empty in case of an invalid value.
        uint8_t                SystemID;//SystemID
        char                   Checksum[2];
        NMEAGSA_()
        {
            Sentence=NMEA_Sentence::GSA;
        }
    } NMEAGSA;
    /**
    * Course Over Ground & Ground Speed. The actual course and speed relative to the ground
    * $<TalkerID>VTG,<COGT>,T,<COGM>,M,<SOGN>,N,<SOGK>,K,<ModeInd>*<Checksum><CR><LF>
    * */
    typedef  struct NMEAVTG_
    {
        NMEA_TalkerID        TalkerID;
        NMEA_Sentence        Sentence;
        NMEA_PTYPE             COGT;     //Course over ground, in true north course direction.Note that this field is empty in case of an invalid value.
        char                    T;       //Course over ground.(degrees true, fixed field)Note that this field is empty in case of an invalid value.
        NMEA_PTYPE             COGM;     //Course over ground (magnetic).Not supported.
        char                    M;       //Course over ground.(degrees magnetic, fixed field)Note that this field is empty in case of an invalid value.
        NMEA_PTYPE             SOGN;     //Speed over ground in knots.Note that this field is empty in case of an invalid value
        char                    N;       //Speed over ground (knots, fixed field).Note that this field is empty in case of an invalid value
        NMEA_PTYPE             SOGK;     //Speed over ground in kilometers per hour.Note that this field is empty in case of an invalid value
        char                    K;       //Speed over ground.(kilometers per hour, fixed field)Note that this field is empty in case of an invalid value.
        NMEA_Mode_Indicator Mode;
        char     Checksum[2];
        NMEAVTG_()
        {
            Sentence=NMEA_Sentence::VTG;
        }
    } NMEAVTG;

    /**
      * Geographic Position-Latitude/Longitude.
      * Latitude and longitude of the GNSS receiver position,
      * the time of position fix and status.
      *
      *$<TalkerID>GLL,<Lat>,<N/S>,<Lon>,<E/W>,<UTC>,<Status>,<ModeInd>*<Checksum><CR><LF>
      * */
    typedef  struct NMEARGLL_
    {
        NMEA_TalkerID        TalkerID;//Talker identifier.
        NMEA_Sentence        Sentence;
        NMEA_Degree          Lat;     //Latitude:dd: Degrees (00–90)mm: Minutes (00–59)mmmmmm: Decimal fraction of minutes


        NMEA_Direction       NS;      //Latitude direction:N = NorthS = South Note that this field is empty in case of an invalid value.*/
        NMEA_Direction       Lon;     //Longitude
        NMEA_Direction       EW;      //Longitude direction:
        NMEA_UTC             UTC;     //Position UTC:
        NMEA_Status          Status;  //Positioning system status:
        NMEA_Mode_Indicator  Mode;    //The mode indicator of the positioning system:
        char     Checksum[2];
        NMEARGLL_()
        {
            Sentence=NMEA_Sentence::GLL;
        }
    } NMEARGLL;


    virtual bool isContainsNMEA( QString str,QString regex)=0;
Q_SIGNALS:
public Q_SLOTS:
private:
};
#endif // NMEA_BASE_H
