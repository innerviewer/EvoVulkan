//
// Created by Nikita on 12.04.2021.
//

#ifndef EVOVULKAN_VULKANKERNEL_H
#define EVOVULKAN_VULKANKERNEL_H

/*
    1. helper
    2. types
    3. tools
    4. core
 */

#include <EvoVulkan/Tools/VulkanTools.h>

namespace EvoVulkan::Core {
    class VulkanKernel;

    /*typedef void (__stdcall *KernelRenderFunction)(
            EvoVulkan::Core::VulkanKernel* kernel,
            //EvoVulkan::Types::Swapchain* swapchain,
            EvoVulkan::Types::Device* device,
            VkFence* waitFences,
            VkCommandBuffer* drawBuffers,
            VkSubmitInfo& submitInfo, // copy
            uint32_t currentBuffer
            );*/

    class VulkanKernel {
    public:
        VulkanKernel(const VulkanKernel&) = delete;
    protected:
        VulkanKernel()  = default;
        ~VulkanKernel() = default;
    private:
        std::vector<const char*>   m_instExtensions       = {};
        std::vector<const char*>   m_validationLayers     = {};

        std::string                m_appName              = "Unknown";
        std::string                m_engineName           = "NoEngine";

        VkInstance                 m_instance             = VK_NULL_HANDLE;

        VkDebugUtilsMessengerEXT   m_debugMessenger       = VK_NULL_HANDLE;

        bool                       m_validationEnabled    = false;

        bool                       m_isPreInitialized     = false;
        bool                       m_isInitialized        = false;
        bool                       m_isPostInitialized    = false;
    protected:
        VkClearValue m_clearValues[2] {
                { .color = {{0.5f, 0.5f, 0.5f, 1.0f}} },
                { .depthStencil = { 1.0f, 0 } }
        };
    protected:
        unsigned int               m_width                = 0;
        unsigned int               m_height               = 0;

        VkRenderPass               m_renderPass           = VK_NULL_HANDLE;
        VkPipelineCache            m_pipelineCache        = VK_NULL_HANDLE;

        Types::Device*             m_device               = nullptr;
        Types::Surface*            m_surface              = nullptr;
        Types::Swapchain*          m_swapchain            = nullptr;
        Types::CmdPool*            m_cmdPool              = nullptr;
        Types::DepthStencil*       m_depthStencil         = nullptr;

        Types::Synchronization     m_syncs                = {};
        VkSubmitInfo               m_submitInfo           = {};

        unsigned __int8            m_countDCB             = 0;
        //Types::CmdBuffer**         m_drawCmdBuffs         = nullptr;
        VkCommandBuffer*           m_drawCmdBuffs         = nullptr;
        std::vector<VkFence>       m_waitFences           = std::vector<VkFence>();
        std::vector<VkFramebuffer> m_frameBuffers         = std::vector<VkFramebuffer>();
        uint32_t                   m_currentBuffer        = 0;

        VkPipelineStageFlags       m_submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    private:
        //KernelRenderFunction       m_renderFunction       = nullptr;
    private:
        bool ReCreateFrameBuffers();
        void DestroyFrameBuffers();
    public:
        void PrepareFrame();
        void NextFrame();
        void SubmitFrame();
    public:
        virtual void Render() { /* nothing */ }
        virtual bool BuildCmdBuffers() = 0;
    public:
        [[nodiscard]] inline VkCommandBuffer* GetDrawCmdBuffs() const { return m_drawCmdBuffs; }
        [[nodiscard]] inline Types::Device* GetDevice() const { return m_device; }

        inline void SetCurrentBufferID(uint32_t id) { this->m_currentBuffer = id; }

        //inline bool SetRenderFunction(KernelRenderFunction drawFunction) {
        //    this->m_renderFunction = drawFunction;
        //    return true;
        //}

        inline bool SetValidationLayersEnabled(const bool& value) {
            if (m_isPreInitialized) {
                Tools::VkDebug::Error("VulkanKernel::SetValidationLayersEnabled() : at this stage it is not possible to set this parameter!");
                return false;
            }

            this->m_validationEnabled = value;

            return true;
        }
        inline void SetSize(unsigned int width, unsigned int height) {
            this->m_width  = width;
            this->m_height = height;
        }
    public:
        //static VulkanKernel* Create();
        bool Destroy();
    public:
        bool PreInit(
                const std::string& appName,
                const std::string& engineName,
                const std::vector<const char*>& instExtensions,
                const std::vector<const char*>& validationLayers
                );

        bool Init(
                const std::function<VkSurfaceKHR(const VkInstance&)>& platformCreate,
                const std::vector<const char*>& deviceExtensions, const bool& enableSampleShading,
                bool vsync
                );
        bool PostInit();

        [[nodiscard]] inline VkRenderPass GetRenderPass() const { return m_renderPass; }
        [[nodiscard]] inline VkFramebuffer* GetFrameBuffers() { return m_frameBuffers.data(); }
    };
}

#endif //EVOVULKAN_VULKANKERNEL_H
