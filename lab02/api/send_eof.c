/* send_eof.c - send_eof */

#include <cnaiapi.h>

/*------------------------------------------------------------------------
 * send_eof - signal EOF by shutting down send side of connection
 *------------------------------------------------------------------------
 */
int
send_eof(connection conn)
{
	return shutdown(conn, 1);
}
