/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qlog_data_center.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-24      victor.gong
* ***************************************************************************/
#ifndef QLOG_DATA_CENTER_H
#define QLOG_DATA_CENTER_H

#include <list>
#include <array>
#include <algorithm>
#include "nmea_base.h"
#include "qgnss_nmea_parse.h"

#define CALCULATE_SNR_SIZE 4

typedef struct base_data_
{
  char  signal_id;
  char  CN0_Average;
  char  CN0_Max;
  char  CN0_Min;
  unsigned short TotalNumSatView;
  unsigned short TotalNumSatUsed;
  base_data_(char signalid )
  {
    signal_id = signalid;
    CN0_Average = -1;
    CN0_Max = -1;
    CN0_Min = -1;
    TotalNumSatView = 0;
    TotalNumSatUsed = 0;
  }
}
/**
  *data
  * signal_id
  * CN0_Average
  * CN0_Max
  * CN0_Min
  * TotalNumSatView
  * TotalNumSatUsed
  * */
  Base_data;

typedef struct System_data_
{
  NMEA_Base::NMEA_System_ID   system_id;
  ///all signal data
  std::vector<Base_data>           Data;

  ///find signalid in vector
  int find(char  signalid)
  {
    for(int i=0;i<Data.size ();i++)
    {
      if(Data[i].signal_id==signalid)
      {
        return i;
      }
    }
    return -1;
  }
  System_data_(NMEA_Base::NMEA_System_ID id)
  {
    system_id = id;
  }
}
/**
  *all Base_data in  statelite system
  * */
System_data;

typedef  System_data Systemdata;
typedef  struct LOG_Frame_
{
  NMEA_Base::NMEA_Time      UTC;
  NMEA_Base::NMEA_Data      Date_t;
  NMEA_Base::NMEA_Status    RMC_Status;
  std::vector<Systemdata>   Systemdata_;
  ///find systemid in vector
  int find(NMEA_Base::NMEA_System_ID systemid )
  {
    for(int i=0;i<Systemdata_.size ();i++)
    {
      if(Systemdata_[i].system_id==systemid)
      {
        return i;
      }
    }
    return -1;
  }
  LOG_Frame_()
  {

  }
~ LOG_Frame_()
  {

  }
  void clear()
  {
    Systemdata_.clear ();
    UTC.cclear ();
    Date_t.cclear ();
  }
}
/**
* a frame data (UTC Date_t RMC_Status Systemdata_)
**/
LOG_Frame;

/**
 * @brief The QLOG_CFG class filter cfg  NMEA_Base::NMEA_Sat_Info
 */
class QNMEA_EXTERN QLOG_CFG
{
 public:
  NMEA_Base::NMEA_Sat_Info Sinfo;//filter cfg
};

/**
 * @brief The QLOG_Data class QGNSS log data source
 */
class QNMEA_EXTERN QLOG_Data
{
 public:
  QLOG_Data();
  ~QLOG_Data();
  typedef struct plotdata
  {
    NMEA_Base::NMEA_Time UTC;
    NMEA_Base::NMEA_Data date;
    Base_data *data;
    size_t RMC_Index;//RMC index  index in oreder to RMC
    NMEA_Base::NMEA_Status RMC_Status;
    plotdata()
    {
      data=nullptr;
      RMC_Index=0;
    }
    ~plotdata()
    {
      if(data)
      {
        delete data;
        data=nullptr;
      }
    }
  }
  /**
    * plotData  UTC date data RMC_Index RMC_Status
    * */
  PlotData;

  typedef struct signaldata
  {
    char signalid;
    std::vector<PlotData*> plotdatas;
    signaldata(char sid)
    {
      signalid=sid;
    }
    bool isEmpty()
    {
      return plotdatas.size ()==0;
    }
    void clear()
    {
      for(auto it:plotdatas)
      {
        delete  it;
      }
      FMT_LOG ("plotdatas  size: {:d} \n",plotdatas.size ());
      plotdatas.clear ();
    }
    ~signaldata()
    {
      clear ();
    }
  }
  /**
    * a signal all plotData
    * */
  SignalData;

