#include <fstream>
#include <vector>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <pcap.h>
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "osal/Console.h"
#include "utility/Assert.h"
#include "utility/Endian.h"
#include "utility/GenericError.h"
#include "network-osal/Ethernet.h"
#include "network-osal/IPV4Address.h"
#include "pcap-interface.h"
#include "pcap-interfaces.h"

osal::Console console;

void PrintData(const std::uint8_t * data , std::size_t size);

void PrintEthernetHeader(const std::uint8_t * buffer, std::size_t size)
{
    auto ethernetHeaderLength = sizeof(ethhdr);
    ASSERT(size >= ethernetHeaderLength);
    const struct ethhdr * ethHeader = reinterpret_cast<const struct ethhdr *>(buffer);

    console << fgcolor(osal::ConsoleColor::Yellow);
    console << "------------------------------------ETHERNET------------------------------------" << std::endl;
    console << std::endl;
    console << "Ethernet Header (Length: " << ethernetHeaderLength << ")" << std::endl;
    network::MACAddress destAddress(ethHeader->h_dest, 6);
    network::MACAddress sourceAddress(ethHeader->h_source, 6);
    auto protocol = utility::FromNetworkByteOrder(ethHeader->h_proto);
    auto ethernetType = static_cast<network::EthernetType>(protocol);
    console << "0000 | Destination Address : " << destAddress << std::endl;
    console << "0006 | Source Address      : " << sourceAddress << std::endl;
    console << "000C | Protocol            : " << serialization::Serialize(protocol, 4, 16) << " (" << serialization::Serialize(ethernetType) << ")" << std::endl;
    console << std::endl;
    PrintData(buffer, ethernetHeaderLength);
    console << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);
}

void PrintIPHeader(const std::uint8_t * buffer, std::size_t size)
{
    console << fgcolor(osal::ConsoleColor::Default);

    PrintEthernetHeader(buffer, size);
  
    console << fgcolor(osal::ConsoleColor::Green);
    console << "---------------------------------------IP---------------------------------------" << std::endl;
        
    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(buffer + sizeof(struct ethhdr));
    std::uint8_t version = ipHeader->version;
    std::uint16_t ipHeaderLength = ipHeader->ihl * 4;
    std::uint8_t tos = ipHeader->tos;
    std::uint16_t totalLength = utility::FromNetworkByteOrder(ipHeader->tot_len);
    std::uint16_t id = utility::FromNetworkByteOrder(ipHeader->id);
    std::uint16_t fragmentOffset = utility::FromNetworkByteOrder(ipHeader->frag_off);
    std::uint8_t ttl = ipHeader->ttl;
    std::uint8_t protocol = ipHeader->protocol;
    std::uint16_t headerChecksum = utility::FromNetworkByteOrder(ipHeader->check);

    console << std::endl;
    console << "IP Header (Length: " << ipHeaderLength << ")" << std::endl;
    console << "0000 | Version             : " << serialization::Serialize(version, 0) << std::endl;
    console << "0000 | Header length       : " << ipHeaderLength << std::endl;
    console << "0001 | Type Of Service     : " << serialization::Serialize(tos, 0) << " (0x" << serialization::Serialize(tos, 2, 16) << ")" << std::endl;
    console << "0002 | Total length        : " << totalLength << std::endl;
    console << "0004 | ID                  : " << id << " (0x" << serialization::Serialize(id, 4, 16) << ")" << std::endl;
    console << "0006 | Fragment offset     : " << fragmentOffset << " (0x" << serialization::Serialize(fragmentOffset, 4, 16) << ")" << std::endl;
    console << "0008 | TTL                 : " << serialization::Serialize(ttl, 0) << " (0x" << serialization::Serialize(ttl, 2, 16) << ")" << std::endl;
    console << "0009 | Protocol            : " << serialization::Serialize(protocol, 0) << " (0x" << serialization::Serialize(protocol, 2, 16) << ")" << std::endl;
    console << "000A | Header Checksum     : " << headerChecksum << " (0x" << serialization::Serialize(headerChecksum, 4, 16) << ")" << std::endl;
    if (version == 4)
    {
        network::IPV4Address sourceAddress(in_addr { utility::FromNetworkByteOrder(ipHeader->saddr) });
        network::IPV4Address destAddress(in_addr { utility::FromNetworkByteOrder(ipHeader->daddr) });
        console << "000C | Source address      : " << sourceAddress << std::endl;
        console << "0010 | Destination address : " << destAddress << std::endl;
    }
    else
    {
        console << "Cannot handle IPV6 packet yet" << std::endl;
    }
    console << std::endl;
    PrintData(buffer + sizeof(struct ethhdr), ipHeaderLength);
    console << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);
}

