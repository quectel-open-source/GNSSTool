#include "QG_Print.h"


QG_NAMESPACE_BEGAIN
std::mutex print_mutex;
print_cb cb = NULL;

int buffsize = 10*1024;

 void set_print_cb(print_cb pcb)
{
    cb = pcb;
}

 void set_buffsize(int size)
 {
     buffsize = size;
 }

 /// signal parameter support
 void print(MsgType type, const char* fmt, ...)
 {
     QG_PRINT_LOCK
     char msgprefix[PREFIXSIZE]{0};
     FILE* stream = stdout;
     switch (type)
     {
     case MsgType::DEBUG_:
         std::strcpy(msgprefix,"[DEBUG]");
         break;

     case MsgType::INFO_:
         std::strcpy(msgprefix, "[INFO]");
         break;

     case MsgType::WARNING_:
         std::strcpy(msgprefix, "[WARNING]");
         stream = stderr;
         break;

     case MsgType::ERROR_:
         std::strcpy(msgprefix, "[ERROR]");
         stream = stderr;
         break;
     default:
         std::strcpy(msgprefix, "[DEBUG]");
         break;
     }
     std::strcat(msgprefix, fmt);

     char* msg = (char*)QG_C_MALLOC(buffsize);
     va_list list;
     va_start(list, fmt);
     vfprintf(stream, msgprefix, list);
     int cout = vsnprintf(msg, buffsize, fmt, list);
     va_end(list);
     fflush(stream);
     if(msg)
     msg[cout] = '\0';
     if (cb)
     {
         cb(type, msg, cout);
     }
 };
 ///multiple parameter support
 void mprint_c(MsgType type, const char* fmt, ...)
 {
     QG_PRINT_LOCK
         //char msgprefix[PREFIXSIZE]{ 0 };
         std::string msgprefix;
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

     va_list list;
     va_start(list, fmt);
     const char* sr = std::strchr(fmt, '%');
     int index = 0;
     int lindex = 0;
     while (sr)
     {
         lindex = sr - fmt ;//get % index
         msgprefix.append(fmt + index, lindex - index);
         index = lindex + 2;
         sr++;
         char fmtc = sr[0];
         switch (fmtc)
         {
         case 's':
         case 'S':
         {
          char *strtemp=   va_arg(list, char*);
          if(strtemp)
          msgprefix.append(strtemp);
          break;
         }
         case 'd':
         case 'D':
         {
             int  dvalue = va_arg(list, int);
             char strtemp[10]{ 0 };
             auto err= _itoa_s(dvalue, strtemp, 10);
             if (!err)
             msgprefix.append(strtemp);
             break;
         }
         case 'f':
         case 'F':
         {
             double  dvalue = va_arg(list, double);
             std::string strtemp = std::to_string(dvalue);
             if (!strtemp.empty())
             msgprefix.append(strtemp);
             break;
         }
         case 'x':
         {
             int  dvalue = va_arg(list, int);
             char strtemp[10]{ 0 };
             auto err= _itoa_s(dvalue, strtemp, 16);
             if (!err)
             {
                 std::transform(strtemp, strtemp + 8, strtemp, std::tolower);
                 msgprefix.append(strtemp);
             }
             break;
         }
         case 'X':
         {
             int  dvalue = va_arg(list, int);
             char strtemp[10]{ 0 };
             auto err= _itoa_s(dvalue, strtemp, 16);
             if (!err)
             {
                 std::transform(strtemp, strtemp + 8, strtemp, std::toupper);
                 msgprefix.append(strtemp);
             }
             break;
         }
         default:
             break;
         }
         sr = std::strchr(sr, '%');

         if (!sr)
         {
             msgprefix.append(fmt + index, std::strlen(fmt) - (index));
         }
         
     }
     if (!index)
     {
         msgprefix.append(fmt, std::strlen(fmt));
     }
     va_end(list);

     #ifndef QG_OUTPUT_DEBUG
          fprintf(stream,"%s", msgprefix.c_str());
          fflush(stream);
     #endif

     //callback func
     if (cb)
     {
         cb(type, msgprefix.c_str(), msgprefix.size());
     }
 };
 QG_NAMESPACE_END
