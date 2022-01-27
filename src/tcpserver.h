#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_CLIENT_COUNT 10
typedef void* (*FUNC_PTR) (void*);

using namespace std;

class TcpServer {
private:
    struct sockaddr_in server_saddr;
    int max_client_count;
    int server_fd;
    
public:
    bool init(int port);
    bool start_server();
    bool close_server();

    // thread functions
    static void accept_client(void* ptr);
    static void read_client(void* ptr);
};