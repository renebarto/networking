//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : RenderBuffer.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"

#include "RenderBuffer.h"

namespace sound {

RenderBuffer::RenderBuffer(std::size_t bufferSize)
    : m_bufferSize(bufferSize)
    , m_buffer(bufferSize)
    , m_isClaimed()
    , m_isUsed()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

RenderBuffer::~RenderBuffer()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

std::size_t RenderBuffer::BufferSize() const
{
    return m_bufferSize;
}

bool RenderBuffer::IsUsed() const
{
    return m_isUsed;
}

std::vector<std::uint8_t> & RenderBuffer::Claim()
{
    m_isClaimed = true;
    return m_buffer;
}

void RenderBuffer::Unclaim()
{
    if (m_isClaimed)
    {
        m_isClaimed = false;
        m_isUsed = false;
    }
}

void RenderBuffer::Release()
{
    if (m_isClaimed)
    {
        m_isClaimed = false;
        m_isUsed = true;
    }
}

const std::vector<std::uint8_t> & RenderBuffer::Buffer() const
{
    return m_buffer;
}

} // namespace sound

#endif
