/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         QLOG.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-24      victor.gong
* ***************************************************************************/

#ifndef qnmea_H
#define qnmea_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
/* Windows - set up dll import/export decorators. */
# if defined(BUILDING_QNMEA_SHARED)
/* Building shared library. */
#   define QNMEA_EXTERN __declspec(dllexport)
# elif defined(USING_QNMEA_SHARED)
/* Using shared library. */
#   define QNMEA_EXTERN __declspec(dllimport)
# else
/* Building static library. */
#   define QNMEA_EXTERN /* nothing */
# endif
#elif __GNUC__ >= 4
# define QNMEA_EXTERN __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550) /* Sun Studio >= 8 */
# define QNMEA_EXTERN __global
#else
# define QNMEA_EXTERN /* nothing */
#endif

//##############QLOG data Region##########################
/**
* 1. QNMEA_NEW()
* 2. QNMEA_DELETE()
* 3. set_qlog_data_cb()
* 4. set_qlog_status_cb()
* 5. Set_Read_PKG()
* 6. Set_Qlog_cfg()
* 7. Read_log()
* */

///qlog data qgnsslog 需要的数据结构体
struct QLOG_Data;

///读取状态信息
struct async_read_status;
///过滤结构体
struct QLOG_CFG;

/**
 *数据接收回调
 */
typedef  void(*sys_data_cb)(QLOG_Data *logdata,void *udata);
/**
 *状态接收回调
 */
typedef  void(*sys_status_cb)(async_read_status *status,void *udata);

/**
 * @brief QNMEA_NEW  初始化 数据
 */
QNMEA_EXTERN void QNMEA_NEW();
/**
 * @brief QNMEA_DELETE 删除数据
 */
QNMEA_EXTERN void QNMEA_DELETE();

/**
 * @brief set_qlog_data_cb 设置数据接回调 解析完数据后 触发
 * @param 回调函数sys_data_cb  用户自定义userdata  用于传输非static类指针到static 域
 */
QNMEA_EXTERN void set_qlog_data_cb(sys_data_cb,void* userdata);
/**
 * @brief set_qlog_status_cb 设置解析状态回调函数 会按照Set_Read_PKG 里的大小 为阈值触发。
 * @param sys_status_cb 状态回调函数 userdata   用于传输非static类指针到static 域
 */
QNMEA_EXTERN void set_qlog_status_cb(sys_status_cb,void* userdata);
/**
 * @brief Read_log 输入路径 开始解析(先清除上次缓存的数据)
 */
QNMEA_EXTERN void Read_log(const char *);

QNMEA_EXTERN void Set_Read_PKG(int PKG_Size/*Bit*/);//1m =1024*1024*1
QNMEA_EXTERN void Set_Qlog_cfg(QLOG_CFG *logcfg);
//########################################################

#ifdef __cplusplus
}
#endif
enum Read_Status
{
  BegianRead,
  Running,
  Stoped
};

/**
 * @brief The async_read_status struct
 * ReadStatus  is_lastframe progress totalfilesize totalreadsize
 */
struct async_read_status
{
  Read_Status ReadStatus;
  bool is_lastframe;
  float progress;
  unsigned int totalfilesize;
  unsigned int totalreadsize;
  async_read_status()
  {
    Resert();
  }
  void Resert()
  {
    ReadStatus=Read_Status::BegianRead;
    is_lastframe=false;
    progress=0.0;
    totalfilesize=0;
    totalreadsize=0;
  }
};

#endif // qnmea_H
