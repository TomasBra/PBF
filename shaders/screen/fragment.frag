#version 430 core
out vec4 FragColor;

uniform bool renderFluid = true;
uniform bool blurFilter = true;
uniform int seletedBlur = 0;



uniform float gaussSigma = 1.0f;
uniform int gaussRadius = 15;

uniform float bilateralDepthFactor = 10.0f;
uniform int bilateralRadius = 15;
uniform float bilateralSigma = 1.0f;


uniform int narrowRangeRadius = 15;
uniform float narrowRangeSigma = 1.0f;


uniform int STAGE_NUMBER = 0;
uniform vec2 blurDirection = vec2(1,0);

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform sampler2D sceneColorTexture;
uniform sampler2D sceneDepthTexture;
uniform sampler2D particlesColorTexture;
uniform sampler2D particlesDepthTexture;

uniform sampler2D particlesThicknesTexture;
uniform float sphereRadius;

in vec2 uv;

uniform vec2 texelsize = vec2(1,0);
uniform vec2 screenSize = vec2(1920,1080);

const float near = 0.1f;
const float far = 100.0f;

const float PI = 3.1415926535897932384626433832795;
const float tanAlfaHalf = 0.41421356237;
const vec3 k = vec3(0.9, 0.5, 0.1);


float screenSigma(float worldSigma, float depth){
   return ceil((screenSize.y/2)/(abs(depth)*tanAlfaHalf) * worldSigma);
}

float gaussian(float x, float sigma) {
   return exp(-(pow(x, 2))/(2 * pow(sigma, 2))) / (2 * PI * pow(sigma, 2));
}

float LinearizeDepth(float depth){
   float ndc = depth * 2.0 - 1.0;
   ndc = (2*depth - gl_DepthRange.near - gl_DepthRange.far)/gl_DepthRange.diff;

   float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));

   return linearDepth;
}

vec3 ViewPos(vec2 uv){
   float depth = texture(particlesColorTexture, uv).x*far;
   vec4 viewVector = (inverse(Projection) * vec4(uv*2.0-1.0, 1, 1));

   vec3 ray = viewVector.xyz / viewVector.w;

   return ray * (-depth / ray.z);
}

vec3 WorldPos(vec2 uv){
   vec3 viewPos = ViewPos(uv);

   return (inverse(View)*vec4(viewPos, 0)).xyz;
}

//mi <= delta...must be true
vec4 NarrowRangeFilter(vec2 _uv, sampler2D text, int radius,float sigma, float delta, float mi){
   float delta_low = delta;
   float delta_high = delta;

   vec4 result = vec4(0.0);
   vec2 size = textureSize(text, 0);
   float total_weight = 0;
   /*INFINIT DEPTH*/
   vec4 _sample = texture(text, _uv);
   float _center_depth = _sample.x;
   if(_sample.x == 1){
      return _sample;
   }

   for (int x_offset = -radius; x_offset <= radius; ++x_offset)
   {
      vec2 offset = blurDirection * x_offset / size;
      _sample = texture(text, _uv + offset);
      vec4 _sample_k = texture(text, _uv - offset);

      float weight = gaussian(x_offset, sigma);
      if(_sample.x > _center_depth + delta_high || _sample_k.x > _center_depth + delta_high){
         weight = 0;
      }

      vec4 clamped_sample = vec4(_center_depth - mi, _center_depth - mi, _center_depth - mi, 1);
      if(_sample.x >= _center_depth - delta_low){
         clamped_sample = _sample;
      }

      if(_sample.x != 1){
         total_weight += weight;
         result += clamped_sample * weight;
      }

      delta_low = max(delta_high, _center_depth - _sample.x + delta);
      delta_high = max(delta_high, _sample.x - _center_depth + delta);
   }

   return vec4(result.xyz/total_weight,1); //normalization because of missing samples
}

vec4 BilateralFilter(vec2 _uv, sampler2D text, int radius,float sigma){
   vec4 _sample = texture(text, uv);
   float centerDepth = _sample.x;

   if(_sample.x == 1){
      return _sample;
   }

   vec2 size = textureSize(text, 0);

   vec4 sum = vec4(0);
   float wsum = 0;

   for (int x_offset = -radius; x_offset <= radius; ++x_offset)
   {
      vec2 offset = blurDirection * x_offset / size;
      _sample = texture(text, _uv + offset);
      float _sampleDepth = _sample.x;
      float depthDifference = (centerDepth - _sampleDepth)* far * bilateralDepthFactor;

      if(_sample.x != 1){
         float depthWeight = exp(-depthDifference * depthDifference);
         float gausWeight = gaussian(x_offset, sigma);
         float totalWeight = depthWeight * gausWeight;

         wsum += totalWeight;
         sum += _sample * totalWeight;
      }
   }

   if(wsum > 0.0){
      return sum/wsum;
   }
   return sum;
}

