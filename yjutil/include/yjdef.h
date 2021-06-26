#ifndef YJDEF_H_
#define YJDEF_H_

#include <cstddef>
#include <cstdio>

// ==========tool macro===========->Begin
/**
 * @addtogroup macro_tools
 * @{
 */

/**
 * @brief Delete a ptr and set it to null
 *
 * @param p is a ptr of a certain object
 */
#define SAFE_DELETE(p) \
    if ((p) != NULL) { \
        delete p;      \
        p = NULL;      \
    }

/**
 * Macro for start _Tick's clock
 * @see TICK_BEGIN_PRINT
 */
#define TICK_BEGIN yjutil::_Tick::start();

/**
 * Macro for stop _Tick's clock
 * @see TICK_END_PRINT
 */
#define TICK_END yjutil::_Tick::end();

/**
 * Macro for start _Tick's clock and print info
 * @see TICK_BEGIN
 */
#define TICK_BEGIN_PRINT yjutil::_Tick::start_out();

/**
 * Macro for stop _Tick's clock and print info
 * @see TICK_END
 */
#define TICK_END_PRINT yjutil::_Tick::end_out();

/**
 * Macro for print debug info
 * @see DEBUG_PRINT
 */
#ifdef _DEBUG
#define DEBUG_PRINT(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif

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
 * @}
 */

// ==========tool macro===========-<End
#endif