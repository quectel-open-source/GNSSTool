
#if !defined( CRC32_H )
#define CRC32_H

#ifdef __cplusplus
extern "C" {
#endif
#include <cstdint>


    /*****************************************************************************
       defines and macros (scope: module-local)
    *****************************************************************************/

#define CRC32_EVAL_BUF(d,c,b,l)       d = crc32_eval(c,b,l)
#define CRC32_EVAL_STR(d,c,s,l)       d = crc32_eval(c,s,l)
#define CRC32_EVAL_INT(d,c,i)         { unsigned int val = (i); (d) = crc32_eval(c,&val,sizeof(val)); }

    /*****************************************************************************
       typedefs and structures (scope: module-local)
    *****************************************************************************/

    /*****************************************************************************
       exported variables
    *****************************************************************************/

    /*****************************************************************************
       exported function prototypes
    *****************************************************************************/

    extern unsigned int crc32_eval(unsigned int crc32val, const void* void_ptr, const unsigned int len);
#ifdef __cplusplus
}
#endif
#endif /* CRC32_H */
