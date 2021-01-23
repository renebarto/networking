#include "GoogleTest.h"

#include "tracing/LogCategory.h"

#include "utility/Serialization.h"

namespace tracing {

TEST(LogCategoryTest, Serialize)
{
    EXPECT_EQ("Info ", serialization::Serialize(LogCategory::Information));
    EXPECT_EQ("Warn ", serialization::Serialize(LogCategory::Warning));
    EXPECT_EQ("Error", serialization::Serialize(LogCategory::Error));
    EXPECT_EQ("Fatal", serialization::Serialize(LogCategory::Fatal));
}

TEST(LogCategoryTest, EnumLogCategories)
{
    std::vector<LogCategory> expected = { 
        LogCategory::Information,
        LogCategory::Warning,
        LogCategory::Error,
        LogCategory::Fatal,
    };
    EXPECT_EQ(expected, EnumLogCategories());
}

} // namespace tracing
