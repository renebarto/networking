#include "application/TraceDatabase.h"

#include <fstream>

#include "json/json.h"
#include "tracing/ScopedTracing.h"
#include "tracing/TraceRegistry.h"

namespace application {

static std::shared_ptr<json::Object> CreateNewDatabaseObject()
{
    auto result = std::make_shared<json::Object>();

    auto defaultTraceFilterValue = std::make_shared<json::Number>(std::uint32_t {0x0});
    result->AddPair("default", defaultTraceFilterValue);
    return result;
}

TraceDatabase::TraceDatabase()
    : m_databaseObject()
{
}

bool TraceDatabase::Load(const std::string & path)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("path={}", path); }, 
        [&] () { return utility::FormatString("result={}", result); });

    std::ifstream databaseFile(path);
    if (!databaseFile.good())
        return result;

    auto parsedObject = json::Parse(databaseFile);
    if (parsedObject == nullptr)
        m_databaseObject = CreateNewDatabaseObject();
    else
        m_databaseObject = ToObject(parsedObject);

    auto defaultTraceFilterEntry = m_databaseObject->Find("default");
    if (defaultTraceFilterEntry == nullptr)
        return result;


    result = true;
    return result;
}

bool TraceDatabase::Save(const std::string & path)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("path={}", path); }, 
        [&] () { return utility::FormatString("result={}", result); });

    std::ofstream databaseFile(path, std::ios_base::out | std::ios_base::trunc);
    if (!databaseFile.good())
        return result;

    databaseFile << json::Serialize(*m_databaseObject);
    if (!databaseFile.good())
        return result;

    result = true;
    return result;
}

} // namespace application