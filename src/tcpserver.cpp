#include "tcpserver.h"

#define TCP_BUFFER_SIZE     1024

class SocketData {
public:
    TcpServer* self;
    int client_sock_fd;
    sockaddr_in* cli_saddr;
};

bool TcpServer::init(int port)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd < 0)
    {
        std::cerr << "tcp socket error : " << strerror(errno) << std::endl;
        return false;
    }

    cout << "tcp create socket on " << server_fd << endl;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0)
    {
        std::cerr << "tcp socket options error : " << strerror(errno) << std::endl;
        return false;
    }

    cout << "tcp socket set options SO_REUSEADDR | SO_REUSEADDR" << endl;

    server_saddr.sin_family = AF_INET;
    server_saddr.sin_port = htons(port); 
    server_saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    return true;
}

bool TcpServer::start_server()
{
    if (bind(server_fd, (sockaddr*) &server_saddr, sizeof(server_saddr)) != 0)
    {
        std::cerr << "tcp socket bind error : " << strerror(errno) << std::endl;
        return false;
    }

    cout << "tcp socket bind to " << inet_ntoa(server_saddr.sin_addr) << ":" << ntohs(server_saddr.sin_port) << endl;

    if (listen(server_fd, max_client_count) < 0)
    {
        std::cerr << "tcp socket listen error : " << strerror(errno) << std::endl;
        return false;
    }

    cout << "tcp socket listening ..." << endl;

    pthread_t accept_client_th;    
    int rc = pthread_create(&accept_client_th, NULL, (FUNC_PTR) TcpServer::accept_client, (void*) this);
    if (rc < 0)
    {
        std::cerr << "tcp socket accept client thread error : " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool TcpServer::close_server()
{
    if(shutdown(server_fd, SHUT_RDWR) < 0)
    {
        std::cerr << "socket shutdown failed ! : " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void TcpServer::accept_client(void* ptr)
{
    TcpServer* self = (TcpServer*) ptr;
    while(true)
    {
        struct sockaddr_in cli_saddr;
        socklen_t cli_addr_len = sizeof(cli_saddr);
        int client_sock = accept(self->server_fd, (sockaddr*)&cli_saddr, &cli_addr_len);
        if (client_sock < 0)
        {
            std::cerr << "tcp socket accept client error : " << strerror(errno) << std::endl;
            continue;
        }

        cout << "tcp socket accept client(" << client_sock << ") from " << inet_ntoa(cli_saddr.sin_addr) << ":" << ntohs(cli_saddr.sin_port) << endl;

        SocketData sock_data;
        sock_data.self = self;
        sock_data.client_sock_fd = client_sock;
        sock_data.cli_saddr = &cli_saddr;

        pthread_t read_client_th;    
        int rc = pthread_create(&read_client_th, NULL, (FUNC_PTR) TcpServer::read_client, (void*) &sock_data);
        if (rc < 0)
        {
            std::cerr << "tcp socket read client thread error : " << strerror(errno) << std::endl;
            continue;
        }
    }
}

void TcpServer::read_client(void* ptr)
{
    SocketData* sock_data = (SocketData*) ptr;
    sockaddr_in* cli_saddr = sock_data->cli_saddr;
    
    int sock = sock_data->client_sock_fd;
    unsigned char* buffer = new unsigned char[TCP_BUFFER_SIZE];

    cout << "tcp socket ready read client from " << inet_ntoa(cli_saddr->sin_addr) << ":" << ntohs(cli_saddr->sin_port) << endl;

    while(true)
    {
        // 100 micro second
        usleep(100);
        bzero(buffer, TCP_BUFFER_SIZE);
        
        int flags = 0;
        int len = recv(sock, buffer, TCP_BUFFER_SIZE, flags);

        if(!len)
        {
            cout << "tcp socket disconnected client " << inet_ntoa(cli_saddr->sin_addr) << ":" << ntohs(cli_saddr->sin_port) << endl;
            break;
        }

        if (len < 0)
        {
            std::cerr << "tcp socket client read error : " << strerror(errno) << std::endl;
            // remove_socket();
            break;
        }

        cout << buffer << endl; // hex << buffer[0];
    }
}