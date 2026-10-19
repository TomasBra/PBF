//
// Created by brabect on 05.10.26.
//

#ifndef POSITIONBASEDFLUIDS_RENDEREDOBJECT_H
#define POSITIONBASEDFLUIDS_RENDEREDOBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>

#include "FrameBuffer.h"
#include "ShaderProgram.h"

class RenderedObject {
public:
    std::shared_ptr<ShaderProgram> renderShaderProgram; //Shader that will be used to render the mesh
    std::shared_ptr<std::vector<glm::vec4>> vercities; //vertices and its positions, must be vec4 due to allignment
    std::shared_ptr<std::vector<glm::vec2>> uvs;

    virtual void BufferData();
    virtual void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition);
    virtual void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer);
    virtual void Update(long double dt);

    unsigned int VBO, VAO, EBO = 0; //vertex buffer object handler, vertex array object handler, elements buffer object
    bool usingEBO = false;
    bool usingUVS = false;
    GLenum renderMode = GL_POINTS;

};


#endif //POSITIONBASEDFLUIDS_RENDEREDOBJECT_H
