#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> // Gethostbyname

char* channelIdPadding(char * id) {
	int len = strlen(id);
	int index;
	char buff[10];
	char* ret = malloc(10);
	if((len = strlen(id)) != 10 ) {
		for( index = 0; index < len; index++)
			buff[index] = id[index];
		for( index; index < 10; index++ )
			buff[index] = ' ';

		strcpy(ret, buff);
		return ret;
	}
	return id;
}

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
	char* formattedId = channelIdPadding(channelId);
	
	printf("Server HostName: %s\n", server_hostName);
	printf("Port: %d \n", port);
	printf("client Type: %s\n", clientType);
	printf("channel Id: %s \n", formattedId);
	printf("sizeof ID: %d\n", (int)strlen(formattedId));
	
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

	printf("Closing Connection \n");

	close(socketfd);


	free(formattedId);

	return 0;
}