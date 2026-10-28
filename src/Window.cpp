//
// Created by brabect on 03.10.26.
//
#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Window.h"

#include <chrono>
#include <iostream>
#include "SimulationScene.h"
#include <GLFW/glfw3.h>

#include "imgui/imstb_rectpack.h"

Window::Window(int width, int height, std::string title) {

    this->Title = title;
    this->Width = width;
    this->Height = height;

    this->GLFWInit(width, height, title); //creates window
    this->IMGUIInit(); //initzialiaze the IMGUI rendering
    this->UIInit(); //creates UI for the main window
    this->currentScene = std::make_unique<SimulationScene>(width, height);
}



int Window::Run() {
    if (currentScene == nullptr) {
        throw std::runtime_error("Function Window::Run tries to access nullptr");
    }

    //main loop
    while (!glfwWindowShouldClose(window)) {
        std::chrono::time_point<std::chrono::high_resolution_clock> currentFrameTime;
        //waiting until the opengl is initzialiazed
        long double ldt = 0;
        do {
            //framerate
            currentFrameTime = std::chrono::high_resolution_clock::now();
            ldt = (currentFrameTime - lastFrameTime).count()/(long double)1e6; //miliseconds!
            lastFrameTime = currentFrameTime;
        }while (ldt/1000 > 1);

        dt = ldt / 1000; //seconds

        deltaTimes.push_back(ldt);

        dt *= simulationSpeed;


        accumulator += dt;
        while (accumulator >= fixedTimeStep) {
            //update of informations
            currentScene->Update(window, fixedTimeStep);
            accumulator -= fixedTimeStep;
        }

        this->processInput(window);
        // render
        // ------
        /*glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);*/

        //IMGUI render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render
        // ------
        currentScene->Render(window, this->Width, this->Height);

        currentScene->Display(this->Width, this->Height);


        ui.Render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (not cursorEnabled)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    glfwTerminate();
    return 0;
}




Window::~Window() {
    glfwSetWindowUserPointer(window, NULL);
    glfwDestroyWindow(this->window);
}




int Window::GLFWInit(int width, int height, std::string& title) {
    //incialization of GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    glfwSetWindowUserPointer(window, this);

    if (this->window  == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(0); // unlock frames

    return 0;
}




int Window::IMGUIInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}




void Window::UIInit() {
    ui.Render = [&]() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        // ImGUI First window creation
        ImGui::Begin("Position Based Fluids Simulation");

        // Framerate
        std::stringstream ss;
        ss << "Application average " << AverageDeltaTime() << " ms/frame (" << FPS() << " FPS)" << std::endl;
        ImGui::Text(ss.str().c_str());
        // Camera positon
        ImGui::InputFloat3("Camera position", glm::value_ptr(currentScene->camera.position));
        // View Direction
        ImGui::InputFloat3("View direction", glm::value_ptr(currentScene->camera.viewDirection));
        // Mouse Angles
        ImGui::InputFloat2("Mouse Yaw, Pitch", glm::value_ptr(currentScene->camera.rotation));

        // Ends the window
        ImGui::End();
    };
}





long double Window::FPS() {
    long double totalDeltaTime = 0;
    int frames = 0;
    for (int i = deltaTimes.size()-1; i >= 0; i--) {
        frames++;
        totalDeltaTime += deltaTimes[i];
        //one second passed
        if (totalDeltaTime >= 1000) {
            deltaTimes.erase(deltaTimes.begin(), deltaTimes.end() - frames);
            long double seconds = totalDeltaTime/1000;
            long double fps = 1.0/(seconds/frames);
            return fps;
        }
    }

    //default hodnota
    return 1.0/(totalDeltaTime/1000/(frames));
}






long double Window::AverageDeltaTime() {
    long double totalDeltaTime = 0;
    int frames = 0;
    for (int i = deltaTimes.size()-1; i >= 0; i--) {
        frames++;
        totalDeltaTime += deltaTimes[i];
        //one second passed
        if (totalDeltaTime >= 1000) {
            deltaTimes.erase(deltaTimes.begin(), deltaTimes.end() - frames);
            return totalDeltaTime/frames;
        }
    }

    //default hodnota
    return totalDeltaTime/frames;
}






//callbacks
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    //update the window size information
    Window* windowObject = (Window*)glfwGetWindowUserPointer(window); //sketchy shit
    windowObject -> Width = width;
    windowObject -> Height = height;
}





//Mouse movement callback
void Window::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    Window* windowObject = (Window*)glfwGetWindowUserPointer(window); //sketchy shit

    if (windowObject == NULL) {
        throw std::runtime_error("Function Window::mouse_callback tries to access nullptr");
    }

    if (not windowObject->cursorEnabled)
        windowObject -> currentScene -> processMouseInput(window, xpos, ypos);
}




//Press, Relase, ... key events
void Window::keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Window* windowObject = (Window*)glfwGetWindowUserPointer(window); //sketchy shit

    if (windowObject == NULL) {
        throw std::runtime_error("Function Window::keyboard_callback tries to access nullptr");
    }

    //Cursor Enabled/Disabled
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        windowObject->cursorEnabled = !windowObject->cursorEnabled;
    }

    //Fullscreen
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (windowObject->fullscreen) {
            monitor = nullptr;
        }

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        windowObject->fullscreen = !windowObject->fullscreen;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        windowObject->currentScene->pause();
    }
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Window::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    Window* windowObject = (Window*)glfwGetWindowUserPointer(window); //sketchy shit
    if (windowObject == NULL or windowObject->currentScene == nullptr) {
        throw std::runtime_error("Function Window::processInput tries to access nullptr");
    }
    windowObject -> currentScene -> processInput(window, (float)windowObject->dt, windowObject->Width, windowObject->Height);
}


