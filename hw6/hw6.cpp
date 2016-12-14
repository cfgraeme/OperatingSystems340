#include <stdio.h>
#include <stdlib.h>
#include <pcap.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "sniffex.h"

int main(int argc, char **argv) {
  
  char* inet;
  char* host;
  char* netmask;
  int error_code;
  
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *alldevs, *d;
  
  bpf_u_int32 netp; /* ip */
  bpf_u_int32 maskp;/* subnet mask */
  
  struct in_addr addr;
  std::vector<std::string> ndevs;
  std::vector<char*> inets;
  
  /**
   * 
   * Ask the user to select a network device on the
   * host machine.
   * 
   */
  
  if ( pcap_findalldevs( &alldevs, errbuf ) == -1 ) {
    
    std::cerr<<"Error in pcap_findalldevs: "<< errbuf<< std::endl;
    exit(1);
    
  }
  
  int i = 0;
  int selected_dev = 0;
  
  std::cout<< "------------------------------------------"<< std::endl;
  std::cout<<"List of available network devices:"<<std::endl;
  std::cout<< "------------------------------------------"<< std::endl;
  
  for ( d=alldevs; d; d=d->next ) {
    
    ndevs.push_back( d->name );
    
    
    inet = (char*)malloc(16);
    inet[0] = '\0';
   
    for ( pcap_addr_t *a=d->addresses; a!=NULL; a=a->next ) {
      if (a->addr->sa_family == AF_INET )   
	strncat(inet, inet_ntoa( ( ( struct sockaddr_in* )a->addr )->sin_addr ), 16 );
    }
    
    inets.push_back( inet );
    
    std::cout<<"("<<(i)<<") "<<ndevs[i++]<<" ";
    
    if ( d->description )
      std::cout<<"\t\""<<d->description<<"\""<<std::endl;
    else
      std::cout<<""<< std::endl;
      
  }
  
  std::cout<< "------------------------------------------"<< std::endl;
  std::cout<<"Please select a device [1 through "<<i<<"]: ";
  std::cin>>selected_dev;
  std::cout<<"Selected device: "<<ndevs[selected_dev-1]<<std::endl;

  /*
	TODO: 1

	Use pcap_lookupnet function to find network number
	and mask for the selected device.

	http://linux.die.net/man/3/pcap_lookupnet
  */
  const char* dev_name = ndevs[selected_dev-1].c_str();
  pcap_lookupnet(dev_name, &netp, &maskp, (char*) errbuf);

  /*
	TODO: 2

	Display host, inet, and mask information
	for the selected device.

	Hints:
	 1. use inet_ntoa to find host info.
	 2. use netp and maskp (found above) to display inet
	    and netmask info.

	http://linux.die.net/man/3/inet_ntoa
  */

  struct in_addr netp_addr;
  netp_addr.s_addr = netp;
  host = inet_ntoa(netp_addr);
  std::cout<<"host "<<host<<std::endl;

  inet = inets[selected_dev-1];
  std::cout<<"inet "<<inet<<std::endl;

  struct in_addr maskp_addr;
  maskp_addr.s_addr = maskp;
  netmask = inet_ntoa(maskp_addr);
  std::cout<<"netmask "<<netmask<<std::endl;
  
  for ( int i=0; i<inets.size(); i++ ) free( inets[i] ); 
  std::cout<< "------------------------------------------"<< std::endl;

  /**
   * 
   * Live packet capture
   *  
   */
   
  std::cout<< "Starting live packet capture!"<<std::endl;
 
  const u_char* packet;
  struct pcap_pkthdr hdr;
  struct sniff_ethernet* eth_hdr;
  struct sniff_ip* ip_hdr;
  struct sniff_tcp* tcp_hdr;
  int sport;
  int dport;

  pcap_t* cap_desc;

  struct timespec nano_time_spec;
  nano_time_spec.tv_nsec=100000;
  nano_time_spec.tv_sec=0;

  /*
	TODO: 3
	Start live packet capture using pcap_open_live function.
  */
  
  cap_desc = pcap_open_live(dev_name, BUFSIZ, 0, 1000, errbuf);  
  while ( true ) {
    packet = pcap_next( cap_desc, &hdr );  
    if ( packet != NULL ) {
      /*
	     TODO: 4
	     For each packet inspect the headers as follows:
		   1. Get the ETHERNET frame
                2. Get the IP datagram
		   3. Get the TCP segment
	        2. If the ether_type == ETHERTYPE_IP AND the ip_p == IPPROTO_TCP then
		   display the sender and receiver info if either has port = 80.

      Ethernet header

      struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];    destination host address 
        u_char  ether_shost[ETHER_ADDR_LEN];     source host address 
        u_short ether_type;                      IP? ARP? RARP? etc 
      };

      struct sniff_ip {
        ...
        u_char  ip_p;                    protocol 
        u_short ip_sum;                  checksum 
        struct  in_addr ip_src,ip_dst;   source and dest address 
      };
      struct sniff_tcp {
        u_short th_sport;               source port
        u_short th_dport;               destination port
        ....
      }
      */
      
      eth_hdr = (struct sniff_ethernet *) packet; 
      ip_hdr = (struct sniff_ip*)(packet + SIZE_ETHERNET);
      u_int ip_size = (IP_HL(ip_hdr)*4);
      tcp_hdr = (struct sniff_tcp *)(packet + SIZE_ETHERNET + ip_size);
      sport = ntohs(tcp_hdr->th_sport);
      dport = ntohs(tcp_hdr->th_dport);
      u_short eh = eth_hdr->ether_type;
      u_short ip_pr = ip_hdr->ip_p;

      if ((eh == ntohs(ETHERTYPE_IP)) && (ip_pr == IPPROTO_TCP)){
        if ((sport == 80) || (dport == 80)){
          char* source_a = inet_ntoa(ip_hdr->ip_src);
          char* dest_a = inet_ntoa(ip_hdr->ip_dst);
          printf("Source [%s : %d] -> Destination [%s : %d]\n", source_a, sport, dest_a, dport);
        }
      }
     
    }
  }   
  return 0;
} 
