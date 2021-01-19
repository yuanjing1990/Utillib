
#include <gtest/gtest.h>
#include "yjutil.h"


class TestServer : public ::testing::Test {
    public:
        TestServer(){}
        virtual ~TestServer(){}

        virtual void SetUp() {
            DEBUG_PRINT("SetUp ... ");
        }

        virtual void TearDown() {
            DEBUG_PRINT("TearDown ... ");
        }
};

TEST_F(TestServer, test_demo) {
    yjutil::server_socket server(12345);
    server.start(NULL);
}