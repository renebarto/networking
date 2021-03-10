//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : FakeSoundAPI.cpp
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "FakeSoundAPI.h"

namespace sound {

FakeSoundAPI::FakeSoundAPI()
    : m_isInitialized()
{
}

FakeSoundAPI::~FakeSoundAPI()
{
    Uninitialize();
}

bool FakeSoundAPI::Initialize(const std::string & /*deviceName*/)
{
    if (m_isInitialized)
        return true;

    m_isInitialized = true;
    return true;
}

void FakeSoundAPI::Uninitialize()
{
    m_isInitialized = false;
}

bool FakeSoundAPI::IsInitialized()
{
    return m_isInitialized;
}

bool FakeSoundAPI::Start(IAudioSource * /*audioSource*/)
{
    return true;
}

void FakeSoundAPI::Stop()
{

}

} // namespace sound
