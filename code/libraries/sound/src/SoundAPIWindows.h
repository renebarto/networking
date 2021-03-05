//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundAPIWindows.h
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

#include <map>
#include <string>
#include <vector>
#include <windows.h>
#include <MMDeviceAPI.h>

#include "sound/ISoundAPI.h"
#include "WindowsCOM.h"

struct IMMDevice;

using DeviceWrapper = InterfaceWrapper<IMMDevice>;

namespace sound {

class DeviceCollection;
class SoundClient;

class SoundAPIWindows
    : public ISoundAPI
{
private:
    WindowsCOM m_comBase;
    bool m_isInitialized;
    std::unique_ptr<DeviceCollection> m_deviceCollection;
    std::unique_ptr<DeviceWrapper> m_device;
    std::unique_ptr<SoundClient> m_audioClient;

public:
    SoundAPIWindows();
    ~SoundAPIWindows();

    bool Initialize(const std::string & deviceName) override;
    void Uninitialize() override;
    bool IsInitialized() override;
};

} // namespace sound

#endif