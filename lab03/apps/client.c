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
		exit(1);
	}

	int port = atoi(argv[2]);
	char *dns = argv[1];

	//Step 1 - Start the socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) {
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
		//fprintf(stderr, "Connect Error\n");
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
			send(sockfd, buff, len, 0);

			//Step 5 - Rec Message
			int length = recv(sockfd, recvBuff, BUFF_SIZE, 0);
			if( recvBuff[0] == '1' && recvBuff[1] == '\n' ) {
				(void) printf(SERVER_PROMPT);
				(void) fflush(stdout);
				write(1, recvBuff, length);
				(void) printf("\n");
				(void) fflush(stdout);
			} else if ( recvBuff[0] == '-' ){
				(void) printf(SERVER_PROMPT);
				(void) fflush(stdout);
				write(1, recvBuff, 2);
				(void) printf("\n");
				(void) fflush(stdout);
			} else {
				(void) printf(SERVER_PROMPT);
				(void) fflush(stdout);
				write(1, recvBuff, length);
				(void) printf("\n");
				(void) fflush(stdout);
			}
			(void) printf(CLIENT_PROMPT);
			(void) fflush(stdout);
		}
		break;
	}
	
	//Step 6 - Close
	close( sockfd );
}