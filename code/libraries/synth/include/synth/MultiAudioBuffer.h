#pragma once

#include "synth/AudioBuffer.h"

namespace synth {

class MultiAudioBuffer
{
private:
    using Buffer = std::vector<float>;
    using MultiBuffer = std::vector<Buffer>;
    MultiBuffer m_buffers;

public:
    MultiAudioBuffer(std::size_t count, std::size_t size)
        : m_buffers(count)
    {
        for (std::size_t index = 0; index < m_buffers.size(); ++index)
        {
            m_buffers[index] = Buffer(size);
        }
    }

    MultiBuffer::iterator begin() { return m_buffers.begin(); }
    MultiBuffer::iterator end() { return m_buffers.end(); }

    void Resize(std::size_t count, std::size_t size)
    {
        m_buffers = MultiBuffer(count);
        for (std::size_t index = 0; index < m_buffers.size(); ++index)
        {
            m_buffers[index] = Buffer(size);
        }
    }

    Buffer & operator[](std::size_t index)
    {
        return m_buffers[index];
    }
};

} // namespace synth
