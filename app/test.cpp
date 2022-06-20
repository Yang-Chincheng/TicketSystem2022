#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

const int BUFF_LEN = 100;
const int PORT = 5000;

int func(std::stringstream &is, std::stringstream &os) {
    std::string str;
    is >> str;
    os << "[" + str + "]";
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        puts("bind error!");
    }
    if(listen(sock, 5) == -1) {
        puts("listen error!");
        return 0;
    }
    int client;
    struct sockaddr_in caddr;
    socklen_t naddr = sizeof(caddr);
    char buff[BUFF_LEN];

    while(1) {
        memset(buff, 0, sizeof(buff));
        
        client = accept(sock, (struct sockaddr*)&caddr, &naddr);
        if(client == -1) {
            puts("accept error!"); continue;
        }
        int fb = recv(client, buff, BUFF_LEN, 0);
        
        std::stringstream is(buff);
        std::stringstream os;
        func(is, os);
        memset(buff, 0, sizeof(buff));
        os >> buff;

        fb = send(client, buff, BUFF_LEN, 0);

        shutdown(sock, SHUT_RDWR);
    }

    return 0;
}