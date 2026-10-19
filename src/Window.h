//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_WINDOW_H
#define POSITIONBASEDFLUIDS_WINDOW_H
#include "Scene.h"
#include "ShaderProgram.h"

#include <chrono>
#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#include "UI.h"

class Window {
public:
    UI ui;
    long double dt;
    int Width, Height;
    std::string Title;
    std::unique_ptr<Scene> currentScene;

    long double accumulator = 0.0;
    float simulationSpeed = 1;
    const float fixedTimeStep = 1/120.0f;

    Window(int width, int height, std::string title);
    int Run();
    ~Window();

private:
    bool cursorEnabled = true;
    bool fullscreen = false;
    GLFWwindow* window;
    //deltaTime
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;
    //FPS
    std::vector<long double> deltaTimes;

    int GLFWInit(int width, int height, std::string& title);
    int IMGUIInit();
    void UIInit();

    long double FPS();
    long double AverageDeltaTime();

    //callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void processInput(GLFWwindow *window);
};

#endif