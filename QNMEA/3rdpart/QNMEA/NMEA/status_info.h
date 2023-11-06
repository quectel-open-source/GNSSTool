/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         status_info.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-08-08      victor.gong
* ***************************************************************************/
#ifndef STATUS_INFO_H
#define STATUS_INFO_H

/**
 * @brief The Status_Info class read file status
 */
class Status_Info
{
 public:
  Status_Info();

  bool is_Reading()
  {
    return Reading;
  }

  void set_read_status(bool rdg)
  {
    Reading=rdg;
  }

  float Progress(){ return Read_Progress;}
 private:
  bool Reading=false;

  float Read_Progress=0.0;
};

#endif // STATUS_INFO_H
