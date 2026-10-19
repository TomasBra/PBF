//
// Created by brabect on 03.10.26.
//
#include "imgui/imgui.h"
#include "Mesh.h"
#include "SimulationScene.h"

#include "Particles.h"
#include "Window.h"


SimulationScene::SimulationScene(int width, int height): Scene(width, height){

    this->particlesFrameBuffer0 = std::make_shared<FrameBuffer>(width, height, 3);
    this->particlesFrameBuffer1 = std::make_shared<FrameBuffer>(width, height, 3);
    /*particles = std::make_shared<Particles>(Particles(std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>({glm::vec4(-1,0,0,0), glm::vec4(1,0,0,0)})),
        std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>({glm::vec4(0.5,0,0,0), glm::vec4(-0.5,0,0,0)})),
        glm::vec3(0), glm::vec3(2,1,1),
        std::make_shared<ShaderProgram>(this->sphereImpostorComputeShader),
        std::make_shared<ShaderProgram>(this->sphereImpostorRenderShader),
        std::make_shared<ShaderProgram>(this->classicRenderShader)));*/

    /*particles = std::make_shared<Particles>(Particles(std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>({glm::vec4(-1,0,0,0), glm::vec4(1,0,0,0),
    glm::vec4(0,0,-1,0)})),
        std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>({glm::vec4(1,0,0,0), glm::vec4(-1,0,0,0), glm::vec4(0,0,1,0)})),
        glm::vec3(0), glm::vec3(2,1,1),
        std::make_shared<ShaderProgram>(this->sphereImpostorComputeShader),
        std::make_shared<ShaderProgram>(this->sphereImpostorRenderShader),
        std::make_shared<ShaderProgram>(this->classicRenderShader)));*/

    particles = std::make_shared<Particles>(Particles(20,10,5,
        glm::vec3(0), glm::vec3(2,1,1),
        std::make_shared<ShaderProgram>(this->sphereImpostorComputeShader),
        std::make_shared<ShaderProgram>(this->sphereImpostorRenderShader),
        std::make_shared<ShaderProgram>(this->classicRenderShader)));
    particles->renderBoundingBox = true;

    std::shared_ptr<Mesh> objectMesh = std::make_shared<Mesh>(
        std::make_shared<std::vector<glm::vec4>>(std::vector<glm::vec4>{
            glm::vec4(-1.0f, -1.0f,  1.0f,  1.0f),
            glm::vec4( 1.0f, -1.0f,  1.0f,  1.0f),
            glm::vec4( 1.0f,  1.0f,  1.0f,  1.0f),
            glm::vec4(-1.0f,  1.0f,  1.0f,  1.0f),

            glm::vec4(-1.0f, -1.0f, -1.0f,  1.0f),
            glm::vec4( 1.0f, -1.0f, -1.0f,  1.0f),
            glm::vec4( 1.0f,  1.0f, -1.0f,  1.0f),
            glm::vec4(-1.0f,  1.0f, -1.0f,  1.0f)
        }),
        std::make_shared<std::vector<unsigned int>>(std::vector<unsigned int>{
            0, 1, 2,   2, 3, 0,
            1, 5, 6,   6, 2, 1,
            5, 4, 7,   7, 6, 5,
            4, 0, 3,   3, 7, 4,
            3, 2, 6,   6, 7, 3,
            4, 5, 1,   1, 0, 4
        })
    );
    objectMesh->renderMode = GL_TRIANGLES;
    objectMesh->renderShaderProgram = std::make_shared<ShaderProgram>(classicRenderShader);
    objectMesh->scale = glm::vec3(1.0);
    objectMesh->position = glm::vec3(0.0,0.0,-2.0);

    this->objects.push_back(objectMesh);
}

//update values of simulation with fixed timestep
void SimulationScene::Update(GLFWwindow *window, const long double dt) {
    if (this->particles == nullptr) {
        throw std::runtime_error("Function SimulationScene::Update tries to access nullptr");
    }

    Scene::Update(window, dt);
    //updates physics of the simulation with fixed dt
    this->particles->Update(dt);
}




