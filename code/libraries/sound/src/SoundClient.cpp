//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundClient.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include <MMDeviceAPI.h>

#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"
#include "utility/EnumSerialization.h"
#include "utility/Error.h"

#include "WindowsCOM.h"
#include "SoundClient.h"
#include "RenderThread.h"
#include "AudioSessionControl.h"

namespace sound {

static const int TargetLatency = 50; // msec

SoundClient::SoundClient(std::unique_ptr<DeviceWrapper> && device)
    : m_refCount(1)
    , m_isInitialized()
    , m_device(std::move(device))
    , m_audioClient()
    , m_renderClient()
    , m_renderThread()
    , m_mixFormat()
    , m_frameSize()
    , m_bufferSize()
    , m_renderSampleType()
    , m_engineLatency(TargetLatency)
    , m_shutdownEvent()
    , m_enableStreamSwitch(false)
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

SoundClient::~SoundClient()
{
    if (m_isInitialized)
        Uninitialize();
}

bool SoundClient::Initialize()
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

    if (m_engineLatency < 50)
    {
        printf("Engine latency in shared mode timer driven cannot be less than 50ms\n");
        LogError(__FILE__, __LINE__, __func__, "Engine latency in shared mode timer driven cannot be less than 50ms, but is {}", m_engineLatency);
        return result;
    }

    //
    //  Create our shutdown event - we want auto reset events that start in the not-signaled state.
    //
    m_shutdownEvent = ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (m_shutdownEvent == nullptr)
    {
        auto errorCode = static_cast<int>(GetLastError());
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Unable to create shutdown event"));
        return result;
    }

    //
    //  Now activate an IAudioClient object on our preferred endpoint and retrieve the mix format for that endpoint.
    //
    IAudioClient * audioClient {};
    HRESULT hr = m_device->Get()->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void **>(&audioClient));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to instantiate audio client: {,X:8}", hr);
        return result;
    }
    m_audioClient = std::make_unique<AudioClientWrapper>(audioClient);

    if (!DetermineAudioFormat())
        return result;

    if (!InitializeAudioEngine())
        return result;

    if (m_enableStreamSwitch)
    {
        m_audioSessionControl = std::make_unique<AudioSessionControl>(audioClient);
        if ((m_audioSessionControl == nullptr) || !m_audioSessionControl->Initialize())
           return result;
    }

    m_isInitialized = true;
    result = true;
    return result;
}

void SoundClient::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
        return;
        
    if (m_audioSessionControl != nullptr)
    {
        m_audioSessionControl->Uninitialize();
        m_audioSessionControl.reset();
    }
    UninitializeAudioEngine();
    if (m_mixFormat != nullptr)
    {
        ::CoTaskMemFree(m_mixFormat);
        m_mixFormat = nullptr;
    }
    m_audioClient.reset();
    if (m_shutdownEvent != nullptr)
    {
        ::CloseHandle(m_shutdownEvent);
        m_shutdownEvent = nullptr;
    }
    m_isInitialized = false;
}

//
//  Retrieve the format we'll use to render samples.
//
//  We use the Mix format since we're rendering in shared mode.
//
bool SoundClient::DetermineAudioFormat()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    HRESULT hr = m_audioClient->Get()->GetMixFormat(&m_mixFormat);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to retrieve mix format for new audio client: {,X:8}", hr);
        return result;
    }

    TraceMessage(__FILE__, __LINE__, __func__, "Audio format: {}, blockalign={}, bits/sample={}", m_mixFormat->wFormatTag, m_mixFormat->nBlockAlign, m_mixFormat->wBitsPerSample);
    m_frameSize = m_mixFormat->nBlockAlign;

    if (m_mixFormat->wFormatTag == WAVE_FORMAT_PCM || 
        m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
            reinterpret_cast<WAVEFORMATEXTENSIBLE *>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
    {
        if (m_mixFormat->wBitsPerSample == 16)
        {
            m_renderSampleType = SampleType16BitPCM;
        }
        else
        {
            LogError(__FILE__, __LINE__, __func__, "Unknown PCM integer sample type: bits/sample={}", m_mixFormat->wBitsPerSample);
            return result;
        }
    }
    else if (m_mixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
            (m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
            reinterpret_cast<WAVEFORMATEXTENSIBLE *>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
    {
        m_renderSampleType = SampleTypeFloat;
    }
    else 
    {
        LogError(__FILE__, __LINE__, __func__, "Unknown format type: format={}", m_mixFormat->wFormatTag);
        return result;
    }

    result = true;
    return result;
}

bool SoundClient::InitializeAudioEngine()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    HRESULT hr = m_audioClient->Get()->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST, 
        m_engineLatency * 10000,  0,  m_mixFormat, nullptr);

    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to initialize audio client: {,X:8}", hr);
        return result;
    }

    //
    //  Retrieve the buffer size for the audio client.
    //
    hr = m_audioClient->Get()->GetBufferSize(&m_bufferSize);
    if(FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get audio client buffer: {,X:8}", hr);
        return result;
    }

    IAudioRenderClient * renderClient {};
    hr = m_audioClient->Get()->GetService(IID_PPV_ARGS(&renderClient));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get new render client: {,X:8}", hr);
        return result;
    }
    m_renderClient = std::make_unique<AudioRenderClientWrapper>(renderClient);

    result = true;
    return result;
}

void SoundClient::UninitializeAudioEngine()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    m_renderClient.reset();
}

} // namespace sound

#endif
