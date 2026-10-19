//
// Created by brabect on 03.10.26.
//

#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"



//dt has to be in seconds
void Camera::Move(glm::vec3 direction, float dt) {
    if (glm::length(direction) == 0)
        return;

    direction = glm::normalize(direction);
    this->position = glm::vec3(position[0]+speed*direction[0]*dt, position[1]+speed*direction[1]*dt, position[2]+speed*direction[2]*dt);
}




void Camera::Rotate(glm::vec2 mousePosition) {
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    if (firstMouse) // initially set to true
    {
        lastMousePosition = mousePosition;
        lastMousePosition = mousePosition;
        firstMouse = false;
    }

    float xoffset = mousePosition[0] - lastMousePosition[0];
    float yoffset = lastMousePosition[1] - mousePosition[1]; // reversed since y-coordinates range from bottom to top

    lastMousePosition[0] = mousePosition[0];
    lastMousePosition[1] = mousePosition[1];

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    rotation = glm::vec2(rotation.x + xoffset, rotation.y + yoffset);
    if(rotation[1] > 89.0f)
        rotation[1] =  89.0f;
    if(rotation[1] < -89.0f)
        rotation[1] = -89.0f;


    viewDirection.x = glm::cos(glm::radians(rotation[0])) * cos(glm::radians(rotation[1]));
    viewDirection.y = glm::sin(glm::radians(rotation[1]));
    viewDirection.z = glm::sin(glm::radians(rotation[0])) * cos(glm::radians(rotation[1]));
    viewDirection = glm::normalize(viewDirection);

    rightDirection = glm::normalize(glm::cross(viewDirection,worldUp));
    upDirection = glm::cross(rightDirection, viewDirection);
}




//not tested
//cinematic rotation around the start point (0,0,0)
void Camera::RotationMove(float radius, float height, float totalTime) {
    this->position = {radius * glm::cos(totalTime), height, radius * glm::sin(totalTime)};
}




glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(
        position,           // Camera is here
        position+viewDirection,// and looks here : at the same position, plus "direction"
        upDirection// Head is up (set to 0,-1,0 to look upside-down)
    );
}





glm::mat4 Camera::GetProjectionMatrix(int Width, int Height) {
    return glm::perspective(glm::radians(45.0f), (float)Width/(float)Height, 0.1f, 100.0f);
}
