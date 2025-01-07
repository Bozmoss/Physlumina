#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>

#include "programH.hpp"
#include "object.hpp"
#include "material.hpp"

class UniformBuffer {
private:
    GLuint m_handle;
    unsigned m_number;
public:
    UniformBuffer();

    UniformBuffer(const UniformBuffer& other) = delete;

    UniformBuffer operator=(const UniformBuffer& other) = delete;

    ~UniformBuffer();

    GLuint handle() const;

    void fill(const std::vector<float>& data, Program& p, const GLchar* name) {
        m_number = data.size();
        glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * m_number, data.data(), GL_DYNAMIC_DRAW);
        GLuint blockIndex = glGetUniformBlockIndex(p.handle(), name);
        if (blockIndex == -1) {
            std::cout << "could not bind uniform block: " << name << std::endl;
        }
        glUniformBlockBinding(p.handle(), blockIndex, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_handle);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};
