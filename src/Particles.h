//
// Created by brabect on 05.10.26.
//

#ifndef POSITIONBASEDFLUIDS_PARTICLES_H
#define POSITIONBASEDFLUIDS_PARTICLES_H
#include "Mesh.h"
#include "RenderedObject.h"
#include "HashGrid.h"

class Particles: public RenderedObject {
public:
    bool renderBoundingBox = false;
    Particles(std::shared_ptr<std::vector<glm::vec4>> vertices, std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader);
    Particles(std::shared_ptr<std::vector<glm::vec4>> vertices, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
        std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader);
    Particles(std::shared_ptr<std::vector<glm::vec4>> vertices, std::shared_ptr<std::vector<glm::vec4>> velocities, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
        std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader);
    Particles(uint particlesAmount, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
        std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader);

    Particles(uint particlesAmountx, uint particlesAmounty, uint particlesAmountz, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
        std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader);

    std::shared_ptr<ShaderProgram> computeShaderProgram; //Shader that will be used to render the mesh

    std::shared_ptr<std::vector<glm::vec4>> newPositions; //vertices and its velocities, must be vec4 due to allignment
    std::shared_ptr<std::vector<glm::vec4>> velocities; //vertices and its velocities, must be vec4 due to allignment
    std::shared_ptr<std::vector<glm::vec4>> utilities; //xyz = deltaPositions,w = lambda
    std::shared_ptr<std::vector<unsigned int>> temps;

    std::shared_ptr<Mesh> boundingBoxMesh;

    float sphereRadius = 0.0725;
    int iterations = 2;
    float particleMass = 1.0f;

    float REST_DENSITY = 2500.0f; //6378
    float h = 0.1f; //4*SphereRadius, 0.04
    float relaxation = 600.0f;

    float artificial_pressure_radius = 0.1f;
    float k = 0.0001f;
    int n = 4;
    float q_size = 0.03f;

    bool artificial_pressure = true;
    bool paused = true;

    void BufferData() override;
    void Update(long double dt) override;
    void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer) override;
    void Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition) override;

private:
    //aditional buffers
    glm::vec3 boundingBoxCenter = glm::vec3(0);
    glm::vec3 boundingBoxSize = glm::vec3(0);


    unsigned int ColorsBuffer, PositionsBuffer, NewPositionsBuffer, VelocitiesBuffer, UtilitiesBuffer, TempsBuffer, GridTableBuffer, GridIndicesBuffer = 0;

    void InitBoundingBox(glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize, std::shared_ptr<ShaderProgram> boundingBoxRenderShader);
    void InitParticles(uint particlesAmount, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize);
    void InitParticles(uint particlesAmountx, uint particlesAmounty, uint particlesAmountz, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize);
};


#endif //POSITIONBASEDFLUIDS_PARTICLES_H
