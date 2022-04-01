#include <iostream>
#include "stdio.h"
#include "includes.h"
#include "vec_operations.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#pragma warning(disable: 4996)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void sat_moving(satellite sat1);
void sat_moving(satellite sat1) {
    while(true){
        sat1.change_genCoord( make_iter_RK(sat1.get_Mass(), sat1.get_genCoord(), 1));
        vector<double> r = sat1.get_Coord();
        cout<< "Current satellite coordinates are:" << "[ "<<r<< "] " << std::endl;
        sleep_until(system_clock::now() + seconds(1));
    }
}

void ClientHandler(unsigned int ID, SOCKET connection) {
    char msg[256];
    while(true) {
        recv(connection, msg, sizeof(msg), NULL);
        std::cout<<msg;
        //send(connection, msg, sizeof(msg), NULL);
        Sleep(10);
    }
}

void StringToChar(std::string A, char* a){
    for(int i=0; i<A.size(); i++){
        a[i] = A[i];
    }
}
char commands(char *message){
    std::string getC = "get coords";
    char getC_char[getC.size()];
};

int main() {
    //sat init
    vector<double>  set_start{6800e3, 0, 0, 0, 7771, 0};
    satellite telc_sat(1111, 10, set_start);
    vector<double> start = telc_sat.get_Coord();

    cout<< "Satellite start position is: " "[ " << start << " ]" << std::endl;

    //WSAStartup
    SOCKET Connection;

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

    Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if(connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: failed connect to server.\n";
        return 1;
    }
    std::cout << "Connected!\n";
    unsigned int ID = telc_sat.get_ID();
    char msg[256];
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) sat_moving, (LPVOID) (&telc_sat), NULL, NULL);
    while(true) {
        recv(Connection, msg, sizeof(msg), NULL);
        vector<double> r = telc_sat.get_Coord();
        char x_coord [100],  y_coord [100],  z_coord [100];
        send(Connection, "\n", sizeof("\n"), NULL);
        Sleep(10);
        sprintf (x_coord, "%f", r[0]);
        send(Connection, x_coord, sizeof(x_coord), NULL);
        Sleep(10);
        send(Connection, ", ", sizeof(", "), NULL);
        Sleep(10);
        sprintf (y_coord, "%f", r[0]);
        send(Connection, y_coord, sizeof(y_coord), NULL);
        Sleep(10);
        send(Connection, ", ", sizeof(", "), NULL);
        Sleep(10);
        sprintf (z_coord, "%f", r[0]);
        send(Connection, z_coord, sizeof(z_coord), NULL);
        Sleep(10);
    }
    //CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientHandler, (LPVOID) (&ID, &Connection), NULL, NULL);

    cin.get();//Keeps Threads living
    return 0;
}
