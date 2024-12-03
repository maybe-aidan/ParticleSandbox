#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "stb_image.h"

class Texture2D {
public:
    Texture2D(const std::string texture_path, bool hasAlphaChannel);
    unsigned int GetID();

private:
    unsigned int ID;
};

#endif