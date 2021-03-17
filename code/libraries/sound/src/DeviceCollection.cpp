//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : DeviceCollection.cpp
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
#include <functiondiscoverykeys.h>

#include "tracing/ScopedTracing.h"
#include "utility/StringFunctions.h"

#include "WindowsCOM.h"
#include "DeviceCollection.h"
#include "PropertyVariant.h"

struct IPropertyStore;

using PropertyStoreWrapper = InterfaceWrapper<IPropertyStore>;

namespace sound {

DeviceCollection::DeviceCollection()
    : m_isInitialized()
    , m_deviceEnumerator()
    , m_deviceCollection()
    , m_deviceNames()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

DeviceCollection::~DeviceCollection()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool DeviceCollection::Initialize()
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

    IMMDeviceEnumerator * deviceEnumerator;
    HRESULT hr = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to instantiate device enumerator: {,X:8}", hr);
        return result;
    }

    m_deviceEnumerator = std::make_unique<DeviceEnumeratorWrapper>(deviceEnumerator);
    hr = m_deviceEnumerator->Get()->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &m_deviceCollection);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to instantiate device collection: {,X:8}", hr);
        return result;
    }

    unsigned deviceCount {};
    hr = m_deviceCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to retrieve device count: {,X:8}", hr);
        return result;
    }

    for (unsigned i = 0 ; i < deviceCount; ++i)
    {
        std::string deviceName;
        std::string deviceID;
        result = GetDeviceName(i, deviceName, deviceID);
        if (result)
        {
            m_deviceNames.push_back(deviceName);
            m_deviceIDs.push_back(deviceID);
        }
        else
        {
            break;
        }
    }

    m_isInitialized = true;
    result = true;

    return result;
}

void DeviceCollection::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
        return;
        
    m_deviceEnumerator.reset();
    SafeRelease(&m_deviceCollection);
    m_deviceNames.clear();
    m_isInitialized = false;
}

std::unique_ptr<DeviceWrapper> DeviceCollection::GetDefaultDevice()
{
    std::unique_ptr<DeviceWrapper> result {};
    IMMDevice * device {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result.get()); });
    HRESULT hr = m_deviceEnumerator->Get()->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to instantiate device: {,X:8}", hr);

    }
    else
    {
        result = std::make_unique<DeviceWrapper>(device);
    }

    return result;
}

std::vector<std::string> DeviceCollection::GetDeviceNames()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    return m_deviceNames;
}

std::vector<std::string> DeviceCollection::GetDeviceIDs()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    return m_deviceIDs;
}

bool DeviceCollection::GetDeviceName(unsigned deviceIndex, std::string & deviceName, std::string & deviceID)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("deviceIndex={}", deviceIndex); }, 
        [&] () { return utility::FormatString("result={}", result); });

    IMMDevice * device {};

    HRESULT hr = m_deviceCollection->Item(deviceIndex, &device);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get device {}: {,X:8}", deviceIndex, hr);
        return result;
    }
    DeviceWrapper deviceWrapper(device);
    LPWSTR deviceIDWString {};

    hr = deviceWrapper.Get()->GetId(&deviceIDWString);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get device {} id: {,X:8}", deviceIndex, hr);
        return result;
    }
    deviceID = utility::WStringToString(deviceIDWString);
    CoTaskMemFree(deviceIDWString);

    IPropertyStore * propertyStore {};
    hr = deviceWrapper.Get()->OpenPropertyStore(STGM_READ, &propertyStore);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get device {} property store: {,X:8}", deviceIndex, hr);
        return result;
    }

    PropertyStoreWrapper propertyStoreWrapper(propertyStore);
    PropertyVariant friendlyName;
    hr = propertyStoreWrapper.Get()->GetValue(PKEY_Device_FriendlyName, &friendlyName.Get());
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to get device {} friendly name: {,X:8}", deviceIndex, hr);
        return result;
    }

    if (friendlyName.Get().vt == VT_LPWSTR)
    {
        deviceName = utility::WStringToString(friendlyName.Get().pwszVal);
    }
    else if (friendlyName.Get().vt == VT_LPSTR)
    {
        deviceName = friendlyName.Get().pszVal;
    }
    else
    {
        deviceName = "Unknown";
    }

    result = true;
    return result;
}

std::unique_ptr<DeviceWrapper> DeviceCollection::GetDevice(const std::string & deviceName)
{
    std::unique_ptr<DeviceWrapper> result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("deviceName={}", deviceName); }, 
        [&] () { return utility::FormatString("result={}", result.get()); });
    for (unsigned index = 0; index < m_deviceNames.size(); ++index)
    {
        if (m_deviceNames[index] == deviceName)
        {
            result = GetDevice(index);
            break;
        }
    }

    return result;
}

std::unique_ptr<DeviceWrapper> DeviceCollection::GetDeviceByID(const std::string & deviceID)
{
    std::unique_ptr<DeviceWrapper> result {};
    IMMDevice * device {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("deviceID={}", deviceID); }, 
        [&] () { return utility::FormatString("result={}", result.get()); });

    std::wstring id = utility::StringToWString(deviceID);
    HRESULT hr = m_deviceEnumerator->Get()->GetDevice(id.c_str(), &device);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to retrieve device {}: {,X:8}", deviceID, hr);
    }
    else
    {
        result = std::make_unique<DeviceWrapper>(device);
    }

    return result;
}

std::unique_ptr<DeviceWrapper> DeviceCollection::GetDevice(unsigned deviceIndex)
{
    std::unique_ptr<DeviceWrapper> result {};
    IMMDevice * device {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("deviceIndex={}", deviceIndex); }, 
        [&] () { return utility::FormatString("result={}", result.get()); });

    HRESULT hr = m_deviceCollection->Item(deviceIndex, &device);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to retrieve device {}: {,X:8}", deviceIndex, hr);
    }
    else
    {
        result = std::make_unique<DeviceWrapper>(device);
    }

    return result;
}

} // namespace sound

#endif
