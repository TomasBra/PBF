//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_MESH_H
#define POSITIONBASEDFLUIDS_MESH_H
#include <memory>
#include <vector>

#include "RenderedObject.h"
#include "ShaderProgram.h"


//triangular mesh
class Mesh: public RenderedObject {

public:
    //model matrix and other later...
    glm::vec3 position = {0,0,0};
    glm::vec3 scale = {1,1,1};
    glm::vec3 rotation = {0,0,0};

    glm::mat4 GetModelMatrix();

    //mesh attributes
    std::shared_ptr<std::vector<unsigned int>> faces; //faces with indices of vertices

    Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, GLenum _renderMode = GL_POINTS);
    Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<glm::vec2>> _uvs, GLenum _renderMode = GL_POINTS);
    Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<unsigned int>> _faces, GLenum _renderMode = GL_TRIANGLES);
    Mesh(std::shared_ptr<std::vector<glm::vec4>> _vertices, std::shared_ptr<std::vector<unsigned int>> _faces,std::shared_ptr<std::vector<glm::vec2>> _uvs, GLenum _renderMode = GL_TRIANGLES);


    void BufferData() override;
    void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition) override;
    void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer) override;
    void Update(long double dt) override;
};


#endif //POSITIONBASEDFLUIDS_MESH_H
