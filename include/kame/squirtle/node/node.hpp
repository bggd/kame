#pragma once

#include <kame/math/math.hpp>

#include <list>
#include <cassert>

enum SquirtleNodeType {
    kSquirtleNode = 0,
    kSquirtleCameraNode,
    kSquirtleMeshNode,
    kSquirtleLightNode,
    kSquirtleGltfNode
};

enum SquirtleRotationMode {
    kSquirtleRotationModeXYZEuler,
    kSquirtleRotationModeQuaternion
};

namespace kame::squirtle {

struct Node {

    Node* parent = nullptr;
    std::list<Node*> children;

    kame::math::Vector3f location = kame::math::Vector3f::zero();
    kame::math::Vector3f rotation = kame::math::Vector3f::zero(); // euler rotation order is x,y,z
    kame::math::Vector3f scale = kame::math::Vector3f::one();

    kame::math::Quaternion quatRotation = kame::math::Quaternion::identity();

    SquirtleRotationMode rotationMode = kSquirtleRotationModeXYZEuler;

    kame::math::Matrix4x4f localTransform = kame::math::Matrix4x4f::identity();
    kame::math::Matrix4x4f globalTransform = kame::math::Matrix4x4f::identity();

    virtual SquirtleNodeType getType() const
    {
        return kSquirtleNode;
    }

    std::list<kame::squirtle::Node*>& getChildren()
    {
        return children;
    }

    void addChild(kame::squirtle::Node* n)
    {
        children.push_back(n);
        n->parent = this;
    }

    kame::math::Vector3f getLocation()
    {
        return location;
    }

    kame::math::Vector3f getGlobalLocation()
    {
        return kame::math::Vector3f(globalTransform.m41, globalTransform.m42, globalTransform.m43);
    }

    void setLocation(kame::math::Vector3f v)
    {
        location = v;
    }

    void setLocation(float x, float y, float z)
    {
        location = kame::math::Vector3f(x, y, z);
    }

    kame::math::Vector3f getRotation()
    {
        return rotation;
    }

    void setRotation(kame::math::Vector3f v)
    {
        rotation = v;
    }

    void setRotation(float x, float y, float z)
    {
        rotation = kame::math::Vector3f(x, y, z);
    }

    kame::math::Vector3f getScale()
    {
        return scale;
    }

    void setScale(kame::math::Vector3f v)
    {
        scale = v;
    }

    void setScale(float x, float y, float z)
    {
        scale = kame::math::Vector3f(x, y, z);
    }

    kame::math::Quaternion getQuatRotation()
    {
        return quatRotation;
    }

    void setQuatRotation(kame::math::Quaternion quat)
    {
        quatRotation = quat;
    }

    void setQuatRotation(float x, float y, float z, float w)
    {
        quatRotation = kame::math::Quaternion(x, y, z, w);
    }

    kame::math::Matrix4x4f getLocalTransform()
    {
        if (rotationMode == kSquirtleRotationModeXYZEuler)
        {
            kame::math::Matrix4x4f xformX = kame::math::Matrix4x4f::createRotationX(kame::math::helper::toRadians(rotation.x));
            kame::math::Matrix4x4f xformY = kame::math::Matrix4x4f::createRotationY(kame::math::helper::toRadians(rotation.y));
            kame::math::Matrix4x4f xformZ = kame::math::Matrix4x4f::createRotationZ(kame::math::helper::toRadians(rotation.z));

            kame::math::Matrix4x4f R = xformX * xformY * xformZ;
            localTransform = kame::math::Matrix4x4f::createScale(scale) * R * kame::math::Matrix4x4f::createTranslation(location);

            return localTransform;
        }
        else if (rotationMode == kSquirtleRotationModeQuaternion)
        {
            kame::math::Matrix4x4f R = kame::math::Matrix4x4f::CreateFromQuaternion(quatRotation);
            localTransform = kame::math::Matrix4x4f::createScale(scale) * R * kame::math::Matrix4x4f::createTranslation(location);
            return localTransform;
        }

        assert(rotationMode == kSquirtleRotationModeXYZEuler || rotationMode == kSquirtleRotationModeQuaternion);
    }

    kame::math::Matrix4x4f getGlobalTransform()
    {
        return globalTransform;
    }

    void setGlobalTransform(const kame::math::Matrix4x4f& m)
    {
        globalTransform = m;
    }

    virtual void onUpdate(float dt){};
};

} // namespace kame::squirtle
