//
// Created by brabect on 28.10.26.
//

#ifndef POSITIONBASEDFLUIDS_HASHGRID_H
#define POSITIONBASEDFLUIDS_HASHGRID_H
#include <array>
#include <chrono>
#include <stdexcept>

#include "Camera.h"
#include "glm/vec4.hpp"

///First try was to build hash grid on cpu and then move it to gpu, but the movement it revealed to be very expensive
class HashGrid {

public:
    std::vector<unsigned int> tableArray;
    std::vector<unsigned int> particlesIndices;
    std::vector<glm::vec4> particlesArray;

    HashGrid(int _particlesCount, float _gridSpace);

    unsigned int Hash(glm::vec4 pos);
    unsigned int GetTableIndex(glm::vec4 pos);

    void Build(std::vector<glm::vec4> particles, float _gridSpace);

    std::vector<unsigned int> GetNeighboursParticles(glm::vec4 position, float maxDistance);
    std::vector<unsigned int> GetGridCellParticles(glm::vec4 position);

    int particlesCount;
    float gridSpace;
private:

};


#endif //POSITIONBASEDFLUIDS_HASHGRID_H
