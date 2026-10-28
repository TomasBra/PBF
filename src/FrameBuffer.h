//
// Created by brabect on 14.10.26.
//

#ifndef POSITIONBASEDFLUIDS_FRAMEBUFFER_H
#define POSITIONBASEDFLUIDS_FRAMEBUFFER_H
#include <memory>

#include "Texture.h"

#define COLOR_TEXTURE 1
#define DEPTH_TEXTURE 2
#define COLOR_AND_DEPTH_TEXTURE 3
#define HIGH_R_COLOR_TEXTURE 4
#define HIGH_R_COLOR_AND_DEPTH_TEXTURE 6
#define HIGH_RG_COLOR_TEXTURE 8
#define HIGH_RG_COLOR_AND_DEPTH_TEXTURE 10

class FrameBuffer {

    public:
    FrameBuffer(int width, int height, int _textureAttachments = COLOR_TEXTURE);
    void Bind(); //read and write
    void UnBind(); //read and write
    void ResizeTextures(int width, int height);

    unsigned int fbo;
    int textureAttachments;
    std::unique_ptr<Texture> colorTexture, depthTexture, stencilTexture;


    private:
    void defaultSettings();
};


#endif //POSITIONBASEDFLUIDS_FRAMEBUFFER_H
