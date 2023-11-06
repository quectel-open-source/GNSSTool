/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         async_read_log.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-20      victor.gong
* ***************************************************************************/
#ifndef ASYNC_READ_LOG_H
#define ASYNC_READ_LOG_H

#include <stdio.h>
#include <stdlib.h>

#include "uv.h"

#include "qgnss_nmea_parse.h"


class QNMEA_EXTERN Async_Read_Log
{
 public:
  Async_Read_Log();
  ~Async_Read_Log();
  void Read_LOG_Async(const char *path);

  static async_read_status *asyncData;

  typedef void(*async_Stream_NMEA_cb)(NMEA_DT *dt);

  typedef void(*readstatu_cb)(async_read_status *state);
  static void set_Default_CB(async_Stream_NMEA_cb cb);

  static void Set_Read_PKG(int PKG_Size/*B*/);//1m =1024*1024*1

  static void set_Default_read_status_cb(readstatu_cb cb);
 private:
  static size_t filesize;
  uv_loop_t* create_loop()
  {
    uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    if (loop)
    {
      uv_loop_init(loop);
    }
    return loop;
  }

  static QGNSS_NMEA_Parse NMEA_Parse;

  uv_thread_t read_td_;
  uv_thread_t readsize_td_;
  static uv_loop_t *read_loop_;

  static uv_fs_t open_req;
  static uv_fs_t read_req;

  static uv_async_t async_req;

  static uv_buf_t readbuf;
  static char *buffer;
  static char *temppath;
  static size_t readsize;

  static async_Stream_NMEA_cb  StreamNMEADatacb;
  static readstatu_cb  readstatucb;
  static void Read_Async_TD_Worker(void *path);
  static void file_size(void* filename);
  static void On_Open(uv_fs_t *fs);
  static void On_Read(uv_fs_t *fs);
  static void On_Write(uv_fs_t *req);

  static void On_Get_NMEA(uv_async_t *req);
  static void Default_CB(NMEA_DT *cb);
  static void Default_read_cb(async_read_status *status);
};

#endif // ASYNC_READ_LOG_H
