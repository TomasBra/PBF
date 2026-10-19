#version 430 core
layout (local_size_x = 1) in;

const float PI = 3.1415926535897932384626433832795;
uniform float REST_DENSITY = 5500.0f;
uniform float r = 0.1f;
uniform float relaxation = 3500.0f;
uniform bool artificial_pressure = true;
uniform float ar = 0.1f;
uniform float k = 0.001f;
uniform int n = 4;
uniform float q_size = 0.1f;
uniform float ParticleMass = 1.0f;

//kg/m^3
//radius in which the particles are classified as neighbours

uniform float dt;
uniform float sphereRadius;
uniform vec3 bbCenter;//boundingBox center
uniform vec3 bbSize;//boundingBox size
uniform int totalParticlesAmount;
uniform int STAGE_NUMBER;


float ParticleInvMass = 1.0 / ParticleMass;
const vec3 g = vec3(0, -9.81, 0);
const float friction = 1.0f;

const vec3 normal_plane_xy = vec3(0,0,1);
const vec3 normal_plane_yz = vec3(1,0,0);
const vec3 normal_plane_xz = vec3(0,1,0);
vec3 collision_normal = vec3(0,0,0);
uint idx;


layout(std140, binding=0) buffer Pos {
    vec4 Positions[];
};

layout(std140, binding=1) buffer newPos {
    vec4 NewPositions[];
};

layout(std140, binding=2) buffer Vel {
    vec4 Velocities[];
};

layout(std140, binding=3) buffer Util {
    vec4 Utilities[]; //xyz = deltaPositions,w = lambda
};

layout(std140, binding=4) buffer Graph {
    vec4 Graphics[]; //x = density
};





void boxCollision(uint particleIndex){
    vec3 displacement = NewPositions[particleIndex].xyz - bbCenter;

    vec3 error = vec3(abs(displacement.x) + sphereRadius - bbSize.x, abs(displacement.y) + sphereRadius -bbSize.y, abs(displacement.z) + sphereRadius - bbSize.z);

    if(error.x > 0){
        collision_normal = normalize(vec3(-displacement.x/abs(displacement.x),0,0));
        Velocities[particleIndex] = vec4(reflect(Velocities[particleIndex].xyz, collision_normal), 0)*friction;
        NewPositions[particleIndex] += vec4(collision_normal * error.x,0);
    }

    if(error.y > 0){
        collision_normal = normalize(vec3(0,-displacement.y/abs(displacement.y),0));
        Velocities[particleIndex] = vec4(reflect(Velocities[particleIndex].xyz, collision_normal), 0)*friction;
        NewPositions[particleIndex] += vec4(collision_normal * error.y,0);
    }

    if(error.z > 0){
        collision_normal = normalize(vec3(0,0,-displacement.z/abs(displacement.z)));
        Velocities[particleIndex] = vec4(reflect(Velocities[particleIndex].xyz, collision_normal), 0)*friction;
        NewPositions[particleIndex] += vec4(collision_normal * error.z,0);
    }
}

//checks collisions between two particles and creates response if particles collide
//particles have same weight and radius
void particlesCollision(uint i, uint j){
    vec3 C_A = NewPositions[i].xyz;
    vec3 C_B = NewPositions[j].xyz;

    vec3 AB = C_B - C_A;
    float AB_length_sq = dot(AB,AB);

    vec3 dir = normalize(AB);

    if(AB_length_sq - 4*sphereRadius*sphereRadius > 0){
        return;
    }

    //always positive
    float correction = (2*sphereRadius-length(AB))/2;

    vec4 temp = Velocities[i];
    Velocities[i] = Velocities[j];
    Velocities[j] = temp;


    //Position corrections
    NewPositions[i] -= vec4(correction*dir, 0);
    NewPositions[j] += vec4(correction*dir, 0);
}

//With gradient calculations
void keepOnWire(uint idx, vec3 center, float radius){
    float Constraint = length(NewPositions[idx].xyz - center)-radius;
    if (Constraint == 0)
    return;

    vec3 ConstraintGraditent = normalize(NewPositions[idx].xyz - center);

    float lambda = -Constraint/dot(ConstraintGraditent, ConstraintGraditent);
    vec3 deltaPosition = ConstraintGraditent*lambda;
    NewPositions[idx] += vec4(deltaPosition, 0);
}


