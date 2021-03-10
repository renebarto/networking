//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : RenderBuffer.h
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#if defined(PLATFORM_WINDOWS)

namespace sound {

class RenderBuffer
{
private:
    std::size_t m_bufferSize;
    std::vector<std::uint8_t> m_buffer;
    bool m_isClaimed;
    bool m_isUsed;

public:
    RenderBuffer(std::size_t bufferSize);
    ~RenderBuffer();

    std::size_t BufferSize() const;
    bool IsUsed() const;
    std::vector<std::uint8_t> & Claim();
    void Unclaim();
    void Release();
    const std::vector<std::uint8_t> & Buffer() const;
};

} // namespace sound

#endif