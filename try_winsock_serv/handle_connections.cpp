//
// Created by AS on 4/15/2022.
//
#include "handle_connections.h"
#include "includes.h"
#include <sstream>

/** SAT and CLIENT contain ID and SOCKET of units **/
std::map<int, SOCKET> SAT;
std::map<int, SOCKET> CLIENT;

/** Function used to send Message string to client **/
void send_to_client(const SOCKET& connection, const std::string& msg) {
    int msg_size = msg.size();
    send(connection, (char*)&msg_size, sizeof(int), 0);
    send(connection, msg.c_str(), msg_size, 0);
    Sleep(10);
}

/** Function used to send Message string to satellite **/
void send_to_satellite(int sat_ID, int command) {
    if (SAT.find(sat_ID) == SAT.end()) std::cout << "There is no Satellite with this ID or it is not chosen\n";
    SOCKET sat = SAT.find(sat_ID)->second;
    send(sat, (char*)&sat_ID, sizeof(int), 0);
}

int msg_from_client(SOCKET client) {
    int next_msg_size;
    char c;
    int i;
    recv(client, (char *) &next_msg_size, sizeof(int), 0);
    char *next_msg = new char[next_msg_size + 1];
    next_msg[next_msg_size] = '\0';
    recv(client, next_msg, next_msg_size, 0);
    if (std::string(next_msg) == "back") {
        send_to_client(client, "You left successfully");
        delete[] next_msg;
        return 0;
    }
    else {
        std::stringstream ss(next_msg);
        ss >> i;
        if (ss.fail() or ss.get(c)) {
            send_to_client(client, "Not an integer, try again or type 'back'");
            delete[] next_msg;
            return msg_from_client(client);
        }
        if (SAT.find(i) == SAT.end()) {
            send_to_client(client, "There is no such ID, try again or type 'back'");
            delete[] next_msg;
            msg_from_client(client);
            return msg_from_client(client);
        }
    }
    delete[] next_msg;
    return i;
}


/** Function that converts message to command ID. Returns 1 if the command doesn't exist **/
int commands(const char *message) {
    std::string msg(message);
    if (msg == "disconnect") return 0;
    if (msg == "refresh") return 3;
    if (msg == "get coords") return 4;
    if (msg == "get coords cont") return 5;
    if (msg == "stop getting coords") return 6;
    return 1;
}

void get_coords(char *message){
    //send(SAT[!!!], message, sizeof(message), 0);
    //place for iterator
}


/** Client handler - receives and sends requests from/to client **/
void ClientHandler(const int& client_ID) {
    SOCKET client = (CLIENT.find(client_ID))->second;
    int msg_size;
    int connected = 1;
    int satellite_id;
    while(connected) {
        recv(client, (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        //char* msg = new char[11 + 1];
        //std::cout<<std::endl<<msg_size<<std::endl;
        msg[msg_size] = '\0';
        recv(client, msg, msg_size, 0);
        std::cout << msg << std::endl;
        switch(commands(msg)) {
            case 0: { //Disconnection check
                connected = 0;
                break;
            }
            case 1: { //No such command error
                send_to_client(client, "Error: there is no such command");
                break;
            }
            case 3: { //Refresh block
                std::stringstream refresh_str;
                refresh_str.setf(std::ios::fixed);
                refresh_str << "Available Satellites (ID):\n";
                if (SAT.empty()) {
                    send_to_client(client, "There is no Satellites online right now");
                    break;
                }
                for (auto & satellite : SAT) {
                    refresh_str << satellite.first << "\n";
                    std::cout << satellite.first << "\n";
                }
                refresh_str << "Type ID of chosen Satellite (if you want to return, write 'back'):";
                send_to_client(client, refresh_str.str());

                /** Processing next message **/
                satellite_id = msg_from_client(client);
                if (satellite_id == 0) break;
                std::stringstream sat_connect_str;
                refresh_str.setf(std::ios::fixed);
                sat_connect_str << "You have successfully connected to satellite " << satellite_id;
                send_to_client(client, sat_connect_str.str());
                break;
            }
            case 4: { // Get coords
                if (satellite_id == 0) send_to_client(client, "Satellite is not chosen. Connect to Satellite and try again");
                auto sat_ = SAT.find(satellite_id)->second;
                send(sat_, (char*)&client_ID, sizeof(int), 0);
                send(sat_, "4", sizeof("4"), 0);
                break;
            }
            case 5: { // Get coords continuously
                if (satellite_id == 0) send_to_client(client, "Satellite is not chosen. Connect to Satellite and try again");
                auto sat_ = SAT.find(satellite_id)->second;
                send(sat_, (char*)&client_ID, sizeof(int), 0);
                send(sat_, "5", sizeof("5"), 0);
                break;
            }
            default: {
                break;
            }
        }
        //здесь должна быть обработка какому спутнику отправить сообщение

        delete[] msg;
    }
    std::cout << "Successfully disconnected" << std::endl;
    CLIENT.erase(client_ID);
    Sleep(100);
}

/** Satellite handler - receives and sends requests from/to satellite **/
[[noreturn]] void SatHandler(int sat_ID) {
    SOCKET sat_ = SAT.find(sat_ID)->second;
    int msg_size;
    int connected = 1;
    int client_ID;
    while(connected) {
        //Asks for client ID
        recv(sat_, (char*)&client_ID, sizeof(int), 0);

        //Getting message from satellite
        recv(sat_, (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';

        recv(sat_, msg, msg_size, 0);

        //Sends message from satellite to client
        send_to_client(CLIENT.find(client_ID)->second, std::string(msg));
        //здесь должна быть обработка какому клиенту отправить сообщение (в целом клиент пока может быть один)
        delete[] msg;
    }
}

/** Connects new unit to the server, distinguishes if it is a client or satellite **/
[[noreturn]] void NewClients(SOCKET sListen, SOCKADDR_IN addr){
    SOCKET newConnection;
    int sizeofaddr = sizeof(addr);

    while(true){
        newConnection = accept(sListen, (SOCKADDR *) &addr, &sizeofaddr);

        if (newConnection == 0) {
            std::cout << "Error #2\n";
        }

        else {
            char connection_type[1] ;
            int unit_ID;
            //receive of connection type:
            // 0 = sat, 1 = client (0 or 1 are sent in a msg from client bellow)
            recv(newConnection, connection_type, sizeof(connection_type), 0);
            recv(newConnection, (char*)&unit_ID, sizeof(int), 0);
            std::cout << "connection type is: " << connection_type[0] << std::endl;
            std::cout << "unit id is: " << unit_ID << std::endl;

            //depends on connection type we create a new SatHandler/ClientHandler thread:

            if (connection_type[0] == '0'){
                //stress-case feature - not done yet (after 50 connection server should be considered as over-lasted)
                SAT[unit_ID] = newConnection;
                std::cout<<"Sat connected successfully"<<std::endl;
                std::thread newThread(SatHandler, unit_ID);
                newThread.detach();
            }

            else{
                CLIENT[unit_ID] = newConnection;
                std::cout<<"Client connected successfully"<<std::endl;
                std::thread newThread(ClientHandler, unit_ID);
                newThread.detach();
            }
        }
    }
}
