
#include "loop_thread.h"
#include "yjutil.h"
#include <gtest/gtest.h>

class TestServer : public ::testing::Test {
  public:
    TestServer() {}
    virtual ~TestServer() {}

    virtual void SetUp() {
        DEBUG_PRINT("SetUp ... ");
    }

    virtual void TearDown() {
        DEBUG_PRINT("TearDown ... ");
    }
};

TEST_F(TestServer, test_demo) {
    // yjutil::server_socket server(12345);
    // server.start(NULL);
    loop_thread lp;
    lp.start("Test");
    lp.postMessage(std::move(std::make_shared<loop_thread::thread_msg>(0)), 5000);
}