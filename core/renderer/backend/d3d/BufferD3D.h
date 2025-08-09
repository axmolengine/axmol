// -------------------------------------------------------------------------------------------------
// BufferImpl.hpp
// Direct3D-11 implementation of the abstract Buffer interface
// -------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <cassert>
#include <d3d11.h>
#include <wrl/client.h>

#include "../Buffer.h"           // 你的抽象接口头
//#include "DxgiFormatMapping.h"  // 若有自定义格式映射，可移除

namespace ax::backend::d3d
{

class BufferImpl final : public Buffer
{
public:
    /**
     * @param device   已创建好的 D3D11 设备接口
     * @param context  与 device 对应的立即上下文
     * @param size     缓冲区字节数
     * @param type     BufferType::VERTEX or BufferType::INDEX
     * @param usage    BufferUsage::STATIC / DYNAMIC / STREAM
     * @param initial  可选初始数据指针，为空则仅分配
     */
    BufferImpl(ID3D11Device* device,
               ID3D11DeviceContext* context,
               std::size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial = nullptr);

    // --- Buffer 接口实现 -------------------------------------------------------------------------
    void updateData(const void* data, std::size_t size) override;
    void updateSubData(const void* data, std::size_t offset, std::size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    // --- D3D11 专用辅助 --------------------------------------------------------------------------
    ID3D11Buffer* internalHandle() const noexcept { return _buffer.Get(); }
    D3D11_BIND_FLAG getBindFlag() const noexcept { return _bindFlag; }

private:
    // 创建底层 D3D11 缓冲区
    void createNativeBuffer(const void* initial);

    // CPU 侧默认数据缓存（仅当 _needDefaultStoredData = true 时启用）
    std::vector<uint8_t> _defaultData;
    bool _needDefaultStoredData = true;

    // 底层 D3D11 对象
    ID3D11Device* _device; // weak ref
    ID3D11DeviceContext* _context;  // weak ref
    Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;

    // 映射辅助
    D3D11_USAGE _nativeUsage  = D3D11_USAGE_DYNAMIC;
    UINT _cpuAccess           = D3D11_CPU_ACCESS_WRITE;
    D3D11_BIND_FLAG _bindFlag = D3D11_BIND_VERTEX_BUFFER;
};

}  // namespace ax::d3d
