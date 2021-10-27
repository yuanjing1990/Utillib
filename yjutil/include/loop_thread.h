#ifndef LOOP_THREAD_H
#define LOOP_THREAD_H
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

class loop_thread {
  public:
    loop_thread(){};
    ~loop_thread();
    struct thread_msg {
        int what;
    };
    int start(std::string name);
    int stop();
    int postMessage(const std::shared_ptr<thread_msg> &msg, int delayMs = 0);
    bool isCurrentThread();
    void run();
    void requestThreadExit();
    bool exitThreadPending();
    int cancelMessage(int what);

  private:
    struct thread_event {
        int64_t whenMs;
        std::shared_ptr<thread_msg> msg;
    };
    bool threadLoop();
    std::thread m_thread;
    bool m_exitFlag{false};
    std::mutex m_exitFlagMutex;
    std::list<thread_event> m_eventList;
    std::mutex m_eventListMutex;
    std::condition_variable m_eventListCond;
    typedef std::shared_ptr<thread_msg> sp_thread_msg;
    typedef void (*thread_msg_handler)(sp_thread_msg &msg);
    thread_msg_handler m_handler{nullptr};
};
#endif