//render simulation
void SimulationScene::Render(GLFWwindow *window, int Width, int Height) {
    if (this->sceneFrameBuffer == nullptr or this->particlesFrameBuffer0 == nullptr or
        this->particlesFrameBuffer1 == nullptr or this->particles == nullptr) {
        throw std::runtime_error("Function SimulationScene::Render tries to access nullptr");
    }

    this->ClearBuffers();

    this->sceneFrameBuffer->ResizeTextures(Width, Height);
    this->particlesFrameBuffer0->ResizeTextures(Width, Height);
    this->particlesFrameBuffer1->ResizeTextures(Width, Height);

    this->particles->Render(camera.GetViewMatrix(), camera.GetProjectionMatrix(Width, Height), camera.position, this->particlesFrameBuffer0);

    for (std::shared_ptr<RenderedObject> mesh: objects) {
        if (mesh == nullptr)
            throw std::runtime_error("Function SimulationScene::Render tries to access nullptr");

        mesh->Render(camera.GetViewMatrix(), camera.GetProjectionMatrix(Width, Height), camera.position, this->sceneFrameBuffer);
    }

    Window* windowObject = (Window*)glfwGetWindowUserPointer(window); //sketchy shit

    // ImGUI First window creation
    ImGui::Begin("Fluids parameters");
    ImGui::InputFloat("Simulation speed", &windowObject->simulationSpeed, 0.1f);
    ImGui::Checkbox("Paused", &particles->paused);
    ImGui::InputInt("Simulation substeps", &particles->iterations);
    // Text that appears in the window
    ImGui::Text("Particles count: %d", particles->vercities->size());
    ImGui::Text("Kernel");
    ImGui::InputFloat("Smooth radius", &(this->particles->h));
    ImGui::InputFloat("Rest density", &(this->particles->REST_DENSITY));
    ImGui::InputFloat("Relaxation", &(this->particles->relaxation));
    ImGui::Checkbox("Artificial Pressure", &particles->artificial_pressure);
    ImGui::InputFloat("Radius", &(this->particles->artificial_pressure_radius));
    ImGui::InputFloat("k", &(this->particles->k));
    ImGui::InputInt("n", &(this->particles->n));
    ImGui::InputFloat("q", &(this->particles->q_size));


    // Ends the window
    ImGui::End();
}

void SimulationScene::ClearBuffers() {
    if (this->particlesFrameBuffer0 == nullptr) {
        throw std::runtime_error("Function SimulationScene::ClearBuffers tries to access nullptr");
    }

    Scene::ClearBuffers();

    particlesFrameBuffer0->Bind();
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    particlesFrameBuffer1->Bind();
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}


void SimulationScene::Display(int width, int height) {
    //first pass
    DisplayFirstPass(width, height);
    //second pass
    DisplaySecondPass(width, height);
    //
    DisplayThirdPass(width, height);
}


void SimulationScene::processInput(GLFWwindow *window, float deltaTime, int windowWidth, int windowHeight) {
    if (this->particles == nullptr) {
        throw std::runtime_error("Function SimulationScene::processInput tries to access nullptr");
    }

    //checks for camera movement etc...
    Scene::processInput(window, deltaTime, windowWidth, windowHeight);
    //rest of the input check


    glm::vec3 containerMovement = glm::vec3(0,0,0);
    //camera movement
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        containerMovement -= glm::vec3(0,0,1);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        containerMovement += glm::vec3(0,0,1);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        containerMovement -= glm::vec3(1,0,0);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        containerMovement += glm::vec3(1,0,0);
    }
    glm::vec3 direction = containerMovement;
    if (glm::length(direction) == 0)
        return;

    const float speed = 1;
    direction = glm::normalize(direction);
    this->particles->boundingBoxMesh->position = glm::vec3(this->particles->boundingBoxMesh->position+speed*direction*deltaTime);

}




void SimulationScene::processMouseInput(GLFWwindow *window, double xpos, double ypos) {
    Scene::processMouseInput(window, xpos, ypos);
}

void SimulationScene::pause() {
    if (this->particles == nullptr)
        throw std::runtime_error("Function SimulationScene::pause tries to access nullptr");

    this->particles->paused = !this->particles->paused;
}



//the partilces are rendered in particlesFrameBuffer0 => Apply postprocessing and draw to particlesFrameBuffer1
void SimulationScene::DisplayFirstPass(int width, int height) {
    if (this->particlesFrameBuffer0 == nullptr or this->particlesFrameBuffer1 == nullptr or
        this->particlesFrameBuffer0 == nullptr or this->displayQuad == nullptr or
        this->displayQuad->renderShaderProgram == nullptr or this->particlesFrameBuffer0->colorTexture == nullptr or
        this->particlesFrameBuffer0->depthTexture == nullptr) {
        throw std::runtime_error("Function SimulationScene::DisplayFirstPass tries to access nullptr");
    }


    this->particlesFrameBuffer0->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first


    this->particlesFrameBuffer1->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first



    this->particlesFrameBuffer0->colorTexture->Bind(GL_TEXTURE2);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesColorTexture", 2);

    this->particlesFrameBuffer0->depthTexture->Bind(GL_TEXTURE3);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesDepthTexture", 3);

    this->displayQuad->renderShaderProgram->SetUniformInt("STAGE_NUMBER", 0);
    this->displayQuad->renderShaderProgram->SetUniformVector2("blurDirection", glm::vec2(1,0));//horizontal blur
    this->displayQuad->renderShaderProgram->SetUniformVector2("texelsize", glm::vec2(1.0/width,1.0/height));
    this->displayQuad->Render(glm::mat4(1.0), glm::mat4(1.0), glm::vec3(1.0));
}


