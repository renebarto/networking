//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : AudioSessionControl.h
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

#include <AudioPolicy.h>

struct IMMDeviceEnumerator;
struct IAudioClient;
struct IAudioSessionControl;

using DeviceEnumeratorWrapper = InterfaceWrapper<IMMDeviceEnumerator>;
using AudioSessionControlWrapper = InterfaceWrapper<IAudioSessionControl>;

namespace sound {

class AudioSessionControl
    : IMMNotificationClient
    , IAudioSessionEvents
{
private:
    unsigned long m_refCount;
    bool m_isInitialized;
    IAudioClient * m_audioClient;
    ERole m_defaultDeviceRole; // EMultiMedia
    HANDLE m_streamSwitchEvent;
    HANDLE m_streamSwitchCompleteEvent;
    std::unique_ptr<AudioSessionControlWrapper> m_audioSessionControl;
    std::unique_ptr<DeviceEnumeratorWrapper> m_deviceEnumerator;
    bool m_inStreamSwitch;

public:
    AudioSessionControl(IAudioClient * audioClient);
    virtual ~AudioSessionControl();
    
    //
    //  IUnknown
    //
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID iid, void **object);

    //
    // IMMNotificationClient
    //
    STDMETHOD(OnDefaultDeviceChanged) (EDataFlow flow, ERole role, LPCWSTR newDefaultDeviceId);
    STDMETHOD(OnDeviceAdded) (LPCWSTR /*DeviceId*/) { return S_OK; };
    STDMETHOD(OnDeviceRemoved) (LPCWSTR /*DeviceId(*/) { return S_OK; };
    STDMETHOD(OnDeviceStateChanged) (LPCWSTR /*DeviceId*/, DWORD /*NewState*/) { return S_OK; }
    STDMETHOD(OnPropertyValueChanged) (LPCWSTR /*DeviceId*/, const PROPERTYKEY /*Key*/){return S_OK; };

    //
    // IAudioSessionEvents
    //

    STDMETHOD(OnChannelVolumeChanged) (DWORD /*ChannelCount*/, float /*NewChannelVolumes*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnDisplayNameChanged) (LPCWSTR /*NewDisplayName*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnGroupingParamChanged) (LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) {return S_OK; };
    STDMETHOD(OnIconPathChanged) (LPCWSTR /*NewIconPath*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnSessionDisconnected) (AudioSessionDisconnectReason disconnectReason);
    STDMETHOD(OnSimpleVolumeChanged) (float /*NewSimpleVolume*/, BOOL /*NewMute*/, LPCGUID /*EventContext*/) { return S_OK; }
    STDMETHOD(OnStateChanged) (AudioSessionState /*NewState*/) { return S_OK; };

    bool Initialize();
    void Uninitialize();
};

} // namespace sound

#endif