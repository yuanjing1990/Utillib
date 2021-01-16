#include <ctime>

#include "yjlog.h"

namespace yjutil {

clock_t _Tick::m_start = 0;
bool _Tick::m_bflag = 0;

clock_t _Tick::start() {
    if (m_bflag)
        return 0;
    m_bflag = true;
    m_start = clock();
    return m_start;
}

clock_t _Tick::start_out() {
    if (m_bflag) {
        DEBUG_PRINT("Last time counter have not been stopped!");
        return 0;
    }
    DEBUG_PRINT("Start time counter---------------");
    m_bflag = true;
    m_start = clock();
    return m_start;
}

clock_t _Tick::end() {
    if (!m_bflag) {
        DEBUG_PRINT("Last time counter have not been started!");
        return 0;
    }
    printf("time elapse：%f s\n", (double)(clock() - m_start) / 1000);
    m_bflag = false;
    m_start = 0;
    return 0;
}

clock_t _Tick::end_out() {
    end();
    DEBUG_PRINT("Stop time counter---------------");
    return 0;
}

} // end namespace yjutil