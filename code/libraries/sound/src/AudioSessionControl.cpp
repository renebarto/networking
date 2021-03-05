//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : AudioSessionControl.cpp
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
#include "AudioSessionControl.h"

namespace sound {

static const int TargetLatency = 50; // msec

AudioSessionControl::AudioSessionControl(IAudioClient * audioClient)
    : m_refCount(1)
    , m_isInitialized()
    , m_audioClient(audioClient)
    , m_defaultDeviceRole(eMultimedia)
    , m_streamSwitchEvent()
    , m_streamSwitchCompleteEvent()
    , m_audioSessionControl()
    , m_deviceEnumerator()
    , m_inStreamSwitch()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

AudioSessionControl::~AudioSessionControl()
{
    if (m_isInitialized)
        Uninitialize();
}

bool AudioSessionControl::Initialize()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    IMMDeviceEnumerator * deviceEnumerator {};
    HRESULT hr = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to instantiate device enumerator: {,X:8}", hr);
        return result;
    }
    m_deviceEnumerator = std::make_unique<DeviceEnumeratorWrapper>(deviceEnumerator);

    IAudioSessionControl * audioSessionControl {};
    hr = m_audioClient->GetService(IID_PPV_ARGS(&audioSessionControl));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to retrieve session control: {,X:8}", hr);
        return result;
    }
    m_audioSessionControl = std::make_unique<AudioSessionControlWrapper>(audioSessionControl);

    //
    //  Create our stream switch event- we want an auto reset event that starts in the not-signaled state.
    //  Note that we create this event even if we're not going to stream switch - that's because the event is used
    //  in the main loop of the renderer and thus it has to be set.
    //
    m_streamSwitchEvent = ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (m_streamSwitchEvent == nullptr)
    {
        auto errorCode = static_cast<int>(GetLastError());
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Unable to create stream switch event"));
        return result;
    }

    //
    //  Create the stream switch complete event- we want a manual reset event that starts in the not-signaled state.
    //
    m_streamSwitchCompleteEvent = CreateEventEx(nullptr, nullptr, CREATE_EVENT_INITIAL_SET | CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (m_streamSwitchCompleteEvent == nullptr)
    {
        auto errorCode = static_cast<int>(GetLastError());
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Unable to create stream switch event"));
        return result;
    }

    //  Register for session and endpoint change notifications.  
    //
    //  A stream switch is initiated when we receive a session disconnect notification or we receive a default device changed notification.
    //
    hr = m_audioSessionControl->Get()->RegisterAudioSessionNotification(this);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to register for session stream switch notifications: {,X:8}", hr);
        return result;
    }

    hr = m_deviceEnumerator->Get()->RegisterEndpointNotificationCallback(this);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to register for endpoint stream switch notifications: {,X:8}", hr);
        return result;
    }

    result = true;
    return result;
}

void AudioSessionControl::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    HRESULT hr;
    if (m_audioSessionControl->Get() != nullptr)
    {
        hr = m_audioSessionControl->Get()->UnregisterAudioSessionNotification(this);
        if (FAILED(hr))
        {
            LogError(__FILE__, __LINE__, __func__, "Unable to unregister for session notifications: {,X:8}", hr);
        }
    }

    if (m_deviceEnumerator)
    {
        hr = m_deviceEnumerator->Get()->UnregisterEndpointNotificationCallback(this);
        if (FAILED(hr))
        {
            LogError(__FILE__, __LINE__, __func__, "Unable to unregister for endpoint notifications: {,X:8}", hr);
        }
    }

    if (m_streamSwitchEvent != nullptr)
    {
        ::CloseHandle(m_streamSwitchEvent);
        m_streamSwitchEvent = nullptr;
    }
    if (m_streamSwitchCompleteEvent != nullptr)
    {
        CloseHandle(m_streamSwitchCompleteEvent);
        m_streamSwitchCompleteEvent = nullptr;
    }

    m_audioSessionControl.reset();
    m_deviceEnumerator.reset();
}

//
// IUnknown
//
ULONG AudioSessionControl::AddRef() noexcept
{
    return InterlockedIncrement(&m_refCount);
}

ULONG AudioSessionControl::Release() noexcept
{
    ULONG returnValue = InterlockedDecrement(&m_refCount);
    if (returnValue == 0)
    {
        delete this;
    }
    return returnValue;
}

HRESULT AudioSessionControl::QueryInterface(REFIID iid, void **object) noexcept
{
    if (object == NULL)
    {
        return E_POINTER;
    }
    *object = NULL;

    if (iid == IID_IUnknown)
    {
        *object = static_cast<IUnknown *>(static_cast<IAudioSessionEvents *>(this));
        AddRef();
    }
    else if (iid == __uuidof(IMMNotificationClient))
    {
        *object = static_cast<IMMNotificationClient *>(this);
        AddRef();
    }
    else if (iid == __uuidof(IAudioSessionEvents))
    {
        *object = static_cast<IAudioSessionEvents *>(this);
        AddRef();
    }
    else
    {
        return E_NOINTERFACE;
    }
    return S_OK;
}

//
// IMMNotificationClient
//
HRESULT AudioSessionControl::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR /*newDefaultDeviceId*/) noexcept
{
    if (flow == eRender && role == m_defaultDeviceRole)
    {
        //
        //  The default render device for our configuredf role was changed.  
        //
        //  If we're not in a stream switch already, we want to initiate a stream switch event.  
        //  We also we want to set the stream switch complete event.  That will signal the render thread that it's ok to re-initialize the
        //  audio renderer.
        //
        if (!m_inStreamSwitch)
        {
            m_inStreamSwitch = true;
            SetEvent(m_streamSwitchEvent);
        }
        SetEvent(m_streamSwitchCompleteEvent);
    }
    return S_OK;
}

//
// IAudioSessionEvents
//

HRESULT AudioSessionControl::OnSessionDisconnected(AudioSessionDisconnectReason disconnectReason) noexcept
{
    if (disconnectReason == DisconnectReasonDeviceRemoval)
    {
        //
        //  The stream was disconnected because the device we're rendering to was removed.
        //
        //  We want to reset the stream switch complete event (so we'll block when the HandleStreamSwitchEvent function
        //  waits until the default device changed event occurs).
        //
        //  Note that we _don't_ set the _StreamSwitchCompleteEvent - that will be set when the OnDefaultDeviceChanged event occurs.
        //
        m_inStreamSwitch = true;
        SetEvent(m_streamSwitchEvent);
    }
    if (disconnectReason == DisconnectReasonFormatChanged)
    {
        //
        //  The stream was disconnected because the format changed on our render device.
        //
        //  We want to flag that we're in a stream switch and then set the stream switch event (which breaks out of the renderer).  We also
        //  want to set the _StreamSwitchCompleteEvent because we're not going to see a default device changed event after this.
        //
        m_inStreamSwitch = true;
        SetEvent(m_streamSwitchEvent);
        SetEvent(m_streamSwitchCompleteEvent);
    }
    return S_OK;
}

} // namespace sound

#endif
