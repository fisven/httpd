#include <stdio.h>
// 网络通信相关
#include <winsock2.h>
#pragma comment(lib, "WS2_32.lib")

void error_die(const char* str) {
    perror(str);
    exit(1);
}

int startup(unsigned short* port) {
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
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		error_die("bind error ...");
	}

    // 动态分配端口.
    int nameLen = sizeof(server_addr);
    if(*port == 0) {
        int temp = getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen);
        if(temp < 0) {
            error_die("getsockname error .. ");
        }
        *port = ntohs(server_addr.sin_port);
    }

    // 监听队列.
    ret = listen(server_socket, 5);
    if(ret < 0) {
        error_die("listen error ..");
    }

    return server_socket;
}

// 处理用户请求的线程函数.
DWORD WINAPI accept_request(LPVOID arg) {
    
    return 0;
}

int main(void) {
    unsigned short port = 80;
    int server_sock = startup(&port);
    printf("server begin ... %d", port);

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while(1) {
        // 阻塞等待式.
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if(client_sock == -1) {
            error_die("accept error ...");
        }
        DWORD threadId = 0;
        CreateThread(0, 0, accept_request, (void*)client_sock, 0, &threadId);
        
    }

    closesocket(server_socket);
}