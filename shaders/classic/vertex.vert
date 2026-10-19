#version 430 core

layout(location = 0) in vec4 Pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 pos;

void main(){
   gl_PointSize = 8.0f;

   gl_Position = Projection * View * Model *  Pos;
   pos = Pos.xyz;
}
