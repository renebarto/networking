//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundClient.h
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#if defined(PLATFORM_WINDOWS)

#include <memory>

struct IMMDevice;
struct IMMDeviceEnumerator;
struct IMMDeviceCollection;

using DeviceWrapper = InterfaceWrapper<IMMDevice>;
using DeviceEnumeratorWrapper = InterfaceWrapper<IMMDeviceEnumerator>;

namespace sound {

class DeviceCollection
{
private:
    bool m_isInitialized;
    std::unique_ptr<DeviceEnumeratorWrapper> m_deviceEnumerator;
    IMMDeviceCollection * m_deviceCollection;
    std::vector<std::string> m_deviceNames;
    std::vector<std::string> m_deviceIDs;

public:
    DeviceCollection();
    ~DeviceCollection();

    bool Initialize();
    void Uninitialize();

    std::unique_ptr<DeviceWrapper> GetDefaultDevice();
    std::vector<std::string> GetDeviceNames();
    std::vector<std::string> GetDeviceIDs();
    bool GetDeviceName(unsigned deviceIndex, std::string & deviceName, std::string & deviceID);
    std::unique_ptr<DeviceWrapper> GetDevice(const std::string & deviceName);
    std::unique_ptr<DeviceWrapper> GetDevice(unsigned deviceIndex);
    std::unique_ptr<DeviceWrapper> GetDeviceByID(const std::string & deviceID);
};

} // namespace sound

#endif