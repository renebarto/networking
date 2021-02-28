//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : IMidiAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include "utility/StringFunctions.h"

namespace midi {

class MidiCapabilities
{
private:
    std::string m_name;

public:
    MidiCapabilities()
        : m_name()
    {        
    }
    explicit MidiCapabilities(const std::string & name)
        : m_name(name)
    {        
    }
    MidiCapabilities(const std::wstring & name)
        : m_name(utility::WStringToString(name))
    {        
    }

    std::string Name() const { return m_name; }
};

} // namespace midi
