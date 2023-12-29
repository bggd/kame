#pragma once

#include <spirv_cross.hpp>

#include <kame/vk/vk.hpp>

#include <kame/squirtle/squirtle.hpp>

namespace kame::vk::etna {

using kame::squirtle::loadSPIRV;

struct BufferVK {
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;
};

struct StagingBuffer : BufferVK {};

struct SSBO : BufferVK {};

struct GraphicsPipeLineCreateInfo {

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};

    VkViewport viewport{};

    VkRect2D scissor{};

    VkPipelineViewportStateCreateInfo viewportStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor};

    VkPipelineRasterizationStateCreateInfo rasterizerInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .lineWidth = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f};

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .maxDepthBounds = 1.0f};

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendingInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment};

    std::vector<VkPipelineShaderStageCreateInfo> stagesInfos;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pViewportState = &viewportStateInfo,
        .pRasterizationState = &rasterizerInfo,
        .pMultisampleState = &multisamplingInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colorBlendingInfo,
        .pDynamicState = &dynamicStateInfo};
};

// namespace kame::vk::etna

struct Etna : kame::vk::Vulkan {

    void createStagingBuffer(VkDeviceSize size, StagingBuffer& bufferResult, const void* data)
    {
        VkBuffer stagingBuffer = createBuffer(
            VkBufferCreateInfo{
                .size = size,
                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT});

        VkMemoryRequirements req = getBufferMemoryRequirements(stagingBuffer);

        VkDeviceMemory memory = allocateDeviceMemory(req, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        assert(memory);

        bindBufferMemory(stagingBuffer, memory);

        memcpyDeviceMemory(memory, data, size);

        bufferResult._buffer = stagingBuffer;
        bufferResult._memory = memory;
        bufferResult._size = size;
    }

    void destroyStagingBuffer(StagingBuffer& buffer)
    {
        destroyBuffer(buffer._buffer);

        freeDeviceMemory(buffer._memory);

        buffer._size = 0;
    }

    void createSSBO(VkDeviceSize size, SSBO& bufferResult)
    {
        VkBuffer ssboBuffer = createBuffer(
            VkBufferCreateInfo{
                .size = size,
                .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT});

        VkMemoryRequirements req = getBufferMemoryRequirements(ssboBuffer);

        VkDeviceMemory memory = allocateDeviceMemory(req, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        bindBufferMemory(ssboBuffer, memory);

        bufferResult._buffer = ssboBuffer;
        bufferResult._memory = memory;
        bufferResult._size = size;
    }

    void destroySSBO(SSBO& ssbo)
    {
        destroyBuffer(ssbo._buffer);

        freeDeviceMemory(ssbo._memory);

        ssbo._size = 0;
    }

    void updateSSBO(SSBO& ssbo, const void* data)
    {
        StagingBuffer stagingBuffer;

        createStagingBuffer(ssbo._size, stagingBuffer, data);

        VkBufferCopy region{};
        region.size = ssbo._size;

        cmdCopyBuffer(stagingBuffer._buffer, ssbo._buffer, region);

        cmdMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT);

        submitCmds();

        destroyStagingBuffer(stagingBuffer);
    }
};

} // namespace kame::vk::etna
