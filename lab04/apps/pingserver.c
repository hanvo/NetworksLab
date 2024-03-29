#include <stdio.h>
#include <stdlib.h>
#include <string.h>		
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h> // for EAGAIN and EWOULDBLOCK
#include <unistd.h> //sleep


/*-----------------------------------------------------------------------
 *
 * Program: pingserver (UDP Style)
 * Purpose: Sets up a server for pingclient to ping. 
 * Usage:   echoserver <port> [awake_period_in_seconds] [sleep_period_in_seconds]
 *
 *-----------------------------------------------------------------------
 */

int main(int argc, char *argv[])
{
	int socketfd;
	struct sockaddr_in serverInfo; 
	struct timeval awake;
	int32_t msg = 2;

	if( argc != 4 )
		exit(1);

	int port = atoi(argv[1]);
	int sleepTime = atoi(argv[3]);
	int convertedAwake = atoi(argv[2]);

	memset( &awake, 0, sizeof(awake) );

	awake.tv_sec = convertedAwake;

	/* -  Create a socket and bind the server's address to it. */

	socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( socketfd < 0 )
		exit(1);

 	memset( &serverInfo, 0, sizeof(serverInfo) );
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons((u_short) port);
	serverInfo.sin_addr.s_addr = INADDR_ANY;

	if( bind(socketfd, (struct sockaddr *) &serverInfo, sizeof(struct sockaddr)) < 0 ) {
		exit(1);
	}

	/* -  Use the timeval struct to store the values of sleep and awake time periods. */
    struct timeval t1, t2; 
    struct sockaddr_storage sender; //saves info of what packet came from where
	socklen_t sendsize = sizeof(sender);

    while( 1 ) {
    	if( setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &awake, sizeof(struct timeval)) ) {
			//printf("setsockoopt failed with errno: %s\n", strerror(errno));    	
		}

	    /* 1 - Get the current time (t1)
	       2 - Start receiving packets using rcvfrom 
	       3 - Check the return value of recvfrom and number of last error (errno), if it is EAGAIN or EWOULDBLOCK call sleep().  
	       4 - Get the current time again (t2), update awake_time based on the 
	         difference t2 -t1 (i.e. the awake_time_period = awake_time_period - (t2 - t1))
	       5 - The server replies back to the client with an 32 bit unsigned integer number (value = 2)
	    */

  		gettimeofday (&t1, NULL);
       	char buff[1];
       	if( recvfrom(socketfd, buff, sizeof(buff), 0, (struct sockaddr*)&sender, &sendsize) < 0 ) {
       		if(errno == EAGAIN ||errno == EWOULDBLOCK) {
       			sleep(sleepTime);
       		}
       	} else {
 			gettimeofday (&t2, NULL);
 			awake.tv_sec = awake.tv_sec - (t2.tv_sec - t1.tv_sec);
 			sendto(socketfd, &msg, sizeof(msg), 0,(struct sockaddr *) &sender, sizeof(struct sockaddr));
       	}
    }
}

