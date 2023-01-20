//
// Created by Nikita on 12.04.2021.
//

#ifndef EVOVULKAN_FAMILYQUEUES_H
#define EVOVULKAN_FAMILYQUEUES_H

#include <vulkan/vulkan.h>
#include <EvoVulkan/Types/Base/VulkanObject.h>

namespace EvoVulkan::Types {
    class Surface;
    class Device;

    class DLL_EVK_EXPORT FamilyQueues : public IVkObject {
    public:
        FamilyQueues(VkPhysicalDevice physicalDevice, const Surface* pSurface);
        ~FamilyQueues() override;

    public:
        static FamilyQueues* Find(VkPhysicalDevice physicalDevice, const Surface* pSurface);

    public:
        EVK_NODISCARD bool Initialize(VkDevice logicalDevice);

        EVK_NODISCARD bool IsComplete() const override;
        EVK_NODISCARD bool IsReady() const override;

        EVK_NODISCARD VkQueue GetGraphicsQueue() const noexcept { return m_graphicsQueue; }
        EVK_NODISCARD VkQueue GetPresentQueue() const noexcept { return m_presentQueue; }

        EVK_NODISCARD uint32_t GetGraphicsIndex() const noexcept { return static_cast<uint32_t>(m_iGraphics); }
        EVK_NODISCARD uint32_t GetPresentIndex() const noexcept { return static_cast<uint32_t>(m_iPresent); }

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_logicalDevice = VK_NULL_HANDLE;

        const Surface* m_surface = nullptr;

        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;
        VkQueue m_transferQueue = VK_NULL_HANDLE;
        VkQueue m_computeQueue = VK_NULL_HANDLE;

        int32_t m_iGraphics = EVK_ID_INVALID;
        int32_t m_iPresent  = EVK_ID_INVALID;

    };
}

#endif //EVOVULKAN_FAMILYQUEUES_H
