//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_SCENE_H
#define POSITIONBASEDFLUIDS_SCENE_H
#include "ShaderProgram.h"
#include <vector>

#include "RenderedObject.h"
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "FrameBuffer.h"
#include "Mesh.h"

class Scene {
public:
    //camera object
    //...
    Scene(int width, int height);
    virtual ~Scene() = default;

    std::unique_ptr<Mesh> displayQuad;
    Camera camera;
    std::vector<std::shared_ptr<RenderedObject>> objects; //

    //shaders
    ShaderProgram screenRenderShader;
    ShaderProgram classicRenderShader;
    ShaderProgram sphereImpostorComputeShader;
    ShaderProgram sphereImpostorRenderShader;

    std::shared_ptr<FrameBuffer> sceneFrameBuffer;

    virtual void Update(GLFWwindow *window, long double dt);
    virtual void Render(GLFWwindow *window, int Width, int Height);
    virtual void ClearBuffers();
    virtual void Display(int width, int height);

    virtual void processInput(GLFWwindow *window, float deltaTime, int windowWidth, int windowHeight);
    virtual void processMouseInput(GLFWwindow *window, double xpos, double ypos);

    virtual void pause();
    private:
};


#endif //POSITIONBASEDFLUIDS_SCENE_H
