#pragma once

#include <kame/math/math.hpp>

#include <list>

namespace kame::squirtle {

struct Node {
    kame::math::Vector3 position = kame::math::Vector3::zero();
    kame::math::Vector3 scale = kame::math::Vector3::one();
    kame::math::Quaternion rotation = kame::math::Quaternion::identity();

    kame::math::Matrix localXForm = kame::math::Matrix::identity();
    kame::math::Matrix globalXForm = kame::math::Matrix::identity();

    Node* parent = nullptr;
    std::list<Node*> children;

    kame::math::Matrix updateLocalXForm()
    {
        localXForm = kame::math::Matrix::createScale(scale) * kame::math::Matrix::createFromQuaternion(rotation) * kame::math::Matrix::createTranslation(position);
        return localXForm;
    }

    void updateWorldXForm()
    {
        if (parent) {
            globalXForm = parent->globalXForm * updateLocalXForm();
        }
        else {
            globalXForm = updateLocalXForm();
        }

        for (auto i : children) {
            i->updateWorldXForm();
        }
    }
};

} // namespace kame::squirtle
