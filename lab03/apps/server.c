/* Server Lab 03 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

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

	//Step 2 - Bind to a socket
	struct sockaddr_in serverIPAddress; 
 	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_port = htons((u_short) port);

	bindfd = bind( sockfd, (struct sockaddr *) &serverIPAddress, sizeof(serverIPAddress) );
	if( bindfd < 0 ) {
		printf("Bind error");
		return -1;
	}

	//Step 3 - Listen 
	int listenReturn = listen ( sockfd, 1 );
	if( listenReturn < 0 ) {
		printf( "Listen Failed " );
		return -1;
	}

	//Step 4 - Accepting 
	while( 1 ) {
		struct sockaddr_in client;
		int clientSize = sizeof( client );

		int clientSocketfd = accept( sockfd, (struct sockaddr *) &client, (socklen_t *) &clientSize);
		if( clientSocketfd < 0 ) {
			printf( "Client Socket Accept Failed" );
			return -1;
		}

		//Step 5 - Recv / Send 
		printf( " Work Performed here " );

		//Step 6 - Close
		close( clientSocketfd );
	}

}