//
// Created by brabect on 14.10.26.
//
#include "glad/glad.h"
#include "FrameBuffer.h"

#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, int _textureAttachments): textureAttachments(_textureAttachments) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    defaultSettings();

    if (_textureAttachments == COLOR_TEXTURE or _textureAttachments == COLOR_AND_DEPTH_TEXTURE) {
        this->colorTexture = std::make_unique<Texture>(width, height, GL_COLOR_ATTACHMENT0, true);
    }

    if (_textureAttachments == DEPTH_TEXTURE or _textureAttachments == COLOR_AND_DEPTH_TEXTURE) {
        this->depthTexture = std::make_unique<Texture>(width, height, GL_DEPTH_ATTACHMENT, true);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);
}

void FrameBuffer::UnBind() {
    glDisable(GL_DEPTH_TEST);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBuffer::ResizeTextures(int width, int height) {
    if (colorTexture != nullptr) {
        colorTexture->ResizeTextureAttachment(width, height);
    }

    if (depthTexture != nullptr) {
        depthTexture->ResizeTextureAttachment(width, height);
    }

    if (stencilTexture != nullptr) {
        stencilTexture->ResizeTextureAttachment(width, height);
    }
}

void FrameBuffer::defaultSettings() {
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_PROGRAM_POINT_SIZE );
    glLineWidth(4.0f);
}
