#include <iostream>
#include <iomanip>
#include <string>
#include <pcrecpp.h>

#include "net/net.h"

extern "C" {
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
}

static inline std::string ipv4addressToString(const void * src) {
  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, src, ip, INET_ADDRSTRLEN);
  return std::string(ip);
}

namespace mckeys {

using namespace std;

// Like getInstance. Used for creating commands from packets.
MemcacheCommand MemcacheCommand::create(const Packet& pkt,
                                        const bpf_u_int32 captureAddress,
                                        const memcache_command_t expectedCmdType)
{
  static ssize_t ethernetHeaderSize = sizeof(struct ether_header);

  const struct ether_header* ethernetHeader;
  const struct ip* ipHeader;
  const struct tcphdr* tcpHeader;

  const Packet::Header* pkthdr = &pkt.getHeader();
  const Packet::Data* packet = pkt.getData();

  u_char *data;
  uint32_t dataLength = 0;
  uint32_t dataOffset;

  string sourceAddress = "";
  string destinationAddress = "";

  // must be an IP packet
  // TODO add support for dumping localhost
  ethernetHeader = (struct ether_header*)packet;
  auto etype = ntohs(ethernetHeader->ether_type);
  if (etype != ETHERTYPE_IP) {
    return MemcacheCommand();
  }

  // must be TCP - TODO add support for UDP
  ipHeader = (struct ip*)(packet + ethernetHeaderSize);
  ssize_t ipHeaderSize = ipHeader->ip_hl * 4;
  auto itype = ipHeader->ip_p;
  if (itype != IPPROTO_TCP) {
    return MemcacheCommand();
  }
  sourceAddress = ipv4addressToString(&(ipHeader->ip_src));
  destinationAddress = ipv4addressToString(&(ipHeader->ip_dst));

  tcpHeader = (struct tcphdr*)(packet + ethernetHeaderSize + ipHeaderSize);
  ssize_t tcpHeaderSize = tcpHeader->doff * 4;
  dataOffset = ethernetHeaderSize + ipHeaderSize + tcpHeaderSize;
  data = (u_char*)(packet + dataOffset);
  dataLength = pkthdr->len - dataOffset;
  if (dataLength > pkthdr->caplen) {
    dataLength = pkthdr->caplen;
  }

  // The packet was destined for our capture address, this is a request
  // This bit of optimization lets us ignore a reasonably large percentage of
  // traffic
  if (expectedCmdType == MC_REQUEST) {
    if (ipHeader->ip_dst.s_addr == captureAddress) { // a request packet to server
      return makeRequestCommand(data, dataLength, sourceAddress, destinationAddress);
    }
  } else if (expectedCmdType == MC_RESPONSE) {
    if (ipHeader->ip_src.s_addr == captureAddress) { // a response packet from server
      return makeResponseCommand(data, dataLength, sourceAddress, destinationAddress);
    }
  }
  return MemcacheCommand();
}


// protected default constructor
MemcacheCommand::MemcacheCommand()
  : cmdType_(MC_UNKNOWN),
    sourceAddress_(),
    destinationAddress_(),
    commandName_(),
    objectKey_(),
    objectSize_(0)
{}

// protected constructor
MemcacheCommand::MemcacheCommand(const memcache_command_t cmdType,
                                 const string sourceAddress,
                                 const string destinationAddress,
                                 const string commandName,
                                 const string objectKey,
                                 uint32_t objectSize)
    : cmdType_(cmdType),
      sourceAddress_(sourceAddress),
      destinationAddress_(destinationAddress),
      commandName_(commandName),
      objectKey_(objectKey),
      objectSize_(objectSize)
{}

// static protected
MemcacheCommand MemcacheCommand::makeRequestCommand(u_char* data,
                                                    int length,
                                                    string sourceAddress,
                                                    string destinationAddress)
{
  // set <key> <flags> <exptime> <bytes> [noreply]\r\n
  static string commandName = "set";
  static pcrecpp::RE re(commandName + string(" (\\S+) \\d+ \\d+ (\\d+)"),
                        pcrecpp::RE_Options(PCRE_MULTILINE));
  string key;
  int size = -1;
  string input = "";

  for (int i = 0; i < length; i++) {
    int cid = (int)data[i];
    if (isprint(cid) || cid == 10 || cid == 13) {
      input += static_cast<char>(cid);
    }
  }
  if (input.length() < 11) { // set k 0 0 1
    return MemcacheCommand();
  }

  re.PartialMatch(input, &key, &size);
  if (size >= 0) {
    return MemcacheCommand(MC_REQUEST, sourceAddress, destinationAddress, commandName, key, size);
  }
  return MemcacheCommand();
}

// static protected
MemcacheCommand MemcacheCommand::makeResponseCommand(u_char *data,
                                                     int length,
                                                     string sourceAddress,
                                                     string destinationAddress)
{
  // VALUE <key> <flags> <bytes> [<cas unique>]\r\n
  static string commandName = "get";
  static pcrecpp::RE re("VALUE (\\S+) \\d+ (\\d+)",
                        pcrecpp::RE_Options(PCRE_MULTILINE));
  string key;
  int size = -1;
  string input = "";

  for (int i = 0; i < length; i++) {
    int cid = (int)data[i];
    if (isprint(cid) || cid == 10 || cid == 13) {
      input += static_cast<char>(cid);
    }
  }
  if (input.length() < 11) { // VALUE k 0 1
    return MemcacheCommand();
  }

  re.PartialMatch(input, &key, &size);
  if (size >= 0) {
    return MemcacheCommand(MC_RESPONSE, sourceAddress, destinationAddress, commandName, key, size);
  } else {
    return MemcacheCommand();
  }
}

} // end namespace
