#include <SDKDDKVer.h>#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

#pragma warning(disable: 4996)

SOCKET Connection;

[[noreturn]] void ClientHandler() {
    int msg_size;
    while(true) {
        recv(Connection, (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        recv(Connection, msg, msg_size, 0);
        std::cout << msg << std::endl;
        delete[] msg;
    }
}

int main(int argc, char* argv[]) {
    //WSAStartup
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    Connection = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: failed connect to server.\n";
        return 1;
    }
    std::cout << "Connected!\n";

    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ClientHandler, nullptr, 0, nullptr);

    std::string msg1;
    while(true) {
        std::getline(std::cin, msg1);
        if (msg1 == "_Exit") break;
        int msg_size = msg1.size();
        send(Connection, (char*)&msg_size, sizeof(int), 0);
        send(Connection, msg1.c_str(), msg_size, 0);
        Sleep(100);
    }

    system("pause");
    return 0;
}