//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SynthFactory.cpp
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"
#include "utility/StringFunctions.h"
#include "synth/SynthFactory.h"
#include "synth/TestSynth.h"

namespace synth {

SynthFactory::SynthFactory()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

SynthFactory::~SynthFactory()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

ISynthPtr SynthFactory::Create(const std::string & type)
{
    ISynthPtr result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("type={}", type); }, 
        [&] () { return utility::FormatString("result={}", result); });

    if (utility::IsEqualIgnoreCase("TestSynth", type))
    {
        result = std::make_shared<TestSynth>();
    }
    else
    {
        LogError(__FILE__, __LINE__, __func__, "Cannot create synth of type {}", type);
        return result;
    }

    return result;
}

} // namespace synth
