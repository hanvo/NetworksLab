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
	int len; 
	char buff[BUFFSIZE];
	uint32_t expected, recieved;

	if ( argc != 2 ) {
		(void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
		exit(1);
	}
	
	/* wait for a connection from a client */
	conn = await_contact((appnum)atoi(argv[1]));

	if ( conn < 0 ) {
		exit(1);
	}

	//Read first few bytes for length of paragraph
	while( (len = recv(conn, &expected, sizeof(uint32_t), 0 ) > 0 )) {
		(void) printf(RECEIVED_PROMPT);
		fflush(stdout);

		expected = ntohl(expected);

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
	
	//program stops at EOF found on stdin
	(void) send_eof(conn);
	(void) printf("\n");
	return 0;
}