#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> // Gethostbyname
#include <unistd.h>

#define STDIN 0
#define BUFF_SIZE 1024

char* makeMessage(char*, char*);
int readln(char *, int);
int recvline(int , char *, int );

int main(int argc, char *argv[]) 
{
	int socketfd;
	fd_set readfds;
	if ( argc != 5 ) {
		printf("Not enough args.\n");
		exit(1);
	}

	char* server_hostName = argv[1];
	int port = atoi(argv[2]);
	char* clientType = argv[3];
	char* channelId = argv[4];

	if(strlen(clientType) != 3){
		exit(1);
	}
	if(strlen(channelId) > 10) {
		exit(1);
	}

	char *msg = makeMessage(clientType, channelId);

	//Step 1 - Start the Socket
	socketfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( socketfd < 0 ) {
		perror("Error: ");
		exit(1);
	}

	//Step 2 - Connect to Specified Server from Args
	struct sockaddr_in serverAddr; 
	struct hostent *host;
	struct in_addr **addr_list;
	struct in_addr addr;

	memset( &serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = (u_short)port;

	//Pulling out from gethostbyname the address of DNS of server we trying to 
	//connect to. Gethostbyname returns a whole list of all possible connections
	//Pull out the first one of the list. and using that has our IP address for the server
	host = gethostbyname( server_hostName );
	addr_list = (struct in_addr **)host->h_addr_list;
	addr.s_addr = addr_list[0]->s_addr;
	serverAddr.sin_addr = addr;

	//Step 3 - Connect to the Server
	if( connect( socketfd, (struct sockaddr *) &serverAddr, sizeof( struct sockaddr ) ) < 0) {
		perror("Error: ");
	}

	//Step 4 - Send the initial message of ADV/CON
	if(send(socketfd, msg, strlen(msg), 0) < 0 ) 
		perror("Error: ");

	while(1) {
		//Step 5 - Setup Select Function. We want socket and STDIN communication
	    FD_ZERO(&readfds);
	    FD_SET(socketfd, &readfds);
	    FD_SET(STDIN, &readfds);
		if( select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0 ) {
			//perror("Select: ");
			exit(1);
		}

		//If it is coming from socket we know to display it to STDOUT
		if(FD_ISSET(socketfd, &readfds) ) {
			char recvBuff[BUFF_SIZE];
			int length = recvline(socketfd, recvBuff, BUFF_SIZE); 
			if( length != 0 ) {
				write(STDOUT_FILENO, recvBuff, length);
			} else {
				close( socketfd );
			}
		}
		//If it is coming from standard in we want to read from the input and send it
		//off to the client
		if(FD_ISSET(STDIN, &readfds)) {
			char buff[BUFF_SIZE];
			int len = readln(buff, BUFF_SIZE);
			if(len > 0 ) {
				if( send(socketfd, buff, len, 0) < 0 )
					perror("Send:");
			} else {
				close(socketfd);
			}
		}
	}

	printf("Closing Connection \n");
	free(msg);
	close(socketfd);
	return 0;
}

//Helper method to take Type of Message and pad the channel id together
char* makeMessage(char* type, char* id){
	char *msg = (char*) malloc(sizeof(char) * 14);
	strncpy(msg, type, strlen(type));
	strncat(msg, " ", sizeof(char));
	strncat(msg, id, strlen(id));
	int len = strlen(msg);
	if(len != 14){
		for(len; len != 14; len++ ) 
			strncat(msg, " ", sizeof(char));
	}
	return msg;
}

