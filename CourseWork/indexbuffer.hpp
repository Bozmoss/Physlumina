#pragma once

#include <GL/glew.h>
#include <vector>

class IndexBuffer {
private:
    GLuint m_handle;
    unsigned m_number;

public:
    IndexBuffer();

    IndexBuffer(const IndexBuffer& other) = delete;

    IndexBuffer operator=(const IndexBuffer& other) = delete;

    ~IndexBuffer();

    GLuint handle() const;

    void fill(const std::vector<GLuint>& indices);

    unsigned number() const;
};
