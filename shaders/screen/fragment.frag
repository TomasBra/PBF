#version 430 core
out vec4 FragColor;

uniform int STAGE_NUMBER = 0;
uniform vec2 blurDirection = vec2(1,0);

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform sampler2D sceneColorTexture;
uniform sampler2D sceneDepthTexture;
uniform sampler2D particlesColorTexture;
uniform sampler2D particlesDepthTexture;

in vec2 uv;

uniform vec2 texelsize = vec2(1,0);

const float near = 0.1f;
const float far = 100.0f;

const int SAMPLE_COUNT = 21;

const float OFFSETS[21] = float[21](
        -19.451443690876673,
        -17.4563973459256,
        -15.461359605469259,
        -13.466329392628689,
        -11.47130567957191,
        -9.476287534090357,
        -7.481274151572068,
        -5.486264802951924,
        -3.491258516565368,
        -1.4962530470023068,
        0.4987498861131981,
        2.493755926496706,
        4.48876136715687,
        6.483769021125497,
        8.478780293057468,
        10.473795964235448,
        12.468816784674702,
        14.463843624018438,
        16.458877468078487,
        18.4539193747561,
        20
);

const float WEIGHTS[21] = float[21](
        0.012478241324905347,
        0.018043032550019177,
        0.025069773986982367,
        0.033471596868284194,
        0.042942589993714204,
        0.0529403102498448,
        0.06271499876166393,
        0.07139083819110861,
        0.07809055393495573,
        0.08208052011726609,
        0.08290289995204973,
        0.08046061245882896,
        0.07503863600939073,
        0.06724674280737294,
        0.05790860824683086,
        0.04791831325702582,
        0.03810189398995028,
        0.029112382389044004,
        0.0213744214175789,
        0.015079810173818195,
        0.00563322331936511
);




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

void firstPass(){
   float depth = texture(particlesColorTexture, uv).x;
   if(depth == 0){
      FragColor = vec4(1,0,0,1);
   }else {
      FragColor = vec4(depth,0,0,1);
   }
}

void secondPass(){
   vec4 result = vec4(0.0);
   vec2 size = textureSize(particlesColorTexture, 0);
   for (int i = 0; i < SAMPLE_COUNT; ++i)
   {
      vec2 offset = blurDirection * OFFSETS[i] / size;
      float weight = WEIGHTS[i];
      result += texture(particlesColorTexture, uv + offset) * weight;
   }

   FragColor = result;
   //FragColor = texture(particlesColorTexture, uv);
}

void thirdPass(){
   vec4 result = vec4(0.0);
   vec2 size = textureSize(particlesColorTexture, 0);
   for (int i = 0; i < SAMPLE_COUNT; ++i)
   {
      vec2 offset = blurDirection * OFFSETS[i] / size;
      float weight = WEIGHTS[i];
      result += texture(particlesColorTexture, uv + offset) * weight;
   }

   FragColor = result;
   //FragColor = texture(particlesColorTexture, uv);
}

void combinationPass(){
   if(min(LinearizeDepth(texture(sceneDepthTexture, uv).x)*5/far,1.0f) <= texture(particlesColorTexture, uv).x + 0.000001){
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
      FragColor = vec4(worldNormal, 1);

      float color = texture(particlesColorTexture, uv).x;
      FragColor = vec4(color, color, color, 1);
   }
}

void main()
{
   if(STAGE_NUMBER == 0){
      firstPass();
   }else if(STAGE_NUMBER == 1){
      secondPass();
   }else if(STAGE_NUMBER == 2){
      thirdPass();
   }else if(STAGE_NUMBER == 3){
      combinationPass();
   }
}