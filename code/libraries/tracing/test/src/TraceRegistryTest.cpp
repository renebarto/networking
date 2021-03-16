#include "GoogleTest.h"

#include "tracing/TraceRegistry.h"

namespace tracing {

TEST(TraceRegistryTest, FindCompilationUnit)
{
    auto registry = ::TraceRegistry();
    auto entry = registry.FindCompilationUnit("code/libraries/tracing/src/TraceRegistry.cpp");
    EXPECT_NOT_NULL(entry);
}

TEST(TraceRegistryTest, TraceCategoryIsEnabledForRegisteredCompilationUnitWithOverruledTraceFilter)
{
    auto registry = ::TraceRegistry();
    registry.SetTraceFilter("code/libraries/tracing/src/TraceRegistry.cpp", TraceCategory::FunctionEnter | TraceCategory::FunctionLeave);
    EXPECT_TRUE(registry.IsTraceCategoryEnabled("code/libraries/tracing/src/TraceRegistry.cpp", TraceCategory::FunctionEnter));
    registry.SetTraceFilter("code/libraries/tracing/src/TraceRegistry.cpp", TraceCategory::None);
}

TEST(TraceRegistryTest, TraceCategoryNotEnabledForRegisteredCompilationUnitWithDefaultTraceFilter)
{
    auto registry = ::TraceRegistry();
    EXPECT_FALSE(registry.IsTraceCategoryEnabled("code/libraries/tracing/src/TraceRegistry.cpp", TraceCategory::FunctionEnter));
}

TEST(TraceRegistryTest, TraceCategoryNotEnabledForUnregisteredCompilationUnit)
{
    auto registry = ::TraceRegistry();
    
    EXPECT_FALSE(registry.IsTraceCategoryEnabled("code/libraries/tracing/src/MyNonExistingFile.cpp", TraceCategory::FunctionEnter));
}

TEST(TraceRegistryTest, TraceCategoryIsEnabledForUnregisteredCompilationUnitWhenDefaultEnables)
{
    auto registry = ::TraceRegistry();
    
    registry.SetDefaultTraceFilter(TraceCategory::FunctionEnter | TraceCategory::FunctionLeave);
    EXPECT_TRUE(registry.IsTraceCategoryEnabled("code/libraries/tracing/src/MyNonExistingFile.cpp", TraceCategory::FunctionEnter));
    registry.SetDefaultTraceFilter(TraceCategory::None);
}

} // namespace tracing
