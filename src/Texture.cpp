//
// Created by brabect on 17.10.26.
//

#include "Texture.h"
#include <stdexcept>

Texture::Texture(int width, int height, GLenum attachmentType, bool attachToBuffer): attachmentType(attachmentType), width(width), height(height) {
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    if (attachmentType == GL_COLOR_ATTACHMENT0) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }else if (attachmentType == GL_DEPTH_ATTACHMENT) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    }else if (attachmentType == GL_STENCIL_ATTACHMENT) {
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Stencil attachments are not supported yet");
    }

    if (attachToBuffer) {
        //mapping to bounded buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, handle, 0);
    }


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::ResizeTextureAttachment(int _width, int _height) {
    if (width == _width && height == _height)
        return;

    width = _width;
    height = _height;

    glBindTexture(GL_TEXTURE_2D, handle);

    if (attachmentType == GL_COLOR_ATTACHMENT0) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }else if (attachmentType == GL_DEPTH_ATTACHMENT) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    }else if (attachmentType == GL_STENCIL_ATTACHMENT) {
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Stencil attachments are not supported yet");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(GLenum unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, handle);
}
