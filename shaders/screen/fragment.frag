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

uniform vec2 texelsize = vec2(1,1);

const int SAMPLE_COUNT = 30;

const float OFFSETS[30] = float[30](
        -28.499287551080844,
        -26.499337550956973,
        -24.499387548996058,
        -22.499437544767737,
        -20.499487537782343,
        -18.49953752748464,
        -16.499587513246524,
        -14.49963749435975,
        -12.499687470027371,
        -10.499737439354503,
        -8.499787401338647,
        -6.4998373548584,
        -4.499887298661818,
        -2.4999372313531225,
        -0.49998718135373565,
        1.4999621930526983,
        3.499912266489702,
        5.49986232805838,
        7.499812379231655,
        9.499762421331635,
        11.499712455543497,
        13.499662482927798,
        15.499612504431878,
        17.49956252090044,
        19.49951253308502,
        21.499462541652775,
        23.499412547194407,
        25.499362550231684,
        27.499312551223795,
        29
);

const float WEIGHTS[30] = float[30](
        0.03302145587301725,
        0.033203556217610085,
        0.033373309362281625,
        0.03353051661845077,
        0.03367499354061956,
        0.03380657030682477,
        0.033925092073096436,
        0.03403041930111521,
        0.034122428058238376,
        0.034201010289228936,
        0.03426607405901285,
        0.034317543765964784,
        0.034355360325249315,
        0.03437948132181533,
        0.03438987907109618,
        0.0343863973182838,
        0.034369134765091784,
        0.03433816173228983,
        0.03429351225219843,
        0.03423523709198431,
        0.034163403615902505,
        0.034078095617106466,
        0.033979413119259644,
        0.03386747214847555,
        0.033742404475986074,
        0.03360435733217827,
        0.03345349309267747,
        0.033289988937163506,
        0.033114036481817946,
        0.016487201835962927
);


void firstPass(){
   vec4 result = vec4(0.0);
   vec2 size = textureSize(particlesColorTexture, 0);
   for (int i = 0; i < SAMPLE_COUNT; ++i)
   {
      vec2 offset = blurDirection * OFFSETS[i] / size;
      float weight = WEIGHTS[i];
      result += texture(particlesColorTexture, uv + offset) * weight;
   }

   //FragColor = result;
   FragColor = texture(particlesColorTexture, uv);
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

   //FragColor = result;
   FragColor = texture(particlesColorTexture, uv);
}

void combinationPass(){
   float depth = texture(particlesDepthTexture, uv).x;


   if(texture(sceneDepthTexture, uv).x <= texture(particlesDepthTexture, uv).x){
      FragColor = texture(sceneColorTexture, uv);
   }else{
      vec3 posEye = (inverse(Projection)*vec4(uv, 1, 1) * texture(particlesDepthTexture, uv).x).xyz;

      vec3 ddx = ((inverse(Projection)*vec4(uv+texelsize.x*vec2(1,0), 1, 1)) * texture(particlesDepthTexture, uv + texelsize.x*vec2(1,0)).x).xyz - posEye;
      vec3 ddx2 = posEye - ((inverse(Projection)*vec4(uv+texelsize.x*vec2(-1,0), 1, 1)) * texture(particlesDepthTexture, uv + texelsize.x*vec2(-1,0)).x).xyz;

      if(abs(ddx.z) > abs(ddx2.z)){
         ddx = ddx2;
      }

      vec3 ddy = ((inverse(Projection)*vec4(uv+texelsize.y*vec2(0,1), 1, 1)) * texture(particlesDepthTexture, uv + texelsize.y*vec2(0,1)).x).xyz - posEye;
      vec3 ddy2 = posEye - ((inverse(Projection)*vec4(uv+texelsize.y*vec2(0,-1), 1, 1)) * texture(particlesDepthTexture, uv + texelsize.y*vec2(0,-1)).x).xyz;

      if(abs(ddy.z) < abs(ddy2.z)){
         ddy = ddy2;
      }

      vec3 n = normalize(cross(ddx, ddy));
      FragColor = vec4(n, 1.0);
      //FragColor = texture(particlesColorTexture, uv);
      //zFragColor = vec4(texture(particlesDepthTexture, uv).x,texture(particlesDepthTexture, uv).x, texture(particlesDepthTexture, uv).x, 1);
   }
}

void main()
{
   if(STAGE_NUMBER == 0){
      firstPass();
   }else if(STAGE_NUMBER == 1){
      secondPass();
   }else if(STAGE_NUMBER == 2){
      combinationPass();
   }
}