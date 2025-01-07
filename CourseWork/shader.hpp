#pragma once

#include <GL/glew.h>
#include "filehandler.hpp"

class Shader {
private:
    GLint m_handle;
    bool m_inError;
    std::string m_error;

public:
    enum Type {
        VERTEX,
        FRAGMENT
    };

    Shader(Type type, const char* fileName);

    Shader(const Shader& other) = delete;

    Shader operator=(const Shader& other) = delete;

    ~Shader();

    GLint handle() const;

    bool inError() const;

    const std::string& error() const;
};