void PrintTCPPacket(const std::uint8_t * buffer, std::size_t size)
{
    console << fgcolor(osal::ConsoleColor::White);
    console << "\n***********************************TCP Packet***********************************\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);

    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(buffer  + sizeof(struct ethhdr));
    std::uint16_t ipHeaderLength = ipHeader->ihl * 4;
    const struct tcphdr * tcpHeader = reinterpret_cast<const struct tcphdr *>(buffer + ipHeaderLength + sizeof(struct ethhdr));
    std::uint16_t tcpHeaderLength = tcpHeader->doff * 4;
    std::uint16_t sourcePort = utility::FromNetworkByteOrder(tcpHeader->th_sport);
    std::uint16_t destPort = utility::FromNetworkByteOrder(tcpHeader->th_dport);
    std::uint32_t sequenceNumber = utility::FromNetworkByteOrder(tcpHeader->th_seq);
    std::uint32_t acknowledgeNumber = utility::FromNetworkByteOrder(tcpHeader->th_ack);
            
    PrintIPHeader(buffer, size);

    console << fgcolor(osal::ConsoleColor::Cyan);
    console << "---------------------------------------TCP--------------------------------------" << std::endl;

    std::cout <<  std::endl;
    std::cout <<  "TCP Header (Length: " << tcpHeaderLength << ")" << std::endl;
    console << "0000 | Source port         : " << sourcePort << " (0x" << serialization::Serialize(sourcePort, 4, 16) << ")" << std::endl;
    console << "0002 | Destination port    : " << destPort << " (0x" << serialization::Serialize(destPort, 4, 16) << ")" << std::endl;
    console << "0004 | Sequence number     : " << sequenceNumber << " (0x" << serialization::Serialize(sequenceNumber, 4, 16) << ")" << std::endl;
    console << "0008 | Acknowledge number  : " << acknowledgeNumber << " (0x" << serialization::Serialize(acknowledgeNumber, 4, 16) << ")" << std::endl;
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
        
    console << std::endl;
    PrintData(buffer + sizeof(struct ethhdr) + ipHeaderLength, tcpHeaderLength);
    console << std::endl;

    console << "----------------------------------Data payload----------------------------------" << std::endl;
    console <<  "Data Payload" << std::endl;	
    PrintData(buffer  + sizeof(struct ethhdr) + ipHeaderLength + tcpHeaderLength, size - (sizeof(struct ethhdr) + ipHeaderLength + tcpHeaderLength));
                        
    console << "\n###################################TCP Packet###################################\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);
}

void PrintUDPPacket(const std::uint8_t * buffer, std::size_t size)
{
    console << fgcolor(osal::ConsoleColor::White);
    console <<  "\n***********************************UDP Packet*********************************\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);

    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(buffer  + sizeof(struct ethhdr));
    std::uint16_t ipHeaderLength = ipHeader->ihl * 4;
    const struct udphdr * udpHeader = reinterpret_cast<const struct udphdr *>(buffer + ipHeaderLength + sizeof(struct ethhdr));
    std::uint16_t udpHeaderLength = sizeof(udphdr);
    std::uint16_t sourcePort = utility::FromNetworkByteOrder(udpHeader->uh_sport);
    std::uint16_t destPort = utility::FromNetworkByteOrder(udpHeader->uh_dport);
    std::uint16_t udpDataLength = utility::FromNetworkByteOrder(udpHeader->uh_ulen);
    std::uint16_t checksum = utility::FromNetworkByteOrder(udpHeader->uh_sum);
            
    PrintIPHeader(buffer, size);

    console << fgcolor(osal::ConsoleColor::Cyan);
    console << "---------------------------------------UDP--------------------------------------" << std::endl;

    std::cout <<  std::endl;
    std::cout <<  "UDP Header (Length: " << udpHeaderLength << ")" << std::endl;
    console << "0000 | Source port         : " << sourcePort << " (0x" << serialization::Serialize(sourcePort, 4, 16) << ")" << std::endl;
    console << "0002 | Destination port    : " << destPort << " (0x" << serialization::Serialize(destPort, 4, 16) << ")" << std::endl;
    console << "0004 | UDP Data Length     : " << udpDataLength << " (0x" << serialization::Serialize(udpDataLength, 4, 16) << ")" << std::endl;
    console << "0006 | UDP Checksum        : " << checksum << " (0x" << serialization::Serialize(checksum, 4, 16) << ")" << std::endl;
    
    console << std::endl;
    PrintData(buffer + sizeof(struct ethhdr) + ipHeaderLength, udpHeaderLength);
    console << std::endl;

    console << "----------------------------------Data payload----------------------------------" << std::endl;
    console <<  "Data Payload" << std::endl;	
    PrintData(buffer  + sizeof(struct ethhdr) + ipHeaderLength + udpHeaderLength, size - (sizeof(struct ethhdr) + ipHeaderLength + udpHeaderLength));
                        
    console << "\n###################################UDP Packet###################################\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);
}

