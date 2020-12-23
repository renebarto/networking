#pragma once

#include <string>
#include <map>

namespace packetcapture
{

class Interface;

class Interfaces
{
public:
    Interfaces();
    explicit Interfaces(const std::string & interfaceName);
    ~Interfaces();

    bool IsValid() const { return m_devices != nullptr; }
    bool HaveInterface(const std::string & interfaceName) const;
    const Interface & GetInterface(const std::string & interfaceName) const;
    const std::map<std::string, Interface> & GetAllInterfaces() const { return m_interfaces; }

private:
    std::map<std::string, Interface> m_interfaces;
    void * m_devices;

    void ExtractInterfaceInfo();
};

} // namespace packetcapture