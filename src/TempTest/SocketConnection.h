/*
A simply Class to manage socket connections.

*/



#ifndef _SOCKETCONNECTION_H_
#define _SOCKETCONNECTION_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/timeb.h>
//#include <fstream>
//#include <iostream>
//#include "HVPowerSupply.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <math.h>

/*windows*/
//#include "winsock.h"
//#include "stdio.h"
//#include "windows.h"

#define SERVER_IP "192.168.1.31"
#define SERVER_PORT 20000
#define SERVER 0
#define CLIENT 1
#define BUFFERLENGTH 1024
class SocketConnection{
private:
	int server,client, portno, n,nb;
	struct sockaddr_in serv_addr,client_addr;
	char oneByte;
//	WSADATA wsaData;

public:
	char buffer[BUFFERLENGTH];
	SocketConnection();
	SocketConnection(int option);
	int init();
	int initClient();
	int connectClient();
	int waitClients();
	int readUntil(char c);
	int writeData(char* msg);
	int printBuffer();
	int closeServer();
	int closeClient();
	//~SocketConnection();
};
#endif
