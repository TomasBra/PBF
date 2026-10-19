//
// Created by brabect on 05.10.26.
//

#include "RenderedObject.h"

//abstract method - to be implemented
void RenderedObject::BufferData() {
}

//abstract method - to be implemented
void RenderedObject::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer) {
    if (frameBuffer == nullptr) {
        throw std::runtime_error("Function RenderedObject::Render tries to access nullptr");
    }

    frameBuffer->Bind();
    this->Render(ViewMatrix, ProjectionMatrix, cameraPosition);
    frameBuffer->UnBind();
}

void RenderedObject::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition) {
}

void RenderedObject::Update(long double dt) {
}

