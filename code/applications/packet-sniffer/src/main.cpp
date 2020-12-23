#include <vector>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/ip.h>
#include <pcap.h>
#include "tracing/Tracing.h"
#include "utility/GenericError.h"
#include "pcap-interface.h"
#include "pcap-interfaces.h"

bool TraceEnabled(tracing::TraceCategory /*category*/)
{
    return true;
}

void print_ethernet_header(const u_char *buffer, int /*size*/)
{
	const struct ethhdr *eth = reinterpret_cast<const struct ethhdr *>(buffer);
	
	std::cout <<  std::endl;
	std::cout <<  "Ethernet Header" << std::endl;
    network::MACAddress destAddress(eth->h_dest, 6);
    network::MACAddress srceAddress(eth->h_source, 6);
	std::cout <<  "   |-Destination Address : " << destAddress << std::endl;
	std::cout <<  "   |-Source Address      : " << srceAddress << std::endl;
	std::cout <<  "   |-Protocol            : " << static_cast<std::uint16_t>(eth->h_proto) << std::endl;
}

void print_ip_header(const u_char * Buffer, int Size)
{
	print_ethernet_header(Buffer , Size);
  
	// unsigned short iphdrlen;
		
	// struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
	// iphdrlen =iph->ihl*4;
	
	// memset(&source, 0, sizeof(source));
	// source.sin_addr.s_addr = iph->saddr;
	
	// memset(&dest, 0, sizeof(dest));
	// dest.sin_addr.s_addr = iph->daddr;
	
	// std::cout <<  " << std::endl;
	// std::cout <<  "IP Header << std::endl;
	// std::cout <<  "   |-IP Version        : %d\n",(unsigned int)iph->version);
	// std::cout <<  "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	// std::cout <<  "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
	// std::cout <<  "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
	// std::cout <<  "   |-Identification    : %d\n",ntohs(iph->id));
	// //std::cout <<  "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	// //std::cout <<  "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	// //std::cout <<  "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	// std::cout <<  "   |-TTL      : %d\n",(unsigned int)iph->ttl);
	// std::cout <<  "   |-Protocol : %d\n",(unsigned int)iph->protocol);
	// std::cout <<  "   |-Checksum : %d\n",ntohs(iph->check));
	// std::cout <<  "   |-Source IP        : %s\n" , inet_ntoa(source.sin_addr) );
	// std::cout <<  "   |-Destination IP   : %s\n" , inet_ntoa(dest.sin_addr) );
}

void print_tcp_packet(const u_char * Buffer, int Size)
{
	// unsigned short iphdrlen;
	
	// struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
	// iphdrlen = iph->ihl*4;
	
	// struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));
			
	// int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
	
	std::cout <<  "\n\n***********************TCP Packet*************************" << std::endl;
		
	print_ip_header(Buffer,Size);
		
	std::cout <<  std::endl;
	std::cout <<  "TCP Header" << std::endl;
	// std::cout <<  "   |-Source Port      : %u\n",ntohs(tcph->source));
	// std::cout <<  "   |-Destination Port : %u\n",ntohs(tcph->dest));
	// std::cout <<  "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
	// std::cout <<  "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
	// std::cout <<  "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
	// //std::cout <<  "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
	// //std::cout <<  "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
	// std::cout <<  "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
	// std::cout <<  "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
	// std::cout <<  "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
	// std::cout <<  "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
	// std::cout <<  "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
	// std::cout <<  "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
	// std::cout <<  "   |-Window         : %d\n",ntohs(tcph->window));
	// std::cout <<  "   |-Checksum       : %d\n",ntohs(tcph->check));
	// std::cout <<  "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
	// std::cout <<  " << std::endl;
	// std::cout <<  "                        DATA Dump                         ");
	// std::cout <<  " << std::endl;
		
	// std::cout <<  "IP Header << std::endl;
	// PrintData(Buffer,iphdrlen);
		
	// std::cout <<  "TCP Header << std::endl;
	// PrintData(Buffer+iphdrlen,tcph->doff*4);
		
	// std::cout <<  "Data Payload << std::endl;	
	// PrintData(Buffer + header_size , Size - header_size );
						
	std::cout <<  "\n###########################################################" << std::endl;
}

void print_udp_packet(const u_char *Buffer , int Size)
{
	
	// unsigned short iphdrlen;
	
	// struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
	// iphdrlen = iph->ihl*4;
	
	// struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));
	
	// int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
	
	std::cout <<  "\n\n***********************UDP Packet*************************" << std::endl;
	
	print_ip_header(Buffer,Size);			
	
	std::cout <<  "\nUDP Header" << std::endl;
	// std::cout <<  "   |-Source Port      : %d\n" , ntohs(udph->source));
	// std::cout <<  "   |-Destination Port : %d\n" , ntohs(udph->dest));
	// std::cout <<  "   |-UDP Length       : %d\n" , ntohs(udph->len));
	// std::cout <<  "   |-UDP Checksum     : %d\n" , ntohs(udph->check));
	
	// std::cout <<  " << std::endl;
	// std::cout <<  "IP Header << std::endl;
	// PrintData(Buffer , iphdrlen);
		
	// std::cout <<  "UDP Header << std::endl;
	// PrintData(Buffer+iphdrlen , sizeof udph);
		
	// std::cout <<  "Data Payload << std::endl;	
	
	// //Move the pointer ahead and reduce the size of string
	// PrintData(Buffer + header_size , Size - header_size);
	
	std::cout <<  "\n###########################################################" << std::endl;
}