void PrintICMPPacket(const std::uint8_t * buffer, std::size_t size)
{
    console << fgcolor(osal::ConsoleColor::White);
    console <<  "\n***********************************ICMP Packet********************************\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);

    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(buffer  + sizeof(struct ethhdr));
    std::uint16_t ipHeaderLength = ipHeader->ihl * 4;
    // const struct icmphdr * icmpHeader = reinterpret_cast<const struct icmphdr *>(buffer + ipHeaderLength + sizeof(struct ethhdr));
    std::uint16_t icmpHeaderLength = sizeof(icmphdr);
    
    PrintIPHeader(buffer, size);

    console << fgcolor(osal::ConsoleColor::Magenta);
    console << "---------------------------------------ICMP-------------------------------------" << std::endl;

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

    console << std::endl;
    PrintData(buffer + sizeof(struct ethhdr) + ipHeaderLength, icmpHeaderLength);
    console << std::endl;

    console << "----------------------------------Data payload----------------------------------" << std::endl;
    console <<  "Data Payload" << std::endl;	
    PrintData(buffer  + sizeof(struct ethhdr) + ipHeaderLength + icmpHeaderLength, size - (sizeof(struct ethhdr) + ipHeaderLength + icmpHeaderLength));
                        
    console << "\n###################################ICMP Packet##################################\n" << std::endl;
    console << fgcolor(osal::ConsoleColor::Default);
}

void PrintData(const std::uint8_t * data , std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i)
    {
        if (i != 0 && i % 16 == 0)
        {
            console <<  "         ";
            for (std::size_t j = i-16; j < i; ++j)
            {
                if (std::isprint(data[j]))
                    console <<  static_cast<char>(data[j]);
                else
                    console <<  ".";
            }
            console << std::endl;
        } 
        
        if (i % 16 == 0)
            console <<  "   ";
        console <<  serialization::Serialize(data[i], 3, 16);
                
        if (i == size - 1)
        {
            for (std::size_t j = 0; j < 15 - i % 16; ++j) 
            {
                console <<  "   ";
            }
            
            console <<  "         ";
            
            for (std::size_t j = i - i % 16; j <= i; ++j)
            {
                if (std::isprint(data[j]))
                    console <<  static_cast<char>(data[j]);
                else
                    console <<  ".";
            }
            
            console << std::endl;
        }
    }
}

void DataHandler(std::uint8_t * user, const struct pcap_pkthdr * h, const std::uint8_t * data)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Data callback for {}", reinterpret_cast<char *>(user));
    const char * fileName = "packets.bin";
    TraceMessage(__FILE__, __LINE__, __func__, "Open dump file {}", fileName);
    std::ofstream dumpFile(fileName, std::ios_base::binary | std::ios_base::app);
    if (!dumpFile)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Create dump file {}", fileName);
        dumpFile.open(fileName, std::ios_base::binary | std::ios_base::out);

    }
    std::size_t size = h->caplen;
    dumpFile.write(reinterpret_cast<const char *>(data), size);
    dumpFile.close();
    const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(data + sizeof(struct ethhdr));
    console << "Package protocol=" << serialization::Serialize(ipHeader->protocol, 2, 16) << " size=" << size << std::endl;
    switch (ipHeader->protocol) //Check the Protocol and do accordingly...
    {
        case 1:  //ICMP Protocol
            console << "Received ICMP" << std::endl;
            // ++icmp;
            PrintICMPPacket(data, size);
            break;
        
        case 2:  //IGMP Protocol
            console << "Received IGMP" << std::endl;
            // ++igmp;
            break;
        
        case 6:  //TCP Protocol
            console << "Received TCP" << std::endl;
            // ++tcp;
            PrintTCPPacket(data, size);
            break;
        
        case 17: //UDP Protocol
            console << "Received UDP" << std::endl;
            // ++udp;
            PrintUDPPacket(data, size);
            break;
        
        default: //Some Other Protocol like ARP etc.
            console << "Received other" << std::endl;
            // ++others;
            break;
    }
    // printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);

}

