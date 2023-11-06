/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         nmea_data.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-23      victor.gong
* ***************************************************************************/

#ifndef NMEA_DATA_H
#define NMEA_DATA_H
#include "nmea_base.h"
/**
 * @brief The NMEA_DT  存储NMEA的单向链表(NMEA_List) 保存解析后的NMEA语句(RT_NMEA_D)
 */
class QNMEA_EXTERN NMEA_DT
{
 public:
  NMEA_DT();
  typedef  struct RT_NMEA_D_
  {
    NMEA_Base::NMEA_Sentence_ID ID;
    NMEA_Base::NMEA_SBASE *nmea_base;
    ///默认构造
    RT_NMEA_D_()
    {
      ID=NMEA_Base::NMEA_Sentence_ID::NMEA_SENTENCE_NO;
      nmea_base=nullptr;
    }
    ///默认构造 指定语句
    RT_NMEA_D_(const NMEA_Base::NMEA_Sentence_ID ID_,NMEA_Base::NMEA_SBASE *nmea_base_)
    {
      ID=ID_;
      nmea_base=nmea_base_;
    }
    RT_NMEA_D_& operator=(RT_NMEA_D_&&rt)
    {
      if (this != &rt)
      {
        ID=rt.ID;
        nmea_base=rt.nmea_base;
        rt.nmea_base=nullptr;
      }
      return *this;
    }
    ~ RT_NMEA_D_()
    {
      if(nmea_base)
      {
        delete nmea_base;
      }
    }
  }RT_NMEA_D;

  typedef struct NMEA_List_
  {
    RT_NMEA_D RT_NEMA;
    NMEA_List_ *Next;
    NMEA_List_()
    {
      Next=nullptr;
    }
  } NMEA_List;

  ///暴露外部的操作地址
  NMEA_List *operate_address=nullptr;
  void free_();
 private:
  NMEA_List *list_=nullptr;

  ///内部清除地址
  NMEA_List *startaddress_=nullptr;
  int list_size_=0;
  ~NMEA_DT();
 public:

  int size(){return list_size_;}

  /**
   *拼接数据
  */
  template<typename RT_NEMA>
  void append(RT_NEMA &&RTNEMA);
  template<> void append<RT_NMEA_D>(RT_NMEA_D &&RTNEMA)
  {
    if(!RTNEMA.nmea_base)
    {
      return;
    }
    if(!list_)
    {
      list_=new NMEA_List;
      startaddress_=list_;
      operate_address=list_;
    }
    list_->RT_NEMA=std::forward<RT_NMEA_D> (RTNEMA);
    if(!list_->Next)
    {
      list_->Next=new NMEA_List_;
      list_=list_->Next;
    }
    ++list_size_;
  }

  ///清除
  void clear()
  {
    while(startaddress_)
    {
      auto temp=startaddress_;
      startaddress_=startaddress_->Next;
      delete temp;
      list_size_--;
    }
    list_=nullptr;
    operate_address=nullptr;
    list_size_=0;
  }
};
#endif // NMEA_DATA_H
