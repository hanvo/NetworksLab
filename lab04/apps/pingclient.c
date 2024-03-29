#include <stdio.h>
#include <stdlib.h>
#include <string.h>		
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h> //Removes Casting error for gethostbyname
#include <arpa/inet.h> // inet_ntoa
#include <sys/time.h>

#define clientRes "Reply from server %s, latency : %ldusec\n"

/*-----------------------------------------------------------------------
 *
 * Program: pingclient (UDP Style)
 * Purpose: Sends a ping to servers and outputs Server IP in 
 * 			Dot Notation with Latency in useconds.
 * Usage:   pingclient <port> server1 server2 ... Up to 12
 *
 *-----------------------------------------------------------------------
 */

int main(int argc, char *argv[])
{
	int socketfd;
	int32_t msg = 1;
	char recBuff[1];

	/* - Check if args has at least 1 server to ping but at most 12 */
	if( argc < 3 )
		exit(1);

	if( argc > 14 )
		exit(1);
	
	int port = atoi(argv[1]);

	/* -  Create a socket  */
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if( socketfd < 0 )
		exit(1);

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
	if (bindErr < 0 ) 
		exit(1);

  	memset(&serverAddr, '0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short) port);
	struct hostent *destination;

	/* - Send the packet to server */ 
	int counter;
	struct timeval start;
	gettimeofday(&start, NULL);
	for(counter = 2; counter < argc; counter++) {
		destination = gethostbyname(argv[counter]);
		addr_list = (struct in_addr**)destination->h_addr_list;
		addr.s_addr = addr_list[0]->s_addr;
		serverAddr.sin_addr = addr;
		sendto(socketfd, &msg, sizeof(msg), 0, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
	  	//Clear the memory and then reset values
	  	memset(&serverAddr, '0', sizeof(serverAddr));
  	    serverAddr.sin_family = AF_INET;
    	serverAddr.sin_port = htons((u_short) port);
	}

	/* - Recv packets  */ 
 	for(counter = 2; counter < argc; counter++) {
 		struct timeval end,difference;
    	struct sockaddr_storage sender;
		socklen_t sendsize = sizeof(sender);
   		recvfrom(socketfd, recBuff, sizeof(recBuff), 0, (struct sockaddr*)&sender, &sendsize);
   		if(recBuff[0] == 2) {
   			gettimeofday(&end, NULL);
 			struct sockaddr_in *ipv4Sender = (struct sockaddr_in *)&sender;
 			struct in_addr ipAddrSender = ipv4Sender->sin_addr;
 			char * ipAddr = inet_ntoa(ipAddrSender);
 			timersub(&end ,&start, &difference);
 			long totalTime = (long)difference.tv_usec + ((long)difference.tv_sec * 1000000);
 			(void) printf(clientRes,ipAddr,totalTime);
   		}
 	}
}
