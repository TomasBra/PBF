//
// Created by brabect on 28.10.26.
//

#include "HashGrid.h"
HashGrid::HashGrid(int _particlesCount, float _gridSpace): particlesCount(_particlesCount), gridSpace(_gridSpace) {
    this->particlesArray.resize(_particlesCount);
    this->particlesIndices.resize(_particlesCount);
    this->tableArray.resize(_particlesCount + 1); //should work well according Mathias Muller a.k.a. PBF creator
}

void HashGrid::Build(std::vector<glm::vec4> particles, float _gridSpace) {
    if (particles.size() > particlesCount) {
        throw std::runtime_error("Too many particles in HashGrid");
    }

    gridSpace = _gridSpace;

    this->particlesArray.clear();
    std::fill(particlesIndices.begin(), particlesIndices.end(), 0);
    std::fill(tableArray.begin(), tableArray.end(), 0);

    this->particlesArray.resize(particlesCount);

    particlesArray = particles;

    //init
    for (int i = 0; i < particles.size(); i++) {
        int particleArrayIndex = GetTableIndex(particles[i]);
        tableArray[particleArrayIndex]++;
    }

    //partial sums
    for (int i = 1; i < tableArray.size(); i++) {
        tableArray[i] = tableArray[i] + tableArray[i-1];
    }

    //particlesArray - filling
    for (int i = 0; i < particles.size(); i++) {
        int particleArrayIndex = GetTableIndex(particles[i]);
        tableArray[particleArrayIndex]--;
        particlesIndices[tableArray[particleArrayIndex]] = i;
    }
}


std::vector<unsigned int> HashGrid::GetNeighboursParticles(glm::vec4 position, float maxDistance) {
    std::vector<unsigned int> result;
    std::vector<unsigned int> possible_neighbours;
    std::vector<int> vissited;

    for (int x_offset = -1; x_offset < 2; x_offset++) {
        for (int y_offset = -1; y_offset < 2; y_offset++) {
            for (int z_offset = -1; z_offset < 2; z_offset++) {
                int index = GetTableIndex(position + gridSpace*glm::vec4(x_offset, y_offset, z_offset,0));
                if (std::find(vissited.begin(),vissited.end(),index) == vissited.end()) {
                    auto temp = GetGridCellParticles(position + gridSpace*glm::vec4(x_offset, y_offset, z_offset,0));
                    possible_neighbours.reserve(possible_neighbours.size()+temp.size());
                    possible_neighbours.insert(possible_neighbours.end(),temp.begin(), temp.end());
                    vissited.push_back(index);
                }
            }
        }
    }

    result.reserve(possible_neighbours.size());
    for (int i = 0; i < possible_neighbours.size(); i++) {
        if (glm::distance(position,particlesArray[possible_neighbours[i]]) < maxDistance) {
            result.push_back(possible_neighbours[i]);
        }
    }


    return result;
}


std::vector<unsigned int> HashGrid::GetGridCellParticles(glm::vec4 position) {
    std::vector<unsigned int> result;

    int table_index = GetTableIndex(position);
    int first_cell_particle_index = tableArray[table_index];
    int last_cell_particle_index = tableArray[table_index+1]; //check for array boundary not needed, cause of representation nature

    result.reserve(last_cell_particle_index-first_cell_particle_index);
    result = std::vector<unsigned int>(particlesIndices.begin()+first_cell_particle_index, particlesIndices.begin()+last_cell_particle_index);
    return result;
}


unsigned int HashGrid::Hash(glm::vec4 pos) {
    int x_i = std::floor(pos.x / gridSpace);
    int y_i = std::floor(pos.y / gridSpace);
    int z_i = std::floor(pos.z / gridSpace);

    return (x_i * 92837111u) ^ (y_i * 689287499u) ^ (z_i * 283923481u);
}


unsigned int HashGrid::GetTableIndex(glm::vec4 pos) {
    return Hash(glm::vec4(pos))%(tableArray.size()-1);
}