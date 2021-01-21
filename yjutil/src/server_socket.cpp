#include "server_socket.h"
#include <algorithm>
#include "yjdef.h"

#define MCLIENT 5
namespace yjutil
{
    server_socket::server_socket(uint16_t port) : m_port(port), m_clients(MCLIENT, 0), m_handler(NULL)
    {
    }

    server_socket::~server_socket()
    {
    }

    void server_socket::start(void *)
    {
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
        while (true)
        {
            FD_ZERO(&readfds);
            FD_SET(listen_fd, &readfds);
            int max_sd = std::max(listen_fd, *std::max_element(m_clients.begin(), m_clients.end()));

            std::for_each(m_clients.begin(), m_clients.end(), [&](int sd) {
                if (sd > 0)
                    FD_SET(sd, &readfds);
            });

            int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            if ((activity < 0) && (errno != EINTR))
                continue;

            if (FD_ISSET(listen_fd, &readfds))
            {
                int client_socket = accept(listen_fd,
                                           (struct sockaddr *)&address, &addrlen);
                if (client_socket < 0)
                    continue;

                accept_client(client_socket);
            }

            std::for_each(m_clients.begin(), m_clients.end(), [&](int fd) {
                if (FD_ISSET(fd, &readfds))
                    handle_client_request(fd);
            });
        }
    }

    int server_socket::accept_client(int client_socket)
    {
        auto it = std::find(m_clients.begin(), m_clients.end(), 0);
        if (it != m_clients.end())
        {
            *it = client_socket;
            std::sort(m_clients.begin(), m_clients.end());
            printf("Accept client:%d\n", client_socket);
            return EXIT_SUCCESS;
        }
        else
        {
            close(client_socket);
            printf("Client is full,not accept client:%d\n", client_socket);
            return EXIT_FAILURE;
        }
    }

    int server_socket::reject_client(int client_socket)
    {
        auto it = std::find(m_clients.begin(), m_clients.end(), 0);
        if (it != m_clients.end())
        {
            *it = 0;
            std::sort(m_clients.begin(), m_clients.end());
            close(client_socket);
            printf("Reject client:%d\n", client_socket);
            return EXIT_SUCCESS;
        }
        else
        {
            printf("Client is not connected:%d\n", client_socket);
            return EXIT_FAILURE;
        }
    }

