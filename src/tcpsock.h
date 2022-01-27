#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_CLIENT_COUNT 10
typedef void* (*FUNC_PTR) (void*);

enum TcpMode {
    SERVER,
    CLIENT
};

using namespace std;
class TcpSocket {
private:
    // socket options
    TcpMode mode;
    int socket_fd;
    bool is_busy;
    bool is_verbose;
    
    // server
    int max_client_count;
    struct sockaddr_in server_saddr;

    bool init_socket();

  public:
    void verbose(bool value);
    bool create_server(int port);
    bool connect_server(string host, int port);
    bool close();

    // thread functions
    static void accept_client(void* ptr);
    static void read_client(void* ptr);
};