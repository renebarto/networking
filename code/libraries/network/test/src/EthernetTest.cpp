#include "GoogleTest.h"

#include "network/Ethernet.h"

namespace network {

TEST(EthernetTest, Serialize)
{
    EXPECT_EQ("IP", serialization::Serialize(EthernetType::IP));
    EXPECT_EQ("ARP", serialization::Serialize(EthernetType::ARP));
    EXPECT_EQ("RARP", serialization::Serialize(EthernetType::RARP));
}

} // namespace network