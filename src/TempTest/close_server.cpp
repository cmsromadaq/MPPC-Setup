#include "SocketConnection.h"

int main(int argc, char *argv[]){
	SocketConnection *sock=new SocketConnection(CLIENT);
	sock->connectClient();
	sock->writeData("option -1!");
	sock->readUntil('!');
	sock->printBuffer();
	sock->closeClient();	
	
	return 0;
}
