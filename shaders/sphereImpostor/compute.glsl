#version 430 core
layout (local_size_x = 32) in;

const float PI = 3.1415926535897932384626433832795;
uniform float REST_DENSITY;
uniform float r; //gridSpace...kernel_radius
uniform float relaxation;
uniform bool artificial_pressure;
uniform float ar;
uniform float k;
uniform int n;
uniform float q_size;

//kg/m^3
//radius in which the particles are classified as neighbours

uniform float dt;
uniform float sphereRadius;
uniform vec3 bbCenter;//boundingBox center
uniform vec3 bbSize;//boundingBox size
uniform int totalParticlesAmount;
uniform int STAGE_NUMBER;


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

//size = totalParticlesAmount
layout(std430, binding=4) buffer Graph {
    uint Graphics[]; //x = density
};

//size = totalParticlesAmount + 1
layout(std430, binding=5) buffer GridTableBuff {
    uint GridTable[];
};

//size = totalParticlesAmount
layout(std430, binding=6) buffer GridIndicesBuff {
    uint GridIndices[];
};





uint[1024*5] tempNeighbours;
int neighbours_count = 0;




ivec3 GetCell(vec3 pos) {
    return ivec3(floor(pos / r));
}

uint Hash(ivec3 cell) {
    return (uint(cell.x) * 92837111u) ^
    (uint(cell.y) * 689287499u) ^
    (uint(cell.z) * 283923481u);
}

uint GetTableIndex(vec3 pos) {
    return Hash(GetCell(pos)) % uint(totalParticlesAmount);
}


//runs once on every thread
void initHashGrid(uint particleIndex){
    uint particleArrayIndex = GetTableIndex(NewPositions[particleIndex].xyz);
    atomicAdd(GridTable[particleArrayIndex], 1);
}

//runs once on single thread!!! BEWARE
void prefixSumsGrid(){
    //because of GridTable size
    for (int i = 1; i < totalParticlesAmount+1; i++) {
        GridTable[i] = GridTable[i] + GridTable[i-1];
    }
}

//runs once on every thread
void fillingHashGrid(uint particleIndex){
    //particlesArray - filling
    uint particleArrayIndex = GetTableIndex(NewPositions[particleIndex].xyz);
    uint offset = atomicAdd(GridTable[particleArrayIndex], uint(-1)) - 1u;
    GridIndices[offset] = particleIndex;
}


//fills temp neighbours with neighbours of particle with index particleIndex
void PrepareNeighboursParticles(int particleIndex, inout int neighbours_count) {
    vec3 particlePosition = NewPositions[particleIndex].xyz;

    uint visited[27];
    int visitedCount = 0;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {

                ivec3 cell = GetCell(particlePosition);
                ivec3 neighbourCell = cell + ivec3(x,y,z);

                uint tableIndex = Hash(neighbourCell) % uint(totalParticlesAmount);

                bool alreadyVisited = false;

                for (int v = 0; v < visitedCount; v++) {
                    if (visited[v] == tableIndex) {
                        alreadyVisited = true;
                        break;
                    }
                }

                if (alreadyVisited){
                    continue;
                }

                visited[visitedCount] = tableIndex;
                visitedCount++;

                uint first = GridTable[tableIndex];
                uint last = GridTable[tableIndex + 1];

                for (uint i = first; i < last; i++) {

                    uint neighbourIndex = GridIndices[i]; //index of neighbouring particle

                    if (neighbourIndex == uint(particleIndex)){
                        continue;
                    }

                    vec3 neighbourPosition = NewPositions[neighbourIndex].xyz;

                    vec3 delta = particlePosition - neighbourPosition;

                    if (dot(delta, delta) >= r * r){
                        continue;
                    }

                    tempNeighbours[neighbours_count] = neighbourIndex;
                    neighbours_count++;
                }
            }
        }
    }
}



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
    NewPositions[i] -= friction*vec4(correction*dir, 0);
    NewPositions[j] += friction*vec4(correction*dir, 0);
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

    if (r1 <= 1e-6 || r1 >= h){
        return vec3(0);
    }

    float coeff = -45.0f/PI;

    float h3 = pow(h,3);
    float h6 = pow(h, 6);
    float d = h - r1;
    float d2 = d*d;

    return (coeff/h6)*d2*deltaPosition/r1; //direction of deltaPosition
}

