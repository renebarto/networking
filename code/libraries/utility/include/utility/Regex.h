//---------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : Regex.h
//
// Namespace   : utility
//
// Class       : -
//
// Description :
//  Regular expression matches
//  Needed as long as older WindRiver compiler is used
//
//---------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

namespace utility {

using Matches = std::vector<std::string>;
bool VerifyMatch(const std::string & stringToVerify, const std::string & regexString);
bool VerifyMatch(const std::string & stringToVerify, Matches & matches, const std::string & regexString);
bool VerifyMatches(const std::string & stringToVerify, std::vector<Matches> & matches, const std::string & regexString);

} // namespace utility
