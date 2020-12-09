#pragma once

#include <string>

namespace osal {

std::wstring StringToWString(const std::string & value);
std::string WStringToString(const std::wstring & value);

} // namespace osal