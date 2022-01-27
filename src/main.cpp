#include "tcpserver.h" 
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;
int main(int argc, char* argv[])
{
    // socket options
    std::string host = "";
    int socket_port = 8080;
    bool is_server = true;
    bool verbose = false;
    
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            
            if (std::string("-p") == argv[i] || std::string("--port") == argv[i]) {
                socket_port = atoi(argv[i + 1]);
            }

            if (std::string("--connect") == argv[i]) {
                is_server = false;
                host = std::string(argv[i + 1]);
            }

            if (std::string("-v") == argv[i] || std::string("--verbose") == argv[i]) {
                verbose = false;
            }            
        }
    }

    if (is_server) {
        TcpServer tcp;
        tcp.init(socket_port);
        tcp.start_server();
    } else {

    }

    while(true);
    
    return EXIT_SUCCESS;
}