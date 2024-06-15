#pragma once

#include "base/Object.h"
#include "Texture.h"
#include <assert.h>

NS_AX_BACKEND_BEGIN

class RenderTarget : public ax::Object
{
public:
    struct RenderBuffer
    {
        TextureBackend* texture = nullptr;
        uint8_t level           = 0;  // level when attached to a texture
        explicit operator bool() const { return texture != nullptr; }
    };
    typedef RenderBuffer ColorAttachment[MAX_COLOR_ATTCHMENT];

    RenderTarget(bool defaultRenderTarget) : _defaultRenderTarget(defaultRenderTarget) {}
    virtual ~RenderTarget()
    {
        for (auto colorItem : _color)
            AX_SAFE_RELEASE(colorItem.texture);
        AX_SAFE_RELEASE(_depth.texture);
        AX_SAFE_RELEASE(_stencil.texture);
    }

    bool isDefaultRenderTarget() const { return _defaultRenderTarget; }

    void addFlag(TargetBufferFlags flag) {
        setTargetFlags(_flags |= flag);
    }
    void removeFlag(TargetBufferFlags flag) { 
        setTargetFlags(_flags & ~flag);
    }

    TargetBufferFlags getTargetFlags() const { return _flags; }
    void setTargetFlags(TargetBufferFlags flags) { 
        _flags = flags; 
    }

    void setColorAttachment(ColorAttachment attachment)
    {
        for (auto colorItem : _color)
            AX_SAFE_RELEASE(colorItem.texture);
        memcpy(_color, attachment, sizeof(ColorAttachment));
        for (auto colorItem : _color)
            AX_SAFE_RETAIN(colorItem.texture);

        _dirty = true;
    };

    void setColorAttachment(TextureBackend* attachment, int level = 0, int index = 0) { 
        AX_SAFE_RELEASE(_color[index].texture);
        _color[index].texture = attachment;
        _color[index].level   = level;
        AX_SAFE_RETAIN(_color[index].texture);
        _dirty = true;
    }

    void setDepthAttachment(TextureBackend* attachment, int level = 0)
    {
        AX_SAFE_RELEASE(_depth.texture);
        _depth.texture = attachment;
        _depth.level   = level;
        AX_SAFE_RETAIN(_depth.texture);

        _dirty = true;
    };
    void setStencilAttachment(TextureBackend* attachment, int level = 0)
    {
        AX_SAFE_RELEASE(_stencil.texture);
        _stencil.texture = attachment;
        _stencil.level   = level;
        AX_SAFE_RETAIN(_stencil.texture);

        _dirty = true;
    };
    
    bool isDirty() const { return _dirty; }

    ColorAttachment _color{};
    RenderBuffer _depth{};
    RenderBuffer _stencil{};
    TargetBufferFlags _flags{};

protected:
    bool _defaultRenderTarget = false;
    mutable bool _dirty = false;
    // uint8_t samples = 1;
};

NS_AX_BACKEND_END