//ok
float incompressibilityLambdaCalc(uint idx){
    //Ri = sum j over(poly6Kernel(pi-pj))
    float Ri = poly6Kernel(vec3(0), r);;
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

    for(uint j = 0; j < neighbours_count;j++ ){
        uint neighbour_index = tempNeighbours[j];

        if(idx != neighbour_index){
            vec3 deltaPosition = (NewPositions[idx]-NewPositions[neighbour_index]).xyz;
            if(dot(deltaPosition, deltaPosition) < r*r){
                //idx and neighbour_index particle are neigbours
                Ri += poly6Kernel(deltaPosition, r);

                vec3 nablaW = spikyKernelGradient(deltaPosition, r)/REST_DENSITY;

                Ki += nablaW;
                Kj += dot(-nablaW, -nablaW);
            }
        }
    }

    /*
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
    }*/

    //Graphics[idx].x = Ri;

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


    for(uint j = 0; j < neighbours_count;j++ ){
        uint neighbour_index = tempNeighbours[j];

        if(idx != neighbour_index){
            localDeltaPosition = (NewPositions[idx]-NewPositions[neighbour_index]).xyz;
            //neighbour
            if(dot(localDeltaPosition, localDeltaPosition) < r*r){
                float denominator = poly6Kernel(vec3(q_size, 0, 0), ar);

                if (artificial_pressure && denominator > 1e-12) {
                    s_corr = -k * pow(poly6Kernel(localDeltaPosition, ar) / denominator, n);
                }

                idxDeltaPosition += (lambda_idx + Utilities[neighbour_index].w + s_corr) * spikyKernelGradient(localDeltaPosition, r);
            }
        }
    }

    /*
    //over all particles
    for(int j = 0; j<totalParticlesAmount; j++){
        if(idx != j){
            localDeltaPosition = (NewPositions[idx]-NewPositions[j]).xyz;
            //neighbour
            if(dot(localDeltaPosition, localDeltaPosition) < r*r){
                if(artificial_pressure){
                    s_corr = -k * pow((poly6Kernel(localDeltaPosition, ar)) / (poly6Kernel(vec3(q_size, 0, 0),ar)), n);
                }

                idxDeltaPosition += (lambda_idx + Utilities[j].w + s_corr) * spikyKernelGradient(localDeltaPosition, r);
            }
        }
    }*/

    return idxDeltaPosition/REST_DENSITY;
}

void main() {
    idx = gl_GlobalInvocationID.x;

    if (idx >= uint(totalParticlesAmount))
        return;

    if(STAGE_NUMBER == 0){
        //STAGE 0 gueses the values
        Velocities[idx] = vec4(Velocities[idx].xyz + g * dt, 0);
        NewPositions[idx] = vec4(Positions[idx].xyz + Velocities[idx].xyz * dt, 1);
    }
    //HASHGRID INIT
    else if(STAGE_NUMBER == 1){
        initHashGrid(idx);
    }else if(STAGE_NUMBER == 2){
        if (idx == 0) {
            prefixSumsGrid();
        }
    }else if(STAGE_NUMBER == 3){
        fillingHashGrid(idx);
    }
    //HASHGRID INIT END

    else if(STAGE_NUMBER == 4){
        PrepareNeighboursParticles(int(idx), neighbours_count);
        Utilities[idx].w = incompressibilityLambdaCalc(idx);
        //calculate lambda_idx
    }else if(STAGE_NUMBER == 5){
        PrepareNeighboursParticles(int(idx), neighbours_count);
        //calculate p_idx
        Utilities[idx].xyz = incompressibilityDeltaPositionCalc(idx);
    }else if(STAGE_NUMBER == 6){
        NewPositions[idx] += vec4(Utilities[idx].xyz,0);
    }else if(STAGE_NUMBER == 7){
        Velocities[idx] = (NewPositions[idx] - Positions[idx])/dt;
        Positions[idx] = NewPositions[idx];
    }else if(STAGE_NUMBER == 8){
        boxCollision(idx);

        /*for(uint j = 0; j < neighbours_count;j++ ){
            particlesCollision(idx, tempNeighbours[j]);
        }*/

        /*for (uint j = 0; j < totalParticlesAmount; j++){
            if(j != idx){
                particlesCollision(idx, j);
            }
        }*/



        Positions[idx] = NewPositions[idx];
    }
}