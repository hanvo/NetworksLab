/* cname_to_comp.c */

#include <cnaiapi.h>

/*-----------------------------------------------------------------------
 * cname_to_comp - look up a host by name and return its IP address
 *-----------------------------------------------------------------------
 */
computer
cname_to_comp(char *cname)
{
	computer		c;
	struct hostent		*hp;

	cnaiapi_init();

#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&cname_mutex);
#elif defined(WIN32)
	WaitForSingleObject(cname_mutex, INFINITE);
#endif

	hp = gethostbyname(cname);
	if (hp == NULL) {
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
		ReleaseMutex(cname_mutex);
#endif
		return -1;
	}
	
	if (hp->h_addrtype != AF_INET ||
	    hp->h_length != sizeof(computer)) {
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
		ReleaseMutex(cname_mutex);
#endif
		return -1;
	}
	  
	c = *((computer *) hp->h_addr);
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
	ReleaseMutex(cname_mutex);
#endif
	return c;
}
