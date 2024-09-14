#include <kame/ogl/ogl.hpp>
#include <kame/gltf/gltf.hpp>

#include <unordered_map>
#include <string>

namespace kame::squirtle {

namespace detail {

void loadAsset(std::string path, kame::ogl::Texture2D** pptr)
{
    *pptr = kame::ogl::loadTexture2D(path.c_str());
}

void loadAsset(std::string path, kame::gltf::Gltf** pptr)
{
    *pptr = kame::gltf::loadGLTF(path.c_str());
}

} // namespace detail

template <typename T>
struct AssetManager {
    std::unordered_map<std::string, T*> dict;

    void loadAsset(std::string path)
    {
        auto it = dict.find(path);
        if (it != dict.end())
        {
            return;
        }

        T* ptr = nullptr;

        kame::squirtle::detail::loadAsset(path, &ptr);
        dict[path] = ptr;
    }

    T* getAsset(std::string path)
    {
        auto it = dict.find(path);

        assert(it != dict.end());

        return it->second;
    }
};

} // namespace kame::squirtle
