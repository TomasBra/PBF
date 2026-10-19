#version 430 core

layout(location = 0) in vec4 Pos;

uniform int STAGE_NUMBER;
out vec2 uv;

void main(){
   gl_PointSize = 8.0f;
   gl_Position = vec4(Pos.xy, 0,1);
   uv = Pos.zw;
}
