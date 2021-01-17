#include "GoogleTest.h"

#include "core/Regex.h"

namespace core {

TEST(RegexTest, VerifyMatchNoCapture)
{
    std::string regexString = "\\s*(\\w+)\\s*";
    std::string text = "  Hello  ";
    EXPECT_TRUE(VerifyMatch(text, regexString));
}

TEST(RegexTest, VerifyMatch)
{
    std::string regexString = "\\s*(\\w+)\\s*";
    std::string text = "  Hello  ";
    Matches matches;
    EXPECT_TRUE(VerifyMatch(text, matches, regexString));
    ASSERT_EQ(std::size_t {1}, matches.size());
    EXPECT_EQ("Hello", matches[0]);
}

TEST(RegexTest, VerifyMatches)
{
    std::string regexString = "\\s*(\\w+)\\s*";
    std::string text = "Hello World";
    std::vector<Matches> matches;
    EXPECT_TRUE(VerifyMatches(text, matches, regexString));
    ASSERT_EQ(std::size_t {2}, matches.size());
    ASSERT_EQ(std::size_t {1}, matches[0].size());
    EXPECT_EQ("Hello", matches[0][0]);
    ASSERT_EQ(std::size_t {1}, matches[1].size());
    EXPECT_EQ("World", matches[1][0]);
}

} // namespace core
