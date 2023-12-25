#pragma once

#include "kame/vk/vk.hpp"

namespace kame::vk::etna {

struct BufferVK {
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;
};

struct StagingBuffer : BufferVK {};

struct SSBO : BufferVK {};

struct Etna : kame::vk::Vulkan {

    void beginFrame()
    {
        beginCmd();
    }

    void endFrame()
    {
        setMemoryBarrier(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

        endCmd();

        flush(true);
    }

    void createStagingBuffer(VkDeviceSize size, StagingBuffer& buffer, const void* data)
    {
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkMemoryRequirements req;

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        createBuffer(size, usage, stagingBuffer, req);

        allocateMemory(req, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory);

        bindBufferMemory(stagingBuffer, memory, 0);

        void* dst;
        mapMemory(memory, 0, size, 0, &dst);

        std::memcpy(dst, data, size);

        unmapMemory(memory);

        buffer._buffer = stagingBuffer;
        buffer._memory = memory;
        buffer._size = size;
    }

    void destroyStagingBuffer(StagingBuffer& buffer)
    {
        destroyBuffer(buffer._buffer);

        freeMemory(buffer._memory);

        buffer._size = 0;
    }

    void createSSBO(VkDeviceSize size, SSBO& buffer)
    {
        VkBuffer ssboBuffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkMemoryRequirements req;

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        createBuffer(size, usage, ssboBuffer, req);

        allocateMemory(req, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory);

        bindBufferMemory(ssboBuffer, memory, 0);

        buffer._buffer = ssboBuffer;
        buffer._memory = memory;
        buffer._size = size;
    }

    void destroySSBO(SSBO& ssbo)
    {
        destroyBuffer(ssbo._buffer);

        freeMemory(ssbo._memory);

        ssbo._size = 0;
    }

    void updateSSBO(SSBO& ssbo, const void* data)
    {
        StagingBuffer stagingBuffer;

        createStagingBuffer(ssbo._size, stagingBuffer, data);

        VkBufferCopy region{};
        region.size = ssbo._size;

        cmdCopyBuffer(stagingBuffer._buffer, ssbo._buffer, region);

        setMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

        destroyStagingBuffer(stagingBuffer);
    }
};

} // namespace kame::vk::etna
