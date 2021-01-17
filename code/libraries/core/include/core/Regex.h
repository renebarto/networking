#pragma once

#include <string>
#include <vector>

namespace core {

using Matches = std::vector<std::string>;
bool VerifyMatch(const std::string & stringToVerify, const std::string & regexString);
bool VerifyMatch(const std::string & stringToVerify, Matches & matches, const std::string & regexString);
bool VerifyMatches(const std::string & stringToVerify, std::vector<Matches> & matches, const std::string & regexString);

} // namespace core
