#include "SocketConnection.h"

	void error(const char *msg)
	{
	   perror(msg);
	   //WSACleanup();
	   exit(1);
	}
	SocketConnection::SocketConnection(){
		init();
		}
	SocketConnection::SocketConnection(int option){
		switch(option){
		case SERVER:	
			init();
			break;
		case CLIENT: 
			server=(int)NULL;
			initClient();
			break;
		}
		
	}

int SocketConnection::init(){
		/*int err;
		err = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
		if ( err != 0 ){//|| ( LOBYTE( wsaData.wVersion ) != 0 || HIBYTE( wsaData.wVersion ) != 2 )) {
			error("Could not find useable sock dll");
		}*/

		//Initialize sockets and set any options
		int * p_int ;
		server = socket(AF_INET, SOCK_STREAM, 0);
		if(server== -1){
			error("Error initializing socket");
		}
		//setting options of the socket, 
		p_int = (int*)malloc(sizeof(int));
		*p_int = 1;
		if( (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )|| (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
			free(p_int);
			close(server);
			error("Error sesetting options");
		
		}
		free(p_int);
    
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(SERVER_PORT);


		if( bind( server, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1 ){
			close(server);
			//WSACleanup();
			error("Error binding to socket, make sure nothing else is listening on this port");
		}
		if(listen( server, 1) == -1 ){
			close(server);
			error("Error listening");
		}
		return 0;
	}

int SocketConnection::initClient(){
		/*int err;
		err = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
		if ( err != 0 ){//|| ( LOBYTE( wsaData.wVersion ) != 0 || HIBYTE( wsaData.wVersion ) != 2 )) {
			error("Could not find useable sock dll");
		}*/

		//Initialize sockets and set any options
		client = socket(AF_INET, SOCK_STREAM, 0);
		if(client== -1){
			error("Error initializing socket");
		}
    
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
		client_addr.sin_port = htons(SERVER_PORT);

		return 0;
	}
int	SocketConnection::connectClient(){
	if (connect(client,(struct sockaddr*)&client_addr,sizeof(client_addr))<0)
		error("Client connection failed\n");
	return 0;
}
int	SocketConnection::waitClients(){
		int addr_size = sizeof(struct sockaddr);

		//waiting clients.
		while(true){
			printf("waiting for a connection\n");
			if((client= accept( server, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size))<0){
				printf("Received connection from %s\n",inet_ntoa(client_addr.sin_addr));
				break;
	//			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
			}
			else{
				error("Error accepting");
			}
		}
		return 0;
	}
	
int	SocketConnection::readUntil(char c){
			oneByte='\0';
			nb=0;
			memset(buffer,0,BUFFERLENGTH);
			while((nb<BUFFERLENGTH)&&oneByte!=c){
				n = read(client,&oneByte,1);
				if (n <= 0){
					close(client);
					if (server != (int)NULL) close(server);
					error("ERROR reading from socket");
				}
				else {
					buffer[nb]=oneByte;
					nb++;
				}
			}
			return 0;
}
	
int	SocketConnection::writeData(char* msg){
		memset(buffer,0,BUFFERLENGTH);
		strcpy(buffer,msg);
		int res;
		res = write( client, buffer, (int)strlen(buffer));
		if (res < 0 ) {
			close(client);
			if (server != (int)NULL) close(server);
			error("send failed");
		}
		return 0;
	}
int	SocketConnection::printBuffer(){
	printf("%s\n\n",buffer);
	return 0;
	}
int SocketConnection::closeClient(){
	close(client);
	return 0;
	}

int SocketConnection::closeServer(){
	close(client);
	close(server);
	//WSACleanup();
	return 0;
	}
