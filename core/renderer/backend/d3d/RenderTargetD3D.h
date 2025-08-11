#pragma once

#include "../RenderTarget.h"
#include <d3d11.h>
#include <array>

namespace ax::backend::d3d
{

class RenderTargetImpl : public RenderTarget {
public:
    struct Attachment
    {
        ID3D11Texture2D* texure;
        TextureDescriptor desc;
    };
    RenderTargetImpl(ID3D11Device* device, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void invalidate();

    void update(ID3D11DeviceContext*) const;

    void apply(ID3D11DeviceContext*) const;

    ID3D11RenderTargetView* getRTV(size_t idx) const { return _rtvs[idx]; }
    ID3D11DepthStencilView* getDSV() const { return _dsv; }

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment(int index) const;

private:
    ID3D11Device* _device = nullptr;
    mutable std::array<ID3D11RenderTargetView*, MAX_COLOR_ATTCHMENT> _rtvs{};
    mutable ID3D11DepthStencilView* _dsv = nullptr;
};
}
