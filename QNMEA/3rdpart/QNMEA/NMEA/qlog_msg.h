/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         qlog_msg.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-07-24      victor.gong
* ***************************************************************************/

#ifndef QLOG_MSG_H
#define QLOG_MSG_H
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include "qnmea.h"


#define FMT_LOG(format, ...) \
  //QNMEA::fmtlog(FMT_STRING(format), __VA_ARGS__)

#define MAX_SENTENCE_SIZE 256
class QNMEA_EXTERN QLOG_MSG
{
  // public:
 private:
  static void    print( const char *fmt, ...)
  {
#ifdef PRINT_CONSOLE
    va_list args;
    va_start (args,fmt);
    vprintf(fmt,args);
    va_end(args);
    return ;
#elif defined(PRINT_STR)

    char *strbuf=(char*)malloc(sizeof(char)*MAX_SENTENCE_SIZE) ;
    va_list ap;
    int n=0;
    va_start(ap, fmt);
    n=vsprintf (strbuf, fmt, ap);
    va_end(ap);
    return ;
#else
    return ;
#endif
  }
  QLOG_MSG()=delete ;
};

namespace  QNMEA
{
QNMEA_EXTERN inline void  vlog( fmt::string_view format,fmt::format_args args)
{
#ifdef PRINT_CONSOLE
  fmt::vprint (format, args);
#elif defined(PRINT_STR)
 // fmt::vformat (format, args);
#endif
}

template <typename S, typename... Args>
QNMEA_EXTERN inline void fmtlog( const S& format, Args&&... args)
{
  //vlog( format, fmt::make_format_args(args...));
}




} //QNMEA

#endif // QLOG_MSG_H
