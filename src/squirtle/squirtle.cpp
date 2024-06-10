#include <all.hpp>

namespace kame::squirtle {

char* loadFile(const char* fileName, int64_t& len)
{
    assert(fileName);

    len = 0;
    SDL_IOStream* io = SDL_IOFromFile(fileName, "rb");
    if (io == nullptr)
    {
        SPDLOG_CRITICAL("{}", SDL_GetError());
        return nullptr;
    }

    Sint64 res_size = SDL_GetIOSize(io);
    char* res = (char*)malloc(res_size + 1);
    assert(res);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0)
    {
        nb_read = SDL_ReadIO(io, buf, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_CloseIO(io);
    if (nb_read_total != res_size)
    {
        free(res);
        return nullptr;
    }

    res[nb_read_total] = '\0';
    len = nb_read_total;
    return res;
}

char* loadFile(const char* fileName)
{
    int64_t _;
    return loadFile(fileName, _);
}

std::vector<char> loadSPIRV(const char* fileName)
{
    assert(fileName);

    int64_t len;
    char* str = loadFile(fileName, len);
    assert(str);
    assert(len > 0);
    std::vector<char> bin(str, str + len);
    return bin;
}

} // namespace kame::squirtle