int main(int /*argc*/, char * /*argv*/[])
{
    tracing::Tracing::SetTracingFunctions(nullptr, TraceEnabled);
    TraceMessage(__FILE__, __LINE__, __func__, "packet-sniffer");

    packetcapture::Interfaces interfaces;
    for (auto const & item : interfaces.GetAllInterfaces())
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Device: {} - {} ({})", item.first, item.second.Description(), (item.second.IsLoopback() ? "loopback" : ""));
    }

    const char * nicName = "ens33";
    int length = 65536;
    int promiscuous = 1;
    int timeout = 5000;
    char error_buffer[PCAP_ERRBUF_SIZE]; // Size defined in pcap.h

    pcap_t * capture = pcap_open_live(nicName, length, promiscuous, timeout, error_buffer);
    if (!capture)
    {
        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Error opening capture: {}}", error_buffer));
    }
    int result = pcap_dispatch(capture, 3, DataHandler, const_cast<std::uint8_t *>(reinterpret_cast<const std::uint8_t *>("MyCapture")));
    if (result == -1)
    {
        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Error capturing: {}}", pcap_geterr(capture)));
    }
    pcap_close(capture);

    // const char * fileName = "/home/rene/repos/MyProject/output/Linux/Debug/bin/packets.bin";
    // TraceMessage(__FILE__, __LINE__, __func__, "Open dump file {}", fileName);
    // std::ifstream dumpFile(fileName, std::ios_base::binary | std::ios_base::in);
    // if (!dumpFile.good())
    // {
    //     tracing::Tracer::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Error opening file: {}", fileName));
    // }
    // const std::size_t BufferSize = 4096;
    // char buffer[BufferSize];
    // dumpFile.read(buffer, BufferSize);
    // std::size_t bytesRead = dumpFile.gcount();
    // const std::size_t MinEthernetPacketSize = 46;

    // while (bytesRead != 0)
    // {
    //     std::size_t offset = 0;
    //     while (true)
    //     {
    //         const std::uint8_t * data = reinterpret_cast<const uint8_t *>(buffer) + offset;
    //         std::size_t totalPacketLength {};
    //         network::EthernetType ethernetType {};
    //         if (bytesRead - offset >= MinEthernetPacketSize)
    //         {
    //             const struct ethhdr * ethHeader = reinterpret_cast<const struct ethhdr *>(data);

    //             auto protocol = utility::FromNetworkByteOrder(ethHeader->h_proto);
    //             ethernetType = static_cast<network::EthernetType>(protocol);
    //             switch (ethernetType)
    //             {
    //                 case network::EthernetType::IP:
    //                     {
    //                         const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(data + sizeof(struct ethhdr));
    //                         std::uint16_t ipPacketLength = utility::FromNetworkByteOrder(ipHeader->tot_len);
    //                         totalPacketLength = sizeof(struct ethhdr) + ipPacketLength;// + sizeof(uint32_t);
    //                     }
    //                     break;

    //                 case network::EthernetType::IPV6:
    //                     {
    //                         const struct ip6_hdr * ipHeader = reinterpret_cast<const struct ip6_hdr *>(data + sizeof(struct ethhdr));
    //                         std::uint16_t ipPacketLength = sizeof(ip6_hdr) + utility::FromNetworkByteOrder(ipHeader->ip6_ctlun.ip6_un1.ip6_un1_plen);
    //                         totalPacketLength = sizeof(struct ethhdr) + ipPacketLength;// + sizeof(uint32_t);
    //                     }
    //                     break;

    //                 case network::EthernetType::ARP:
    //                     totalPacketLength = sizeof(struct ethhdr) + 28;//MinEthernetPacketSize;
    //                     break;

    //                 case network::EthernetType::RARP:
    //                     totalPacketLength = sizeof(struct ethhdr) + 28;//MinEthernetPacketSize;
    //                     break;
    //             }

    //         }
    //         else
    //         {
    //             break;
    //         }
            
    //         if (bytesRead - offset >= totalPacketLength)
    //         {
    //             switch (ethernetType)
    //             {
    //                 case network::EthernetType::IP:
    //                     {
    //                         const struct iphdr * ipHeader = reinterpret_cast<const struct iphdr *>(data + sizeof(struct ethhdr));

    //                         switch (ipHeader->protocol)
    //                         {
    //                             case 1:  //ICMP Protocol
    //                                 PrintICMPPacket(data, totalPacketLength);
    //                                 break;
                                
    //                             case 2:  //IGMP Protocol
    //                                 break;
                                
    //                             case 6:  //TCP Protocol
    //                                 PrintTCPPacket(data, totalPacketLength);
    //                                 break;
                                
    //                             case 17: //UDP Protocol
    //                                 PrintUDPPacket(data, totalPacketLength);
    //                                 break;
                                
    //                             default: //Some Other Protocol like ARP etc.
    //                                 break;
    //                         }       
    //                     }
    //                     break;

    //                 case network::EthernetType::IPV6:
    //                     break;

    //                 case network::EthernetType::ARP:
    //                     break;

    //                 case network::EthernetType::RARP:
    //                     break;
    //             }
    //         }
    //         else
    //         {
    //             break;
    //         }
    //         offset += totalPacketLength;
    //     }
    //     if ((bytesRead != 0) && !dumpFile.eof())
    //     {
    //         dumpFile.read(buffer, BufferSize);
    //         bytesRead = dumpFile.gcount();
    //     }
    // }
    // return 0;
}