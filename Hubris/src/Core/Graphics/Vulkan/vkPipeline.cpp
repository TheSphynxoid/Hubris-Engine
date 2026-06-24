#include "Core/Graphics/Vulkan/vkPipeline.h"
#include "Core/Graphics/Vulkan/Utility.h"
#include "Core/Graphics/Vulkan/vkBackend.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#include "Core/Graphics/Vulkan/vkShader.h"

Hubris::Graphics::Vulkan::VulkanPipeline::VulkanPipeline(const PipelineDescriptor &desc)
{
    static const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pScissors = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = !desc.rasterizeConfig.DepthClip; //DepthClip is the inverse of DepthClamp.
    rasterizer.rasterizerDiscardEnable = desc.rasterizeConfig.discard;
    rasterizer.polygonMode = PolygonModeToVK(desc.rasterizeConfig.polygonMode);
    rasterizer.cullMode = CullModeToVk(desc.rasterizeConfig.cullMode);
    rasterizer.frontFace = FrontFaceOrderToVk(desc.rasterizeConfig.frontFace);
    rasterizer.depthBiasEnable = desc.rasterizeConfig.depthBias;
    rasterizer.depthBiasSlopeFactor = desc.rasterizeConfig.depthBiasSlopeFactor;
    rasterizer.depthBiasConstantFactor = desc.rasterizeConfig.depthBiasConstantFactor;
    rasterizer.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = SampleCountToVk(desc.multiSampleConfig.sampleCount);
    multisampling.sampleShadingEnable = desc.multiSampleConfig.enabled;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = desc.blendConfig.blendEnable;
    colorBlendAttachment.colorWriteMask = ColorComponentFlagToVK(desc.blendConfig.colorWriteMask);
    colorBlendAttachment.colorBlendOp = BlendOpToVk(desc.blendConfig.colorBlendOp);
    colorBlendAttachment.alphaBlendOp = BlendOpToVk(desc.blendConfig.alphaBlendOp);
    
    colorBlendAttachment.srcColorBlendFactor = BlendFactorToVk(desc.blendConfig.srcColorBlendFactor);
    colorBlendAttachment.srcAlphaBlendFactor = BlendFactorToVk(desc.blendConfig.srcAlphaBlendFactor);

    colorBlendAttachment.dstColorBlendFactor = BlendFactorToVk(desc.blendConfig.dstColorBlendFactor);
    colorBlendAttachment.dstAlphaBlendFactor = BlendFactorToVk(desc.blendConfig.dstAlphaBlendFactor);

    //

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = desc.blendConfig.logicOpEnable;
    colorBlending.logicOp = LogicOpToVk(desc.blendConfig.logicOp);
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = desc.blendConfig.blendConstants[0];
    colorBlending.blendConstants[1] = desc.blendConfig.blendConstants[1];
    colorBlending.blendConstants[2] = desc.blendConfig.blendConstants[2];
    colorBlending.blendConstants[3] = desc.blendConfig.blendConstants[3];

    VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
    pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingCreateInfo.colorAttachmentCount = 1;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = &((VulkanSwapchain*)Engine::GetWindow()->GetSwapchain())->GetNativeFormat();
    //Add depth attachement.

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = ToVkPrimitiveTopology(desc.topology);
    inputAssembly.primitiveRestartEnable = desc.primitiveRestartEnable;

    VkGraphicsPipelineCreateInfo pipelineInfo{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipelineRenderingCreateInfo,
        .pVertexInputState = &vertexInputInfo, .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState, .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling, .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState, .layout = (VkPipelineLayout)desc.pipelineLayout->GetNative(), .renderPass = nullptr};

    pipelineInfo.stageCount = desc.shaders.size();
    
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();
    for(auto& pShader : desc.shaders){
        shaderStages.push_back((reinterpret_cast<const VulkanShader*>(pShader.get())->GetShaderPipelineCreateInfo()));
    }
    pipelineInfo.pStages = shaderStages.data();

    vkCreateGraphicsPipelines(VulkanBackend::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE, &graphicsPipeline);

    if(!graphicsPipeline){
        Logger::Fatal("failed to create a graphics pipeline");
        return;
    }
}

Hubris::Graphics::Vulkan::VulkanPipeline::~VulkanPipeline() {
    if (graphicsPipeline) {
        vkDestroyPipeline(VulkanBackend::GetDevice(), graphicsPipeline, VulkanBackend::GetAllocator());
        graphicsPipeline = nullptr;
    }
}