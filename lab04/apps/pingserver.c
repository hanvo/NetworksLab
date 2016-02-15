#include <stdio.h>
#include <stdlib.h>
#include <string.h>		
#include <netinet/in.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{
	int socketfd;
	struct sockaddr_in serverInfo; 
	struct timeval awake, sleep;

	if( argc < 4 ) {
		printf("Not enough Args\n" );
		exit(1);
	}

	int port = atoi(argv[1]);
	awake.tv_sec = atoi(argv[2]);
	sleep.tv_sec = atoi(argv[3]);

	printf("Port: %d\n", port);

	/* -  Create a socket and bind the server's address to it. */

	socketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if( socketfd < 0 ) {
		printf("Socket Failed");
		exit(1);
	}

 	memset( &serverInfo, 0, sizeof(serverInfo) );
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons((u_short) port);

	if( bind(socketfd, (struct sockaddr *) &serverInfo, sizeof(struct sockaddr)) < 0 ) {
		printf("Bind Failed\n");
		exit(1);
	}

	/* -  Use the timeval struct to store the values of sleep and awake time periods. */

    struct timeval tvalBefore, tvalAfter; 
    struct sockaddr_storage sender;
	socklen_t sendsize = sizeof(sender);

    while( 1 ) {
    	int err = setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &awake, sizeof(awake));
	   
	    /* 1 - Get the current time (t1)
	       2 - Start receiving packets using rcvfrom 
	       3 - Check the return value of recvfrom and number of last error (errno), if it is EAGAIN or EWOULDBLOCK call sleep().  
	       4 - Get the current time again (t2), update awake_time based on the 
	         difference t2 -t1 (i.e. the awake_time_period = awake_time_period - (t2 - t1))
	       5 - The server replies back to the client with an 32 bit unsigned integer number (value = 2)
	    */
       gettimeofday (&tvalBefore, NULL);
       char buff[1];
       printf("recvfroming");
       int recvErr = recvfrom(socketfd, buff, sizeof(buff), 0, (struct sockaddr*)&sender, &sendsize);
       if(recvErr < 0 ) {
       		printf("Error with recvfrom");
       		exit(1);
       }


    }


	close(socketfd);

	return 0;
}

