#pragma once

namespace synth {

class AudioBuffer
{
private:
    using Buffer = std::vector<float>;
    Buffer m_buffer;

public:
    AudioBuffer()
        : m_buffer()
    {}
    explicit AudioBuffer(std::size_t size)
        : m_buffer(size)
    {}

    Buffer::iterator begin() { return m_buffer.begin(); }
    Buffer::iterator end() { return m_buffer.end(); }
    Buffer::const_iterator begin() const { return m_buffer.begin(); }
    Buffer::const_iterator end() const { return m_buffer.end(); }
    std::size_t size() const { return m_buffer.size(); }

    void resize(std::size_t size) { m_buffer = Buffer(size); }

    float & operator[](std::size_t index) { return m_buffer[index]; }
    const float & operator[] (std::size_t index) const { return m_buffer[index]; }
};

} // namespace synth
