#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_ROOMS 10
#define LENGTH_OF_MSG 10

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

	char room[MAX_ROOMS][LENGTH_OF_MSG];
	short numRoomsFilled = 0;

	memset(room, '\0', sizeof(room[0][0])* MAX_ROOMS * LENGTH_OF_MSG);

	while( 1 ) {
		//Step 4 - Accept a client  
		struct sockaddr_in client;
		int clientSize = sizeof(struct sockaddr_in);
		clientConnectedFd = accept( socketfd, (struct sockaddr *) &client, &clientSize );
		if( clientConnectedFd < 0 ) {
			perror("Error: ");
			exit(1);
		}

		//Step 5 - Rec message
		char recBuff[14];
		if( recv(clientConnectedFd , recBuff, 14, 0) < 0 )
			perror("Error: ");
			
		//Step 6 - Parse out between ADV CON 
		char* type = strtok(recBuff, " ");

		char chanId[10];
		int counter;
		for(counter = 0; counter < 10; counter++) {
			chanId[counter] = recBuff[counter + 4];
		}

		if( strcmp(type, "ADV") == 0) {
			printf("ADV\n");

			strncpy(room[0], "12        ", 10);
			printf("chanId: %sEND\n", chanId);
			int index;
			for( index = 0; index < MAX_ROOMS; index++) {
				if( strcmp(room[index], chanId) == 0)
					break;
			}

			printf("index = %d\n", index);
			if(index == MAX_ROOMS) {
				int freeSpot;
				for(freeSpot = 0; freeSpot < MAX_ROOMS; freeSpot++) {
					if(strcmp(room[freeSpot], "\0") == 0) {
						printf("Breaking!\n");
						break;
					}
				}
				printf("Next avaiable spot is: %d\n", freeSpot);
				printf("Sizeof channelid: %d\n", (int)sizeof(chanId));
				strncpy(room[freeSpot], chanId, sizeof(chanId));
				printf("Value at FreeSpot: %sEND\n", room[freeSpot]);
			} else{
				printf("found it\n");
			}
		} else if( strcmp(type, "CON") == 0) {
			printf("CON\n");
		} else {
			printf(" Not a valid command exit client gracefully.\n");
		}
	}
	return 0;
}
