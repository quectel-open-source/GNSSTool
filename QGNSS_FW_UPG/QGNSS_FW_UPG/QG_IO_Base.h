
#include <string>
#include <list>
#include <functional>
#include "QG_Print.h"
#include "QG_Common.h"

#define RETURN_ERROR_SIZE 0

class QG_PORTEXTERN QG_IO_Base
{
public:
    enum OpenMode
    {
        NotOpen=0x0000,
        ReadOnly=0x0001,
        WriteOnly=0x0002,
        ReadWrite=ReadOnly | WriteOnly,
        Append=0x0004,
        Truncate=0x0008,
        Text=0x0010,
        Unbuffered=0x0020,
        NewOnly=0x0040,
        ExistingOnly=0x0080
    };
    ~QG_IO_Base() {};

    virtual bool open(OpenMode openMode) = 0;
    virtual bool close() = 0;
    virtual bool flush() = 0;

    virtual std::size_t write(const char* wbuffer, std::size_t size) = 0;
    virtual std::size_t  read(char* rbuffer, std::size_t, std::size_t timeout) = 0;
    
    virtual QG_Handle get_native()=0;
    virtual bool set_native(QG_Handle)=0;
    //def funtion type 
   typedef std::function<void()> ReadReady_FC;
   typedef std::function<void(std::size_t size)> WriteReady_FC;
   typedef std::function<void(std::string)> ErrorMsg_FC;

   ReadReady_FC ReadReady;
   WriteReady_FC WriteReady;
   ErrorMsg_FC ErrorMsg;
};
