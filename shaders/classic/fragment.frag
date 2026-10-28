#version 430 core
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 color;

uniform bool usesColorTexture = false;

const vec4 defaultColor = vec4(1,0,0,1);

in vec3 pos;
in vec2 uv;

void main()
{
   if(usesColorTexture){
      FragColor = texture(colorTexture, uv);
   }else{
      FragColor = defaultColor;
   }
}