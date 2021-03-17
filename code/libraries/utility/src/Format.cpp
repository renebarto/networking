//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : Format.cpp
//
// Namespace   : utility
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "utility/Format.h"

namespace utility {

static std::string s_formatError;
static bool s_haveFormatError;
const std::string & GetFormatError() { return s_formatError; }
bool HaveFormatError() { return s_haveFormatError; }
void ResetFormatError() { s_haveFormatError = false; s_formatError = {}; }
void SetFormatError(const std::string & message) { s_haveFormatError = true; s_formatError = message; }

} // namespace utility
