//
// Created by brabect on 17.10.26.
//

#include "Utils.h"

#include <memory>
#include <stdexcept>

std::shared_ptr<std::vector<float>> Utils::combine(std::shared_ptr<std::vector<glm::vec4>> vercities, std::shared_ptr<std::vector<glm::vec2>> uvs) {
    std::vector<float> result;
    if (vercities->size() != uvs->size()) {
        throw std::runtime_error("vertices and uvs are not the same size thus cannot be combined");
    }

    result.reserve(vercities->size()*4+uvs->size()*2);
    for (size_t i = 0; i < vercities->size(); i++) {
        result.insert(result.end(),(float*)vercities->data()+i, (float*)vercities->data()+i+4);
        result.insert(result.end(),(float*)uvs->data()+i, (float*)uvs->data()+i+2);
    }

    return std::make_shared<std::vector<float>>(result);
}
