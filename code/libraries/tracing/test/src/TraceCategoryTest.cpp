#include "GoogleTest.h"

#include "tracing/TraceCategory.h"

#include "utility/Serialization.h"

namespace tracing {

TEST(TraceCategoryTest, Serialize)
{
    EXPECT_EQ("Enter", serialization::Serialize(TraceCategory::FunctionEnter));
    EXPECT_EQ("Leave", serialization::Serialize(TraceCategory::FunctionLeave));
    EXPECT_EQ("Start", serialization::Serialize(TraceCategory::Startup));
    EXPECT_EQ("Shtdn", serialization::Serialize(TraceCategory::Shutdown));
    EXPECT_EQ("Log  ", serialization::Serialize(TraceCategory::Log));
    EXPECT_EQ("Messg", serialization::Serialize(TraceCategory::Message));
    EXPECT_EQ("Data ", serialization::Serialize(TraceCategory::Data));
    EXPECT_EQ("Debug", serialization::Serialize(TraceCategory::Debug));
}

TEST(TraceCategoryTest, EnumTraceCategories)
{
    std::vector<TraceCategory> expected = { 
        TraceCategory::FunctionEnter,
        TraceCategory::FunctionLeave,
        TraceCategory::Startup,
        TraceCategory::Shutdown,
        TraceCategory::Log,
        TraceCategory::Message,
        TraceCategory::Data,
        TraceCategory::Debug,
    };
    EXPECT_EQ(expected, EnumTraceCategories());
}

} // namespace tracing
