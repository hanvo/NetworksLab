#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
	int socketfd, clientConnectedFd;


	if ( argc != 2 ) {
		printf("Not enough args.\n");
		exit(1);
	}

	int port = atoi(argv[1]);
	printf("Port: %d \n", port);

	//Step 1: Start the socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if( socketfd < 0 ) {
		perror("Error: ");
		exit(1);
	}

	//Step 2 - Bind on to the socket 
	struct sockaddr_in serverAddr;
	memset( &serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = (u_short)port;

	int bindfd = bind( socketfd, (struct sockaddr *) &serverAddr, sizeof( serverAddr ) );
	if( bindfd < 0 ) {
		perror("Error: ");
		exit(1);
	}

	//Step 3 - Listen for incoming connections 
	int lisReturn = listen( socketfd, 1 );
	if( lisReturn < 0 ) {
		perror("Error: ");
	}

	while( 1 ) {
		//Step 4 - Accept a client  
		struct sockaddr_in client;
		int clientSize = sizeof(struct sockaddr_in);
		clientConnectedFd = accept( socketfd, (struct sockaddr *) &client, &clientSize );
		if( clientConnectedFd < 0 ) {
			perror("Error: ");
			return -1;
		}

		



	}




	return 0;
}
