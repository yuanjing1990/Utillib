#ifndef YJLOG_H_
#define YJLOG_H_

#include <iostream>
#include <iterator>

#include "yjdef.h"

namespace yjutil {

// ==========log util class===========->Begin
/**
 * @addtogroup print_tools
 * @{
 */

/**
 * @brief Calculate time period for program.
 * Calculate time period for program,and print start and stop
 * info
 *
 */
struct _Tick {
    /**
	 * @brief Start the clock.
	 *
	 * @return The start clock in milisecond.
	 *
	 */
    static clock_t start();

    /**
	 * @brief Start the clock and print info.
	 *
	 * @return The start clock in minisecond.
	 *
	 */
    static clock_t start_out();

    /**
	 * @brief Stop the clock and print result.
	 *
	 * @return The end clock in minisecond.
	 *
	 */
    static clock_t end();

    /**
	 * @brief Print the clock and print result.
	 * Print the clock and print result,and then print clock will stop.
	 *
	 * @return zero,which is useless
	 *
	 */
    static clock_t end_out();

  private:
    static clock_t m_start;
    static bool m_bflag;
};

/**
 * @brief print stl vector type collection.
 * print stl collection, which have begin() and end() member
 *
 * @param vec,the collection will be print.
 *
 */
template <class T>
void print(const T &vec) {
    using namespace std;
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<typename T::value_type>(cout, " "));
    cout << endl;
}

/**
 * @}
 */
//==========log util class===========-<End

} // namespace yjutil

#endif