    void server_socket::handle_client_request(int socket)
    {
        cmd rmsg = {0};
        cmd smsg = {0};
        ssize_t size_msg = sizeof(cmd);
        ssize_t bytereceived = 0;
        bytereceived = recv(socket, &rmsg, size_msg, 0);
        if ((0 == bytereceived) || (size_msg != bytereceived))
        {
            if (0 == bytereceived)
            {
                reject_client(socket);
                return;
            }
            else
            {
                printf("Recvfrom client:%d error :: bytereceived = %ld:%s :: ERROR == %s\n", socket, bytereceived, (char*)&rmsg, strerror(errno));
                // socklen_t addrlen;
                // struct sockaddr_in address;
                // getpeername(socket, (struct sockaddr *)&address, &addrlen);
                // printf("client to disconnect ip:port %s:%d\n",
                //        inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            }
        }
        else
        {
            if (m_handler != NULL)
                m_handler(&rmsg, &smsg);
        }
        response_to_client(&smsg, socket);
    }

    void server_socket::response_to_client(cmd *rep, int socket)
    {
        send(socket, rep, sizeof(cmd), 0);
    }
} // namespace yjutil

/*
#define MCLIENT 5
int g_server_ip_port = 59394;
int _clientconnect[MCLIENT];

void send_response(int sockfd, audio_i2c_msg *smsg)
{
    slog2f(NULL, 0, SLOG2_INFO, "%s,enter\n", __FUNCTION__);
    ssize_t bytesent = 0;
    ssize_t pkt_len = sizeof(audio_i2c_msg);
    bytesent = send(sockfd, smsg, pkt_len, 0);
    if (pkt_len != bytesent)
    {
        slog2f(NULL, 0, SLOG2_INFO, "%s error :: pkt_len=%ld,bytesent = %ld :: ERROR == %s\n", __FUNCTION__, pkt_len, bytesent, strerror(errno));
    }
    slog2f(NULL, 0, SLOG2_INFO, "%s,exit\n", __FUNCTION__);
}

void process_clientsrequest(fd_set *p_readfds)
{
    slog2f(NULL, 0, SLOG2_INFO, "%s,enter\n", __FUNCTION__);
    int i = 0, sd = 0;
    audio_i2c_msg rmsg = {0};
    audio_i2c_msg smsg = {0};
    ssize_t size_msg = sizeof(audio_i2c_msg);
    ssize_t bytereceived = 0;
    for (i = 0; i < MCLIENT; i++)
    {
        sd = _clientconnect[i];
        if (FD_ISSET(sd, p_readfds))
        {
            bytereceived = 0;
            bytereceived = recv(sd, &rmsg, size_msg, 0);
            if ((0 == bytereceived) || (size_msg != bytereceived))
            {
                if (0 == bytereceived)
                {
                    slog2f(NULL, 0, SLOG2_INFO, "client is shutdown.\n");
                }
                else
                {
                    slog2f(NULL, 0, SLOG2_INFO, "recvfrom socket error :: bytereceived = %ld :: ERROR == %s\n", bytereceived, strerror(errno));
                }
                socklen_t addrlen;
                struct sockaddr_in address;
                getpeername(sd, (struct sockaddr *)&address, &addrlen);
                slog2f(NULL, 0, SLOG2_INFO, "diag_lsm_clientconnect disconnected , ip %s :: port %d",
                       inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                smsg.ret = FAILED;
                smsg.msg_type = rmsg.msg_type;
                send_response(sd, &smsg);
                close(sd);
                sd = 0;
                _clientconnect[i] = 0;
                continue;
            }
            else
            {
                slog2f(NULL, 0, SLOG2_INFO, "\n");
                slog2f(NULL, 0, SLOG2_INFO, "######recv message from client,socket_id=%d#######\n", sd);
                slog2f(NULL, 0, SLOG2_INFO, "bytereceived = %ld,msg_type=%d\n", bytereceived, rmsg.msg_type);
                slog2f(NULL, 0, SLOG2_INFO, "byteAddrLen=%d\n", rmsg.byteAddrLen);
                for (int i = 0; i < rmsg.byteAddrLen; i++)
                {
                    slog2f(NULL, 0, SLOG2_INFO, "%02x ", rmsg.addr[i]);
                }
                slog2f(NULL, 0, SLOG2_INFO, "len=%d\n", rmsg.len);
                for (int i = 0; i < rmsg.len; i++)
                {
                    slog2f(NULL, 0, SLOG2_INFO, "%02x ", rmsg.pBuf[i]);
                }
                handle_render_msg(&rmsg, &smsg);
                smsg.msg_type = rmsg.msg_type;
                send_response(sd, &smsg);
            }
        }
    }
    slog2f(NULL, 0, SLOG2_INFO, "%s,exit\n", __FUNCTION__);
}
int start_socket_server(void)
{
    int ret = -1;
    struct sockaddr_in t_sockaddr;
    memset(&t_sockaddr, 0, sizeof(t_sockaddr));
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(g_server_ip_port);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        slog2f(NULL, 0, SLOG2_INFO, "socket error %s errno: %d\n", strerror(errno), errno);
    }
    int opt = 1;
    int optlen = sizeof(opt);
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, optlen);
    ret = ::bind(listen_fd, (struct sockaddr *)&t_sockaddr, sizeof(t_sockaddr));
    if (ret < 0)
    {
        slog2f(NULL, 0, SLOG2_INFO, "bind socket error %s errno: %d\n", strerror(errno), errno);
    }
    slog2f(NULL, 0, SLOG2_INFO, "######start listen#######\n");
    ret = listen(listen_fd, 1024);
    if (ret < 0)
    {
        slog2f(NULL, 0, SLOG2_INFO, "listen error %s errno: %d\n", strerror(errno), errno);
    }

    fd_set readfds;
    int max_sd;
    int new_socket, activity, i, sd;
    socklen_t addrlen;
    struct sockaddr_in address;
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        max_sd = listen_fd;

        for (i = 0; i < MCLIENT; i++)
        {
            sd = _clientconnect[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
        {
            slog2f(NULL, 0, SLOG2_INFO, "select error\n");
            continue;
        }

        if (FD_ISSET(listen_fd, &readfds))
        {
            slog2f(NULL, 0, SLOG2_INFO, "wait accept...\n");
            if ((new_socket = accept(listen_fd,
                                     (struct sockaddr *)&address, &addrlen)) < 0)
            {
                slog2f(NULL, 0, SLOG2_INFO, "accept failue\n");
                continue;
            }
            slog2f(NULL, 0, SLOG2_INFO, "accept,new_socket=%d\n", new_socket);
            for (i = 0; i < MCLIENT; i++)
            {
                if (_clientconnect[i] == 0)
                {
                    _clientconnect[i] = new_socket;
                    slog2f(NULL, 0, SLOG2_INFO, "_clientconnect[%d]=%d\n", i, _clientconnect[i]);
                    break;
                }
            }
        }

        process_clientsrequest(&readfds);
    }
    return 0;
}
*/