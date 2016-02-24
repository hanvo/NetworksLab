#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> // Gethostbyname

char* makeMessage(char*, char*);

int main(int argc, char *argv[]) 
{
	int socketfd;

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

	if( connect( socketfd, (struct sockaddr *) &serverAddr, sizeof( struct sockaddr ) ) < 0) {
		perror("Error: ");
	}

	printf("Sending: %s",msg);
	printf("sizeof(msg): %d\n", (int)strlen(msg));
	if(send(socketfd, msg, strlen(msg), 0) < 0 ) 
		perror("Error: ");


	printf("Closing Connection \n");
	free(msg);
	close(socketfd);
	return 0;
}

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
