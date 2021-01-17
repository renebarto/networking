#include <core/Regex.h>

#include <regex>

namespace core {

bool VerifyMatch(const std::string & stringToVerify, const std::string & regexString)
{
    std::regex matcher(regexString.c_str());
    std::smatch matchedCaptures;
    return std::regex_match(stringToVerify, matchedCaptures, matcher);
}

bool VerifyMatch(const std::string & stringToVerify, Matches & matches, const std::string & regexString)
{
    std::regex matcher(regexString.c_str());
    std::smatch matchedCaptures;
    auto result = std::regex_match(stringToVerify, matchedCaptures, matcher);
    matches.clear();
    if (result)
    {
        for (std::size_t i = 1; i < matchedCaptures.size(); ++i)
        {
            matches.push_back(matchedCaptures[i]);
        }
    }
    return result;
}

bool VerifyMatches(const std::string & stringToVerify, std::vector<Matches> & matches, const std::string & regexString)
{
    std::regex matcher(regexString);
    auto begin = std::sregex_iterator(stringToVerify.begin(), stringToVerify.end(), matcher);
    auto end = std::sregex_iterator();
    matches.clear();
    for (auto it = begin; it != end; ++it)
    {
        std::smatch matchedCaptures = *it;
        if (matchedCaptures.size() > 1)
        {
            Matches localMatches;
            for (std::size_t i = 1; i < matchedCaptures.size(); ++i)
            {
                localMatches.push_back(matchedCaptures[i]);
            }
            matches.push_back(localMatches);
        }
    }
    return !matches.empty();
}

} // namespace core
