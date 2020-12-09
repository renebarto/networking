#include "GoogleTest.h"

#include "utility/Assert.h"

#include <string>

namespace utility {

class AssertTest : public ::testing::Test
{
public:
    static void AssertionHandler(bool expression, const char * expressionText, const char * file, int line, const char * func)
    {
        expression_ = expression;
        expressionText_ = expressionText;
        file_ = file;
        line_ = line;
        func_ = func;
    }
    static bool expression_;
    static const char * expressionText_;
    static const char * file_;
    static int line_;
    static const char * func_;

    void SetUp() override
    {
        expression_ = false;
        expressionText_ = nullptr;
        file_ = nullptr;
        line_ = 0;
        func_ = nullptr;
    }
};

bool AssertTest::expression_ = false;
const char * AssertTest::expressionText_ = nullptr;
const char * AssertTest::file_ = nullptr;
int AssertTest::line_ = 0;
const char * AssertTest::func_ = nullptr;

TEST_F(AssertTest, AssertSucceeds)
{
    AssertHandler oldHandler = SetAssertionHandler(AssertionHandler);
    EXPECT_FALSE(expression_);
    EXPECT_NULL(expressionText_);
    EXPECT_NULL(file_);
    EXPECT_EQ(0, line_);
    EXPECT_NULL(func_);
    bool b = true;
    ASSERT(b == true);
    EXPECT_FALSE(expression_);
    EXPECT_NULL(expressionText_);
    EXPECT_NULL(file_);
    EXPECT_EQ(0, line_);
    EXPECT_NULL(func_);
    SetAssertionHandler(oldHandler);
}

TEST_F(AssertTest, AssertFails)
{
    AssertHandler oldHandler = SetAssertionHandler(AssertionHandler);
    EXPECT_FALSE(expression_);
    EXPECT_NULL(expressionText_);
    EXPECT_NULL(file_);
    EXPECT_EQ(0, line_);
    EXPECT_NULL(func_);
    bool b = false;
    const char * file = __FILE__;
    const char * func = __func__;
    ASSERT(b == true); int line = __LINE__;
    EXPECT_FALSE(expression_);
    EXPECT_EQ(std::string("b == true"), expressionText_);
    EXPECT_EQ(std::string(file), file_);
    EXPECT_EQ(line, line_);
    EXPECT_EQ(std::string(func), func_);
    SetAssertionHandler(oldHandler);
}

} // namespace utility
