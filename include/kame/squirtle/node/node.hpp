#pragma once

#include <kame/math/math.hpp>

#include <list>

enum SquirtleNodeType {
    kSquirtleNode = 0,
    kSquirtleCameraNode,
    kSquirtleMeshNode,
    kSquirtleLightNode
};

namespace kame::squirtle {

struct Node {

    Node* parent = nullptr;
    std::list<Node*> children;

    kame::math::Vector3f location = kame::math::Vector3f::zero();
    kame::math::Vector3f rotation = kame::math::Vector3f::zero(); // euler rotation order is x,y,z
    kame::math::Vector3f scale = kame::math::Vector3f::one();

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

    kame::math::Vector3f getRotation()
    {
        return rotation;
    }

    void setRotation(kame::math::Vector3f v)
    {
        rotation = v;
    }

    kame::math::Vector3f getScale()
    {
        return scale;
    }

    void setScale(kame::math::Vector3f v)
    {
        scale = v;
    }

    kame::math::Matrix4x4f getLocalTransform()
    {

        kame::math::Matrix4x4f xformX = kame::math::Matrix4x4f::createRotationX(kame::math::helper::toRadians(rotation.x));
        kame::math::Matrix4x4f xformY = kame::math::Matrix4x4f::createRotationY(kame::math::helper::toRadians(rotation.y));
        kame::math::Matrix4x4f xformZ = kame::math::Matrix4x4f::createRotationZ(kame::math::helper::toRadians(rotation.z));

        kame::math::Matrix4x4f R = xformX * xformY * xformZ;
        localTransform = kame::math::Matrix4x4f::createScale(scale) * R * kame::math::Matrix4x4f::createTranslation(location);

        return localTransform;
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
