#include "programH.hpp"
#include <iostream>
Program::Program() {
    m_handle = glCreateProgram();
}
Program::~Program() {
    if (m_handle > 0) {
        glDeleteProgram(m_handle);
        m_handle = 0;
    }
}
GLint Program::handle() const {
    return m_handle;
}
void Program::attachShader(const Shader& shader) {
    glAttachShader(m_handle, shader.handle());
}
void Program::detachShader(const Shader& shader) {
    glDetachShader(m_handle, shader.handle());
}
void Program::activate() {
    glLinkProgram(m_handle);
    GLint status;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        GLsizei length = 0;
        glGetProgramInfoLog(m_handle, bufferSize, &length, buffer);
    }
    glUseProgram(m_handle);
}
