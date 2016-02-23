/* appname_to_appnum.c */

#include <cnaiapi.h>

/*-----------------------------------------------------------------------
 * appname_to_appnum - look up a port by service name
 *-----------------------------------------------------------------------
 */
appnum
appname_to_appnum(char *appname)
{

	struct servent		*sp;
	appnum			port;

	cnaiapi_init();

#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&appname_mutex);
#elif defined(WIN32)
	WaitForSingleObject(appname_mutex, INFINITE);
#endif

	sp = getservbyname(appname, "tcp");
	if (sp == NULL) {
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&appname_mutex);
#elif defined(WIN32)
		ReleaseMutex(appname_mutex);
#endif
		return -1;
	}
	
	port = ntohs(sp->s_port);
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&appname_mutex);
#elif defined(WIN32)
		ReleaseMutex(appname_mutex);
#endif
	return port;
}
