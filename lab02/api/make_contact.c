/* make_contact.c */

#include <cnaiapi.h>
#include <string.h>

/*-----------------------------------------------------------------------
 * make_contact - open a new TCP connection to the specified IP address
 * (c) and port e(a).
 *-----------------------------------------------------------------------
 */
connection
make_contact(computer c, appnum a)
{
	struct sockaddr_in	sockaddr;
	int			sock;

	cnaiapi_init();
  
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return -1;

	(void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(a);
	sockaddr.sin_addr.s_addr = c;

	if (connect(sock, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in)) < 0) {
#if defined(LINUX) || defined(SOLARIS)
		close(sock);
#elif defined(WINDOWS)
		closesocket(sock);
#endif
		return -1;
	}  
	return sock;
}
   
