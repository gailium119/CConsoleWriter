#pragma once

#define FULL_PATH(path,name) path\\name
#define FILE_STR(file_name)#file_name
#define FILE_STR_NAME(file_name) FILE_STR(file_name)

#define INCLUDE_PATH FULL_PATH(..\\LIB_NAME,INCLUDE_NAME)
#define INCLUE_PATH_NAME FILE_STR_NAME(INCLUDE_PATH)
#ifdef _WIN64
#ifdef _M_AMD64
#define ARCH x64
#elif defined (_M_ARM64)
#define ARCH ARM64
#endif
#else
#ifdef _M_IX86
#define ARCH Win32
#elif defined (_M_ARM)
#define ARCH ARM
#endif
#endif
#ifdef _DEBUG
#define CONF Debug
#else
#define CONF Release
#endif
#define LIB_PATH FULL_PATH(..\\ARCH\\CONF,LIB_NAME.lib)
#define LIB_PATH_NAME FILE_STR_NAME(LIB_PATH)
#include INCLUE_PATH_NAME
#pragma comment(lib,LIB_PATH_NAME)
#undef FULL_PATH
#undef FILE_STR
#undef FILE_STR_NAME
#undef INCLUDE_PATH
#undef INCLUE_PATH_NAME
#undef ARCH
#undef CONF
#undef LIB_PATH
#undef LIB_PATH_NAME
#undef LIB_NAME
#undef INCLUDE_NAME