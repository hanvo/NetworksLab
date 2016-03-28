/* packetdump.c - packetdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * packetdump_udp  -  Dumps contents of the UDP data frame
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void packetdump_udp(struct netpacket *pkt) {

	//Source Port
	kprintf("%d, ", ntohs(pkt->net_udpsport));

	//Dest Port
	kprintf("%d, ", ntohs(pkt->net_udpdport));

	//CheckSum
	kprintf("0x%04x, ", ntohs(pkt->net_udpcksum) & 0xffff);

	//UDP Length
	kprintf("%d", ntohs(pkt->net_udplen));

}

/*------------------------------------------------------------------------
 * packetdump_arp  -  Dumps contents of the ARP data frame
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void packetdump_arp(struct netpacket *pkt) {

	struct arppacket *arp = (struct arppacket *)pkt;

	//Checking if its REQUEST OR REPLY 
	switch( ntohs(arp->arp_op) ) {
		case ARP_OP_REQ:
			kprintf("REQUEST, ");
			break;
		case ARP_OP_RPLY:
			kprintf("REPLY, ");
			break;
	}

	//Print out HLEN (ARP hardware address length)
	kprintf("%d, ", arp->arp_hlen);

	//Print Out PLEN (ARP protocol address length)
	kprintf("%d, ", arp->arp_plen);

	//Printing the Source ETH_SRC
	uint32 convertedSourceIP = htonl(arp->arp_sndpa);
	kprintf("%d.%d.%d.%d", (convertedSourceIP >> 24)&0xff, 
		(convertedSourceIP >> 16)&0xff, (convertedSourceIP >> 8)&0xff,
		(convertedSourceIP)&0xff );

	kprintf("[");

	//Printing the source Mac Address
	int counter;
	for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
		if( (counter + 1) == ETH_ADDR_LEN) {
			kprintf("%02x]", *(arp->arp_sndha + counter));
		} else {
			kprintf("%02x:", *(arp->arp_sndha + counter));
		}
	}

	kprintf(" -> ");

	//Printing the Target ETH_DEST
	uint32 convertedDestIP = htonl(arp->arp_tarpa);
	kprintf("%d.%d.%d.%d", (convertedDestIP >> 24)&0xff, 
		(convertedDestIP >> 16)&0xff, (convertedDestIP >> 8)&0xff,
		(convertedDestIP)&0xff );
	
	if( ntohs(arp->arp_op) == ARP_OP_RPLY ) {
		kprintf("[");
		for(counter = 0; counter < ETH_ADDR_LEN; counter++) {
			if( (counter + 1) == ETH_ADDR_LEN) {
				kprintf("%02x]", *(arp->arp_tarha + counter));
			} else {
				kprintf("%02x:", *(arp->arp_tarha + counter));
			}
		}
	} 
}

/*------------------------------------------------------------------------
 * icmpIdSeq  -  Helper function to print just ID and Seq of ICMP
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void icmpIdSeq(struct netpacket *pkt) {
	//Id
	kprintf("id = %d, ", ntohs(pkt-> net_icident));
	//Sequence
	kprintf("seq = %d", ntohs(pkt->net_icseq));
}

/*------------------------------------------------------------------------
 * packetdump_icmp  -  Dumps contents of the ICMP data frame
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void packetdump_icmp(struct netpacket *pkt) {

	if( pkt->net_ictype == ICMP_ECHOREPLY) {
		kprintf( "ECHO_REPLY, " );
		icmpIdSeq(pkt);
	} else if ( pkt->net_ictype == ICMP_ECHOREQST) {
		kprintf( "ECHO_REQUEST, " );
		icmpIdSeq(pkt);
	} else {
		//print type hex
		kprintf("0x%02x", pkt->net_ictype);
	}

}

/*------------------------------------------------------------------------
 * ipv4PayLoadPrint  -  Helper to print PayLoad of IPV4 that we don't cover
 * Arg - HeaderLength, Pointer to a packet	
 *------------------------------------------------------------------------
 */

void ipv4PayLoadPrint(uint32 headerLength, struct netpacket *pptr) {
	//Print 15 bits of Payload after the IHL
	int counter;
	int actualHeaderLength = headerLength * 4;
	int maxReadLength = 15 + actualHeaderLength;
	for( counter = actualHeaderLength ; counter < maxReadLength ; counter++ ) {
		if( (counter + 1 ) == maxReadLength ) {
			kprintf("%02x", *(&(pptr->net_ipvh) + counter) );
		} else {
			kprintf("%02x ", *(&(pptr->net_ipvh) + counter) );
		}
	} 
}

/*------------------------------------------------------------------------
 * ipv4Data  -  Dumps contents of the Ipv4 data frame
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
	int headerLength = (pptr->net_ipvh)&0x0F;
	kprintf("%d, ", headerLength);

	//Print the Total Packet Length
	uint32 convertedPacketLengthHost = htons(pptr->net_iplen);
	kprintf("%d, ", convertedPacketLengthHost);

	//Print Protocol_TYPE
	switch( pptr->net_ipproto ) {
		case IP_UDP:
			kprintf("UDP, ");
			packetdump_udp(pptr);
			break;
		case IP_TCP:
			kprintf("TCP, ");
			ipv4PayLoadPrint(headerLength, pptr);
			break;
		case IP_ICMP:
			kprintf("ICMP, ");
			packetdump_icmp(pptr);
			break;
		default:
			kprintf("0x%02x, ", pptr->net_ipproto);
			ipv4PayLoadPrint(headerLength, pptr);
			break;
	}
};

/*------------------------------------------------------------------------
 * printPayload  -  Helper Method to just dump out first 15 bytes
 * Arg - Pointer to a packet	
 *------------------------------------------------------------------------
 */

void printPayload( struct netpacket * pptr ) {
	//Print out the first 15 bits of the load
	int counter;
	for( counter = 0 ; counter < 15 ; counter++) {
		if( (counter + 1 ) == 15 ) {
			kprintf("%02x", *(&(pptr->net_ipvh) + counter) );
		} else {
			kprintf("%02x ", *(&(pptr->net_ipvh) + counter) );
		}
	} 
}

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
			packetdump_arp(pptr);
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
