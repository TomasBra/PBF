//
// Created by brabect on 17.10.26.
//

#ifndef POSITIONBASEDFLUIDS_UTILS_H
#define POSITIONBASEDFLUIDS_UTILS_H
#include <memory>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"


class Utils {
public:
    static std::shared_ptr<std::vector<float>> combine(std::shared_ptr<std::vector<glm::vec4>> vercities, std::shared_ptr<std::vector<glm::vec2>> uvs);
};


#endif //POSITIONBASEDFLUIDS_UTILS_H
