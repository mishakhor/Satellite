#include <SDKDDKVer.h>#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET Connections[100];

int Counter = 0;

bool commands(char *message){
    char *get_coordinates[10] = {"g", "e", "t", " ",
                               "c", "o", "o", "r", "d", "s"} ;
    for(int i = 0; i<10; i++){
            if(message[i]!= *get_coordinates[i]){
                return 0;
        }
    }
    return 1;
};

void get_coords(char *message){
    send(Connections[0], message, sizeof(message), NULL);
}

void ClientHandler(int index) {
    char msg[256];
    while(true) {
        if(index == 1){
            recv(Connections[index], msg, sizeof(msg), NULL);
            std::cout << msg << std::endl;
            if(commands(msg)){
                get_coords(msg);
                std::cout << "recieved and sent on sat" << std::endl;
            }
        }
        else{
            recv(Connections[index], msg, sizeof(msg), NULL);
            std::cout << msg << std::endl;
            send(Connections[1], msg, sizeof(msg), NULL);
            std::cout << "recieved and sent frwd to client" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    //WSAStartup
    WSAData wsaData;
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

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR *) &addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    for (int i = 0; i <= 100; i++) {
        newConnection = accept(sListen, (SOCKADDR *) &addr, &sizeofaddr);

        if (newConnection == 0) {
            std::cout << "Error #2\n";
        } else {
            std::cout << "Client Connected!\n";
            //char msg[256] = "Hello. It`s my first network program!";
            //send(newConnection, msg, sizeof(msg), NULL);

            Connections[i] = newConnection;
            Counter++;
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientHandler, (LPVOID) (i), NULL, NULL);
        }
    }


    std::cin.get();//Keeps Threads living
    return 0;
}