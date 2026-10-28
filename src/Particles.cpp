//
// Created by brabect on 05.10.26.
//

#include "Particles.h"

#include <iostream>
#include <random>


Particles::Particles(std::shared_ptr<std::vector<glm::vec4>> vertices, std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader) {
    renderBoundingBox=false;
    //particles are always rendered as points
    this->renderMode = GL_POINTS;
    this->vercities = vertices;
    this->newPositions = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0.0));
    this->velocities = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0.0)); //default velocity for each particle
    this->utilities = std::make_shared<std::vector<glm::vec4>>(vercities->size(), glm::vec4(0)); //default lambda for each particle
    this->temps = std::make_shared<std::vector<unsigned int>>(vertices->size(), 0);

    this -> computeShaderProgram = computeShader;
    this->renderShaderProgram = renderShader;

    this->usingEBO = false;

    VAO = 0;
    PositionsBuffer = 0;
    NewPositionsBuffer = 0;
    VelocitiesBuffer = 0;
    UtilitiesBuffer = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &ColorsBuffer);
    glGenBuffers(1, &PositionsBuffer);
    glGenBuffers(1, &NewPositionsBuffer);
    glGenBuffers(1, &VelocitiesBuffer);
    glGenBuffers(1, &UtilitiesBuffer);
    glGenBuffers(1, &TempsBuffer);
    glGenBuffers(1, &GridTableBuffer);
    glGenBuffers(1, &GridIndicesBuffer);


    this->BufferData();
}

Particles::Particles(std::shared_ptr<std::vector<glm::vec4>> vertices, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
        std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader){
    //particles are always rendered as points
    this->renderMode = GL_POINTS;
    this->vercities = vertices;
    this->newPositions = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0.0));
    this->velocities = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0.0)); //default velocity for each particle
    this->utilities = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0)); //default lambda for each particle
    this->temps = std::make_shared<std::vector<unsigned int>>(vertices->size(), 0);

    this->usingEBO = false;

    this -> computeShaderProgram = computeShader;
    this -> renderShaderProgram = renderShader;

    this->renderBoundingBox = true;

    VAO = 0;
    PositionsBuffer = 0;
    VelocitiesBuffer = 0;
    UtilitiesBuffer = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &ColorsBuffer);
    glGenBuffers(1, &PositionsBuffer);
    glGenBuffers(1, &NewPositionsBuffer);
    glGenBuffers(1, &UtilitiesBuffer);
    glGenBuffers(1, &VelocitiesBuffer);
    glGenBuffers(1, &TempsBuffer);
    glGenBuffers(1, &GridTableBuffer);
    glGenBuffers(1, &GridIndicesBuffer);

    this->BufferData();
    this->InitBoundingBox(boundingBoxCenter, boundingBoxSize, boundingBoxRenderShader);
}

Particles::Particles(std::shared_ptr<std::vector<glm::vec4>> vertices,
    std::shared_ptr<std::vector<glm::vec4>> velocities, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
    std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader,
    std::shared_ptr<ShaderProgram> boundingBoxRenderShader) {

    //particles are always rendered as points
    this->renderMode = GL_POINTS;
    this->vercities = vertices;
    this->newPositions = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0.0));
    this->velocities = velocities;
    this->utilities = std::make_shared<std::vector<glm::vec4>>(vertices->size(), glm::vec4(0)); //default lambda for each particle
    this->temps = std::make_shared<std::vector<unsigned int>>(vertices->size(),0);

    this->usingEBO = false;

    this -> computeShaderProgram = computeShader;
    this -> renderShaderProgram = renderShader;

    this->renderBoundingBox = true;

    VAO = 0;
    PositionsBuffer = 0;
    VelocitiesBuffer = 0;
    UtilitiesBuffer = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &ColorsBuffer);
    glGenBuffers(1, &PositionsBuffer);
    glGenBuffers(1, &NewPositionsBuffer);
    glGenBuffers(1, &VelocitiesBuffer);
    glGenBuffers(1, &UtilitiesBuffer);
    glGenBuffers(1, &TempsBuffer);
    glGenBuffers(1, &GridTableBuffer);
    glGenBuffers(1, &GridIndicesBuffer);

    this->BufferData();
    this->InitBoundingBox(boundingBoxCenter, boundingBoxSize, boundingBoxRenderShader);

}

