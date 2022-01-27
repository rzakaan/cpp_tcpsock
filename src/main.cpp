#include "tcpsock.h" 
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;
int main(int argc, char* argv[])
{
    TcpSocket tcp;
    std::string host = "";
    int socket_port = 8080;
    bool is_server = true;    

    if (argc > 0)
    for (int i = 0; i < argc; i++) {
        
        if (std::string("-p") == argv[i] || std::string("--port") == argv[i]) {
            socket_port = atoi(argv[i + 1]);
        }

        if (std::string("--connect") == argv[i]) {
            is_server = false;
            host = std::string(argv[i + 1]);
        }

        if (std::string("-v") == argv[i] || std::string("--verbose") == argv[i]) {
            tcp.verbose(true);
        }            
    }    

    if (is_server) tcp.create_server(socket_port);
    else tcp.connect_server(host, socket_port);

    tcp.connect_server(host, socket_port);

    while(true);
    
    return EXIT_SUCCESS;
}