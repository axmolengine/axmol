
#include "LLGI.BaseDX12.h"

namespace LLGI
{

static bool isGPUDebugEnabled_ = false;

bool GetIsGPUDebugEnabled() { return isGPUDebugEnabled_; }

void SetIsGPUDebugEnabled(bool value) { isGPUDebugEnabled_ = value; }

#if defined(_DEBUG)

#ifdef __DRED__
int32_t dredSettingsCount_ = 0;
ID3D12DeviceRemovedExtendedDataSettings* pDredSettings = nullptr;
#endif

void StartDX12_DRED_Debug()
{
#ifdef __DRED__
	if (dredSettingsCount_ == 0)
	{
		D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings));
		pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}
	dredSettingsCount_++;
#endif
}

void EndDX12_DRED_Debug()
{
#ifdef __DRED__
	dredSettingsCount_--;
	if (dredSettingsCount_ == 0)
	{
		if (pDredSettings != nullptr)
		{
			pDredSettings->Release();
			pDredSettings = nullptr;
		}
	}
#endif
}
#endif

void DumpDX12_DRED(ID3D12Device* device)
{
	if (device->GetDeviceRemovedReason() == S_OK)
		return;
#if defined(_DEBUG)
#ifdef __DRED__
	ID3D12DeviceRemovedExtendedData* pDred = nullptr;
	device->QueryInterface(IID_PPV_ARGS(&pDred));
	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT DredAutoBreadcrumbsOutput;
	D3D12_DRED_PAGE_FAULT_OUTPUT DredPageFaultOutput;
	pDred->GetAutoBreadcrumbsOutput(&DredAutoBreadcrumbsOutput);
	pDred->GetPageFaultAllocationOutput(&DredPageFaultOutput);

	pDred->Release();
#endif
#endif
}

