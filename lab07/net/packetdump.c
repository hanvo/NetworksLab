/* packetdump.c - packetdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * packetdump  -  Dumps contents of an Ethernet packet
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

 void ipv4Data(struct netpacket *pptr) {

	//Printing the ETH_SRC
	uint32 convertedSourceIP = htonl(pptr->net_ipsrc);
	kprintf("%d.%d.%d.%d", (convertedSourceIP >> 24)&0xff, 
		(convertedSourceIP >> 16)&0xff, (convertedSourceIP >> 8)&0xff,
		(convertedSourceIP)&0xff );

	kprintf(" -> ");

	//Printing the ETH_DEST
	uint32 convertedDestIP = htonl(pptr->net_ipdst);
	kprintf("%d.%d.%d.%d, ", (convertedDestIP >> 24)&0xff, 
		(convertedDestIP >> 16)&0xff, (convertedDestIP >> 8)&0xff,
		(convertedDestIP)&0xff );

	//printing the Protocol Version
	kprintf("%d, ", (pptr->net_ipvh >> 4)&0x0F);

	//Printing the Internet Header Length
	kprintf("%d, ", (pptr->net_ipvh)&0x0F);



	//Print the Total Packet Length
	uint32 convertedPacketLengthHost = htons(pptr->net_iplen);
	kprintf("%d, ", convertedPacketLengthHost);

	//Print Protocol_TYPE
	switch( pptr->net_ipproto ) {
		case IP_UDP:
			kprintf("UDP, ");
			break;
		case IP_TCP:
			kprintf("TCP, ");
			break;
		case IP_ICMP:
			kprintf("ICMP, ");
			break;
		default:
			kprintf("0x%02x, ", pptr->net_ipproto);
			break;
	}

	//Print Payload in order of IHL
	

};

void printPayload( struct netpacket * pptr ) {
	//Print out the first 15 bits of the load
	int counter;
	for(counter = 0; counter < 15; counter++) {
		if( (counter + 1 ) == 15 ) {
			kprintf("%02x", *(&(pptr->net_ipvh) + counter) );
		} else {
			kprintf("%02x ", *(&(pptr->net_ipvh) + counter) );
		}
	} 
}

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
			printPayload(pptr);
			break;
		case ETH_IP:
			kprintf("IPv4, ");
			ipv4Data(pptr);
			break;
		case ETH_IPv6:
			kprintf("IPv6, ");
			printPayload(pptr);
			break;
		default:
			kprintf("0x%04x,  ",ntohs(pptr->net_ethtype));
			printPayload(pptr);
			break;
	}

	kprintf("\n");
	return;
}

