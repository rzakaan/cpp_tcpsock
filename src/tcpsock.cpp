#include "tcpsock.h"

#define TCP_BUFFER_SIZE     1024

class SocketData {
public:
    TcpSocket* self;
    int client_sock_fd;
    sockaddr_in* cli_saddr;
};

void TcpSocket::verbose(bool value) {
    is_verbose = value;
}

bool TcpSocket::init_socket()
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd < 0)
    {
        if (is_verbose) std::cerr << "tcp socket creation error : " << strerror(errno) << std::endl;
        return false;
    }

    if (is_verbose) std::cout << "tcp create socket fd : " << socket_fd << endl;

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0)
    {
        if (is_verbose) std::cerr << "tcp socket options error : " << strerror(errno) << std::endl;
        return false;
    }

    if (is_verbose) std::cout << "tcp socket set options SO_REUSEADDR | SO_REUSEADDR" << std::endl;

    return true;
}


bool TcpSocket::create_server(int port)
{
    if (is_busy) {
        if (is_verbose) std::cerr << "socket already working on " <<  port << " port " << std::endl;
        return false;
    }

    init_socket();

    server_saddr.sin_family = AF_INET;
    server_saddr.sin_port = htons(port); 
    server_saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (sockaddr*) &server_saddr, sizeof(server_saddr)) != 0)
    {
        if (is_verbose) std::cerr << "tcp socket bind error : " << strerror(errno) << std::endl;
        return false;
    }

    if (is_verbose) std::cout << "tcp socket bind to " << inet_ntoa(server_saddr.sin_addr) << ":" << ntohs(server_saddr.sin_port) << endl;

    if (listen(socket_fd, max_client_count) < 0)
    {
        if (is_verbose) std::cerr << "tcp socket listen error : " << strerror(errno) << std::endl;
        return false;
    }

    if (is_verbose) std::cout << "tcp socket listening ..." << endl;

    pthread_t accept_client_th;    
    int rc = pthread_create(&accept_client_th, NULL, (FUNC_PTR) TcpSocket::accept_client, (void*) this);
    if (rc < 0)
    {
        if (is_verbose) std::cerr << "tcp socket accept client thread error : " << strerror(errno) << std::endl;
        return false;
    }

    mode = TcpMode::SERVER;
    is_busy = true;
    return true;
}

bool TcpSocket::connect_server(string host, int port)
{
    if (is_busy) {
        if (is_verbose) std::cerr << "socket already working on " <<  port << " port " << std::endl;
        return false;
    }

    init_socket();

    mode = TcpMode::CLIENT;
    is_busy = true;
    return true;
}

bool TcpSocket::close()
{
    if (shutdown(socket_fd, SHUT_RDWR) < 0)
    {
        std::cerr << "socket shutdown failed ! : " << strerror(errno) << std::endl;
        return false;
    }

    is_busy = false;
    return true;
}

void TcpSocket::accept_client(void* ptr)
{
    TcpSocket* self = (TcpSocket*) ptr;
    while(true)
    {
        struct sockaddr_in cli_saddr;
        socklen_t cli_addr_len = sizeof(cli_saddr);
        int client_sock = accept(self->socket_fd, (sockaddr*)&cli_saddr, &cli_addr_len);
        if (client_sock < 0)
        {
            if (self->is_verbose) std::cerr << "tcp socket accept client error : " << strerror(errno) << std::endl;
            continue;
        }

        if (self->is_verbose) std::cout << "tcp socket accept client(" << client_sock << ") from " << inet_ntoa(cli_saddr.sin_addr) << ":" << ntohs(cli_saddr.sin_port) << std::endl;

        SocketData sock_data;
        sock_data.self = self;
        sock_data.client_sock_fd = client_sock;
        sock_data.cli_saddr = &cli_saddr;

        pthread_t read_client_th;    
        int rc = pthread_create(&read_client_th, NULL, (FUNC_PTR) TcpSocket::read_client, (void*) &sock_data);
        if (rc < 0)
        {
            if (self->is_verbose) std::cerr << "tcp socket read client thread error : " << strerror(errno) << std::endl;
            continue;
        }
    }
}

void TcpSocket::read_client(void* ptr)
{
    SocketData* sock_data = (SocketData*) ptr;
    sockaddr_in* cli_saddr = sock_data->cli_saddr;
    
    int sock = sock_data->client_sock_fd;
    unsigned char* buffer = new unsigned char[TCP_BUFFER_SIZE];

    if (sock_data->self->is_verbose) std::cout << "tcp socket ready read client from " << inet_ntoa(cli_saddr->sin_addr) << ":" << ntohs(cli_saddr->sin_port) << std::endl;

    while(true)
    {
        // 100 micro second
        usleep(100);
        bzero(buffer, TCP_BUFFER_SIZE);
        
        int flags = 0;
        int len = recv(sock, buffer, TCP_BUFFER_SIZE, flags);

        if (!len)
        {
            if (sock_data->self->is_verbose) std::cout << "tcp socket disconnected client " << inet_ntoa(cli_saddr->sin_addr) << ":" << ntohs(cli_saddr->sin_port) << std::endl;
            break;
        }

        if (len < 0)
        {
            if (sock_data->self->is_verbose) std::cerr << "tcp socket client read error : " << strerror(errno) << std::endl;
            // remove_socket();
            break;
        }
        
        string str_buff((char*) buffer);
        std::cout << "0x"; for (const auto &item : str_buff) std::cout << std::hex << std::uppercase << int(item);
        std::cout << " : " << buffer << std::endl;
    }
}