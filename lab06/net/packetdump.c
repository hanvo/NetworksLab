/* packetdump.c - packetdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * packetdump  -  Dumps contents of an Ethernet packet
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void	packetdump ( struct	netpacket *pptr )
{
	//Printing the Source Mac Address	
	int counter;
	for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
		if( (counter + 1) == ETH_ADDR_LEN) {
			kprintf("%02x", *(pptr->net_ethsrc + counter));
		} else {
			kprintf("%02x:", *(pptr->net_ethsrc + counter));
		}
	}

	kprintf(" -> ");

	//Printing the Dest Mac Address
	for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
		if( (counter + 1) == ETH_ADDR_LEN) {
			kprintf("%02x, ", *(pptr->net_ethdst + counter));
		} else {
			kprintf("%02x:", *(pptr->net_ethdst + counter));
		}
	}

	//Figuring out what ETHType it is
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

	//Print out the first 15 bits of the load
	for(counter = 0; counter < 15; counter++) {
		if( counter != 14 ) {
			kprintf("%02x ", *(pptr->net_udpdata + counter));
		} else {
			kprintf("%02x", *(pptr->net_udpdata + counter));
		}
	}

	kprintf("\n");
	return;
}
