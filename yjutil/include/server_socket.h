#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace yjutil {
struct cmd {
    uint8_t version;
    uint8_t cmdId;
    uint8_t data[24];
    uint8_t ret;
};

class server_socket {
  private:
    std::vector<int> m_clients;
    uint16_t m_port;
    typedef void (*cmd_handler)(cmd *rmsg, cmd *smsg);
    cmd_handler m_handler;

  public:
    server_socket(uint16_t port);
    virtual ~server_socket();
    void start(void *);
    void set_cmd_handler(cmd_handler handler) {
        m_handler = handler;
    };

  private:
    void handle_client_request(int socket);
    int accept_client(int socket);
    int reject_client(int socket);
    void response_to_client(cmd *rep, int socket);
};
} // namespace yjutil

#endif // SERVER_SOCKET_H_