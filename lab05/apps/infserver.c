#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_ROOMS 10
#define LENGTH_OF_MSG 11
#define BUFF_SIZE 1024

int recvline(int , char *, int );

int main(int argc, char *argv[]) 
{
	int socketfd, clientConnectedFd;

	if ( argc != 2 ) {
		exit(1);
	}

	int port = atoi(argv[1]);

	//Step 1: Start the socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if( socketfd < 0 ) {
		perror("Error: ");
		exit(1);
	}

	//In cases where I shut my server before my client 
	//I would get locked out of my port. THis code allows me to reconnect to same port
	//following that mistake.
	int optval = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof( int ) );

	//Step 2 - Bind on to the Master socket 
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


	/* Book Keeping Arrays
	* Room - Channel Id storage
	* ClientFds - Channel ID's accepted_File_Descriptor 
	* Indices will be the same from Room and ClientFDs
	*/
	char room[MAX_ROOMS][LENGTH_OF_MSG];
	int clientFds[MAX_ROOMS];
	//short numRoomsFilled = 0;

	memset(room, '\0', sizeof(room[0][0])* MAX_ROOMS * LENGTH_OF_MSG);
	memset(clientFds, -1 , sizeof(int));

	while( 1 ) {
		//Step 4 - Accept a client  
		struct sockaddr_in client;
		int clientSize = sizeof(struct sockaddr_in);
		clientConnectedFd = accept( socketfd, (struct sockaddr *) &client, &clientSize );
		if( clientConnectedFd < 0 ) {
			perror("Error: ");
			exit(1);
		}

		//Step 5 - Rec the initial message of [Type] [ChannelId]
		char recBuff[14];
		if( recv(clientConnectedFd , recBuff, 14, 0) < 0 )
			perror("Error: ");
			
		//Step 6 - Parse out between [Type] [ChannelId] 
		char* type = strtok(recBuff, " ");

		char chanId[11];
		int counter;
		for(counter = 0; counter < 10; counter++) {
			chanId[counter] = recBuff[counter + 4];
		}
		chanId[10] = '\0';

		if( strcmp(type, "ADV") == 0) {			
			//Checking if ADV Channel ID already exists 
			int index;
			for( index = 0; index < MAX_ROOMS; index++) {
				if( strcmp(room[index], chanId) == 0)
					break;
			}

			//If it cycles and finds nothing it will add it to room array and 
			// in clientFds array to keep track when something wants to pair up
			if(index == MAX_ROOMS) {
				int freeSpot;
				for(freeSpot = 0; freeSpot < MAX_ROOMS; freeSpot++) {
					if(strcmp(room[freeSpot], "\0") == 0) {
						break;
					}
				}
				strncpy(room[freeSpot], chanId, 10);
				clientFds[freeSpot] = clientConnectedFd;
			} else{
				//If someone req to ADV to same connection just close it. 
				printf("Closed a client\n");
				close(clientConnectedFd);
			}
		} else if( strcmp(type, "CON") == 0) {
			//Check if there are any matching chanIds
			int index;

			for( index = 0; index < MAX_ROOMS; index++) {
				if( strcmp(room[index], chanId) == 0) {
					break;
				}
			}
			if( index != MAX_ROOMS ) {
				int advfd = clientFds[index];
				int pid;
				//Begin the Forking Process
				//Child - Will handle the communciation between ADV and CON
				//Parent - will remove the instances from the two book keeping arrays
				if( (pid = fork()) == -1 ) {
					close(clientConnectedFd);
				} else if( pid == 0 ) {
					close(socketfd);
					while(1) {
						//Start the Select Call Setup
						//It will look for ClientConnectedFd (CON Client) and it will pull out the advfd(ADV client)
						//and set up the message passage between the two clients
						fd_set readfds;
						FD_ZERO(&readfds);
						FD_SET(clientConnectedFd, &readfds);
						FD_SET(advfd, &readfds);
						if( select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0 ) 
							perror("Error: ");

						//If it is coming from the CON client redirect the message to ADVFD
						if(FD_ISSET(clientConnectedFd, &readfds)) {
							char recvBuff[BUFF_SIZE];
							int length = recvline(clientConnectedFd, recvBuff, BUFF_SIZE); 
							if( length == 0 ) {
								exit(1);
							} else {
								int sendLen;
								if( (sendLen = send(advfd, recvBuff, length, 0)) < 0) {
									perror("Error: ");
								}
							}
						}
						//If it is coming from the ADV client take in the message and redirect it to CON client
						if(FD_ISSET(advfd, &readfds)) {
							char recvBuff[BUFF_SIZE];
							int length = recvline(advfd, recvBuff, BUFF_SIZE); 
							if( length == 0 ) {
								exit(1);
							} else {
								int sendLen;
								if( (sendLen = send(clientConnectedFd, recvBuff, length, 0)) < 0) {
									perror("Error: ");
								}
							}
						}
					}
				} else if( pid > 0 ) {
					//parent process go remove stuff from the arrays 
					room[index][0] = '\0';
					clientFds[index] = -1;
					close(advfd);
					close(clientConnectedFd);				
				}
			} else {
				close(clientConnectedFd);
			}
		} else {
			close(clientConnectedFd);
		}
	}
	return 0;
}
