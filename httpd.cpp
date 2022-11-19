#include <iostream>
#include <stdio.h>

// 网络通信相关
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

void error_die(const char* str) {
    perror(str);
    exit(1);
}

int startup(unsigned short *port) {
    WSADATA data;
    int ret = WSAStartup(MAKEWORD(1, 1), &data);
    if(ret) {
        error_die("WSAStartup ...");
    }

    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket == -1) {
        error_die("socket_die .. ");
    }

    // 设置端口复用.
    int opt = 1;
    ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if(ret == -1) {
        error_die("setsockopt error ... ");
    }

    // 配置服务器端网络地址.
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定套接字.
    ret = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(&server_addr)); // 引用.
    if(ret < 0) {
        error_die("bind error ..");
    }

    // 动态分配端口.
    int nameLen = sizeof(server_addr);
    if(*port == 0) {
        int temp = getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen);
        if(temp < 0) {
            error_die("getsockname error .. ");
        }
        *port = server_addr.sin_port;
    }

    // 监听队列.
    ret = listen(server_socket, 5);
    if(ret < 0) {
        error_die("listen error ..");
    }

    return server_socket;
}

int main() {
    unsigned short port = 80;
    int server_sock = startup(&port);
    printf("server begin ... %d", port);
}