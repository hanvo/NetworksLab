#include <stdio.h>
#include <stdlib.h>
#include <string.h>		
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h> //Removes Casting error for gethostbyname
#include <arpa/inet.h> // inet_ntoa

/* Client - ./pingclient  application_number hostname1_to_ping  hostname2_to_ping  ... */

#define clientRes "Reply from server "

int main(int argc, char *argv[])
{
	int socketfd;
	int32_t msg = 1;
	char recBuff[1];

	if( argc < 2 && argc > 13) {
		printf("Not enough Args or too much\n" );
		exit(1);
	}

	int port = atoi(argv[1]);

	/* -  Create a socket  */
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if( socketfd < 0 ) {
		printf("Socket Failed");
		exit(1);
	}

	struct sockaddr_in serverAddr, myAddr;
	struct hostent *server;
	struct in_addr **addr_list;
	struct in_addr addr;


	/* -  Binding - UDP binding needs to set the client   */
  	memset(&myAddr, '0', sizeof(myAddr));
  	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons((u_short) 0);  // This line of code is used to allow OS assign a port number
	myAddr.sin_addr.s_addr = INADDR_ANY;
	int bindErr = bind(socketfd, (struct sockaddr * )&myAddr, sizeof(struct sockaddr_in) );
	if (bindErr < 0 ) {
		printf("Bind Err\n");
		exit(1);
	}

  	memset(&serverAddr, '0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short) port);
	struct hostent *destination;

	/* - Send the packet to server */ 
	int counter;
	for(counter = 2; counter < argc; counter++) {
		struct timeval start;

		destination = gethostbyname(argv[counter]);
		addr_list = (struct in_addr**)destination->h_addr_list;
		addr.s_addr = addr_list[0]->s_addr;
		serverAddr.sin_addr = addr;

		gettimeofday(&start, NULL);

		int sendErr = sendto(socketfd, &msg, sizeof(msg), 0, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
		if( sendErr < -1 ) {
			printf("SendTo broke\n");
			exit(1);
		}

		printf("Sent to server msg: %d\n", msg);
	  	//Clear the memory and then reset values
	  	memset(&serverAddr, '0', sizeof(serverAddr));
  	    serverAddr.sin_family = AF_INET;
    	serverAddr.sin_port = htons((u_short) port);

	}


 	for(counter = 2; counter < argc; counter++) {
 		struct timeval end;
 		printf("Waiting for reply. \n");
    	struct sockaddr_storage sender;
		socklen_t sendsize = sizeof(sender);
   		int recvErr = recvfrom(socketfd, recBuff, sizeof(recBuff), 0, (struct sockaddr*)&sender, &sendsize);
   		if(recvErr < 0 ) {
   			printf("Error with recvfrom");
   			exit(1);
   		}
   		gettimeofday(&end, NULL);
 			
 		struct sockaddr_in *ipv4Sender = (struct sockaddr_in *)&sender;
 		struct in_addr ipAddrSender = ipv4Sender->sin_addr;
 		char * ipAddr = inet_ntoa(ipAddrSender);
 		printf("Ip Address of Sender: %s \n", ipAddr);
 	}


	




   	//printf("Finished Closing\n" );
   	//close(socketfd);

	return 0;
}
