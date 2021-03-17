#pragma once

#include <string>
#include "json/json.h"

namespace application {

class TraceDatabase
{
private:
    std::shared_ptr<json::Object> m_databaseObject;
public: 
    TraceDatabase();

    bool Load(const std::string & path);
    bool Save(const std::string & path);
};

} // namespace application