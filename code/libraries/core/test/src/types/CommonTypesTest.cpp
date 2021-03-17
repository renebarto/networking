//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : CommonTypesTest.cpp
//
// Namespace   : core::types
//
// Class       : -
//
// Description : 
//  Test for common type serialization
//
//------------------------------------------------------------------------------

#include "GoogleTest.h"

#include "core/types/CommonTypes.h"

#include <map>
#include <string>

namespace core {
namespace types {

TEST(CommonTypesTest, StreamLoggingSeverity)
{
    auto map = std::map<types::LoggingSeverity, std::string>
    {
        { types::LoggingSeverity::Fatal, "Fatal" },
        { types::LoggingSeverity::Error, "Error" },
        { types::LoggingSeverity::Warning, "Warning" },
        { types::LoggingSeverity::Info, "Info" },
    };
    for (auto const & item : map)
    {
        EXPECT_EQ(item.second, serialization::Serialize(item.first));
    }
}

TEST(CommonTypesTest, StreamTraceChannel)
{
    auto map = std::map<types::XRayChannel, std::string>
    {
        { types::XRayChannel::FRONTAL, "Frontal" },
        { types::XRayChannel::LATERAL, "Lateral" },
    };
    for (auto const & item : map)
    {
        EXPECT_EQ(item.second, serialization::Serialize(item.first));
    }
}

} // namespace serialization
} // namespace core