float poly6Kernel(vec3 deltaPosition, float h){
    float coeff = 315.0f/(64*PI);
    float r2 = dot(deltaPosition, deltaPosition);
    float h2 = h*h;
    float h9 = pow(h, 9);
    float d = h2-r2;
    float d3 = pow(d, 3);

    if(r2 > h2){
        return 0;
    }

    return (coeff/h9)*d3;
}

vec3 spikyKernelGradient(vec3 deltaPosition, float h){
    float deltaPositionLength = length(deltaPosition);

    float r1 = length(deltaPosition);

    if(r1 > h){
        return vec3(0);
    }

    float coeff = 45.0f/PI;

    float h3 = pow(h,3);
    float h6 = pow(h, 6);
    float d = h - r1;
    float d2 = d*d;

    return (coeff/h6)*d2*deltaPosition/r1; //direction of deltaPosition
}


float incompressibilityLambdaCalc(uint idx){
    //Ri = sum j over(poly6Kernel(pi-pj))
    float Ri = 0;
    //Ci = Ri/REST_DENSITY -1
    float Ci = 0;

    //gradient Ki += nablaW/REST_DENSITY
    vec3 Ki = vec3(0);
    //gradient Kj += |nablaW/REST_DENSITY|^2
    float Kj = 0;
    //gradient K = |Ki|^2 + Kj
    float K = 0;
    //lambda = - Ci / gradient K;
    float lambda = 0;

    for(int j = 0; j<totalParticlesAmount; j++){
        if(idx != j){
            vec3 deltaPosition = (NewPositions[idx]-NewPositions[j]).xyz;
            if(dot(deltaPosition, deltaPosition) < r*r){
                //idx and j particle are neigbours
                Ri += poly6Kernel(deltaPosition, r);

                vec3 nablaW = spikyKernelGradient(deltaPosition, r)/REST_DENSITY;

                Ki += nablaW;
                Kj += dot(-nablaW, -nablaW);
            }
        }
    }

    Graphics[idx].x = Ri;

    //gradient K = |Ki|^2 + Kj
    K = relaxation + dot(Ki,Ki) + Kj;
    //Ci = Ri/REST_DENSITY -1
    Ci = Ri/REST_DENSITY - 1;
    //lambda = - Ci / gradient K;
    lambda = -Ci/K;

    return lambda;
}

vec3 incompressibilityDeltaPositionCalc(uint idx){

    vec3 idxDeltaPosition = vec3(0);
    float lambda_idx = Utilities[idx].w;
    vec3 localDeltaPosition = vec3(0);
    float s_corr = 0;
    //over all particles
    for(int j = 0; j<totalParticlesAmount; j++){
        if(idx != j){
            localDeltaPosition = (NewPositions[idx]-NewPositions[j]).xyz;
            //neighbour
            if(dot(localDeltaPosition, localDeltaPosition) < r*r){
                if(artificial_pressure){
                    s_corr = -k * pow((poly6Kernel(localDeltaPosition, ar)) / (poly6Kernel(vec3(q_size * ar, 0, 0),ar)), n);
                }

                idxDeltaPosition += (lambda_idx + Utilities[j].w + s_corr) * spikyKernelGradient(localDeltaPosition, r);
            }
        }
    }

    return idxDeltaPosition/REST_DENSITY;
}

void main() {
    idx = gl_GlobalInvocationID.x;

    if(STAGE_NUMBER == 0){
        //STAGE 0 gueses the values
        Velocities[idx] = vec4(Velocities[idx].xyz + g * dt, 0);
        NewPositions[idx] = vec4(Positions[idx].xyz + Velocities[idx].xyz * dt, 1);

    } else if(STAGE_NUMBER == 1){
        Utilities[idx].w = incompressibilityLambdaCalc(idx);
        //calculate lambda_idx
    }else if(STAGE_NUMBER == 2){
        //calculate p_idx
        Utilities[idx].xyz = incompressibilityDeltaPositionCalc(idx);
    }else if(STAGE_NUMBER == 3){
        NewPositions[idx] += vec4(Utilities[idx].xyz,0);
    }else if(STAGE_NUMBER == 4){
        Velocities[idx] = (NewPositions[idx] - Positions[idx])/dt;
        Positions[idx] = NewPositions[idx];
    }else if(STAGE_NUMBER == 5){
        boxCollision(idx);

        /*
        for (uint j = 0; j < totalParticlesAmount; j++){
            if(j != idx){
                particlesCollision(idx, j);
            }
        }*/

        Positions[idx] = NewPositions[idx];
    }
}