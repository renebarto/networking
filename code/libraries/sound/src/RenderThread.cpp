//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : RenderThread.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include <cmath>

#include "tracing/ScopedTracing.h"
#include "utility/Error.h"
#include "utility/GenericError.h"

#include "RenderThread.h"
#include "SoundClient.h"
#include "WindowsCOM.h"

#undef min
#undef max

#include <avrt.h>

namespace sound {

static const int BufferSizeInMSec = 100;

RenderThread::RenderThread(SoundClient & soundClient)
    : core::threading::ActiveObject("RenderThread")
    , m_soundClient(soundClient)
    , m_isInitialized()
    , m_stop()
    , m_enableMMCSS(true) // Enable MultiMedia Class Scheduler Service
    , m_renderBufferQueue()
    , m_sampleBuffer()
    , m_renderClient()
    , m_shutdownEvent()
    , m_mixFormat(soundClient.GetMixFormat())
    , m_frameSize(m_mixFormat.nBlockAlign)
    , m_bufferSize(m_soundClient.BufferSize())
    , m_renderBufferSizeInBytes(BufferSizePerPeriod() * FrameSize())
    , m_renderDataLength((SamplesPerSecond() * BufferSizeInMSec * FrameSize() / 1000) + (m_renderBufferSizeInBytes - 1))
    , m_renderBufferCount(m_renderDataLength / (m_renderBufferSizeInBytes))
    , m_renderBufferReadIndex()
    , m_renderBufferWriteIndex()
    , m_renderSampleType(m_soundClient.GetRenderSampleType())
    , m_audioSource()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

RenderThread::~RenderThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

void RenderThread::InitThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
    {
        LogError(__FILE__, __LINE__, __func__, "Not initialized");
        return;
    }
    TraceInfo(__FILE__, __LINE__, __func__, "Initializing thread");
}

void RenderThread::Run()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
    {
        LogError(__FILE__, __LINE__, __func__, "Not initialized");
        return;
    }

    TraceInfo(__FILE__, __LINE__, __func__, "Starting thread");
    WindowsCOM comBase;
    comBase.Initialize();

    //
    //  We want to pre-roll one buffer's worth of silence into the pipeline.  That way the audio engine won't glitch on startup.  
    //  We pre-roll silence instead of audio buffers because our buffer size is significantly smaller than the engine latency 
    //  and we can only pre-roll one buffer's worth of audio samples.
    //  
    //
    BYTE * data {};
    HRESULT hr = m_renderClient->Get()->GetBuffer(m_bufferSize, &data);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Failed to get buffer: {,X:8}", hr);
        return;
    }
    hr = m_renderClient->Get()->ReleaseBuffer(m_bufferSize, AUDCLNT_BUFFERFLAGS_SILENT);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Failed to release buffer: {,X:8}", hr);
        return;
    }
    m_renderBufferReadIndex = 0;
    m_renderBufferWriteIndex = 0;

    HANDLE mmcssHandle = NULL;
    DWORD mmcssTaskIndex = 0;

    HANDLE waitArray[1] = {GetShutdownEvent()};

    if (m_enableMMCSS)
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Hook up to MMCSS");
        mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
        if (mmcssHandle == nullptr)
        {
            auto errorCode = static_cast<int>(GetLastError());
            tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Unable to enable MMCSS on render thread"));
        }
    }

    auto engineLatency = m_soundClient.GetEngineLatency();

    while (!m_stop)
    {
        //
        //  In Timer Driven mode, we want to wait for half the desired latency in milliseconds.
        //
        //  That way we'll wake up half way through the processing period to hand the 
        //  next set of samples to the engine.
        //
        DWORD waitResult = WaitForMultipleObjects(1, waitArray, FALSE, static_cast<DWORD>(engineLatency / 2));
        switch (waitResult)
        {
            case WAIT_OBJECT_0 + 0:     // ShutdownEvent
                //
                //  We've received a shutdown request.
                //
                TraceInfo(__FILE__, __LINE__, __func__, "Received shutdown event, stopping");
                m_stop = true;
                break;

            case WAIT_TIMEOUT:          // Timeout
                //
                //  We need to provide the next buffer of samples to the audio renderer.  If we're done with our samples, we're done.
                //
                AddAudioSamples();
                break;

            default:
                TraceInfo(__FILE__, __LINE__, __func__, "Unknown wait result: {}", static_cast<int>(waitResult));
                m_stop = true;
                break;
        }

    }

    //
    //  Unhook from MMCSS.
    //
    if (m_enableMMCSS && (mmcssHandle != nullptr))
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Unhook from MMCSS");
        AvRevertMmThreadCharacteristics(mmcssHandle);
    }

    TraceInfo(__FILE__, __LINE__, __func__, "Stopping thread");
}

void RenderThread::ExitThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    Uninitialize();

    TraceInfo(__FILE__, __LINE__, __func__, "Exiting thread");
}

void RenderThread::FlushThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    TraceInfo(__FILE__, __LINE__, __func__, "Flushing thread");
    if (m_shutdownEvent != nullptr)
    {
        SetEvent(m_shutdownEvent);
    }

}

