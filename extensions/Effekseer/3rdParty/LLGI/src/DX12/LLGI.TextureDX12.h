
#pragma once

#include "../LLGI.Texture.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{

class TextureDX12 : public Texture
{
private:
	GraphicsDX12* graphics_ = nullptr;
	bool hasStrongRef_ = false;
	ID3D12Device* device_ = nullptr;
	ID3D12CommandQueue* commandQueue_ = nullptr;

	ID3D12Resource* texture_ = nullptr;
	ID3D12Resource* buffer_ = nullptr;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint_;
	D3D12_RESOURCE_STATES state_ = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
	DXGI_FORMAT dxgiFormat_;

	//! DX12 doesn't have packed buffer
	std::vector<uint8_t> lockedBuffer_;

	Vec3I textureSize_;
	int32_t cpuMemorySize_;
	TextureParameter parameter_;

	void CreateBuffer();

public:
	TextureDX12(GraphicsDX12* graphics, bool hasStrongRef);

	//! init as screen texture
	TextureDX12(ID3D12Resource* textureResource, ID3D12Device* device, ID3D12CommandQueue* commandQueue);

	~TextureDX12() override;

	bool Initialize(const TextureParameter& parameter);

	//! init as external texture
	bool Initialize(ID3D12Resource* textureResource);

	void* Lock() override;
	void Unlock() override;

	const TextureParameter& GetParameter() const { return parameter_; }

	Vec3I GetSize() const { return textureSize_; }

	Vec2I GetSizeAs2D() const override;
	ID3D12Resource* Get() const { return texture_; }

	TextureFormatType GetFormat() const override { return format_; }

	DXGI_FORMAT GetDXGIFormat() const { return dxgiFormat_; }

	const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& GetFootprint() const { return footprint_; }

	D3D12_RESOURCE_STATES GetState() const { return state_; }

	//! set a resource barrior and change a state
	void ResourceBarrior(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
};
} // namespace LLGI
