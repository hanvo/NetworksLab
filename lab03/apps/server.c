/* Server Lab 03 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>


#define SERVER_PROMPT "S> "

#define BUFF_SIZE 1028

int main(int argc, char *argv[]) {	
	int sockfd, bindfd, clientSocketfd; 

	if ( argc != 2 ) {
		printf(" Not enough args ");
		return -1;
	}

	int port = atoi(argv[1]);

	//Step 1 - Start the socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) {
		printf( "Socket invalid.");
		return -1;
	}

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof( int ) );

	//Step 2 - Bind to a socket
	struct sockaddr_in serverIPAddress; 
 	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_port = htons((u_short) port);

	bindfd = bind( sockfd, (struct sockaddr *) &serverIPAddress, sizeof(serverIPAddress) );
	if( bindfd < 0 ) {
		printf("Bind error \n");
		return -1;
	}

	//Step 3 - Listen 
	int listenReturn = listen ( sockfd, 1 );
	if( listenReturn < 0 ) {
		printf( "Listen Failed " );
		return -1;
	}

	while( 1 ) {
		int len;
		//Step 4 - Accepting 
		struct sockaddr_in client;
		int clientSize = sizeof( struct sockaddr_in );

		clientSocketfd = accept( sockfd, (struct sockaddr *) &client, &clientSize);
		if( clientSocketfd < 0 ) {
			printf( "Client Socket Accept Failed" );
			return -1;
		} 
	
		//Step 5 - Recv 
		char buff[BUFF_SIZE];
		char keyOpen[5];
		char keyRead[5];
		char keyBack[5];
		char keyClos[5];

		char check[5];
		strcpy(keyOpen, "OPEN ");
		strcpy(keyRead, "READ ");
		strcpy(keyBack, "BACK ");
		strcpy(keyClos, "CLOS ");

		while((len = recv(clientSocketfd, buff, BUFF_SIZE, 0) > 0)) {
			strncpy(check, buff, 5);
			if( strncmp(check, keyOpen, 5) == 0 ) {
				printf("OPEN\n");
				char fileBuffer[BUFF_SIZE];
				int x;
				int counter = 0; 
				for( x = 5; buff[x] != '\n'; x++ ) {
					fileBuffer[counter] = buff[x];
					counter++;
				}
				fileBuffer[counter] = '\0';

				int openFd = open(fileBuffer, O_RDONLY);
				printf("OPENFB: %d\n", openFd);

			} else if( strncmp(check, keyRead, 5) == 0  ) {
				printf("READ\n");
			} else if( strncmp(check, keyBack, 5) == 0  ) {
				printf("Back\n");
			} else if( strncmp(check, keyClos, 5) == 0  ) {
				printf("clos\n");
			} else {
				printf("No command.\n");
			}
			printf("Await new message \n");					
		}



		//Step 6 - Send	
	} 

	//Step 7 - Close
	//close( clientSocketfd );
}
