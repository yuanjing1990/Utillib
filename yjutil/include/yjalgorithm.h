#ifndef YJALGORITHM_H_
#define YJALGORITHM_H_

#include <stddef.h>

namespace yjutil {

/**
 * @addtogroup algorithm_tools
 * @{
 */

/**
 * @brief Sequential search in the vector for a value
 *
 * @param vec is the collection to be search
 * @param e is the value be search
 *
 * @return the iterator of e in vec,if not found,vec.end() will be return
 *
 */
template <class T, class VT>
typename T::iterator seq_search(T &vec, VT e) {
    typename T::iterator _it = vec.begin();
    while (_it != vec.end()) {
        if (*_it == e)
            return _it;
        ++_it;
    }
    return _it;
}

/**
 * @brief Sequential search in the vector for a elem which satified with a condition
 *
 * @param vec is the collection to be search
 * @param _op is the conditional function
 *
 * @return the iterator of elem in vec which satify the condition,if not found,vec.end()
 * 		   will be return
 *
 */
template <class T, class Op>
typename T::iterator seq_search_if(T &vec, Op _op) {
    typename T::iterator _it = vec.begin();
    for (_it; _it != vec.end(); ++_it) {
        if (_op(*_it))
            return _it;
    }
    return _it;
}

/**
 * @brief Binary search in the vector for a value
 *
 * @param vec is the collection to be search
 * @param _op is the conditional function
 *
 * @return the iterator of elem in vec,if not found,vec.end() will be return
 *
 */
template <class T>
bool binary_search(T &vec, const typename T::value_type &e) {
    size_t _up_pos = vec.size() - 1;
    size_t _down_pos = 0;
    size_t _cur_pos = (_up_pos + _down_pos) / 2;
    typename T::value_type _value = vec.at(_cur_pos);
    while (_value != e) {
        if (_value < e) {
            _down_pos = _cur_pos + 1;
        } else {
            _up_pos = _cur_pos - 1;
        }
        if ((int)_up_pos - (int)_down_pos < 0)
            break;
        _cur_pos = (_up_pos + _down_pos) / 2;
        _value = vec.at(_cur_pos);
    }
    if ((int)_up_pos - (int)_down_pos < 0)
        return false;
    return true;
}

/**
 * @}
 */

} // namespace yjutil

#endif