/* Client Lab 03 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define CLIENT_PROMPT "C> "
#define SERVER_PROMPT "S> "

#define BUFF_SIZE 1028

int readln(char *, int);

int main(int argc, char *argv[]) {	
	int sockfd;

	if ( argc < 2 || argc > 3) {
		printf(" Not enough args ");
		exit(1);
	}

	int port = atoi(argv[2]);
	char *dns = argv[1];

	//Step 1 - Start the socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) {
		printf( "Socket invalid.");
		exit(1);
	}

	//Step 2 - Connect to specified server
	struct sockaddr_in serverIPAddress; 
	struct hostent *host;
	struct in_addr **addr_list;
	struct in_addr addr;

 	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_port = htons((u_short) port);

	host = gethostbyname(dns);
	addr_list = (struct in_addr **)host->h_addr_list;
	addr.s_addr =  addr_list[0]->s_addr;

	serverIPAddress.sin_addr = addr;

	int connectError = connect( sockfd, (struct sockaddr*)&serverIPAddress, sizeof(struct sockaddr) );
	if( connectError < 0 ) {
		fprintf(stderr, "Connect Error\n");
		exit(1);
	}

	//Step 3 - Accept User Input 
	char buff[BUFF_SIZE];
	int len; 

	(void) printf(CLIENT_PROMPT);
	(void) fflush(stdout);

	while( 1 ) {
		while( (len = readln(buff, BUFF_SIZE)) > 0 ) {
			char recvBuff[4000];

			//Step 4 - Send Message
			int sendErr = send(sockfd, buff, len, 0);
			if ( sendErr < 0 ) {
				fprintf(stderr, "Send Error");
				exit(1);
			}

			//Step 5 - Rec Message
			int length = recv(sockfd, recvBuff, BUFF_SIZE, 0);
			if( recvBuff[0] == '1' ) {
				printf(SERVER_PROMPT);
				printf("1 \n");
			} else if ( recvBuff[0] == '-' ){
				printf(SERVER_PROMPT);
				printf("-1 \n");
			} else {
				printf(SERVER_PROMPT);
				fflush(stdout);
				write(1, recvBuff, length);
				printf("\n");
				fflush(stdout);
			}
		

			(void) printf(CLIENT_PROMPT);
			(void) fflush(stdout);
		}
	}
	
	//Step 6 - Close
	//close( sockfd );
}