Particles::Particles(uint particlesAmount, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize,
                     std::shared_ptr<ShaderProgram> computeShader, std::shared_ptr<ShaderProgram> renderShader,
                     std::shared_ptr<ShaderProgram> boundingBoxRenderShader) {

    //particles are always rendered as points
    this->renderMode = GL_POINTS;
    this->vercities = std::make_shared<std::vector<glm::vec4>>();
    this->newPositions = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0.0));
    this->velocities = std::make_shared<std::vector<glm::vec4>>(); //default velocity for each particle
    this->utilities = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0)); //default lambda for each particle
    this->temps = std::make_shared<std::vector<unsigned int>>(particlesAmount, 0);

    this->vercities->reserve(particlesAmount);
    this->velocities->reserve(particlesAmount);

    this->usingEBO = false;

    this -> computeShaderProgram = computeShader;
    this -> renderShaderProgram = renderShader;

    this->renderBoundingBox = true;

    VAO = 0;
    PositionsBuffer = 0;
    VelocitiesBuffer = 0;
    UtilitiesBuffer = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &ColorsBuffer);
    glGenBuffers(1, &PositionsBuffer);
    glGenBuffers(1, &NewPositionsBuffer);
    glGenBuffers(1, &VelocitiesBuffer);
    glGenBuffers(1, &UtilitiesBuffer);
    glGenBuffers(1, &TempsBuffer);
    glGenBuffers(1, &GridTableBuffer);
    glGenBuffers(1, &GridIndicesBuffer);

    this->InitBoundingBox(boundingBoxCenter, boundingBoxSize, boundingBoxRenderShader);
    this->InitParticles(particlesAmount, boundingBoxCenter, boundingBoxSize);

    this->BufferData();
}

Particles::Particles(uint particlesAmountx, uint particlesAmounty, uint particlesAmountz, glm::vec3 boundingBoxCenter,
    glm::vec3 boundingBoxSize, std::shared_ptr<ShaderProgram> computeShader,
    std::shared_ptr<ShaderProgram> renderShader, std::shared_ptr<ShaderProgram> boundingBoxRenderShader) {

    uint particlesAmount = particlesAmountx * particlesAmounty * particlesAmountz;

    //particles are always rendered as points
    this->renderMode = GL_POINTS;
    this->vercities = std::make_shared<std::vector<glm::vec4>>();
    this->newPositions = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0.0));
    this->velocities = std::make_shared<std::vector<glm::vec4>>(); //default velocity for each particle
    this->utilities = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0)); //default lambda for each particle
    this->temps = std::make_shared<std::vector<unsigned int>>(particlesAmount, 0);

    this->vercities->reserve(particlesAmount);
    this->velocities->reserve(particlesAmount);

    this->usingEBO = false;

    this -> computeShaderProgram = computeShader;
    this -> renderShaderProgram = renderShader;

    this->renderBoundingBox = true;

    VAO = 0;
    PositionsBuffer = 0;
    VelocitiesBuffer = 0;
    UtilitiesBuffer = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &ColorsBuffer);
    glGenBuffers(1, &PositionsBuffer);
    glGenBuffers(1, &NewPositionsBuffer);
    glGenBuffers(1, &VelocitiesBuffer);
    glGenBuffers(1, &UtilitiesBuffer);
    glGenBuffers(1, &TempsBuffer);
    glGenBuffers(1, &GridTableBuffer);
    glGenBuffers(1, &GridIndicesBuffer);

    this->InitBoundingBox(boundingBoxCenter, boundingBoxSize, boundingBoxRenderShader);
    this->InitParticles(particlesAmountx, particlesAmounty, particlesAmountz, boundingBoxCenter, boundingBoxSize);

    this->BufferData();
}


