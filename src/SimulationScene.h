//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_SIMULATIONSCENE_H
#define POSITIONBASEDFLUIDS_SIMULATIONSCENE_H
#include "Scene.h"
#include "Particles.h"
#include <GLFW/glfw3.h>

class SimulationScene: public Scene {
    public:
        std::shared_ptr<Particles> particles;

        //ping-pong
        std::shared_ptr<FrameBuffer> particlesFrameBuffer0;
        std::shared_ptr<FrameBuffer> particlesFrameBuffer1;

        SimulationScene(int width, int height);
        void Update(GLFWwindow *window, long double dt) override;
        void Render(GLFWwindow *window, int Width, int Height) override;
        void ClearBuffers() override;

        ///I kinda played with the idea to make postprocessing in computeshaders, because it could be faster but according to
        ///this article https://lisyarus.github.io/blog/posts/compute-blur.html it is still slower with all optimalization than using
        ///classic texture
        void Display(int width, int height) override;

        void processInput(struct ::GLFWwindow *window, float deltaTime, int windowWidth, int windowHeight) override;
        void processMouseInput(struct ::GLFWwindow *window, double xpos, double ypos) override;

        void pause() override;
    private:
        ///Horizontal blur
        void DisplayFirstPass(int width, int height);
        ///Vertical blur
        void DisplaySecondPass(int width, int height);
        ///Normal calculations
        void DisplayThirdPass(int width, int height);
        ///
        void DisplayFourthPass(int width, int height);
};


#endif //POSITIONBASEDFLUIDS_SIMULATIONSCENE_H
