#version 430 core
out vec4 FragColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 CameraPosition;

uniform float sphereRadius;
flat in vec3 toCamera; //vector, always same
in vec2 mapping;
flat in vec3 center; //world-space cordinates
//flat in vec4 graphics;
flat in vec3 up;
flat in vec3 right;

const float near = 0.1f;
const float far = 100.0f;

//not accurate due to the fact that the sphere is bigger then the siluete
void Impostor(out vec3 spherePointPosition, out vec3 cameraNormal){
   float lensqr = dot(mapping, mapping); // mapping is from (-R,-R) to (R,R)
   if (lensqr > sphereRadius * sphereRadius){
      discard;
   }

   cameraNormal = normalize(vec3(-mapping.x, mapping.y, sqrt(sphereRadius*sphereRadius - lensqr)));
   spherePointPosition = vec3(View*vec4(center,1)) + cameraNormal*sphereRadius; //view-space
}

void DepthImpostor(out vec3 spherePointPosition, out vec3 cameraNormal){
   //cameraRay = CameraPosition + t*(-toCamera)
   vec3 lright = normalize(vec3(View*vec4(right,0)));
   vec3 lup = normalize(vec3(View*vec4(up,0)));

   vec3 center = vec3(View*vec4(center,1)); //viewSpace
   vec3 planePos = center + lright * mapping.x + lup * mapping.y; //view space sphere position on plane
   vec3 rayDirection = planePos;

   float a = dot(rayDirection, rayDirection);
   float b = 2*dot(-center, rayDirection);
   float c = dot(center, center) - (sphereRadius*sphereRadius);

   float disk = b*b - 4*a*c;
   float sqrtDisk = 0;
   if(disk < 0.0){
      discard;
   }else if(disk > 0.0){
      sqrtDisk = sqrt(disk);
   }

   float posT = (-b + sqrtDisk)/(2*a);
   float negT = (-b - sqrtDisk)/(2*a);

   float t = min(posT, negT) < 0 ? max(posT, negT) : min(posT, negT);

   spherePointPosition = t*rayDirection;
   cameraNormal = normalize(spherePointPosition - center);
}

float LinearizeDepth(float depth){
   float ndc = depth * 2.0 - 1.0;

   float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
   return linearDepth;
}

void main()
{
   vec3 cameraPos = vec3(0);
   vec3 cameraNormal;

   DepthImpostor(cameraPos, cameraNormal);
   //Impostor(cameraPos, cameraNormal);
   FragColor = vec4(15/255.0f, 82/255.0f, 186/255.0, 1.0f);

   vec4 clipPos = Projection * vec4(cameraPos, 1.0);
   float ndcDepth = clipPos.z / clipPos.w;
   gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;

   float depth = min(LinearizeDepth(gl_FragDepth)/(far), 1.0f);
   FragColor = vec4(depth, depth/10, 0, 1.0);
}