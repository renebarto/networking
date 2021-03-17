//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundAPIWindows.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"
#include "utility/Error.h"

#include "SoundAPIWindows.h"
#include "DeviceCollection.h"
#include "SoundClient.h"

namespace sound {

static const int TargetLatency = 30; // msec

ISoundAPIPtr CreateAPI()
{
    SCOPEDTRACE(nullptr, nullptr);
    return std::make_unique<SoundAPIWindows>();
}

SoundAPIWindows::SoundAPIWindows()
    : m_comBase()
    , m_isInitialized()
    , m_device()
    , m_deviceCollection()
    , m_audioClient()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

SoundAPIWindows::~SoundAPIWindows()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool SoundAPIWindows::Initialize(const std::string & deviceName)
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    if (m_isInitialized)
    {
        result = true;
        return result;
    }
    if (!m_comBase.Initialize())
        return result;

    m_deviceCollection = std::make_unique<DeviceCollection>();
    if (m_deviceCollection == nullptr)
        return result;

    if (!m_deviceCollection->Initialize())
        return result;

    auto deviceNames = m_deviceCollection->GetDeviceNames();
    for (auto const & name : deviceNames)
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Device name {}", name);
    }
    auto deviceIDs = m_deviceCollection->GetDeviceIDs();
    for (auto const & id : deviceIDs)
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Device ID {}", id);
    }

    if (deviceName.empty())
    {
        m_device = m_deviceCollection->GetDefaultDevice();
    }
    else
    {
        m_device = m_deviceCollection->GetDevice(deviceName);
    }
    if (m_device == nullptr)
        return result;

    m_audioClient = std::make_unique<SoundClient>(std::move(m_device));
    if (m_audioClient == nullptr)
        return result;

    if (!m_audioClient->Initialize())
        return result;

    m_isInitialized = true;
    result = true;

    return result;
}

void SoundAPIWindows::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
        return;

    m_audioClient->Uninitialize();    
    m_audioClient.reset();
    m_device.reset();
    m_deviceCollection->Uninitialize();    
    m_deviceCollection.reset();

    m_isInitialized = false;
}

bool SoundAPIWindows::IsInitialized()
{
    return m_isInitialized;
}

bool SoundAPIWindows::Start(ISoundSource * audioSource)
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    result = m_audioClient->Start(audioSource);
    return result;
}

void SoundAPIWindows::Stop()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    m_audioClient->Stop();
}

} // namespace sound

#endif
