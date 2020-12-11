#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcap.h>
#include <sys/types.h>
 #include <pcap-bpf.h>

#include<sys/socket.h>
#include<arpa/inet.h> // for inet_ntoa()
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>	//Provides declarations for icmp header
#include<netinet/udp.h>	//Provides declarations for udp header
#include<netinet/tcp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip
#include <netinet/in.h>
#include <netinet/if_ether.h>



void capture_live(char *dev_name);
void handle_error(char *err);
pcap_if_t *lookup_dev(char *dev);
void print_packet_info(const u_char *packet, struct pcap_pkthdr header);
void packet_handler(u_char *, const struct pcap_pkthdr *, const u_char *);




void usage(void)
{
	printf(
	       "\n"
	       "usage:\n"
	       "\t./monitor \n"
		   "Options:\n"
		   "-i <interface>, Capture from specified network interface\n"
		   "-r <filename>, Analyze from provided file\n"
		   "-h, Help message\n\n"
		   );

	exit(1);
}

int  main(int argc, char *argv[])
{

	int ch;
	

	if (argc < 2)
		usage();

	while ((ch = getopt(argc, argv, "i:r:h")) != -1) {
		switch (ch) {		
		case 'i':
			capture_live(optarg);

			break;
		case 'r':
			fprintf(stdout, "Live capture\n");
			break;
		
		default:
			usage();
		}

	}




	argc -= optind;
	argv += optind;	
	
	return 0;
}

/*
	
	Handles an execution error by prompting user and then terminating the program.
	Args:

			-arg1: A message to print before terminating.
	Returns:

			-None
	Notes: 
			-Terminates execution if called.

*/

void handle_error(char *err)
{
	fprintf(stderr, "Error occurred..! [Msg]: %s\n", err);
	exit(-1);
}

/*

	Handles packets sniffed

*/

void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{


	//determine the packet type

	/*
		
		Packet is actually larger but we care about the first bytes.

	*/
	struct iphdr *iph = (struct iphdr *)(packet + header->len);
	struct ether_header *eth_header;
	switch (iph->protocol) //Check the Protocol and do accordingly...
	{
		case 1:  //ICMP Protocol
			fprintf(stderr, "ICMP\n");
			break;
		
		case 2:  //IGMP Protocol
			fprintf(stderr, "IGMP\n");
			break;
		
		case 6:  //TCP Protocol
			fprintf(stderr, "TCP\n");
			break;
		
		case 17: //UDP Protocol
			fprintf(stderr, "UDP\n");
			break;
		
		default: 
			
			eth_header = (struct ether_header *) packet;
			fprintf(stderr, "[Others]: %d\n", ntohs(eth_header->ether_type));

			break;
	}

	print_packet_info(packet, *header);
	
}

/*

	Given a valid header and packet, prints info concerning a captured packet.

*/

void print_packet_info(const u_char *packet, struct pcap_pkthdr header)
{
	printf("Packet capture length: %d\n",header.caplen);
    printf("Packet total length %d\n", header.len);	
}

/*
	
	Acts as a handler for live capture..

*/
void capture_live(char *dev_name)
{

	pcap_if_t *dev;
	char error_buf[PCAP_ERRBUF_SIZE];
	struct pcap_pkthdr packet_header;
	int timeout = 0; //milliseconds
	int threshold = 1;  //packet limit


	//check the existance..
	if(!(dev = lookup_dev(dev_name)))
	{
		handle_error("Device not found");

	}
	
	//if present, open for live capture..
	
	pcap_t *handler = pcap_open_live(dev_name, BUFSIZ, threshold, timeout, error_buf);

	
	if(handler == NULL)
	{
		fprintf(stderr, "Problems in heaven..\n");
		exit(-1);
	}

	//loop on capturing
	pcap_loop(handler, 0, packet_handler, NULL);

	


	return;

}


/*

	Looks up a specified device, if present returns the device as <pcap_if_t*>, else NULL.
	Args:
			-arg1: The device name  as <char *>
	Returns:
			The device as <pcap_if_t *> or NULL if no such device exists.

*/

pcap_if_t *lookup_dev(char *dev)
{
	char error_buf[PCAP_ERRBUF_SIZE];
	pcap_if_t *devices;

	//get devices, handle error
	if(pcap_findalldevs(&devices, error_buf) == -1)
		handle_error(error_buf);

	//parse available devices..
	while(devices->next != NULL)
	{
		//check the name
		if(strcmp(devices->name, dev) == 0)
			return devices;
		//move on
		devices = devices->next;
	}

	//shouldn't reach here if user typed a valid dev.
	return NULL;
	

}