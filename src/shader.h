#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setBool2(const std::string &name, bool v1, bool v2) const;
    void setBool3(const std::string &name, bool v1, bool v2, bool v3) const;
    void setBool4(const std::string &name, bool v1, bool v2, bool v3, bool v4) const;

    void setInt(const std::string &name, int value) const;
    void setInt2(const std::string &name, int v1, int v2) const;
    void setInt3(const std::string &name, int v1, int v2, int v3) const;
    void setInt4(const std::string &name, int v1, int v2, int v3, int v4) const;

    void setFloat(const std::string &name, float value) const;
    void setFloat2(const std::string &name, float v1, float v2) const;
    void setFloat3(const std::string &name, float v1, float v2, float v3) const;
    void setFloat4(const std::string &name, float v1, float v2, float v3, float v4) const;

    void setFloatArray(const std::string &name, int size, const float* data) const;
    void setFloat3Array(const std::string &name, int size, const float* data) const;

    void setMat3(const std::string &name, glm::mat3 mat) const;

    unsigned int GetID();
};

#endif
