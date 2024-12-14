#include "uniformbuffer.hpp"

UniformBuffer::UniformBuffer() : m_number{ 0 } {
    glGenBuffers(1, &m_handle);
    if (m_handle > 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
        glBufferData(GL_UNIFORM_BUFFER, size(), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

UniformBuffer::~UniformBuffer() {
    if (m_handle > 0) {
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }
}

size_t UniformBuffer::size() const {
    return sizeof(Material) * 100 + sizeof(ObjectData) * 100;
}

GLuint UniformBuffer::handle() const {
    return m_handle;
}