void print_icmp_packet(const u_char * Buffer , int Size)
{
	// unsigned short iphdrlen;
	
	// struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
	// iphdrlen = iph->ihl * 4;
	
	// struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));
	
	// int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;
	
	std::cout <<  "\n\n***********************ICMP Packet*************************" << std::endl;	
	
	print_ip_header(Buffer , Size);
			
	std::cout <<  std::endl;
		
	std::cout <<  "ICMP Header" << std::endl;
	// std::cout <<  "   |-Type : %d",(unsigned int)(icmph->type));
			
	// if((unsigned int)(icmph->type) == 11)
	// {
	// 	std::cout <<  "  (TTL Expired) << std::endl;
	// }
	// else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
	// {
	// 	std::cout <<  "  (ICMP Echo Reply) << std::endl;
	// }
	
	// std::cout <<  "   |-Code : %d\n",(unsigned int)(icmph->code));
	// std::cout <<  "   |-Checksum : %d\n",ntohs(icmph->checksum));
	// //std::cout <<  "   |-ID       : %d\n",ntohs(icmph->id));
	// //std::cout <<  "   |-Sequence : %d\n",ntohs(icmph->sequence));
	// std::cout <<  " << std::endl;

	// std::cout <<  "IP Header << std::endl;
	// PrintData(Buffer,iphdrlen);
		
	// std::cout <<  "UDP Header << std::endl;
	// PrintData(Buffer + iphdrlen , sizeof icmph);
		
	// std::cout <<  "Data Payload << std::endl;	
	
	// //Move the pointer ahead and reduce the size of string
	// PrintData(Buffer + header_size , (Size - header_size) );
	
	std::cout <<  "\n###########################################################" << std::endl;
}

// void PrintData (const u_char * data , int Size)
// {
// 	int i , j;
// 	for(i=0 ; i < Size ; i++)
// 	{
// 		if( i!=0 && i%16==0)   //if one line of hex printing is complete...
// 		{
// 			std::cout <<  "         ");
// 			for(j=i-16 ; j<i ; j++)
// 			{
// 				if(data[j]>=32 && data[j]<=128)
// 					std::cout <<  "%c",(unsigned char)data[j]); //if its a number or alphabet
				
// 				else std::cout <<  "."); //otherwise print a dot
// 			}
// 			std::cout <<  " << std::endl;
// 		} 
		
// 		if(i%16==0) std::cout <<  "   ");
// 			std::cout <<  " %02X",(unsigned int)data[i]);
				
// 		if( i==Size-1)  //print the last spaces
// 		{
// 			for(j=0;j<15-i%16;j++) 
// 			{
// 			  std::cout <<  "   "); //extra spaces
// 			}
			
// 			std::cout <<  "         ");
			
// 			for(j=i-i%16 ; j<=i ; j++)
// 			{
// 				if(data[j]>=32 && data[j]<=128) 
// 				{
// 				  std::cout <<  "%c",(unsigned char)data[j]);
// 				}
// 				else 
// 				{
// 				  std::cout <<  ".");
// 				}
// 			}
			
// 			std::cout <<   "\n" );
// 		}
// 	}
// }

void DataHandler(std::uint8_t * user, const struct pcap_pkthdr * h, const std::uint8_t * data)
{
    TraceInfo(__FILE__, __LINE__, __func__, "Data callback for {}", reinterpret_cast<char *>(user));
    int size = h->len;
    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(data + sizeof(struct ethhdr));
    switch (ipHeader->protocol) //Check the Protocol and do accordingly...
	{
		case 1:  //ICMP Protocol
			// ++icmp;
			print_icmp_packet(data, size);
			break;
		
		case 2:  //IGMP Protocol
			// ++igmp;
			break;
		
		case 6:  //TCP Protocol
			// ++tcp;
			print_tcp_packet(data, size);
			break;
		
		case 17: //UDP Protocol
			// ++udp;
			print_udp_packet(data, size);
			break;
		
		default: //Some Other Protocol like ARP etc.
			// ++others;
			break;
	}
	// printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);

}

int main(int /*argc*/, char * /*argv*/[])
{
    tracing::Tracing::SetTracingFunctions(nullptr, TraceEnabled);
    TraceInfo(__FILE__, __LINE__, __func__, "packet-sniffer");

    packetcapture::Interfaces interfaces;
    for (auto const & item : interfaces.GetAllInterfaces())
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Device: {} - {} ({})", item.first, item.second.Description(), (item.second.IsLoopback() ? "loopback" : ""));
    }

    const char * nicName = "ens33";
    int length = 65536;
    int promiscuous = 1;
    int timeout = 5000;
    char error_buffer[PCAP_ERRBUF_SIZE]; // Size defined in pcap.h

    pcap_t * capture = pcap_open_live(nicName, length, promiscuous, timeout, error_buffer);
    if (!capture)
    {
        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Error opening capture: {}}", error_buffer));
    }
    int result = pcap_dispatch(capture, 1, DataHandler, const_cast<std::uint8_t *>(reinterpret_cast<const std::uint8_t *>("MyCapture")));
    if (result == -1)
    {
        tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Error capturing: {}}", pcap_geterr(capture)));
    }
    pcap_close(capture);

    return 0;
}