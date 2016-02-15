/* await_contact.c */

#include <cnaiapi.h>
#include <string.h>

/*-----------------------------------------------------------------------
 * await_contact - accept a connection on port a. If no master sock is
 * already open for the port, create one and record it in the port-to-
 * socket table.
 *-----------------------------------------------------------------------
 */
connection
await_contact(appnum a)
{
	struct sockaddr_in	sockaddr, csockaddr;
	int			sock, newsock, i, csockaddrlen;
	static int		p2sinit = 0;
	static struct port2sock	p2s[P2S_SIZE];

	cnaiapi_init();
	
	if (a == 0)
		return -1;

#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&await_contact_mutex);
#elif defined(WIN32)
	WaitForSingleObject(await_contact_mutex, INFINITE);
#endif

		if (p2sinit == 0) {
			(void) memset(p2s, 0, sizeof(p2s));
			p2sinit = 1;
		}

	/* look up master socket in port-to-socket table */
	for (sock = -1, i = 0; i < P2S_SIZE; i++) {
		if (p2s[i].port == a) {
			sock = p2s[i].sock;
			break;
		}
	}

	if (sock == -1) {
		/* open new master socket */

		/* look for room in p2s table */
		for (i = 0; i < P2S_SIZE; i++) {
			if (p2s[i].port == 0)
				break;
		}
		if (i == P2S_SIZE) {
			/* no room left in p2s */
#if defined(LINUX) || defined(SOLARIS)
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
#if defined(LINUX) || defined(SOLARIS)
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		(void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(a);
		sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr *) &sockaddr,
			 sizeof(struct sockaddr_in)) < 0 ||
		    listen(sock, LISTEN_Q_LEN) < 0) {
#if defined(LINUX) || defined(SOLARIS)
			close(sock);
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			closesocket(sock);
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		/* record master socket in table */
		p2s[i].sock = sock;
		p2s[i].port = a;
	}
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
	ReleaseMutex(await_contact_mutex);
#endif

	csockaddrlen = sizeof(struct sockaddr_in);
	newsock = accept(sock, (struct sockaddr *) &csockaddr, &csockaddrlen);
	return newsock;
}
