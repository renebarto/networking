#pragma once

#include <string>
#include <map>

namespace network
{

class Interface;

class Interfaces
{
public:
    Interfaces();
    explicit Interfaces(const std::string & interfaceName);
    ~Interfaces();

    bool IsValid() const { return m_interfaces != nullptr; }
    bool HaveInterface(const std::string & interfaceName) const;
    const Interface & GetInterface(const std::string & interfaceName) const;
    const std::map<std::string, Interface> & GetAllInterfaces() const { return m_interfacesMap; }
    std::map<std::string, Interface> GetLocalLoopbackInterfaces() const;
    std::map<std::string, Interface> GetIPV4Interfaces() const;
    std::map<std::string, Interface> GetIPV6Interfaces() const;

private:
    std::map<std::string, Interface> m_interfacesMap;
    void * m_interfaces;

    void ExtractInterfaceInfo(const std::string & interfaceName = {});
};

} // namespace packetcapture