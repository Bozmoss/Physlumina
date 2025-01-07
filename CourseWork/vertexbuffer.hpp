#pragma once

#include <vector>
#include <GL/glew.h>

class VertexBuffer {
private:
    GLuint m_handle;

public:
    VertexBuffer();

    VertexBuffer(const VertexBuffer& other) = delete;

    VertexBuffer operator=(const VertexBuffer& other) = delete;

    ~VertexBuffer();

    GLuint handle() const;

    void fill(const std::vector<GLfloat>& data);
};
