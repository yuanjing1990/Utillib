#include "CircularQueue.hpp"
#include <chrono>
#include <string.h>

namespace wtoeutil {
CCircularQueue::CCircularQueue()
    : m_buf(nullptr), m_bufcapacity(0), m_write_ptr(0), m_read_ptr(0), m_iswork(false), m_curstatus(0) {
}

CCircularQueue::~CCircularQueue() {
}

bool CCircularQueue::init(uint32_t len) {
    if (0 == len) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_buf == nullptr) {
        m_buf = new uint8_t[len];
    } else if (len != m_bufcapacity) {
        delete[] m_buf;
        m_buf = nullptr;
        m_buf = new uint8_t[len];
    }

    if (m_buf == nullptr) {
        return false;
    }

    m_bufcapacity = len;
    m_read_ptr = m_write_ptr = 0;

    m_iswork = true;
    m_curstatus = 0;

    return true;
}

void CCircularQueue::fini() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_iswork = false;
        m_curstatus = 0;
    }

    m_cond.notify_all();

    m_bufcapacity = 0;
    m_read_ptr = m_write_ptr = 0;

    if (m_buf) {
        delete[] m_buf;
        m_buf = nullptr;
    }
}

int32_t CCircularQueue::pushdata(uint8_t *buf, uint32_t len) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_buf || !m_iswork) {
            return 0;
        }

        if (m_curstatus == 2) {
            DEBUG_PRINT("CCircularQueue pushdata overflow0...");
            return -2;
        }

        uint8_t *writePos = m_buf + m_write_ptr;

        if ((m_write_ptr + len) > m_bufcapacity) {
            // 超出尾部,且空间不足.
            if (m_write_ptr + len - m_bufcapacity > m_read_ptr) {
                DEBUG_PRINT("CCircularQueue pushdata overflow1...");
                return -2;
            }
            // 超出尾部,且空间足够.
            memcpy(writePos, buf, sizeof(uint8_t) * (m_bufcapacity - m_write_ptr));
            memcpy(m_buf, buf + (m_bufcapacity - m_write_ptr), sizeof(uint8_t) * (len - (m_bufcapacity - m_write_ptr)));
        } else {
            // 未超出尾部,但空间不足
            if (m_read_ptr > m_write_ptr && m_write_ptr + len > m_read_ptr) {
                //std::cout << "CCircularQueue pushdata overflow2..." << std::endl;
                return -2;
            }
            memcpy(writePos, buf, sizeof(uint8_t) * len);
        }
        m_write_ptr = (m_write_ptr + len) % m_bufcapacity;

        if (m_write_ptr == m_read_ptr)
            m_curstatus = 2;
        else
            m_curstatus = 1;
    }

    m_cond.notify_all();

    return len;
}

int32_t CCircularQueue::popdata(uint8_t *buf, uint32_t len) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_buf || !m_iswork) {
        return -1;
    }

    if (m_write_ptr == m_read_ptr && m_curstatus != 2) {
        if (m_cond.wait_for(m_mutex, std::chrono::milliseconds(100)) == std::cv_status::timeout) {
            // 等待.
            return -1;
        }
        if (false == m_iswork) {
            return -1;
        }

        return 0;
    }

    // 有多少,就读多少.
    uint32_t existingLen = len;

    uint8_t *src = m_buf + m_read_ptr;

    bool wrap = false;

    uint32_t pos = m_write_ptr;

    if (pos <= m_read_ptr) {
        pos += m_bufcapacity;

        if (m_read_ptr + len > m_bufcapacity)
            wrap = true;
    }

    if (m_read_ptr + len > pos)
        existingLen = pos - m_read_ptr;
    else
        existingLen = len;

    if (wrap) {
        wrap = false;

        if (m_read_ptr + existingLen > m_bufcapacity)
            wrap = true;
    }

    if (wrap) {
        memcpy(buf, src, sizeof(uint8_t) * (m_bufcapacity - m_read_ptr));
        memcpy(buf + (m_bufcapacity - m_read_ptr), m_buf, sizeof(uint8_t) * (existingLen - (m_bufcapacity - m_read_ptr)));
    } else {
        memcpy(buf, src, sizeof(uint8_t) * existingLen);
    }
    m_read_ptr = (m_read_ptr + existingLen) % m_bufcapacity;

    if (m_write_ptr == m_read_ptr)
        m_curstatus = 0;
    else
        m_curstatus = 1;

    return existingLen;
}

void CCircularQueue::reset(bool stopwork) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (stopwork) {
            m_iswork = false;
        }
        m_curstatus = 0;
        m_read_ptr = m_write_ptr = 0;
    }

    m_cond.notify_all();
}

uint32_t CCircularQueue::getsize() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_curstatus == 2)
        return m_bufcapacity;
    else if (m_curstatus == 0)
        return 0;
    else {
        if (m_read_ptr < m_write_ptr)
            return m_write_ptr - m_read_ptr;
        else
            return m_bufcapacity - m_read_ptr + m_write_ptr;
    }
}
} // namespace wtoeutil
