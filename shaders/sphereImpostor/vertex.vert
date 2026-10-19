#version 430 core

layout(location = 0) in vec4 pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 CameraPosition;

uniform float sphereRadius;

layout(std140, binding=4) buffer Graph {
   vec4 Graphics[]; //x = density
};

out DATA{
 vec4 graphics;
} data_out;

void main(){
   gl_PointSize = 8.0f;
   gl_Position = pos;

   data_out.graphics = Graphics[gl_InstanceID];
}
