/* Server Lab 03 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>

#define BUFF_SIZE 1028
#define FILE_SIZE 100000

int main(int argc, char *argv[]) {	
	int sockfd, bindfd, clientSocketfd; 

	if ( argc != 2 ) {
		//printf(" Not enough args ");
		return -1;
	}

	int port = atoi(argv[1]);

	//Step 1 - Start the socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) {
		//printf( "Socket invalid.");
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
		//printf("Bind error \n");
		return -1;
	}

	//Step 3 - Listen 
	int listenReturn = listen ( sockfd, 1 );
	if( listenReturn < 0 ) {
		//printf( "Listen Failed " );
		return -1;
	}

	while( 1 ) {
		int len;
		//Step 4 - Accepting 
		struct sockaddr_in client;
		int clientSize = sizeof( struct sockaddr_in );

		clientSocketfd = accept( sockfd, (struct sockaddr *) &client, &clientSize);
		if( clientSocketfd < 0 ) {
			//printf( "Client Socket Accept Failed" );
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

		int openFd = -1;
		int sizeOfFile = 0;
		int fileSizeLeft;

		while((len = recv(clientSocketfd, buff, BUFF_SIZE, 0) > 0)) {
			strncpy(check, buff, 5);
			int reply = 1;
			char serverReply[2];

			if( strncmp(check, keyOpen, 5) == 0 ) {
				if( openFd != -1 ){
					reply = -1;
				} else {
					char fileBuffer[BUFF_SIZE];
					int x;
					int counter = 0; 
					for( x = 5; buff[x] != '\n'; x++ ) {
						fileBuffer[counter] = buff[x];
						counter++;
					}
					fileBuffer[counter] = '\0';

					//check if file Name has..
					if( strstr(fileBuffer, "..") ) {
						//printf("CONTAINS ..\n");
						reply = -1;
					}

					openFd = open(fileBuffer, O_RDWR);
					if( openFd < 0 ) {
						//does not exist file already open 
						//printf("Error opening\n");
						reply = -1;
					}

					sizeOfFile = lseek(openFd, 0, SEEK_END); //get file size
					lseek(openFd, 0, 0); //move pointer back to beginning
					fileSizeLeft = sizeOfFile;
				}

				snprintf(serverReply, 3, "%d",reply);
				int sendErr = send(clientSocketfd, serverReply,(int)strlen(serverReply), 0);

			} else if( strncmp(check, keyRead, 5) == 0  ) {
				//printf("READ\n");
				char readlen[BUFF_SIZE];
				char fileRead[FILE_SIZE];
				char sendBuffer[BUFF_SIZE];
				int x;
				int counter = 0;
				for( x = 5; buff[x] != '\n'; x++ ) {
					readlen[counter] = buff[x];
					counter++;
				}
				readlen[counter] = '\0';
				int length = atoi(readlen);

				int readErr;
				if( (fileSizeLeft - length) > 0 ) {
					//everything is good to read still got room
					fileSizeLeft = fileSizeLeft - length;
					readErr = read(openFd, fileRead, length);
				} else {
					//readlen too long. read rest of file. 
					//printf("READ WHAT IS LEFT\n");
					readErr = read(openFd, fileRead, fileSizeLeft);
					fileSizeLeft = 0;
				}

				if( readErr <= 0 ) {
					reply = -1;
				} else {
					fileRead[readErr] = '\0';
					reply = readErr;
				}

				snprintf(serverReply, 3, "%d",reply);
				strcpy(sendBuffer, serverReply);
				strcat(sendBuffer, " ");
				strcat(sendBuffer, fileRead);
				strcat(sendBuffer, "\0");

				int sendErr = send(clientSocketfd, sendBuffer,(int)strlen(sendBuffer), 0);

			} else if( strncmp(check, keyBack, 5) == 0  ) {
				//printf("Back\n");
				char backlen[BUFF_SIZE];
				int x;
				int counter = 0;
				for( x = 5; buff[x] != '\n'; x++ ) {
					backlen[counter] = buff[x];
					counter++;
				}
				backlen[counter] = '\0';
				int length = atoi(backlen);

				if( (sizeOfFile == fileSizeLeft) || ((fileSizeLeft + length) > sizeOfFile) ) {
					//printf("CANT GO BACK NO MORE \n");
					reply = -1;
				} else {
					int newPos = sizeOfFile - (fileSizeLeft + length);
					//printf("newPos: %d\n",newPos);
					lseek(openFd, newPos, SEEK_SET);
					fileSizeLeft = fileSizeLeft + length;
				}
				snprintf(serverReply, 3, "%d",reply);
				int sendErr = send(clientSocketfd, serverReply,(int)strlen(serverReply), 0);

			} else if( strncmp(check, keyClos, 4) == 0  ) {
				//printf("clos\n");
				if( openFd == -1 ) {
					reply = -1;
				} else {
					int err = close(openFd);
					if( err < 0 ) {
						reply = -1;
					}
					sizeOfFile = 0;
					fileSizeLeft = -1;
					openFd = -1;
				}
				snprintf(serverReply, 3, "%d",reply);
				int sendErr = send(clientSocketfd, serverReply,(int)strlen(serverReply), 0);

			} else {
				//printf("No command.\n");
			}
			//printf("Await new message \n");					
		}
		//Step 6 - Send	
	} 

	//Step 7 - Close
	//close( clientSocketfd );
}