//particles are postprocessed in framebuffer1 => Apply postprocessing and draw to DISPLAYBUFFER
void SimulationScene::DisplaySecondPass(int width, int height) {
    if (this->particlesFrameBuffer0 == nullptr or this->particlesFrameBuffer1 == nullptr or this->sceneFrameBuffer == nullptr or
        this->particlesFrameBuffer1->colorTexture == nullptr or this->sceneFrameBuffer->colorTexture == nullptr or
        this->particlesFrameBuffer0->depthTexture == nullptr or this->displayQuad == nullptr or
        this->displayQuad->renderShaderProgram == nullptr) {

        throw std::runtime_error("Function SimulationScene::DisplaySecondPass tries to access nullptr");
        }

    this->particlesFrameBuffer1->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first

    this->particlesFrameBuffer0->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first


    this->particlesFrameBuffer1->colorTexture->Bind(GL_TEXTURE2);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesColorTexture", 2);


    //partcilesFrameBuffer0 - depthTexture BEWARE!!! DEPTH TEXTURE STAYS ALWAYS IN FIRST FRAMEBUFFER
    this->particlesFrameBuffer0->depthTexture->Bind(GL_TEXTURE3);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesDepthTexture", 3);


    this->displayQuad->renderShaderProgram->SetUniformInt("STAGE_NUMBER", 1);
    this->displayQuad->renderShaderProgram->SetUniformVector2("blurDirection", glm::vec2(0,1)); //vertical blur
    this->displayQuad->renderShaderProgram->SetUniformVector2("texelsize", glm::vec2(1.0/width,1.0/height));
    this->displayQuad->Render(glm::mat4(1.0), glm::mat4(1.0), glm::vec3(1.0));
}

void SimulationScene::DisplayThirdPass(int width, int height) {
    if (this->particlesFrameBuffer0 == nullptr or this->particlesFrameBuffer1 == nullptr or this->sceneFrameBuffer == nullptr or
        this->particlesFrameBuffer1->colorTexture == nullptr or this->sceneFrameBuffer->colorTexture == nullptr or
        this->particlesFrameBuffer0->depthTexture == nullptr or this->displayQuad == nullptr or
        this->displayQuad->renderShaderProgram == nullptr) {

        throw std::runtime_error("Function SimulationScene::DisplaySecondPass tries to access nullptr");
        }

    this->particlesFrameBuffer1->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first

    this->particlesFrameBuffer0->Bind();
    glDisable(GL_DEPTH_TEST); //required to not overwrite depth values from first

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glDisable(GL_DEPTH_TEST);


    this->sceneFrameBuffer->colorTexture->Bind(GL_TEXTURE0);
    this->displayQuad->renderShaderProgram->SetUniformInt("sceneColorTexture", 0);


    this->sceneFrameBuffer->depthTexture->Bind(GL_TEXTURE1);
    this->displayQuad->renderShaderProgram->SetUniformInt("sceneDepthTexture", 1);


    this->particlesFrameBuffer0->colorTexture->Bind(GL_TEXTURE2);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesColorTexture", 2);


    //partcilesFrameBuffer0 - depthTexture BEWARE!!! DEPTH TEXTURE STAYS ALWAYS IN FIRST FRAMEBUFFER
    this->particlesFrameBuffer0->depthTexture->Bind(GL_TEXTURE3);
    this->displayQuad->renderShaderProgram->SetUniformInt("particlesDepthTexture", 3);


    this->displayQuad->renderShaderProgram->SetUniformInt("STAGE_NUMBER", 2);
    this->displayQuad->renderShaderProgram->SetUniformVector2("texelsize", glm::vec2(1.0/width,1.0/height));
    this->displayQuad->Render(glm::mat4(1.0), glm::mat4(1.0), glm::vec3(1.0));
}


