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
			//Step 4 - Send Message
			int sendErr = send(sockfd, buff, len, 0);
			if ( sendErr < 0 ) {
				fprintf(stderr, "Send Error");
				exit(1);
			}
			printf("len sent: %d\n", len + 1);


			//Step 5 - Rec Message


			(void) printf(CLIENT_PROMPT);
			(void) fflush(stdout);
		}
	}
	
	//Step 6 - Close
	//close( sockfd );
}