bool RenderThread::Initialize(ISoundSource * audioSource)
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

    m_audioSource = audioSource;
    if (!m_audioSource)
    {
        LogError(__FILE__, __LINE__, __func__, "Invalid source specified: {}", m_audioSource);
        return result;
    }

    TraceInfo(__FILE__, __LINE__, __func__, "Setting up with {} buffers of {} bytes each ({} frames x {} channels x {} bytes/sample), running @{} frames/sec, total buffer time {} msec",
        m_renderBufferCount, m_renderBufferSizeInBytes, BufferSizePerPeriod(), ChannelCount(), FrameSize() / ChannelCount(), SamplesPerSecond(), BufferSizeInMSec);

    IAudioRenderClient * renderClient {};
    HRESULT hr = m_soundClient.GetAudioClient()->GetService(IID_PPV_ARGS(&renderClient));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get new render client: {,X:8}", hr);
        return result;
    }
    m_renderClient = std::make_unique<AudioRenderClientWrapper>(renderClient);

    //
    //  Create our shutdown event - we want auto reset events that start in the not-signaled state.
    //
    m_shutdownEvent = ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (m_shutdownEvent == nullptr)
    {
        auto errorCode = static_cast<int>(GetLastError());
        tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Unable to create shutdown event"));
        return result;
    }

    m_renderBufferQueue.clear();
    for (std::size_t i = 0; i < m_renderBufferCount; ++i)
    {
        m_renderBufferQueue.push_back(RenderBuffer(m_renderBufferSizeInBytes));
    }

    m_sampleBuffer.clear();
    for (std::uint16_t channel = 0; channel < ChannelCount(); ++channel)
    {
        m_sampleBuffer.push_back(std::vector<float>(BufferSizePerPeriod()));
    }
    m_audioSource->Prepare(SamplesPerSecond(), ChannelCount(), BufferSizePerPeriod());

    m_isInitialized = true;
    result = true;
    return result;
}

void RenderThread::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
        return;

    //
    //  Drain the buffers in the render buffer queue.
    //
    m_renderBufferQueue.clear();
    m_renderClient.reset();
    if (m_shutdownEvent != nullptr)
    {
        ::CloseHandle(m_shutdownEvent);
        m_shutdownEvent = nullptr;
    }
}

void RenderThread::AddAudioSamples()
{
    std::uint32_t padding;
    HRESULT hr = m_soundClient.GetAudioClient()->GetCurrentPadding(&padding);
    if (SUCCEEDED(hr))
    {
        //
        //  Calculate the number of frames available.  We'll render
        //  that many frames or the number of frames left in the buffer, whichever is smaller.
        //
        TraceInfo(__FILE__, __LINE__, __func__, "read {} write {}", m_renderBufferReadIndex, m_renderBufferWriteIndex);
        std::uint32_t framesAvailable = m_bufferSize - padding;
        while (m_renderBufferSizeInBytes <= framesAvailable * m_frameSize)
        {
            std::uint32_t framesToWrite = m_renderBufferSizeInBytes / m_frameSize;
            BYTE * data {};
            hr = m_renderClient->Get()->GetBuffer(framesToWrite, &data);
            if (SUCCEEDED(hr))
            {
                //
                //  Copy data from the render buffer to the output buffer and bump our render pointer.
                //
                auto const & currentBuffer = m_renderBufferQueue[m_renderBufferReadIndex];
                m_renderBufferReadIndex = (m_renderBufferReadIndex + 1) % m_renderBufferCount;
                ::CopyMemory(data, currentBuffer.Buffer().data(), framesToWrite * m_frameSize);
                hr = m_renderClient->Get()->ReleaseBuffer(framesToWrite, 0);
                if (FAILED(hr))
                {
                    printf("Unable to release buffer: %x\n", hr);
                }
            }
            framesAvailable -= framesToWrite;
            if (FAILED(hr))
            {
                m_stop = true;
                break;
            }
        }
        while (m_renderBufferWriteIndex != m_renderBufferReadIndex)
        {
            auto & currentBuffer = m_renderBufferQueue[m_renderBufferWriteIndex];
            m_renderBufferWriteIndex = (m_renderBufferWriteIndex + 1) % m_renderBufferCount;
            auto & bufferData = currentBuffer.Claim();
            uint8_t * bufferDataPtr = bufferData.data();

            m_audioSource->GetSamples(m_sampleBuffer);
            for (std::uint32_t sampleIndex = 0; sampleIndex < BufferSizePerPeriod(); ++sampleIndex)
            {
                for (std::uint16_t channel = 0; channel < ChannelCount(); ++channel)
                {
                    switch (m_renderSampleType)
                    {
                        case RenderSampleType::SampleTypeFloat:
                            {
                                float value = m_sampleBuffer[channel][sampleIndex];
                                memcpy(bufferDataPtr, &value, sizeof(value));
                                bufferDataPtr += sizeof(value);
                            }
                            break;

                        case RenderSampleType::SampleType16BitPCM:
                            {
                                std::int16_t value = std::min(std::int16_t {-32768}, std::max(std::int16_t {32767}, static_cast<std::int16_t>(m_sampleBuffer[channel][sampleIndex] * 32767 + 0.5)));
                                memcpy(bufferDataPtr, &value, sizeof(value));
                                bufferDataPtr += sizeof(value);
                            }
                            break;

                        default:
                            tracing::Tracing::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Invalid RenderSampleType {}", static_cast<int>(m_renderSampleType)));
                            break;
                    }
                }
            }

            currentBuffer.Release();
        }
    }
}

HANDLE RenderThread::GetShutdownEvent() const
{
    return m_shutdownEvent;
}

} // namespace sound

#endif