ID3D12Resource* CreateResourceBuffer(ID3D12Device* device,
									 D3D12_HEAP_TYPE heapType,
									 DXGI_FORMAT format,
									 D3D12_RESOURCE_DIMENSION resourceDimention,
									 D3D12_RESOURCE_STATES resourceState,
									 D3D12_RESOURCE_FLAGS flags,
									 Vec3I size,
									 int32_t samplingCount)
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	D3D12_RESOURCE_DESC resDesc = {};

	ID3D12Resource* resource = nullptr;

	heapProps.Type = heapType;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	heapProps.CreationNodeMask = DirectX12::GetNodeMask();
	heapProps.VisibleNodeMask = DirectX12::GetNodeMask();

	if (samplingCount > 1)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityDesc{};
		msaaQualityDesc.SampleCount = samplingCount;
		msaaQualityDesc.Format = format;
		msaaQualityDesc.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

		HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityDesc, sizeof(msaaQualityDesc));
		if (FAILED(hr))
		{
			SHOW_DX12_ERROR(hr, device);
			return nullptr;
		}
	}

	resDesc.Dimension = resourceDimention;
	resDesc.Width = size.X;
	resDesc.Height = size.Y;
	resDesc.DepthOrArraySize = static_cast<UINT16>(size.Z);
	resDesc.MipLevels = 1;
	resDesc.Format = DirectX12::GetGeneratedFormat(format);
	resDesc.Layout = (resourceDimention == D3D12_RESOURCE_DIMENSION_BUFFER ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN);
	resDesc.SampleDesc.Count = samplingCount;
	resDesc.Flags = flags;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 0.0f;

	if ((flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0)
	{
		clearValue.Format = format;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
	}

	auto setClearValue =
		resourceDimention != D3D12_RESOURCE_DIMENSION_BUFFER &&
		(((flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0) || ((flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0));

	// clearValue causes CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
	auto hr = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, resourceState, (setClearValue ? &clearValue : nullptr), IID_PPV_ARGS(&resource));

	if (FAILED(hr))
	{
		SHOW_DX12_ERROR(hr, device);
		SafeRelease(resource);
		return nullptr;
	}
	return resource;
}

DXGI_FORMAT ConvertFormat(TextureFormatType format)
{
	if (format == TextureFormatType::R8G8B8A8_UNORM)
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	if (format == TextureFormatType::R16G16B16A16_FLOAT)
		return DXGI_FORMAT_R16G16B16A16_FLOAT;

	if (format == TextureFormatType::R32G32B32A32_FLOAT)
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	if (format == TextureFormatType::R8G8B8A8_UNORM_SRGB)
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	if (format == TextureFormatType::R16G16_FLOAT)
		return DXGI_FORMAT_R16G16_FLOAT;

	if (format == TextureFormatType::R8_UNORM)
		return DXGI_FORMAT_R8_UNORM;

	if (format == TextureFormatType::BC1)
		return DXGI_FORMAT_BC1_UNORM;

	if (format == TextureFormatType::BC2)
		return DXGI_FORMAT_BC2_UNORM;

	if (format == TextureFormatType::BC3)
		return DXGI_FORMAT_BC3_UNORM;

	if (format == TextureFormatType::BC1_SRGB)
		return DXGI_FORMAT_BC1_UNORM_SRGB;

	if (format == TextureFormatType::BC2_SRGB)
		return DXGI_FORMAT_BC2_UNORM_SRGB;

	if (format == TextureFormatType::BC3_SRGB)
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	if (format == TextureFormatType::D32)
		return DXGI_FORMAT_D32_FLOAT;

	if (format == TextureFormatType::D24S8)
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (format == TextureFormatType::D32S8)
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	if (format == TextureFormatType::Unknown)
		return DXGI_FORMAT_UNKNOWN;

	throw "Not implemented";
}

TextureFormatType ConvertFormat(DXGI_FORMAT format)
{
	if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
		return TextureFormatType::R8G8B8A8_UNORM;

	if (format == DXGI_FORMAT_R16G16B16A16_FLOAT)
		return TextureFormatType::R16G16B16A16_FLOAT;

	if (format == DXGI_FORMAT_R32G32B32A32_FLOAT)
		return TextureFormatType::R32G32B32A32_FLOAT;

	if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
		return TextureFormatType::R8G8B8A8_UNORM_SRGB;

	if (format == DXGI_FORMAT_R16G16_FLOAT)
		return TextureFormatType::R16G16_FLOAT;

	if (format == DXGI_FORMAT_R8_UNORM)
		return TextureFormatType::R8_UNORM;

	if (format == DXGI_FORMAT_BC1_UNORM)
		return TextureFormatType::BC1;

	if (format == DXGI_FORMAT_BC2_UNORM)
		return TextureFormatType::BC2;

	if (format == DXGI_FORMAT_BC3_UNORM)
		return TextureFormatType::BC3;

	if (format == DXGI_FORMAT_BC1_UNORM_SRGB)
		return TextureFormatType::BC1_SRGB;

	if (format == DXGI_FORMAT_BC2_UNORM_SRGB)
		return TextureFormatType::BC2_SRGB;

	if (format == DXGI_FORMAT_BC3_UNORM_SRGB)
		return TextureFormatType::BC3_SRGB;

	if (format == DXGI_FORMAT_D32_FLOAT)
		return TextureFormatType::D32;

	if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		return TextureFormatType::D24S8;

	if (format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
		return TextureFormatType::D32S8;

	if (format == DXGI_FORMAT_UNKNOWN)
		return TextureFormatType::Unknown;

	throw "Not implemented";
}

namespace DirectX12
{

static int nodeMask_ = 1;

DXGI_FORMAT GetGeneratedFormat(DXGI_FORMAT format)
{
	if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		format = DXGI_FORMAT_R24G8_TYPELESS;

	if (format == DXGI_FORMAT_D32_FLOAT)
		format = DXGI_FORMAT_R32_TYPELESS;

	return format;
}

DXGI_FORMAT GetShaderResourceViewFormat(DXGI_FORMAT format)
{
	if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	if (format == DXGI_FORMAT_D32_FLOAT)
		format = DXGI_FORMAT_R32_FLOAT;

	return format;
}

int32_t GetNodeMask() { return nodeMask_; }

void SetNodeMask(int nodeMask) { nodeMask_ = nodeMask; }

} // namespace DirectX12

} // namespace LLGI
