#ifndef CIRCULARQUEUE_H_
#define CIRCULARQUEUE_H_

#include "utildef.h"
#include <iostream>
#include <boost/thread.hpp>

namespace wtoeutil 
{
    class UTIL_EXPORT CCircularQueue
    {
    public:
        CCircularQueue();

        ~CCircularQueue();
    public:
        bool init( uint32_t len );

        void fini();

        int32_t pushdata( uint8_t *buf, uint32_t len );

        // 还是按len来读,但是不等len满,直接读完.
        int32_t popdata( uint8_t *buf, uint32_t len );

        void reset( bool stopwork );

        uint32_t getsize();
    private:
        uint8_t *m_buf;
        uint32_t m_bufcapacity;
        uint32_t m_write_ptr;
        uint32_t m_read_ptr;
        bool m_iswork;
        uint8_t m_curstatus;//0空 1正常 2满
        boost::mutex m_mutex;
        boost::condition_variable_any m_cond;
    };

}
#endif//CIRCULARQUEUE_H_
