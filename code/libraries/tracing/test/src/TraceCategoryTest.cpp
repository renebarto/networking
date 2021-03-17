#include "GoogleTest.h"

#include "tracing/TraceCategory.h"

#include "utility/Serialization.h"

namespace tracing {

TEST(TraceCategoryTest, Serialize)
{
    EXPECT_EQ("SscfBeg",         serialization::Serialize(TraceCategory::SscfBegin));
    EXPECT_EQ("SscfEnd",         serialization::Serialize(TraceCategory::SscfEnd));
    EXPECT_EQ("SscfEv",          serialization::Serialize(TraceCategory::SscfEvent));
    EXPECT_EQ("SscfLib",         serialization::Serialize(TraceCategory::SscfLib));
    EXPECT_EQ("Drvr",            serialization::Serialize(TraceCategory::DriverLayer));
    EXPECT_EQ("Phys",            serialization::Serialize(TraceCategory::Physical));
    EXPECT_EQ("Appl",            serialization::Serialize(TraceCategory::ApplicationLayer));
    EXPECT_EQ("Ctrl",            serialization::Serialize(TraceCategory::ControlLayer));
    EXPECT_EQ("PlIn",            serialization::Serialize(TraceCategory::PipelineIn));
    EXPECT_EQ("Enter",           serialization::Serialize(TraceCategory::FunctionEnter));
    EXPECT_EQ("Leave",           serialization::Serialize(TraceCategory::FunctionLeave));
    EXPECT_EQ("NmtDbt",          serialization::Serialize(TraceCategory::CanNmtDbt));
    EXPECT_EQ("Info",            serialization::Serialize(TraceCategory::Information));
    EXPECT_EQ("Log",             serialization::Serialize(TraceCategory::Log));
    EXPECT_EQ("Interrupt",       serialization::Serialize(TraceCategory::Interrupt));
    EXPECT_EQ("BistPostInfo",    serialization::Serialize(TraceCategory::BistPostInfo));
    EXPECT_EQ("SscfCan",         serialization::Serialize(TraceCategory::SscfCan));
    EXPECT_EQ("BitfieldDriver",  serialization::Serialize(TraceCategory::BitfieldDriver));
    EXPECT_EQ("StartupShutdown", serialization::Serialize(TraceCategory::StartupShutdown));
    EXPECT_EQ("Cat00001000",     serialization::Serialize(TraceCategory::Cat00001000));
    EXPECT_EQ("ResultFlow",      serialization::Serialize(TraceCategory::ResultFlow));
    EXPECT_EQ("Cat00000400",     serialization::Serialize(TraceCategory::Cat00000400));
    EXPECT_EQ("Cat00000200",     serialization::Serialize(TraceCategory::Cat00000200));
    EXPECT_EQ("Cat00000100",     serialization::Serialize(TraceCategory::Cat00000100));
    EXPECT_EQ("Cat00000080",     serialization::Serialize(TraceCategory::Cat00000080));
    EXPECT_EQ("Cat00000040",     serialization::Serialize(TraceCategory::Cat00000040));
    EXPECT_EQ("Cat00000020",     serialization::Serialize(TraceCategory::Cat00000020));
    EXPECT_EQ("Cat00000010",     serialization::Serialize(TraceCategory::Cat00000010));
    EXPECT_EQ("Cat00000008",     serialization::Serialize(TraceCategory::Cat00000008));
    EXPECT_EQ("Data",            serialization::Serialize(TraceCategory::Data));
    EXPECT_EQ("Flow",            serialization::Serialize(TraceCategory::Flow));
    EXPECT_EQ("Debug",           serialization::Serialize(TraceCategory::Debug));
}

TEST(TraceCategoryTest, EnumTraceCategories)
{
    std::vector<TraceCategory> expected = { 
        TraceCategory::Debug,
        TraceCategory::Flow,
        TraceCategory::Data,
        TraceCategory::Cat00000008,
        TraceCategory::Cat00000010,
        TraceCategory::Cat00000020,
        TraceCategory::Cat00000040,
        TraceCategory::Cat00000080,
        TraceCategory::Cat00000100,
        TraceCategory::Cat00000200,
        TraceCategory::Cat00000400,
        TraceCategory::ResultFlow,
        TraceCategory::Cat00001000,
        TraceCategory::StartupShutdown,
        TraceCategory::BitfieldDriver,
        TraceCategory::SscfCan,
        TraceCategory::BistPostInfo,
        TraceCategory::Interrupt,
        TraceCategory::Log,
        TraceCategory::Information,
        TraceCategory::CanNmtDbt,
        TraceCategory::FunctionLeave,
        TraceCategory::FunctionEnter,
        TraceCategory::PipelineIn,
        TraceCategory::ControlLayer,
        TraceCategory::ApplicationLayer,
        TraceCategory::Physical,
        TraceCategory::DriverLayer,
        TraceCategory::SscfLib,
        TraceCategory::SscfEvent,
        TraceCategory::SscfEnd,
        TraceCategory::SscfBegin,
    };
    EXPECT_EQ(expected, EnumTraceCategories());
}

} // namespace tracing
