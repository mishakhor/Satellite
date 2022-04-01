#include <SDKDDKVer.h>#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <map>

#pragma warning(disable: 4996)

SOCKET Connections[100];
std::map<int, std::map<int, std::string>> ID_Type;

int Counter = 0;

int commands(const char *message) {
    std::string msg(message);
    if (msg == "get coords") return 1;
    if (msg == "get coords cont") return 2;
    return 0;
}

void get_coords(char *message){
    send(Connections[0], message, sizeof(message), 0);
}

[[noreturn]] void ClientHandler(int index) {
    int msg_size;
    while(true) {
        recv(Connections[index], (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        if(index == 1){
            recv(Connections[index], msg, msg_size, 0);
            std::cout << msg << std::endl;
            int command = commands(msg);
            send(Connections[0], (char*)&command, sizeof(int), 0);
        }
        else{
            recv(Connections[index], msg, msg_size, 0);
            std::cout << msg << std::endl;
            send(Connections[1], (char*)&msg_size, sizeof(int), 0);
            send(Connections[1], msg, msg_size, 0);
            std::cout << "recieved and sent frwd to client" << std::endl;
        }
        delete[] msg;
    }
}

int main(int argc, char* argv[]) {
    //WSAStartup
    WSAData wsaData{};
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR *) &addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    for (int i = 0; i <= 100; i++) {
        newConnection = accept(sListen, (SOCKADDR *) &addr, &sizeofaddr);

        if (newConnection == 0) {
            std::cout << "Error #2\n";
        } else {
            std::cout << "Client Connected!\n";
            //std::string msg = "Hello. It`s my first network program!";
            //int msg_size = msg.size();
            //send(newConnection, (char*)&msg_size, sizeof(int), 0);
            //send(newConnection, msg.c_str(), msg_size, 0);

            Connections[i] = newConnection;
            Counter++;
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) ClientHandler, (LPVOID) (i), 0, nullptr);
        }
    }


    std::cin.get();//Keeps Threads living
    return 0;
}