#include "loop_thread.h"
#include <algorithm>

#define LOG(format, ...) printf((format), ##__VA_ARGS__)
loop_thread::~loop_thread() {
    if (!exitThreadPending()) {
        stop();
    }
}

int loop_thread::start(std::string name) {
    m_thread = std::thread(&loop_thread::run, this);
    pthread_setname_np(m_thread.native_handle(), name.c_str());
    return 0;
}

void loop_thread::run() {
    bool ret = true;
    while (ret) {
        ret = threadLoop();
    }
}

int loop_thread::stop() {
    LOG("Stop loop_thread!\n");
    requestThreadExit();
    m_eventListCond.notify_all();
    if (m_thread.joinable() && !isCurrentThread()) {
        m_thread.join();
    }
    return 0;
}

bool loop_thread::threadLoop() {
    thread_event event;
    {
        std::unique_lock<std::mutex> lc(m_eventListMutex);
        if (exitThreadPending()) {
            LOG("Message count:%llu have not deal!\n", m_eventList.size());
            m_eventList.clear();
            return false;
        }
        if (m_eventList.empty()) {
            LOG("Wait message to deal!\n");
            m_eventListCond.wait(lc);
            LOG("Wait message to deal finish!\n");
            return true;
        }
        int64_t whenMs = m_eventList.front().whenMs;
        int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        if (whenMs > nowMs) {
            int delayMs = whenMs - nowMs;
            LOG("Wait message delay:%d to deal!\n", delayMs);
            m_eventListCond.wait_for(lc, std::chrono::milliseconds(delayMs));
            LOG("Wait message delay:%d to deal finish!\n", delayMs);
            return true;
        }
        event = m_eventList.front();
        m_eventList.pop_front();
    }
    if (!event.msg) {
        LOG("Deal null message!\n");
    } else if (event.msg->handler != nullptr) {
        event.msg->handler(event.msg);
    } else if (m_handler != nullptr) {
        m_handler(event.msg);
    }
    return true;
}

void loop_thread::requestThreadExit() {
    std::lock_guard<std::mutex> lc(m_exitFlagMutex);
    m_exitFlag = true;
}

bool loop_thread::exitThreadPending() {
    std::lock_guard<std::mutex> lc(m_exitFlagMutex);
    return m_exitFlag;
}

bool loop_thread::isCurrentThread() {
    return std::this_thread::get_id() == m_thread.get_id();
}

int loop_thread::cancelMessage(int what) {
    std::lock_guard<std::mutex> lc(m_eventListMutex);
    if (exitThreadPending() || m_eventList.empty()) {
        return -1;
    }
    auto it = std::remove_if(m_eventList.begin(), m_eventList.end(), [what](const auto &e) { return bool(e.msg) && e.msg->what == what; });
    size_t sz = m_eventList.size();
    m_eventList.erase(it, m_eventList.end());
    return sz - m_eventList.size();
}

int loop_thread::postMessage(const std::shared_ptr<thread_msg> &msg, int delayMs) {
    std::lock_guard<std::mutex> lc(m_eventListMutex);
    if (exitThreadPending()) {
        return -1;
    }
    int64_t whenMs{0};
    if (delayMs > 0) {
        whenMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() + delayMs;
    } else {
        whenMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    auto first = std::find_if(m_eventList.begin(), m_eventList.end(), [whenMs](const auto &e) { return e.whenMs > whenMs; });
    thread_event event{whenMs, msg};
    if (first == m_eventList.begin()) {
        m_eventListCond.notify_all();
    }
    m_eventList.insert(first, event);
    return 0;
}
