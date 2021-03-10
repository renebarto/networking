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

#include <map>
#include <string>
#include <vector>

#include <AudioClient.h>
#include <AudioPolicy.h>
#include "WindowsCOM.h"
#include "sound/IAudioSource.h"

struct IMMDevice;
struct IAudioClient;

using DeviceWrapper = InterfaceWrapper<IMMDevice>;
using AudioClientWrapper = InterfaceWrapper<IAudioClient>;

namespace sound {

class RenderThread;
class AudioSessionControl;

class SoundClient
{
private:
    unsigned long m_refCount;
    bool m_isInitialized;
    std::unique_ptr<DeviceWrapper> m_device;
    std::unique_ptr<AudioClientWrapper> m_audioClient;
    std::unique_ptr<RenderThread> m_renderThread;
    WAVEFORMATEX * m_mixFormat;
    std::uint32_t m_bufferSize;
    RenderSampleType m_renderSampleType;
    int m_engineLatency;

public:
    SoundClient(std::unique_ptr<DeviceWrapper> && device);
    virtual ~SoundClient();
    
    bool Initialize();
    void Uninitialize();

    bool Start(IAudioSource * audioSource);
    void Stop();

    bool DetermineAudioFormat();
    bool InitializeAudioEngine();
    void UninitializeAudioEngine();

    std::uint32_t BufferSize() const { return m_bufferSize; }

    int GetEngineLatency() const { return m_engineLatency; }
    RenderSampleType GetRenderSampleType() const { return m_renderSampleType; }
    IAudioClient * GetAudioClient() const { return m_audioClient.get()->Get(); }
    const WAVEFORMATEX & GetMixFormat() const;
};

} // namespace sound

#endif