/* packetdump.c - packetdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * packetdump  -  Dumps contents of an Ethernet packet
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void	packetdump ( struct	netpacket *pptr )
{
	/* Put your code here */	
	int counter;
	for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
		if( (counter + 1) == ETH_ADDR_LEN) {
			kprintf("%02x", *(pptr->net_ethsrc + counter));
		} else {
			kprintf("%02x:", *(pptr->net_ethsrc + counter));
		}
	}

	kprintf(" -> ");

	for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
		if( (counter + 1) == ETH_ADDR_LEN) {
			kprintf("%02x, ", *(pptr->net_ethdst + counter));
		} else {
			kprintf("%02x:", *(pptr->net_ethdst + counter));
		}
	}

	switch(ntohs(pptr->net_ethtype)) {
		case ETH_ARP:
			kprintf("ARP, ");
			break;
		case ETH_IP:
			kprintf("IPv4, ");
			break;
		case ETH_IPv6:
			kprintf("IPv6, ");
			break;
		default:
			kprintf("0x%04x,  ",ntohs(pptr->net_ethtype));
			break;
	}

	for(counter = 0; counter < 15; counter++) {
		kprintf("%02x ", *(pptr->net_udpdata + counter));
	}

	kprintf("\n");

	return;
}
