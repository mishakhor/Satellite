#include <iostream>
#include "stdio.h"
#include "includes.h"
#include "vec_operations.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <sstream>
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
    int msg_size;
    while(true) {
        recv(connection, (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size+1];
        msg[msg_size] = '\0';
        recv(connection, msg, msg_size, 0);
        std::cout<<msg;
        //send(connection, msg, sizeof(msg), NULL);
        Sleep(10);
        delete[] msg;
    }
}

//void StringToChar(std::string A, char* a){
//    for(int i=0; i<A.size(); i++){
//        a[i] = A[i];
//    }
//}


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

    Connection = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: failed connect to server.\n";
        return 1;
    }
    std::cout << "Satellite Connected!\n";
    unsigned int ID = telc_sat.get_ID();

    int msg_size;
    int command;
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) sat_moving, (LPVOID) (&telc_sat), 0, nullptr);
    while(true) {
        //recv(Connection, (char*)&msg_size, sizeof(int), 0);
        //char* msg = new char[msg_size+1];
        //msg[msg_size] = '\0';
        recv(Connection, (char*)&command, sizeof(int), 0);
        switch (command) {
            case 0: {
                std::string error = "Error: There is no such command";
                msg_size = error.size();
                send(Connection, (char *) &msg_size, sizeof(int), 0);
                send(Connection, error.c_str(), msg_size, 0);
                break;
            }
            case 1: {
                vector<double> r = telc_sat.get_Coord();
                std::stringstream fut_str;
                fut_str.setf(std::ios::fixed);
                fut_str << r[0] << " " << r[1] << " " << r[2];
                std::string coords = fut_str.str();
                int coords_size = coords.size();
                send(Connection, (char *) &coords_size, sizeof(int), 0);
                send(Connection, coords.c_str(), coords_size, 0);
                Sleep(10);
                break;
            }
            case 2: {
                while (true) {
                    vector<double> r = telc_sat.get_Coord();
                    std::stringstream fut_str;
                    fut_str.setf(std::ios::fixed);
                    fut_str << r[0] << " " << r[1] << " " << r[2];
                    std::string coords = fut_str.str();
                    int coords_size = coords.size();
                    send(Connection, (char *) &coords_size, sizeof(int), 0);
                    send(Connection, coords.c_str(), coords_size, 0);
                    Sleep(1000);
                }
            }
            default: {break;}
        }
    }
}
