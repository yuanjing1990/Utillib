#ifndef UTILDEF_H_
#define UTILDEF_H_

#if defined(WIN32) && defined(_MSC_VER)
#ifdef UTILLIB
#define UTIL_EXPORT __declspec(dllexport)
#else
#define UTIL_EXPORT __declspec(dllimport)
#endif //UTILLIB
#elif defined(__GNUC__)
#define UTIL_EXPORT __attribute__((visibility("default")))
#endif

#define interface struct

/**
 * Macro for print debug info
 * @see DEBUG_PRINT
 */
#ifdef _DEBUG
#define DEBUG_PRINT(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif //_DEBUG

#endif // UTILDEF_H_