void Particles::BufferData() {
    if (this->vercities == nullptr or this->newPositions == nullptr or this->velocities == nullptr or
        this->utilities == nullptr or this->temps == nullptr) {
        throw std::runtime_error("Function Particles::BufferData tries to access nullptr");
    }
    //bind
    glBindVertexArray(VAO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, PositionsBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, vercities->size()*sizeof(glm::vec4), vercities->data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, PositionsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, NewPositionsBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, newPositions->size()*sizeof(glm::vec4), newPositions->data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, NewPositionsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, VelocitiesBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, velocities->size()*sizeof(glm::vec4), velocities->data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VelocitiesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, UtilitiesBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, utilities->size()*sizeof(glm::vec4), utilities->data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, UtilitiesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, TempsBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, temps->size()*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, TempsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    //for grid usage case
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GridTableBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, (newPositions->size()+1)*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, GridTableBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GridIndicesBuffer); //bind
    glBufferData(GL_SHADER_STORAGE_BUFFER, newPositions->size()*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, GridIndicesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

    glBindBuffer(GL_ARRAY_BUFFER, PositionsBuffer);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0); //3D - position
    glEnableVertexAttribArray(0);

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //VAO must be unbinded first otherwise it will affect EBO!
    glBindVertexArray(0);
}

//calls computerShader to update particles
void Particles::Update(long double dt) {
    if (computeShaderProgram == nullptr or this->vercities == nullptr){
        throw std::runtime_error("Function Particles::Update tries to access nullptr");
    }

    if (paused)
        return;

    this->computeShaderProgram->Use();

    computeShaderProgram->SetUniformFloat("dt", static_cast<float>(dt));
    computeShaderProgram->SetUniformInt("totalParticlesAmount", vercities->size());

    if (this->boundingBoxMesh != nullptr) {
        computeShaderProgram->SetUniformVector3("bbCenter", this->boundingBoxMesh->position);
        computeShaderProgram->SetUniformVector3("bbSize", this->boundingBoxMesh->scale);
    }
    computeShaderProgram->SetUniformFloat("sphereRadius", sphereRadius);
    //computeShaderProgram->SetUniformFloat("ParticleMass", particleMass);

    this->computeShaderProgram->SetUniformFloat("REST_DENSITY", REST_DENSITY);
    this->computeShaderProgram->SetUniformFloat("r", h);
    this->computeShaderProgram->SetUniformFloat("relaxation", relaxation);
    this->computeShaderProgram->SetUniformBool("artificial_pressure", artificial_pressure);
    this->computeShaderProgram->SetUniformFloat("ar", artificial_pressure_radius);
    this->computeShaderProgram->SetUniformFloat("k", k);
    this->computeShaderProgram->SetUniformFloat("n", n);
    this->computeShaderProgram->SetUniformFloat("q_size", q_size);

    //calls compute shader
    this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",0);

    constexpr GLuint LOCAL_SIZE = 32;
    GLuint groups = (vercities->size() + LOCAL_SIZE - 1) / LOCAL_SIZE;

    glDispatchCompute(groups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends


    for (int i = 0; i < iterations; i++) {
        //clearing the buffers before building hashgrid
        GLuint zero = 0;
        glBindVertexArray(VAO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, GridTableBuffer);
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, GridIndicesBuffer);
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);

        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);



        //HASHGRID INIT ON GPU
        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",1);
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends

        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",2);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends

        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",3);
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends
        //HASHGRID COMPLETED



        this->computeShaderProgram->SetUniformFloat("dt",dt/((float)iterations));
        //Second call - iterations of solver
        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",4);
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends

        //Third call - end step
        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",5);
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends

        //Third call - end step
        this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",6);
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends
    }

    this->computeShaderProgram->SetUniformFloat("dt",dt);
    //Third call - end step
    this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",7);
    glDispatchCompute(groups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends

    //Third call - end step
    this->computeShaderProgram->SetUniformInt("STAGE_NUMBER",8);
    glDispatchCompute(groups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // waits untill all workgroups ends
}

void Particles::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition, std::shared_ptr<FrameBuffer> frameBuffer) {
    if (frameBuffer == nullptr) {
        throw std::runtime_error("Function Particles::Render tries to access nullptr");
    }

    frameBuffer->Bind();
    this->Render(ViewMatrix, ProjectionMatrix, cameraPosition);
    frameBuffer->UnBind();
}

void Particles::Render(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3 cameraPosition) {
    if (renderShaderProgram == nullptr or this->vercities == nullptr){
        throw std::runtime_error("Function Particles::Render tries to access nullptr");
    }

    this->renderShaderProgram->Use();
    renderShaderProgram->SetUniformFloat("sphereRadius", sphereRadius);
    //dont need model matrix, because all vertices of particle system are already in world possitions
    renderShaderProgram->SetUniformMatrix4("View", ViewMatrix); //setting model matrix
    renderShaderProgram->SetUniformMatrix4("Projection", ProjectionMatrix); //setting model matrix
    renderShaderProgram->SetUniformVector3("CameraPosition", cameraPosition); //setting model matrix

    //binding buffers
    glBindVertexArray(VAO);
    /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, GraphicsBuffer); //bind
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, GraphicsBuffer);*/

    glDrawArrays(renderMode, 0, vercities->size());


    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); //VAO must be unbinded first otherwise it will affect EBO!
    glBindVertexArray(0);
}

