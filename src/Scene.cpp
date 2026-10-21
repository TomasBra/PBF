//
// Created by brabect on 03.10.26.
//
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "Scene.h"

#include <algorithm>
#include <iostream>
#include <GLFW/glfw3.h>

#include "Window.h"




Scene::Scene(int width, int height) {
    this->screenRenderShader.LoadShader(GL_VERTEX_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/screen/vertex.vert"));
    this->screenRenderShader.LoadShader(GL_FRAGMENT_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/screen/fragment.frag"));

    this->screenRenderShader.LinkProgram();

    //create ShaderProgram
    this->classicRenderShader.LoadShader(GL_VERTEX_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/classic/vertex.vert"));
    this->classicRenderShader.LoadShader(GL_FRAGMENT_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/classic/fragment.frag"));

    this->classicRenderShader.LinkProgram();

    this->sphereImpostorRenderShader.LoadShader(GL_VERTEX_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/sphereImpostor/vertex.vert"));
    this->sphereImpostorRenderShader.LoadShader(GL_GEOMETRY_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/sphereImpostor/geometry.glsl"));
    this->sphereImpostorRenderShader.LoadShader(GL_FRAGMENT_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/sphereImpostor/fragment.frag"));

    this->sphereImpostorRenderShader.LinkProgram();

    this->sphereImpostorComputeShader.LoadShader(GL_COMPUTE_SHADER, std::filesystem::path("/home/brabect/CLionProjects/PositionBasedFluids/shaders/sphereImpostor/compute.glsl"));
    this->sphereImpostorComputeShader.LinkProgram();

    displayQuad = std::make_unique<Mesh>(Mesh(std::shared_ptr<std::vector<glm::vec4>>(new std::vector<glm::vec4>({
        // positions   // texCoords
        glm::vec4(-1.0f,  1.0f,  0.0f, 1.0f),
        glm::vec4(-1.0f, -1.0f,  0.0f, 0.0f),
        glm::vec4(1.0f, -1.0f,  1.0f, 0.0f),

        glm::vec4(-1.0f,  1.0f,  0.0f, 1.0f),
        glm::vec4(1.0f, -1.0f,  1.0f, 0.0f),
         glm::vec4(1.0f,  1.0f,  1.0f, 1.0f)
    })), GL_TRIANGLES));

    displayQuad->renderShaderProgram = std::make_shared<ShaderProgram>(screenRenderShader);

    this->sceneFrameBuffer = std::make_shared<FrameBuffer>(width, height, COLOR_AND_DEPTH_TEXTURE);
}

//Updates Physics
void Scene::Update(GLFWwindow *window, long double dt) {
    for (std::shared_ptr<RenderedObject> mesh: objects) {
        if (mesh == nullptr) {
            throw std::runtime_error("Function Scene::Update tries to access nullptr");
        }
        mesh->Update(dt);
    }
}

void Scene::Render(GLFWwindow *window, int Width, int Height) {
    if (this->sceneFrameBuffer == nullptr) {
        throw std::runtime_error("Function Scene::Render tries to access nullptr");
    }


    this->ClearBuffers();
    this->sceneFrameBuffer->ResizeTextures(Width, Height);

    for (std::shared_ptr<RenderedObject> mesh: objects) {
        if (mesh == nullptr) {
            throw std::runtime_error("Function Scene::Render tries to access nullptr");
        }

        mesh->Render(camera.GetViewMatrix(), camera.GetProjectionMatrix(Width, Height), camera.position, sceneFrameBuffer);
    }
}

void Scene::ClearBuffers() {
    if (sceneFrameBuffer == nullptr) {
        throw std::runtime_error("Function Scene::ClearBuffers tries to access nullptr");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    sceneFrameBuffer->Bind();
    glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

}


void Scene::Display(int width, int height) {
    if (this->sceneFrameBuffer == nullptr or this->displayQuad == nullptr or
        this->displayQuad->renderShaderProgram == nullptr or this->sceneFrameBuffer->colorTexture) {
        throw std::runtime_error("Function Scene::Display tries to access nullptr");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glDisable(GL_DEPTH_TEST);

    this->sceneFrameBuffer->colorTexture->Bind(GL_TEXTURE0);
    this->displayQuad->renderShaderProgram->SetUniformInt("screenTexture", 0);

    this->displayQuad->Render(glm::mat4(1.0), glm::mat4(1.0), glm::vec3(1.0));
}


void Scene::processInput(GLFWwindow *window, float deltaTime, int windowWidth, int windowHeight) {
    glm::vec3 movement = glm::vec3(0,0,0);
    //camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        movement += camera.viewDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        movement -= camera.viewDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        movement -= camera.rightDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        movement += camera.rightDirection;
    }

    camera.Move(movement, deltaTime);
}




void Scene::processMouseInput(GLFWwindow *window, double xpos, double ypos) {
    camera.Rotate(glm::vec2(xpos, ypos));
}

void Scene::pause() {
}
