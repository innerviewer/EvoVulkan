//
// Created by Nikita on 09.05.2021.
//

#ifndef EVOVULKAN_TEXTURE_H
#define EVOVULKAN_TEXTURE_H

#include <EvoVulkan/Tools/VulkanInitializers.h>
#include <EvoVulkan/Tools/VulkanInsert.h>
#include <EvoVulkan/Tools/VulkanDebug.h>
#include <EvoVulkan/Tools/VulkanTools.h>
#include "Device.h"

namespace EvoVulkan::Complexes {
    class FrameBuffer;
}

namespace EvoVulkan::Types {
    class StagingBuffer {
    private:
        StagingBuffer() = default;
        StagingBuffer(const StagingBuffer&) = default;
        ~StagingBuffer() = default;
    private:
        VkDeviceMemory m_stagingBufferMemory;
        VkBuffer       m_stagingBuffer;

        Device*        m_device = nullptr;
    public:
        bool Destroy() {
            vkDestroyBuffer(*m_device, m_stagingBuffer, nullptr);
            vkFreeMemory(*m_device, m_stagingBufferMemory, nullptr);
            m_device = nullptr;
            return true;
        }

        void Free() {
            delete this;
        }

        operator VkBuffer() const { return m_stagingBuffer; }
    public:
        static StagingBuffer* Create(Device* device, void* pixels, uint32_t width, uint32_t height) {
            const int bytesPerPixel = 4;
            uint64_t imageSize = width * height * bytesPerPixel;

            auto buffer = new StagingBuffer();

            buffer->m_device = device;

            buffer->m_stagingBuffer = Tools::CreateBuffer(
                    device,
                    imageSize,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    buffer->m_stagingBufferMemory);

            void* data;
            vkMapMemory(*device, buffer->m_stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(*device, buffer->m_stagingBufferMemory);

            return buffer;
        }
    };

    struct Texture {
        friend class EvoVulkan::Complexes::FrameBuffer;
    private:
        Texture() = default;
        ~Texture() = default;
    public:
        Texture(const Texture&) = delete;
    private:
        DeviceMemory   m_deviceMemory   = {};
        VkImage        m_image          = VK_NULL_HANDLE;
        VkImageView    m_view           = VK_NULL_HANDLE;

        VkSampler      m_sampler        = VK_NULL_HANDLE;

        VkImageLayout  m_imageLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat       m_format         = VK_FORMAT_UNDEFINED;
        uint32_t       m_width          = 0,
                       m_height         = 0;
        uint32_t       m_mipLevels      = 0;

        bool           m_canBeDestroyed = false;

        Types::Device* m_device         = nullptr;
    public:
        [[nodiscard]] inline VkDescriptorImageInfo GetDescriptor() const noexcept {
            return { m_sampler, m_view, m_imageLayout };
        }
    public:
        void Destroy() {
            if (!m_canBeDestroyed)
                return;

            if (m_sampler != VK_NULL_HANDLE) {
                vkDestroySampler(*m_device, m_sampler, nullptr);
                m_sampler = VK_NULL_HANDLE;
            }

            if (m_view != VK_NULL_HANDLE) {
                vkDestroyImageView(*m_device, m_view, nullptr);
                m_view = VK_NULL_HANDLE;
            }

            if (m_image != VK_NULL_HANDLE) {
                vkDestroyImage(*m_device, m_image, nullptr);
                m_image = VK_NULL_HANDLE;
            }

            /*if (m_deviceMemory != VK_NULL_HANDLE) {
                vkFreeMemory(*m_device, m_deviceMemory, nullptr);
                m_deviceMemory = VK_NULL_HANDLE;
            }*/
            m_device->FreeMemory(&m_deviceMemory);
        }

        void Free() {
            delete this;
        }

        static bool GenerateMipmaps(Texture* texture, Types::CmdBuffer* singleBuffer);

        static Texture* Load(
                Device *device,
                CmdPool *pool,
                const unsigned char *pixels,
                VkFormat format,
                uint32_t width, uint32_t height,
                uint32_t mipLevels, VkFilter filter);

        static Texture* LoadAutoMip(
                Device *device,
                CmdPool *pool,
                const unsigned char *pixels,
                VkFormat format,
                uint32_t width,
                uint32_t height, VkFilter filter)
        {
#ifdef max
            return Load(device, pool, pixels, format, width, height,
                        static_cast<uint32_t>(std::floor(std::log2(max(width, height)))) + 1, filter);
#else
            return Load(device, pool, pixels, format, width, height,
                        static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1, filter);
#endif
        }

        static Texture* LoadWithoutMip(
                Device *device,
                CmdPool *pool,
                const unsigned char *pixels,
                VkFormat format,
                uint32_t width, uint32_t height, VkFilter filter)
        {
            return Load(device, pool, pixels, format, width, height, 1, filter);
        }
    };
}

#endif //EVOVULKAN_TEXTURE_H
