#include <vector>

#include "tracing/Tracing.h"

// import socket, sys
// from struct import *

// ETH_LENGTH = 14

// def eth_addr (a) :
//   b = "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" % (ord(a[0]) ,
//           ord(a[1]),
//           ord(a[2]),
//           ord(a[3]),
//           ord(a[4]),
//           ord(a[5]))
//   return b

// #create a AF_PACKET type raw socket to sniff at packets
// #define ETH_P_ALL    0x0003 - register to listen to all packets
// try:
//     s = socket.socket( socket.AF_PACKET,
//             socket.SOCK_RAW,
//             socket.ntohs(0x0003))
// except socket.error , message:
//     print 'Socket could not be created. Error Code : %s %s ' %
//         (str(message[0]), message[1])
//     sys.exit()


// def parse_ethernet_header(packet):
//     eth_header = packet[:ETH_LENGTH]
//     eth = unpack('!6s6sH' , eth_header)
//     eth_protocol = socket.ntohs(eth[2])
//     print 'Destination MAC : ' + eth_addr(packet[0:6]) +
//     ' Source MAC : ' + eth_addr(packet[6:12]) +
//     ' Protocol : ' + str(eth_protocol)
//     #IP packets, Protocol number = 8
//     if eth_protocol == 8 :
//         parse_ip_header(packet)
//     else :
//         print  "parsing supported for only IP packets"

// def parse_ip_header(packet):
//     #IP header
//     #First 20 characters for the ip header
//     ip_header = packet[ETH_LENGTH:20+ETH_LENGTH]
//     # '!' to tell that we are not sure about byte ordering
//     iph = unpack('!BBHHHBBH4s4s' , ip_header)
//     version_ihl = iph[0]
//     version = version_ihl >> 4
//     ihl = version_ihl & 0xF
//     iph_length = ihl * 4
//     ttl = iph[5]
//     protocol = iph[6]
//     s_addr = socket.inet_ntoa(iph[8]);
//     d_addr = socket.inet_ntoa(iph[9]);
//     print 'Version : ' + str(version) + ' IP Header Length : ' +
//     str(ihl) + ' TTL : ' + str(ttl) + ' Protocol : ' +
//     str(protocol) + ' Source Address : ' +
//     str(s_addr) + ' Destination Address : ' + str(d_addr)
//     if protocol == 6 :
//         parse_tcp_header(packet, iph_length)
//     elif protocol == 1 :
//         parse_icmp_header(packet, iph_length)
//     elif protocol == 17:
//         parse_udp_header(packet, iph_length)
//     else :
//         print "protocol parsing not supported %d" % protocol


// def parse_tcp_header(packet, iph_length):
//     t = iph_length + ETH_LENGTH
//     tcp_header = packet[t:t+20]
//     tcph = unpack('!HHLLBBHHH' , tcp_header)
//     source_port = tcph[0]
//     dest_port = tcph[1]
//     sequence = tcph[2]
//     acknowledgement = tcph[3]
//     doff_reserved = tcph[4]
//     tcph_length = doff_reserved >> 4
//     print "                      **TCP PACKET**"
//     print 'Source Port : ' + str(source_port) + ' Dest Port : ' +
//     str(dest_port) + ' Sequence Number : ' + str(sequence) +
//     ' Acknowledgement : ' + str(acknowledgement) +
//     ' TCP header length : ' + str(tcph_length)
//     h_size = ETH_LENGTH + iph_length + tcph_length * 4
//     data_size = len(packet) - h_size
//     #get data from the packet
//     data = packet[h_size:]
//     print 'Data : ' + data

// def parse_udp_header(packet, iph_length):
//     u = iph_length + ETH_LENGTH
//     udph_length = 8
//     udp_header = packet[u:u+8]
//     udph = unpack('!HHHH' , udp_header)
//     source_port = udph[0]
//     dest_port = udph[1]
//     length = udph[2]
//     checksum = udph[3]
//     print "                      **UDP PACKET**"
//     print 'Source Port : ' + str(source_port) +
//     ' Dest Port : ' + str(dest_port) + ' Length : ' +
//     str(length) + ' Checksum : ' + str(checksum)
//     h_size = ETH_LENGTH + iph_length + udph_length
//     data_size = len(packet) - h_size
//     #get data from the packet
//     data = packet[h_size:]
//     print 'Data : ' + data

// def parse_icmp_header(packet, iph_length):
//     u = iph_length + ETH_LENGTH
//     icmph_length = 4
//     icmp_header = packet[u:u+4]
//     icmph = unpack('!BBH' , icmp_header)
//     icmp_type = icmph[0]
//     code = icmph[1]
//     checksum = icmph[2]
//     print "                      **ICMP PACKET**"
//     print 'Type : ' + str(icmp_type) + ' Code : ' +
//     str(code) + ' Checksum : ' + str(checksum)
//     h_size = ETH_LENGTH + iph_length + icmph_length
//     data_size = len(packet) - h_size
//     #get data from the packet
//     data = packet[h_size:]
//     print 'Data : ' + data

void parse_ethernet_header(const std::vector<std::uint8_t> & /*packet*/)
{
    // eth_header = packet[:ETH_LENGTH]
    // eth = unpack('!6s6sH' , eth_header)
    // eth_protocol = socket.ntohs(eth[2])
    // print 'Destination MAC : ' + eth_addr(packet[0:6]) +
    // ' Source MAC : ' + eth_addr(packet[6:12]) +
    // ' Protocol : ' + str(eth_protocol)
    // #IP packets, Protocol number = 8
    // if eth_protocol == 8 :
    //     parse_ip_header(packet)
    // else :
    //     print  "parsing supported for only IP packets"
}

#define ETH_P_ALL    0x0003 // register to listen to all packets

int main(int /*argc*/, char * /*argv*/[])
{
    TraceInfo(__FILE__, __LINE__, __func__, "packet-sniffer");
//     // Create a AF_PACKET type raw socket to sniff at packets
//     int s = socket(AF_PACKET, SOCK_RAW, ETH_P_ALL);
//     if (s == -1)
//     {
//         print 'Socket could not be created. Error Code : %s %s ' %
//         (str(message[0]), message[1])
//     }
// except socket.error , message:

//     sys.exit()
//     while (true)
//     {
//         packet = s.recvfrom(0xFFFF);
//         // get first packet from tuple and process
//         parse_ethernet_header(packet)
//         std::cout << "___________________________________________" << std::endl;
//     }
    return 0;
}