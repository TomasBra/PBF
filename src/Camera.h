//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_CAMERA_H
#define POSITIONBASEDFLUIDS_CAMERA_H
#include "glm/glm.hpp"
#include <array>


class Camera {
public:
    //Create a camera that rotates around the object
    //position = ObjectCenter + (radius * cos(time), height, radius * sin(time));
    glm::vec2 lastMousePosition = {0,0};

    glm::vec3 position = glm::vec3(0,0,6);
    glm::vec3 viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 rightDirection = glm::vec3(1.0f, 0.0f,  0.0f);;
    glm::vec3 upDirection = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec2 rotation = {0,0}; //yaw, vertical pitch

    void Move(glm::vec3 direction, float dt);
    void Rotate(glm::vec2 mousePosition);
    void RotationMove(float radius, float height, float totalTime);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(int Width, int Height);
private:
    bool firstMouse = true;
    float speed = 1;
    float mouseSpeed = 1;
};


#endif //POSITIONBASEDFLUIDS_CAMERA_H
