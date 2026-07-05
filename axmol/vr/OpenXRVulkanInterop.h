class OpenXRVulkanInterop final : public ax::rhi::vk::VulkanInterop
{
public:
    OpenXRVulkanInterop(XrInstance instance, XrSystemId system)
        : _instance(instance)
        , _system(system)
    {
    }

private:
    XrInstance _instance{XR_NULL_HANDLE}; // non-owning
    XrSystemId _system{XR_NULL_SYSTEM_ID};
};