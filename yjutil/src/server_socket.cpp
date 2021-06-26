#include "server_socket.h"
#include "yjdef.h"
#include <algorithm>

#define MCLIENT 5
namespace yjutil {
server_socket::server_socket(uint16_t port) : m_port(port), m_clients(MCLIENT, 0), m_handler(NULL) {
}

server_socket::~server_socket() {
}

void server_socket::start(void *) {
    struct sockaddr_in t_sockaddr;
    memset(&t_sockaddr, 0, sizeof(t_sockaddr));
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(m_port);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
        return;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int ret = ::bind(listen_fd, (struct sockaddr *)&t_sockaddr, sizeof(t_sockaddr));
    if (ret < 0)
        return;

    ret = listen(listen_fd, 1024);
    if (ret < 0)
        return;

    fd_set readfds;
    int new_socket, i;
    socklen_t addrlen;
    struct sockaddr_in address;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int max_sd = std::max(listen_fd, *std::max_element(m_clients.begin(), m_clients.end()));

        std::for_each(m_clients.begin(), m_clients.end(), [&](int sd) {
            if (sd != 0)
                FD_SET(sd, &readfds);
        });

        DEBUG_PRINT("selecting max_sd=%d", max_sd);
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
            continue;

        if (FD_ISSET(listen_fd, &readfds)) {
            int client_socket = accept(listen_fd,
                                       (struct sockaddr *)&address, &addrlen);
            if (client_socket < 0)
                continue;

            accept_client(client_socket);
        }

        auto _tmp_clients(m_clients);
        std::for_each(_tmp_clients.begin(), _tmp_clients.end(), [&](int fd) {
            if (fd != 0 && FD_ISSET(fd, &readfds))
                handle_client_request(fd);
        });
    }
}

int server_socket::accept_client(int client_socket) {
    auto it = std::find(m_clients.begin(), m_clients.end(), 0);
    if (it != m_clients.end()) {
        *it = client_socket;
        std::sort(m_clients.begin(), m_clients.end());
        DEBUG_PRINT("Accept client:%d\n", client_socket);
        return EXIT_SUCCESS;
    } else {
        close(client_socket);
        DEBUG_PRINT("Client is full,not accept client:%d\n", client_socket);
        return EXIT_FAILURE;
    }
}

int server_socket::reject_client(int client_socket) {
    auto it = std::find(m_clients.begin(), m_clients.end(), client_socket);
    if (it != m_clients.end()) {
        *it = 0;
        std::sort(m_clients.begin(), m_clients.end());
        close(client_socket);
        DEBUG_PRINT("Reject client:%d\n", client_socket);
        return EXIT_SUCCESS;
    } else {
        DEBUG_PRINT("Client is not connected:%d\n", client_socket);
        return EXIT_FAILURE;
    }
}

void server_socket::handle_client_request(int socket) {
    cmd rmsg = {0};
    cmd smsg = {0};
    ssize_t size_msg = sizeof(cmd);
    ssize_t bytereceived = 0;
    bytereceived = recv(socket, &rmsg, size_msg, 0);
    if ((0 == bytereceived) || (size_msg != bytereceived)) {
        if (0 == bytereceived) {
            reject_client(socket);
            return;
        } else {
            DEBUG_PRINT("Recvfrom client:%d error :: bytereceived = %ld:%s :: ERROR == %s\n", socket, bytereceived, (char *)&rmsg, strerror(errno));
            // socklen_t addrlen;
            // struct sockaddr_in address;
            // getpeername(socket, (struct sockaddr *)&address, &addrlen);
            // DEBUG_PRINT("client to disconnect ip:port %s:%d\n",
            //        inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        }
    } else {
        if (m_handler != NULL)
            m_handler(&rmsg, &smsg);
    }
    response_to_client(&smsg, socket);
}

void server_socket::response_to_client(cmd *rep, int socket) {
    send(socket, rep, sizeof(cmd), 0);
}
} // namespace yjutil