  typedef struct sysdata
  {
    NMEA_Base::NMEA_System_ID sysid;
    std::vector<SignalData*> signaldatas;
    sysdata(NMEA_Base::NMEA_System_ID sys_id)
    {
      sysid=sys_id;
    }
    bool isEmpty()
    {
      return signaldatas.size ()==0;
    }
    int  find(char sgid)
    {
      for(int i=0;i<signaldatas.size ();i++)
      {
        if(sgid==signaldatas[i]->signalid)
        {
          return i;
        }
      }
      return NULLVALUE;
    }
    std::vector<char > get_ids()
    {
      std::vector<char > ids;
      for(auto i:signaldatas)
      {
        ids.push_back (i->signalid);
      }
      return ids;
    }
    void clear()
    {
      for(auto it:signaldatas)
      {
        delete  it;
      }
      signaldatas.clear ();
    }
    ~sysdata()
    {
      clear ();
    }
    void push_back(char sgid,PlotData *plotdata)
    {
      int idx=find(sgid);
      if(idx==NULLVALUE)
      {
        signaldatas.push_back (new SignalData(sgid));
        idx=signaldatas.size ()-1;
      }
      signaldatas[idx]->plotdatas.push_back (plotdata);
    }
  }
  /**
    * a sys all signal plotdatas
    * */
  SysData;
  void clear()
  {
    for(auto it:sysdatas)
    {
      delete  it;
    }
    sysdatas.clear ();
  }
  int  find(NMEA_Base::NMEA_System_ID sysid)
  {
    for(int i=0;i<sysdatas.size ();i++)
    {
      if(sysid==sysdatas[i]->sysid)
      {
        return i;
      }
    }
    return NULLVALUE;
  }
  SysData GetSystemData(NMEA_Base::NMEA_System_ID sys_id);
  std::vector<NMEA_Base::NMEA_System_ID > get_sysids()
  {
    std::vector<NMEA_Base::NMEA_System_ID > ids;
    for(auto i:sysdatas)
    {
      ids.push_back (i->sysid);
    }
    return ids;
  }
  void pushback(NMEA_Base::NMEA_System_ID sysid, char sgid,PlotData *plotdata)
  {
    int idx=find (sysid);
    if(idx==NULLVALUE)
    {
      sysdatas.push_back (new SysData(sysid));
      idx=sysdatas.size ()-1;
    }
    sysdatas[idx]->push_back (sgid,plotdata);
  }
  /**
   * @brief sysdatas QGNSSlog source
   */
  std::vector<SysData*> sysdatas;
};

/**
 * @brief The QLOG_Data_Center class qlog data analyze center
 * log frame datas struct is list sysdata  which a index with all sysdatas
 *
 * log data is a convenience struct all sys data with all signal plotDatas
 * (because qcustomplot api privode list data which can fast show)
 */
class QNMEA_EXTERN QLOG_Data_Center
{
 public:
  QLOG_Data_Center();
  ~QLOG_Data_Center();
  static void clear();

  //typedef   void(*sys_data_cb)( QLOG_Data *logdata,void *udata);
  static void * userdata_;
  static sys_data_cb sys_cb;
  static void * userstasdata_;
  static sys_status_cb sys_stas_cb;
  static void set_sys_data_cb(sys_data_cb cb,void* userdata);

  static void set_sys_status_cb(sys_status_cb cb,void* userdata);

  static void default_sys_data_cb( QLOG_Data *logdata,void *);

  static void SetQlogCFG(QLOG_CFG *cfg=nullptr);

  static QLOG_Data *logdata_;
 private:
  static size_t RMC_Index;
  static QLOG_CFG *QLOGCFG_;

  typedef struct ReserveFrame_
  {
    bool                                    IsNextFrame;
    NMEA_Base::NMEA_Time                           *UTC;
    NMEA_Base::NMEA_Data                          *data;
    NMEA_Base::NMEA_Status                        RMC_Status;
    std::list<NMEA_DT::RT_NMEA_D*>                aframe;
    std::vector<NMEA_Base::NMEA_Sentence_ID>    FrameIds;
    std::vector<NMEA_Base::NMEA_GSA*>          GSA_datas;
    std::vector<NMEA_Base::NMEA_GSV*>          GSV_datas;
    ReserveFrame_()
    {
      IsNextFrame=false;
      UTC=nullptr;
      data=nullptr;
    }

    ///查找一帧NMEA_Sentence_ID (查找到返回true 否则false)
    bool find(NMEA_Base::NMEA_Sentence_ID id)
    {
      for(auto i:FrameIds)
      {
        if(i==id)
        {
          return true;
        }
      }
      return false;
    }

    ///find sysid in GSA vector
    int find(NMEA_Base::NMEA_System_ID sysid)//? question maybe more than one
    {
      if(GSA_datas.size() > 0&&GSA_datas[0]->SystemID==NMEA_Base::NMEA_System_ID::NO)
      {
        return -2;//NMEA_System_ID::NO NMEA 3.0
      }

      for(int  i=0;i<GSA_datas.size ();i++)
      {
        if(GSA_datas[i]->SystemID==sysid)
        {
          return i;
        }
      }
      return -1;
    }

    void Resert()
    {
      IsNextFrame =false;
      if(UTC) {delete  UTC;UTC=nullptr;}
      if(data){delete data;data=nullptr;}
      for (auto tp : aframe)
      {
        delete tp;
      }
      aframe.clear();
      GSA_datas.clear ();
      GSV_datas.clear ();
      FrameIds.clear ();
    }
    /**
     * @brief AnalyzeData  Analyze a frame NMEA to log frame
     * @return
     */
    LOG_Frame* AnalyzeData();
  }
  ///保留一帧的数据 后期进行计算
  ReserveFrame;

  static LOG_Frame *SystemdataVC ;
  static ReserveFrame Read_Status_;

  /**
   * @brief read_cb read status call back
   * @param status
   */
  static void read_cb(async_read_status *status);
  static void ON_NMEA_Ready(NMEA_DT *cb);
  typedef  NMEA_DT::RT_NMEA_D  NMEA_Parse_data;
  static  std::vector<NMEA_Base::NMEA_Sentence_ID> orderdata;
  static void Get_QLOG_Data(NMEA_DT*cb);
  static void aFrame2LOGData(LOG_Frame *frame);
};
#endif // QLOG_DATA_CENTER_H
