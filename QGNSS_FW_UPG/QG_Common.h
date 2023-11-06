#ifndef __QG_COMMON_H__
#define __QG_COMMON_H__


#define QG_NAMESPACE_BEGAIN \
namespace QG  {

#define QG_NAMESPACE_END \
    }

//QG_FW_UPG key
#define QG_INLINE inline
#define QG_EXTERN extern 
#define QG_CNSTEXPR constexpr  
#define QG_C_MALLOC  std::malloc

#ifdef _WIN32 
    #define QG_WIN
    #include <Windows.h>
    #define QG_Handle HANDLE 
    #define QG_ENDL  "\r\n"
#elif defined __linux__
    #define QG_ENDL  "\n"
    #define QG_LINUX
#elif  defined __ANDROID__
    #define QG_ANDROID
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "crc32.h"
#ifdef _WIN32
    /* Windows - set up dll import/export decorators. */
# if defined(BUILDSHARED)
    /* Building shared library. */
    #define QG_PORTEXTERN __declspec(dllexport)
# elif defined(USING_BUILDSHARED)
    /* Using shared library. */
    #define QG_PORTEXTERN __declspec(dllimport)
# else
    /* Building static library. */
    #define QG_PORTEXTERN /* nothing */
# endif
#elif __GNUC__ >= 4
    #define QG_PORTEXTERN __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550) /* Sun Studio >= 8 */
    #define QG_PORTEXTERN __global
#else
    #define QG_PORTEXTERN /* nothing */
#endif


//macro

 //calculate array size macro
#define ARRAY_SIZE(array_,type_) \
(sizeof(array_)/sizeof(type_))
//cmd start char
#define CMDSTARTCHAR '-'


#ifdef __cplusplus
}
#endif

//some helpful c++ func 
#include <type_traits>
#include <algorithm>
#include <cctype> 
#include <chrono>
#include <string>

QG_EXTERN QG_PORTEXTERN std::string wstringToString(const std::wstring& wstr);

QG_EXTERN QG_PORTEXTERN std::wstring StringToWString(const std::string& str);

QG_EXTERN QG_PORTEXTERN bool isLittleEndian();

QG_EXTERN QG_PORTEXTERN char QG_XOR_CheckSum(char* data, unsigned int size);

template<typename stype, typename dtype>
 dtype * QG_reverse(stype &data)
{
    dtype* chars=reinterpret_cast <dtype*>(&data);
    std::reverse(chars,chars+sizeof(stype));
    return chars;
}

 QG_EXTERN QG_PORTEXTERN bool str2hex(const std::string& str, std::string& hexstr);

//true :not over time false :over time
 QG_EXTERN QG_PORTEXTERN bool QG_overtime(std::chrono::steady_clock::time_point start,/*milliseconds*/ unsigned int totaltime);

 QG_EXTERN QG_PORTEXTERN bool QG_Str_Compare(std::string_view str1, std::string_view str2, bool casesenstive = false);

 QG_EXTERN QG_PORTEXTERN std::size_t QG_file_size(
                     const char* filename
                    ,const char* mode = "rb");

 QG_EXTERN QG_PORTEXTERN std::size_t QG_Read_File(
                    const char* filename,
                    const char* mode ,
                    std::size_t size,
                    std::string &data
                     );
 QG_EXTERN QG_PORTEXTERN int   QG_get_progress(uint32_t totalsize,uint32_t now,uint32_t progress_max);

#endif // !__QG_COMMON_H__
