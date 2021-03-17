//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2009 Koninklijke Philips Electronics N.V.
//
// File        : ErrorCode.cpp
//
// Namespace   : osal
//
// Class       : ErrorCode
//
// Description :
//
//------------------------------------------------------------------------------

#include "osal/ErrorCode.h"

#include <cstring>

namespace osal {

std::string ErrorCode::Description() const
{
    return strerror(m_errorCode);
}

} // namespace osal