void Particles::InitBoundingBox(glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize, std::shared_ptr<ShaderProgram> boundingBoxRenderShader) {
    if (boundingBoxRenderShader == nullptr) {
        throw std::runtime_error("Function Particles::InitBoundingBox tries to access nullptr");
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(Mesh(
        std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>{
            glm::vec4(1,1,1,1),
            glm::vec4(1,1,-1,1),
            glm::vec4(1,-1,1,1),
            glm::vec4(1,-1,-1,1),
            glm::vec4(-1,1,1,1),
            glm::vec4(-1,1,-1,1),
            glm::vec4(-1,-1,1,1),
            glm::vec4(-1,-1,-1,1)
        }),
        std::make_shared<std::vector<unsigned int>>(std::vector<unsigned int>{
            6,2,
            2,0,
            0,4,
            4,6,

            2,3,
            3,1,
            1,0,
            0,2,

            3,7,
            7,5,
            5,1,
            1,3,

            7,6,
            6,4,
            4,5,
            5,7,

            4,0,
            0,1,
            1,5,
            5,4,

            6,2,
            2,3,
            3,7,
            7,6

        })));

        mesh->renderShaderProgram = boundingBoxRenderShader;
        mesh->renderMode = GL_LINES;
        mesh->position = boundingBoxCenter;
        mesh->scale = boundingBoxSize;

        this->boundingBoxMesh = mesh;
}

void Particles::InitParticles(uint particlesAmount, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize) {
    if (this->vercities == nullptr) {
        throw std::runtime_error("Function Particles::InitParticles tries to access nullptr");
    }
    std::random_device rd;
    std::mt19937 mt(std::time({}));
    std::uniform_real_distribution<double> dist(-1, 1);

    for (uint i = 0; i < particlesAmount; i++) {
        glm::vec4 particle = glm::vec4(
            boundingBoxCenter.x + dist(mt)*boundingBoxSize.x,
            boundingBoxCenter.y + dist(mt)*boundingBoxSize.y,
            boundingBoxCenter.z + dist(mt)*boundingBoxSize.z,
            1);

        glm::vec4 velocity = glm::vec4(
            boundingBoxCenter.x + dist(mt)*boundingBoxSize.x,
            boundingBoxCenter.y + dist(mt)*boundingBoxSize.y,
            boundingBoxCenter.z + dist(mt)*boundingBoxSize.z,
            1);

        this->vercities->push_back(particle);
    }
    this->velocities = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0));
}

void Particles::InitParticles(uint particlesAmountx, uint particlesAmounty, uint particlesAmountz, glm::vec3 boundingBoxCenter, glm::vec3 boundingBoxSize) {
    if (this->vercities == nullptr) {
        throw std::runtime_error("Function Particles::InitParticles tries to access nullptr");
    }

    float offset = 0.1f;

    float size_x = particlesAmountx * offset;
    float size_y = particlesAmounty * offset;
    float size_z = particlesAmountz * offset;

    std::random_device rd;
    std::mt19937 mt(std::time({}));
    std::uniform_real_distribution<double> dist(-sphereRadius/4, sphereRadius/4);

    glm::vec3 start = boundingBoxCenter - glm::vec3(size_x/2, size_y/2, size_z/2);

    int particles = 0;
    for (int j = 0; j < particlesAmounty; j++) {
        for (int i = 0; i < particlesAmountz; i++) {
            for (int k = 0; k < particlesAmountx; k++) {
                glm::vec4 particle = glm::vec4(start+glm::vec3(k*offset + dist(mt), j*offset + dist(mt), i*offset+ dist(mt)),1);
                this->vercities->push_back(particle);
                particles++;
            }
        }
    }

    uint particlesAmount = particlesAmountx * particlesAmounty * particlesAmountz;
    this->velocities = std::make_shared<std::vector<glm::vec4>>(particlesAmount, glm::vec4(0));
}
