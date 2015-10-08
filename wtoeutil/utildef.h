#ifndef UTILDEF_H_
#define UTILDEF_H_

#if defined( WIN32 ) && defined( _MSC_VER )
#ifdef UTILLIB
#define UTIL_EXPORT __declspec(dllexport)
#else
#define UTIL_EXPORT __declspec(dllimport)
#endif //UTILLIB
#elif defined(__GNUC__)
#define UTIL_EXPORT __attribute__((visibility("default")))
#endif 

#endif
