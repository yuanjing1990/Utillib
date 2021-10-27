#include "loop_thread.h"
#include <algorithm>

loop_thread::~loop_thread() {
    if (!exitThreadPending()) {
        stop();
        if (!isCurrentThread()) {
            m_thread.join();
        }
    }
}

int loop_thread::start(std::string name) {
    m_thread = std::thread(&loop_thread::run, this);
    pthread_setname_np(m_thread.native_handle(), name.c_str());
    return 0;
}

void loop_thread::run() {
    bool ret = true;
    while (!exitThreadPending() && ret) {
        ret = threadLoop();
    }
}

int loop_thread::stop() {
    requestThreadExit();
    m_eventListCond.notify_all();
    return 0;
}

bool loop_thread::threadLoop() {
    thread_event event;
    {
        std::unique_lock<std::mutex> lc(m_eventListMutex);
        if (exitThreadPending()) {
            m_eventList.clear();
            return false;
        }
        if (m_eventList.empty()) {
            m_eventListCond.wait(lc);
            return true;
        }
        int64_t whenMs = m_eventList.front().whenMs;
        int64_t nowMs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        if (whenMs > nowMs) {
            int delayMs = whenMs - nowMs;
            m_eventListCond.wait_for(lc, std::chrono::milliseconds(delayMs));
            return true;
        }
        event = m_eventList.front();
        m_eventList.pop_front();
    }
    if (m_handler != nullptr) {
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
    m_eventList.erase(std::remove_if(m_eventList.begin(), m_eventList.end(), [what](const auto &e) { return e.msg != nullptr && e.msg->what == what; }));
    return 0;
}

int loop_thread::postMessage(const std::shared_ptr<thread_msg> &msg, int delayMs) {
    std::lock_guard<std::mutex> lc(m_eventListMutex);
    if (exitThreadPending()) {
        return -1;
    }
    int64_t whenMs{0};
    if (delayMs > 0) {
        whenMs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() + delayMs;
    } else {
        whenMs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    auto first = std::find_if(m_eventList.begin(), m_eventList.end(), [whenMs](const auto &e) { return e.whenMs > whenMs; });
    thread_event event{whenMs, msg};
    if (first == m_eventList.begin()) {
        m_eventListCond.notify_all();
    }
    m_eventList.insert(first, event);
    return 0;
}