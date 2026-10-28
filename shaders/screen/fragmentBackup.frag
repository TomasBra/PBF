#version 430 core
out vec4 FragColor;

uniform bool renderFluid = true;
uniform bool blurFilter = true;
uniform float depthFactor = 1.0f;
uniform int seletedBlur = 0;

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

in vec2 uv;

uniform vec2 texelsize = vec2(1,0);

const float near = 0.1f;
const float far = 100.0f;

const float PI = 3.1415926535897932384626433832795;

const vec3 k = vec3(0.9, 0.5, 0.1);

const int SAMPLE_COUNT = 31;

const float OFFSETS[31] = float[31](
        -15,
        -14,
        -13,
        -12,
        -11,
        -10,
        -9,
        -8,
        -7,
        -6,
        -5,
        -4,
        -3,
        -2,
        -1,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15
);

const float WEIGHTS[31] = float[31](
        0.03225338735356131,
        0.032254322715357234,
        0.03225519359382736,
        0.032255999983747036,
        0.03225674188027844,
        0.032257419278970585,
        0.032258032175759385,
        0.03225858056696769,
        0.032259064449305305,
        0.032259483819869046,
        0.03225983867614277,
        0.03226012901599738,
        0.03226035483769087,
        0.032260516139868314,
        0.032260612921561904,
        0.032260645182190954,
        0.032260612921561904,
        0.032260516139868314,
        0.03226035483769087,
        0.03226012901599738,
        0.03225983867614277,
        0.032259483819869046,
        0.032259064449305305,
        0.03225858056696769,
        0.032258032175759385,
        0.032257419278970585,
        0.03225674188027844,
        0.032255999983747036,
        0.03225519359382736,
        0.032254322715357234,
        0.03225338735356131
);







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
   float depth = texture(particlesColorTexture, uv).x;
   vec3 viewVector = (inverse(Projection) * vec4(uv*2.0-1.0, 1, 1)).xyz;

   return normalize(viewVector)*depth;
}

vec3 WorldPos(vec2 uv){
   vec3 viewPos = ViewPos(uv);

   return (inverse(View)*vec4(viewPos, 0)).xyz;
}

vec4 BilateralFilter(vec2 _uv, sampler2D text){
   vec4 _sample = texture(text, uv);
   float centerDepth = _sample.x;

   if(_sample.x == 1){
      return _sample;
   }

   vec2 size = textureSize(text, 0);

   vec4 sum = vec4(0);
   float wsum = 0;

   for (int i = 0; i < SAMPLE_COUNT; ++i)
   {
      vec2 offset = blurDirection * OFFSETS[i] / size;
      _sample = texture(text, _uv + offset);
      float _sampleDepth = _sample.x;
      float depthDifference = (centerDepth - _sampleDepth) * depthFactor;

      if(_sample.x != 1){
         float depthWeight = exp(-depthDifference * depthDifference);
         float gausWeight = WEIGHTS[i];
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

vec4 GaussianBlur(vec2 _uv, sampler2D text){
   vec4 result = vec4(0.0);
   vec2 size = textureSize(text, 0);
   float total_weight = 0;
   /*INFINIT DEPTH*/
   vec4 _sample = texture(text, _uv);
   if(_sample.x == 1){
      return _sample;
   }

   for (int i = 0; i < SAMPLE_COUNT; ++i)
   {
      vec2 offset = blurDirection * OFFSETS[i] / size;
      float weight = WEIGHTS[i];
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
   float thicknes = texture(particlesThicknesTexture, uv).x;
   if(depth == 0){
      FragColor = vec4(1,thicknes, 0, 1);
   }else {
      FragColor = vec4(depth, thicknes, 0, 1);
   }
}

void secondPass(){
   if(seletedBlur == 0){
      FragColor = GaussianBlur(uv, particlesColorTexture);
   }else if(seletedBlur == 1){
      FragColor = BilateralFilter(uv, particlesColorTexture);
   }else{
      FragColor = texture(particlesColorTexture, uv);
   }
}

void combinationPass(){
   if(min(LinearizeDepth(texture(sceneDepthTexture, uv).x)/(10*far),1.0f) <= texture(particlesColorTexture, uv).x){
      FragColor = texture(sceneColorTexture, uv);
   }else{
      vec3 posCentre = WorldPos(uv);

      vec3 ddx = WorldPos(uv+vec2(texelsize.x, 0)) - posCentre;
      vec3 ddx2 = posCentre - WorldPos(uv+vec2(-texelsize.x, 0));
      if(abs(ddx.z) > abs(ddx2.z)){
         ddx = ddx2;
      }

      vec3 ddy = WorldPos(uv+vec2(0, texelsize.y)) - posCentre;
      vec3 ddy2 = posCentre - WorldPos(uv+vec2(0, texelsize.y));
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