//
// Created by brabect on 17.10.26.
//

#ifndef POSITIONBASEDFLUIDS_TEXTURE_H
#define POSITIONBASEDFLUIDS_TEXTURE_H
#include <filesystem>

#include "glad/glad.h"


class Texture {
public:
    GLenum attachmentType;
    GLint internalFormat;
    unsigned int handle;

    int width, height;

    Texture(int width, int height, GLenum _attachmentType, GLint _internalFormat, bool attachToBuffer = false);
    Texture(std::filesystem::path src, GLenum _attachmentType, GLint _internalFormat, bool attachToBuffer = false);
    void ResizeTextureAttachment(int _width, int _height);
    void Bind(GLenum unit);

private:

};


#endif //POSITIONBASEDFLUIDS_TEXTURE_H
