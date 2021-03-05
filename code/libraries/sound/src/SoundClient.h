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

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;

using DeviceWrapper = InterfaceWrapper<IMMDevice>;
using AudioClientWrapper = InterfaceWrapper<IAudioClient>;
using AudioRenderClientWrapper = InterfaceWrapper<IAudioRenderClient>;

namespace sound {

class RenderThread;
class AudioSessionControl;

enum RenderSampleType
{
    SampleTypeFloat,
    SampleType16BitPCM,
};

class SoundClient
{
private:
    unsigned long m_refCount;
    bool m_isInitialized;
    std::unique_ptr<DeviceWrapper> m_device;
    std::unique_ptr<AudioClientWrapper> m_audioClient;
    std::unique_ptr<AudioRenderClientWrapper> m_renderClient;
    std::unique_ptr<RenderThread> m_renderThread;
    WAVEFORMATEX * m_mixFormat;
    std::uint32_t m_frameSize;
    std::uint32_t m_bufferSize;
    RenderSampleType m_renderSampleType;
    int m_engineLatency;
    HANDLE m_shutdownEvent;
    bool m_enableStreamSwitch; // false
    std::unique_ptr<AudioSessionControl> m_audioSessionControl;

public:
    SoundClient(std::unique_ptr<DeviceWrapper> && device);
    virtual ~SoundClient();
    
    bool Initialize();
    void Uninitialize();

    bool Start();
    void Stop();

    bool DetermineAudioFormat();
    bool InitializeAudioEngine();
    void UninitializeAudioEngine();

    std::uint16_t ChannelCount() { return (m_mixFormat != nullptr) ? static_cast<std::uint16_t>(m_mixFormat->nChannels) : std::uint16_t {0}; }
    std::uint32_t SamplesPerSecond() { return (m_mixFormat != nullptr) ? static_cast<std::uint32_t>(m_mixFormat->nSamplesPerSec) : std::uint32_t {0}; }
    std::uint32_t BytesPerSample() { return (m_mixFormat != nullptr) ? static_cast<std::uint32_t>(m_mixFormat->wBitsPerSample / 8) : std::uint32_t {0}; }
    std::uint32_t FrameSize() { return m_frameSize; }
    std::uint32_t BufferSize() { return m_bufferSize; }

};

} // namespace sound

#endif