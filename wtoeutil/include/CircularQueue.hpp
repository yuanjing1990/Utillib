#ifndef CIRCULARQUEUE_HPP_
#define CIRCULARQUEUE_HPP_

#include <condition_variable>
#include <iostream>
#include <mutex>

#include "utildef.hpp"

namespace wtoeutil {
/**
     * A cycle queue.
     * CCircularQueue is a cycle queue of binary data,you can use it as a
     * buffer
     */
class UTIL_EXPORT CCircularQueue {
  public:
    CCircularQueue();
    ~CCircularQueue();

  public:
    /**
         * Init the queue with @param len.
         * Alloc the required len of the memory and init it
         */
    bool init(uint32_t len);

    /**
         * Fini the queue and release the memory
         */
    void fini();

    /**
         * Put data into queue
         * @param buf is the data ptr you want to put into the queue
         * @param len is the len of the data with byte
         */
    int32_t pushdata(uint8_t *buf, uint32_t len);

    // 还是按len来读,但是不等len满,直接读完.
    /**
         * Get data from queue
         * @param buf is the dest of the data you take from the queue
         * @param len is the len of the data you want to get
         * @retval 0 when success
         * @retval <0 when error
         */
    int32_t popdata(uint8_t *buf, uint32_t len);

    /**
         * Reset the queue
         */
    void reset(bool stopwork);

    /**
         * Get the usable size
         */
    uint32_t getsize();

  private:
    uint8_t *m_buf;
    uint32_t m_bufcapacity;
    uint32_t m_write_ptr;
    uint32_t m_read_ptr;
    bool m_iswork;
    uint8_t m_curstatus; //0空 1正常 2满
    std::mutex m_mutex;
    std::condition_variable_any m_cond;
};

} // namespace wtoeutil
#endif //CIRCULARQUEUE_H_