vec4 GaussianBlur(vec2 _uv, sampler2D text, int radius, float sigma){
   vec4 result = vec4(0.0);
   vec2 size = textureSize(text, 0);
   float total_weight = 0;
   /*INFINIT DEPTH*/
   vec4 _sample = texture(text, _uv);
   if(_sample.x == 1){
      return _sample;
   }

   for (int x_offset = -radius; x_offset <= radius; ++x_offset)
   {
      vec2 offset = blurDirection * x_offset / size;
      float weight = gaussian(x_offset, sigma);
      _sample = texture(text, _uv + offset);
      if(_sample.x != 1){
         total_weight += weight;
         result += _sample * weight;
      }
   }

   return vec4(result.xyz/total_weight,1); //normalization because of missing samples
}

void firstPass(){
   float depth = texture(particlesColorTexture, uv).x;
   float thicknes = texture(particlesThicknesTexture, uv).y;
   if(texture(particlesDepthTexture, uv).x == 1.0f) //must be like this!!! don't use depth from particlesColorTexture...thats never one
   {
      FragColor = vec4(1.0,thicknes, 0, 1);
   }else {
      FragColor = vec4(depth, thicknes, 0, 1);
   }
}

void secondPass(){
   if(seletedBlur == 0){
      float scSigma = screenSigma(gaussSigma, LinearizeDepth(texture(particlesDepthTexture, uv).x));
      int scRadius = min(int(3*scSigma),30);
      FragColor = GaussianBlur(uv, particlesColorTexture, scRadius, scSigma);
   }else if(seletedBlur == 1){
      float scSigma = screenSigma(bilateralSigma, LinearizeDepth(texture(particlesDepthTexture, uv).x));
      int scRadius = min(int(3*scSigma),30);
      FragColor = BilateralFilter(uv, particlesColorTexture, scRadius, scSigma);
   }else if(seletedBlur == 2){
      float scSigma = screenSigma(bilateralSigma, LinearizeDepth(texture(particlesDepthTexture, uv).x));
      int scRadius = min(int(3*scSigma),30);

      float delta = 5*sphereRadius;
      float mi = sphereRadius;

      FragColor = NarrowRangeFilter(uv, particlesColorTexture, scRadius, scSigma, delta, mi);
   }else{
      FragColor = texture(particlesColorTexture, uv);
   }
}

void combinationPass(){
   if(min(LinearizeDepth(texture(sceneDepthTexture, uv).x)/(far),1.0f) <= texture(particlesColorTexture, uv).x){
      FragColor = texture(sceneColorTexture, uv);
   }else{
      vec3 posCentre = WorldPos(uv);

      vec3 ddx = WorldPos(uv+vec2(texelsize.x, 0)) - posCentre;
      vec3 ddx2 = posCentre - WorldPos(uv+vec2(-texelsize.x, 0));
      if(abs(ddx.z) > abs(ddx2.z)){
         ddx = ddx2;
      }

      vec3 ddy = WorldPos(uv+vec2(0, texelsize.y)) - posCentre;
      vec3 ddy2 = posCentre - WorldPos(uv+vec2(0, -texelsize.y));
      if(abs(ddy.z) > abs(ddy2.z)){
         ddy = ddy2;
      }

      vec3 worldNormal = normalize(cross(ddx, ddy));
      vec3 color = vec3(0);

      if(renderFluid){
         float thicknes = texture(particlesColorTexture, uv).y;
         color = exp(-k * 2 * thicknes);

         color *= texture(sceneColorTexture, uv + worldNormal.xy * thicknes).xyz;
      }else{
         color = worldNormal;
      }

      FragColor = vec4(color, 1);
   }

}

void main()
{
   if(STAGE_NUMBER == 0){
      firstPass();
   }else if(STAGE_NUMBER == 1){
      secondPass();
   }else if(STAGE_NUMBER == 2){
      secondPass();
   }else if(STAGE_NUMBER == 3){
      combinationPass();
   }
}