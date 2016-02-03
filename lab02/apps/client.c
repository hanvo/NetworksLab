/* client.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <netinet/in.h>

#define BUFFSIZE 1000000

int readln(char *, int);

/*-----------------------------------------------------------------------
 *
 * Program: client
 * Purpose: contact server, send user input and print server response
 * Usage:   client <compname> [appnum]
 * Note:    Appnum is optional. If not specified the standard echo appnum
 *          (7) is used.
 *
 *-----------------------------------------------------------------------
 */

int main(int argc, char *argv[]) {
	computer comp;
	appnum app;
	connection conn;
	char buff[BUFFSIZE];
	uint32_t len;

	if( argc < 2 || argc > 3 ) {
		(void) fprintf(stderr, "usage: %s <compname> [appnum]\n",
			argv[0]);
		exit(1);
	}

	//Parsing the computer(Localhost,PC) and the appnumber(port) 
	//into strings
	comp = cname_to_comp( argv[1] );
	if(comp == -1) {
		exit(1);
	}
	if( argc == 3 ) {
		app = (appnum) atoi(argv[2]);
	} else {
		if((app = appname_to_appnum("echo")) == -1) {
			exit(1);
		}
	}

	//Form Connection
	conn = make_contact(comp, app);
	if( conn < 0 ) {
		exit(1);
	}

	//This time we want client to read and then server to output
	//
	while((len = readln(buff, BUFFSIZE)) > 0 ) {
		//(void) printf("len: %d\n", len);
		//Check the beginning of each new perspective paragraph. 
		//Paragraph must start with \t
		if( buff[0] == '\t'){
			//SendLen to ensure you are sending exactly the same amount
			//you are sending
			//SendBuffer = buffer used in send. Contains legal paragraphs
			int sendLen;
			char sendBuffer[BUFFSIZE];

			//changing the length according to endians 
			uint32_t sendLength = htonl(len);

			//(void) printf("sendLength: %d \n", sendLength);

			//sending the length of the file to the server for it to prep incoming
			//messages
			send(conn, &sendLength, sizeof(uint32_t), 0);
			
			//Copy characters into new sending buffer to be sent to server 
			for(sendLen = 0; sendLen < len; sendLen++) {
				//(void)printf("put letter %c into sendbuff \n", buff[sendLen]);
				sendBuffer[sendLen] = buff[sendLen];
			}
			sendBuffer[sendLen + 1] = '\0'; 
			//(void) printf("Send Buffer: %s \n", sendBuffer);
			(void) send(conn, sendBuffer, sendLen, 0);
			(void) fflush(stdout);
		} else {
			//(void) printf("No tab found! Not paragraph Skipping\n");
		}
	}
	send_eof(conn);
	return 0;
}