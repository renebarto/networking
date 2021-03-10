//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : RenderThread.h
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

#include "core/ActiveObject.h"
#include "RenderBuffer.h"
#include "WindowsCOM.h"
#include "sound/IAudioSource.h"

struct IAudioRenderClient;

using AudioRenderClientWrapper = InterfaceWrapper<IAudioRenderClient>;

namespace sound {

class SoundClient;

using ChannelSampleBuffer = std::vector<float>;

class RenderThread
    : public core::ActiveObject
{
private:
    SoundClient & m_soundClient;
    bool m_isInitialized;
    bool m_stop;
    bool m_enableMMCSS;
    std::vector<RenderBuffer> m_renderBufferQueue;
    std::vector<ChannelSampleBuffer> m_sampleBuffer;
    std::unique_ptr<AudioRenderClientWrapper> m_renderClient;
    HANDLE m_shutdownEvent;
    const WAVEFORMATEX & m_mixFormat;
    std::uint32_t m_frameSize;
    std::uint32_t m_bufferSize;
    std::uint32_t m_renderBufferSizeInBytes;
    std::size_t m_renderDataLength;
    std::size_t m_renderBufferCount;
    std::size_t m_renderBufferReadIndex;
    std::size_t m_renderBufferWriteIndex;
    RenderSampleType m_renderSampleType;
    IAudioSource * m_audioSource;

public:
    RenderThread(SoundClient & soundClient);
    ~RenderThread();
    
    void InitThread() override;
    void Run() override;
    void ExitThread() override;
    void FlushThread() override;

    bool Initialize(IAudioSource * audioSource);
    void Uninitialize();

    void AddAudioSamples();
    HANDLE GetShutdownEvent() const;

    std::uint16_t ChannelCount() const { return static_cast<std::uint16_t>(m_mixFormat.nChannels); }
    std::uint32_t SamplesPerSecond() const { return static_cast<std::uint32_t>(m_mixFormat.nSamplesPerSec); }
    std::uint32_t BytesPerSample() const { return static_cast<std::uint32_t>(m_mixFormat.wBitsPerSample / 8); }
    std::uint32_t FrameSize() const { return m_frameSize; }
    std::uint32_t BufferSize() const { return m_bufferSize; }
    //
    //  The Timer Driven renderer will wake up periodically and hand the audio engine
    //  as many buffers worth of data as possible when it wakes up.  That way we'll ensure 
    //  that there is always data in the pipeline so the engine doesn't glitch. 
    //
    std::uint32_t BufferSizePerPeriod() const { return m_bufferSize / 4; }
};

} // namespace sound

#endif