/* server.c */

#include <stdlib.h> 
#include <stdio.h>
#include <cnaiapi.h> 
#include <netinet/in.h>

#define BUFFSIZE 1000000
#define RECEIVED_PROMPT	"Received paragraph >\n"

/*-----------------------------------------------------------------------
 *
 * Program: server
 * Purpose: wait for a connection from an client and echo paragraphs
 * Usage:   echoserver <appnum>
 *
 *-----------------------------------------------------------------------
 */

int main (int argc, char *argv[]) {
	connection conn;
	//Make server listen forever or until ctrl-c 
	while(1) {
		int len; 
		char buff[BUFFSIZE];
		uint32_t expected, recieved;

		if ( argc != 2 ) {
			(void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
			exit(1);
		}
		
		// Wait for a connection from a client 
		conn = await_contact((appnum)atoi(argv[1]));
		if ( conn < 0 ) {
			exit(1);
		}

		//Read in how many bytes the server expects to get
		while( (len = recv(conn, &expected, sizeof(uint32_t), 0 ) > 0 )) {
			(void) printf(RECEIVED_PROMPT);
			fflush(stdout);

			//Take the expected value and parse it according to what
			//machine you are on
			expected = ntohl(expected);

			// Recieve bytes until expected amount is reached
			for( recieved = 0; recieved < expected; ) {
				len = recv(conn, buff, (expected - recieved) < BUFFSIZE ? 
					(expected - recieved) : BUFFSIZE, 0);
				if( len < 0 ) {
					send_eof(conn);
					return 1;
				}
				(void) write(STDOUT_FILENO, buff,len);
				recieved += len;
			}
		}
	}
}