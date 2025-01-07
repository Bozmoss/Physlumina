#pragma once

#include <GL/glew.h>
#include "shader.hpp"

class Program {
private:
    GLint m_handle;

public:
    Program();

    Program(const Program& other) = delete;

    Program operator=(const Program& other) = delete;

    ~Program();

    GLint handle() const;

    void attachShader(const Shader& shader);

    void detachShader(const Shader& shader);

    void activate();
};
