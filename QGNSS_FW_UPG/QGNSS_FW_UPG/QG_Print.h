#ifndef __QG_PRINT_H__
#define __QG_PRINT_H__
#include <iostream>
#include <type_traits>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <mutex>
#include <functional>
#include <cstring>
#include <string>
#include <algorithm>
#include "QG_Common.h"

QG_NAMESPACE_BEGAIN

#define PREFIXSIZE 1024
#define QG_PRINT_LOCK std::lock_guard<std::mutex> lock(print_mutex);



//print type
enum  MsgType
{
    NONE_=-1,
    DEBUG_,
    INFO_,
    WARNING_,
    ERROR_,
    PROGRESS_ //progress info [Progress]*info*total*100*now*10.0
};
//multithread safe
//typedef void(*print_cb)(MsgType msgtype, const char* msg, std::size_t size) ;
typedef std::function<void(MsgType msgtype, const char* msg, std::size_t size)> print_cb;
QG_EXTERN print_cb cb ;
QG_EXTERN std::mutex print_mutex;
QG_EXTERN int buffsize;
QG_EXTERN QG_PORTEXTERN  void set_print_cb(print_cb pcb);
QG_EXTERN QG_PORTEXTERN  void set_buffsize(int size);

[[deprecated("Use mprint(MsgType type, const char* fmt, ...) this only support one parameter")]]
QG_EXTERN QG_PORTEXTERN  void print(MsgType type, const char* fmt, ...);

QG_EXTERN QG_PORTEXTERN  void mprint_c(MsgType type, const char* fmt, ...);



template<typename ... args>
 void mprint(MsgType type,const char* fmt,const args... args_)
{
     QG_PRINT_LOCK
     std::string msgprefix ;
     FILE* stream = stdout;
     switch (type)
     {
     case  MsgType::NONE_:
         msgprefix.append("");
         break;

     case MsgType::DEBUG_:
         msgprefix.append("[DEBUG] ");
         break;

     case MsgType::INFO_:
         msgprefix.append("[INFO] ");
         break;

     case MsgType::WARNING_:
         msgprefix.append("[WARNING] ");
         stream = stderr;
         break;

     case MsgType::ERROR_:
         msgprefix.append("[ERROR] ");
         stream = stderr;
         break;
     case MsgType::PROGRESS_:
         msgprefix.append("[Progress] ");
         stream = stderr;
         break;
     default:
         msgprefix.append("");
         break;
     }
     char temp_c[1024];
     std::sprintf(temp_c,fmt, args_...);
     msgprefix.append(temp_c,strlen(temp_c));
#ifndef QG_OUTPUT_DEBUG
    std::fprintf(stream, fmt, args_...);
#endif
    if (cb)
    {
        cb(type, msgprefix.c_str(), msgprefix.size());
    }
}

QG_NAMESPACE_END

#ifdef QG_OUTPUT_DEBUG
#define DEBUG_CODE         QG::mprint(QG::MsgType::NONE_,"[FUNC] %s [LINE] %d ",__FUNCTION__,__LINE__)
#else
#define DEBUG_CODE
#endif // QG_OUTPUT_DEBUG

#define QGPRINT(fmt,...)               QG::mprint(QG::MsgType::NONE_,fmt, ## __VA_ARGS__) 
#define QGDEBUG(fmt,...)   //DEBUG_CODE; QG::mprint(QG::MsgType::DEBUG_, fmt, ## __VA_ARGS__) 
#define QGINFO(fmt,...)                QG::mprint(QG::MsgType::INFO_,fmt, ## __VA_ARGS__)   
#define QGWARNING(fmt,...) DEBUG_CODE; QG::mprint(QG::MsgType::WARNING_,fmt, ## __VA_ARGS__)
#define QGERROR(fmt,...)   DEBUG_CODE; QG::mprint(QG::MsgType::ERROR_,fmt, ## __VA_ARGS__)
#define QGPROGRESS(fmt,...)            QG::mprint(QG::MsgType::PROGRESS_,fmt, ## __VA_ARGS__)

#define QGPROGRESS_FMT(info,total,rate,now) printf("\r"); QGPROGRESS("*%s*total*%d*rate*%s*progress*%d",info,total,rate,now);now==total?printf(QG_ENDL):printf("")

#endif // !__QG_PRINT_H__
