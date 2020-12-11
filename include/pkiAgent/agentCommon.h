
#ifndef THIS_IS_COMMON_HEADER_201911261537
#define THIS_IS_COMMON_HEADER_201911261537

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #ifdef  PKIAGENT_EXPORT
        #define EXPORT_PKIAGENT extern "C" _declspec(dllexport)
    #else
        #define EXPORT_PKIAGENT extern "C" _declspec(dllimport)
    #endif
#else
	#define EXPORT_PKIAGENT extern "C"
#endif

#ifdef _WIN32
typedef   __int64    int64 ;
#else
typedef   int64_t    int64  ;
#endif


#endif