//
// Created by brabect on 03.10.26.
//

#include "glad/glad.h"
#include "Mesh.h"

#include "Utils.h"


glm::mat4 Mesh::GetModelMatrix() {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position[0], position[1], position[2]));
    glm::mat4 sc = glm::scale (glm::mat4(1.0f), glm::vec3(scale[0], scale[1], scale[2]));

    glm::mat4 rotateX = glm::rotate (glm::mat4(1.0f), rotation[0], glm::vec3(1, 0, 0));
    glm::mat4 rotateY = glm::rotate (glm::mat4(1.0f), rotation[1], glm::vec3(0, 1, 0));
    glm::mat4 rotateZ = glm::rotate (glm::mat4(1.0f), rotation[2], glm::vec3(0, 0, 1));

    glm::mat4 rot = rotateX * rotateY * rotateZ;

    return translation * sc * rot;
}






//USES ELEMENT BUFFER OBJECT
Mesh::Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<unsigned int>> _faces, GLenum _renderMode){
    VAO = 0;
    VBO = 0;
    EBO = 0;

    this->vercities = _vertices;
    this->faces = _faces;
    this->renderMode = _renderMode;
    this->usingEBO = true;


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    BufferData();
}

Mesh::Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<unsigned int>> _faces,
    std::shared_ptr<std::vector<glm::vec2>> _uvs, GLenum _renderMode) {
    VAO = 0;
    VBO = 0;
    EBO = 0;

    this->vercities = _vertices;
    this->faces = _faces;
    this->uvs = _uvs;

    this->renderMode = _renderMode;
    this->usingEBO = true;
    this->usingUVS = true;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    BufferData();
}

//USES ONLY ARRAY BUFFER OBJECT
Mesh::Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, GLenum _renderMode){
    VAO = 0;
    VBO = 0;

    this->vercities = _vertices;
    this->renderMode = _renderMode;
    this->usingEBO = false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    BufferData();
}

Mesh::Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<glm::vec2>> _uvs,
    GLenum _renderMode) {

    VAO = 0;
    VBO = 0;

    this->vercities = _vertices;
    this->uvs = _uvs;
    this->renderMode = _renderMode;
    this->usingEBO = false;
    this->usingUVS = true;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    BufferData();

}


void Mesh::BufferData() {
    if (this->vercities == nullptr or (usingEBO and this->faces == nullptr)) {
        throw std::runtime_error("Function Mesh::BufferData tries to access nullptr");
    }
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vercities->size()*sizeof(glm::vec4), vercities->data(), GL_STATIC_DRAW);

    if (usingEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces->size()*sizeof(unsigned int), faces->data(), GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //3D - position
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); //VAO must be unbinded first otherwise it will affect EBO!
    glBindVertexArray(0);
}




void Mesh::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer) {
    frameBuffer->Bind();
    this->Render(ViewMatrix, ProjectionMatrix, cameraPosition);
    frameBuffer->UnBind();
}





void Mesh::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition) {
    if (renderShaderProgram == nullptr or (usingEBO and this->faces == nullptr) or (not usingEBO and this->vercities == nullptr)) {
        throw std::runtime_error("Function Mesh::Render tries to access nullptr");
    }

    this->renderShaderProgram->Use();

    /*if (GetModelMatrix() != glm::mat4(1))
        renderShaderProgram->SetUniformMatrix4("Model", GetModelMatrix()); //setting model matrix
    if (ViewMatrix != glm::mat4(1))
        renderShaderProgram->SetUniformMatrix4("View", ViewMatrix); //setting model matrix
    if (ProjectionMatrix != glm::mat4(1))
        renderShaderProgram->SetUniformMatrix4("Projection", ProjectionMatrix); //setting model matrix*/

    renderShaderProgram->SetUniformMatrix4("Model", GetModelMatrix()); //setting model matrix
    renderShaderProgram->SetUniformMatrix4("View", ViewMatrix); //setting model matrix
    renderShaderProgram->SetUniformMatrix4("Projection", ProjectionMatrix); //setting model matrix

    //binding buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if (usingEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(this->renderMode,      // Primitive type
            faces->size(),                 // Number of indices to render (2 triangles * 3 indices each)
            GL_UNSIGNED_INT,   // Data type of the indices array
            0                  // Offset pointer in the element buffer (0 starts at the beginning)
        );
    }else {
        glDrawArrays(this->renderMode, 0, vercities->size());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0); //VAO must be unbinded first otherwise it will affect EBO!
    glBindVertexArray(0);
}




void Mesh::Update(long double dt) {
    RenderedObject::Update(dt);
}




