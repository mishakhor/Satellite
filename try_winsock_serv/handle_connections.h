//
// Created by AS on 4/15/2022.
//

#ifndef TRY_WINSOCK_SERV_HANDLE_CONNECTIONS_H
#define TRY_WINSOCK_SERV_HANDLE_CONNECTIONS_H

#include "includes.h"


//functions for new connections

void ClientHandler(const int& client_ID);

[[noreturn]] void SatHandler(int sat_ID);

[[noreturn]] void NewClients(SOCKET sListen, SOCKADDR_IN addr);
void isconnected(const char *message, int* c);

//maybe rewrite them as a new file
int commands(const char *message);
void get_coords(char *message);


#endif //TRY_WINSOCK_SERV_HANDLE_CONNECTIONS_H
