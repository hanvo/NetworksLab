/* cnaiapi_init.c - cnaiapi_init */

#include <cnaiapi.h>
#include <stdio.h>

#if defined(LINUX) || defined(SOLARIS)
pthread_mutex_t		await_contact_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		cname_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		appname_mutex = PTHREAD_MUTEX_INITIALIZER;
#elif defined(WIN32)
HANDLE			await_contact_mutex;	
HANDLE			cname_mutex;
HANDLE			appname_mutex;
#endif

int init = 0;

/*-----------------------------------------------------------------------
 * cnaiapi_init - initialize the library. Initialize Winsock (if Win32)
 * and create mutexes.
 *
 * Note: All CNAIAPI functions call cnaiapi_init first to ensure that
 * the library is initialized. This is done for ease-of-use for
 * beginners. 
 *
 * **
 * ** Having the functions call cnaiapi_init first creates
 * ** a race condition in multi-threaded applications.
 * ** If your application is multithreaded, call cnaiapi_init explicitly
 * ** before you spawn any threads.
 * **
 *
 *-----------------------------------------------------------------------
 */
void
cnaiapi_init(void)
{
#if defined(WIN32)
	WORD    wsavers;
	WSADATA wsadata;
#endif

	if (init != 0)
		return;

#if defined(WIN32)
	wsavers = MAKEWORD(1, 1);
	if (WSAStartup(wsavers, &wsadata) != 0) {
		(void) fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}

	await_contact_mutex = CreateMutex(NULL, FALSE, NULL);
	cname_mutex = CreateMutex(NULL, FALSE, NULL);
	appname_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
	init = 1;
}
