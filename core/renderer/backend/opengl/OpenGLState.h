#pragma once

#include <optional>

#include "base/Types.h"
#include "platform/GL.h"
#include "UtilsGL.h"

#define AX_ENABLE_STATE_GUARD 1

// Inline opengl state set calls

namespace ax::backend {

struct BlendEquationSeparateState
{
    BlendEquationSeparateState(GLenum rgb, GLenum alpha) : rgbBlendOperation(rgb), alphaBlendOperation(alpha) {}
    bool equals(GLenum rgb, GLenum alpha) const
    {
        return this->rgbBlendOperation == rgb && this->alphaBlendOperation == alpha;
    }
    GLenum rgbBlendOperation;
    GLenum alphaBlendOperation;
};

struct BlendFuncSeparateState
{
    BlendFuncSeparateState(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
        : sourceRGBBlendFactor(sfactorRGB)
        , destinationRGBBlendFactor(dfactorRGB)
        , sourceAlphaBlendFactor(sfactorAlpha)
        , destinationAlphaBlendFactor(dfactorAlpha)
    {}
    inline bool equals(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) const
    {
        return this->sourceRGBBlendFactor == sfactorRGB && this->destinationRGBBlendFactor == dfactorRGB &&
               this->sourceAlphaBlendFactor == sfactorAlpha && this->destinationAlphaBlendFactor == dfactorAlpha;
    }

    GLenum sourceRGBBlendFactor;
    GLenum destinationRGBBlendFactor;
    GLenum sourceAlphaBlendFactor;
    GLenum destinationAlphaBlendFactor;
};

struct ColorMaskState
{
    ColorMaskState(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
        : writeMaskRed(r), writeMaskGreen(g), writeMaskBlue(b), writeMaskAlpha(a)
    {}
    inline bool equals(GLboolean r, GLboolean g, GLboolean b, GLboolean a) const
    {
        return this->writeMaskRed == r && this->writeMaskGreen == g && this->writeMaskBlue == b &&
               this->writeMaskAlpha == a;
    }
    GLboolean writeMaskRed;
    GLboolean writeMaskGreen;
    GLboolean writeMaskBlue;
    GLboolean writeMaskAlpha;
};

struct StencilFuncState
{
    StencilFuncState(GLenum func, GLint ref, GLuint mask)
        : stencilCompareFunction(func), stencilReferenceValueFront(ref), readMask(mask)
    {}
    inline bool equals(GLenum func, GLint ref, GLuint mask) const
    {
        return this->stencilCompareFunction == func && this->stencilReferenceValueFront == ref &&
               this->readMask == mask;
    }
    unsigned int stencilCompareFunction;
    int stencilReferenceValueFront;
    unsigned int readMask;
};

struct StencilOperationState
{
    StencilOperationState(GLenum fail, GLenum zfail, GLenum zpass)
        : stencilFailureOperation(fail), depthFailureOperation(zfail), depthStencilPassOperation(zpass)
    {}
    inline bool equals(GLenum fail, GLenum zfail, GLenum zpass) const
    {
        return this->stencilFailureOperation == fail && this->depthFailureOperation == zfail &&
               this->depthStencilPassOperation == zpass;
    }
    unsigned int stencilFailureOperation;
    unsigned int depthFailureOperation;
    unsigned int depthStencilPassOperation;
};

struct CommonBindState
{
    CommonBindState(GLenum t, GLuint h) : target(t), handle(h) {}
    inline bool equals(GLenum t, GLuint h) const { return this->target == t && this->handle == h; }
    GLenum target;
    GLuint handle;
};

struct UniformBufferBaseBindState
{
    UniformBufferBaseBindState(GLenum i, GLuint h) : index(i), handle(h) {}
    inline bool equals(GLenum i, GLuint h) const { return this->index == i && this->handle == h; }

    GLuint index;
    GLuint handle;
};

struct AX_DLL OpenGLState
{
    constexpr static GLenum BufferTargets[] = {
        GL_ARRAY_BUFFER,          // VERTEX of VAO
        GL_ELEMENT_ARRAY_BUFFER,  // INDEX of VAO
        GL_UNIFORM_BUFFER,        // UNIFORM
        GL_PIXEL_PACK_BUFFER,     // PIXEL
    };

    constexpr static int MAX_VERTEX_ATTRIBS = 16;
    constexpr static int MAX_TEXTURE_UNITS  = 16;

    template <typename _Left>
    static inline void try_enable(GLenum target, _Left& opt)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt.has_value() && opt.value())
            return;
        opt = true;
#endif
        glEnable(target);
    }
    template <typename _Left>
    static inline void try_disable(GLenum target, _Left& opt)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (!opt.has_value() || !opt.value())
            return;
        opt = false;
#endif
        glDisable(target);
    }
    template <typename _Func, typename _Left, typename _Right>
    static inline void try_call(_Func&& func, _Left& opt, _Right&& v)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt == v)
            return;
        opt = v;
#endif
        func(v);
    }
    template <typename _Func, typename _Left, typename _Right, typename... _Args>
    static inline void try_callf(_Func&& func, _Left& opt, _Right&& v, _Args&&... args)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt == v)
            return;
        opt = v;
#endif
        func(args...);
    }
    template <typename _Func, typename _Left, typename _Right>
    static inline void try_callu(_Func&& func, GLenum target, _Left& opt, _Right&& v)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt == v)
            return;
        opt = v;
#endif
        func(target, v);
    }
    template <typename _Func, typename _Left, typename... _Args>
    static inline void try_callx(_Func&& func, _Left& opt, _Args&&... args)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt && (*opt).equals(args...))
            return;
        opt.emplace(args...);
#endif
        func(args...);
    }

    template <typename _Func, typename _Left, typename... _Args>
    static inline void try_callxu(_Func&& func, GLenum upvalue, _Left& opt, _Args&&... args)
    {
#if defined(AX_ENABLE_STATE_GUARD)
        if (opt && (*opt).equals(args...))
            return;
        opt.emplace(args...);
#endif
        func(upvalue, args...);
    }

    using CullMode = backend::CullMode;
    using Winding  = backend::Winding;
    using UtilsGL  = backend::UtilsGL;

    static void reset();

    OpenGLState();

    void viewport(const Viewport& v) { try_callf(glViewport, _viewPort, v, v.x, v.y, v.width, v.height); }
    void winding(Winding v) { try_callf(glFrontFace, _winding, v, UtilsGL::toGLFrontFace(v)); }
    void enableDepthTest() { try_enable(GL_DEPTH_TEST, _depthTest); }
    void disableDepthTest() { try_disable(GL_DEPTH_TEST, _depthTest); }
    void enableBlend() { try_enable(GL_BLEND, _blend); }
    void disableBlend() { try_disable(GL_BLEND, _blend); }
    void enableScissor(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        try_enable(GL_SCISSOR_TEST, _scissor);
        glScissor(x, y, width, height);
    }
    void disableScissor() { try_disable(GL_SCISSOR_TEST, _scissor); }
    void lineWidth(float v) { try_call(glLineWidth, _lineWidth, v); }
    void bindFrameBuffer(GLuint v) { try_callu(glBindFramebuffer, GL_FRAMEBUFFER, _frameBufferBind, v); }
    void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
    {
        try_callx(glBlendEquationSeparate, _blendEquationSeparate, modeRGB, modeAlpha);
    }
    void blendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
    {
        try_callx(glBlendFuncSeparate, _blendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    }
    void colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
    {
        try_callx(glColorMask, _colorMask, r, g, b, a);
    }
    void depthMask(GLboolean v) { try_call(glDepthMask, _depthMask, v); }
    void depthFunc(GLenum v) { try_call(glDepthFunc, _depthFunc, v); }
    void enableStencilTest() { try_enable(GL_STENCIL_TEST, _stencilTest); }
    void disableStencilTest() { try_disable(GL_STENCIL_TEST, _stencilTest); }
    void enableCullFace(GLenum mode)
    {
        try_enable(GL_CULL_FACE, _cullFace);
        glCullFace(mode);
    }
    void disableCullFace() { try_disable(GL_CULL_FACE, _cullFace); }
    void useProgram(GLuint v) { try_call(glUseProgram, _programBind, v); }

    void stencilFunc(GLenum func, GLint ref, GLuint mask) { try_callx(glStencilFunc, _stencilFunc, func, ref, mask); }

    void stencilFuncFront(GLenum func, GLint ref, GLuint mask)
    {
        try_callxu(glStencilFuncSeparate, GL_FRONT, _stencilFuncFront, func, ref, mask);
    }
    void stencilFuncBack(GLenum func, GLint ref, GLuint mask)
    {
        try_callxu(glStencilFuncSeparate, GL_BACK, _stencilFuncBack, func, ref, mask);
    }

    void stencilOp(GLenum fail, GLenum zfail, GLenum zpass) { try_callx(glStencilOp, _stencilOp, fail, zfail, zpass); }
    void stencilOpFront(GLenum fail, GLenum zfail, GLenum zpass)
    {
        try_callxu(glStencilOpSeparate, GL_FRONT, _stencilOpFront, fail, zfail, zpass);
    }
    void stencilOpBack(GLenum fail, GLenum zfail, GLenum zpass)
    {
        try_callxu(glStencilOpSeparate, GL_BACK, _stencilOpBack, fail, zfail, zpass);
    }

    void stencilMask(GLuint v) { try_call(glStencilMask, _stencilMask, v); }
    void stencilMaskFront(GLuint v) { try_callu(glStencilMaskSeparate, GL_FRONT, _stencilMaskFront, v); }
    void stencilMaskBack(GLuint v) { try_callu(glStencilMaskSeparate, GL_BACK, _stencilMaskBack, v); }
    void activeTexture(GLenum v) { try_call(glActiveTexture, _activeTexture, v); }
    void bindTexture(GLenum target, GLuint handle) {
        auto activeLayer = _activeTexture.has_value() ? _activeTexture.value() - GL_TEXTURE0 : 0;
        if(activeLayer < MAX_TEXTURE_UNITS)
            try_callx(glBindTexture, _textureBindings[activeLayer], target, handle);
    }
    void deleteTexture(GLenum target, GLuint handle)
    {
        glDeleteTextures(1, &handle);

        for (auto& textureBinding : _textureBindings)
        {
            if (textureBinding.has_value() && textureBinding->handle == handle)
                textureBinding.reset();
        }
    }
    GLenum bindBuffer(BufferType type, GLuint buffer)
    {
        auto target = BufferTargets[static_cast<int>(type)];
        try_callu(glBindBuffer, target, _bufferBindings[static_cast<int>(type)], buffer);
        return target;
    }
    void deleteBuffer(BufferType type, GLuint buffer)
    {
        glDeleteBuffers(1, &buffer);
        if (_bufferBindings[static_cast<int>(type)] == buffer)
            _bufferBindings[static_cast<int>(type)].reset();
    }
    void bindUniformBufferBase(GLuint index, GLuint handle)
    {
        try_callxu(glBindBufferBase, GL_UNIFORM_BUFFER, _uniformBufferState, index, handle);
    }

    // useful for multi GL context before GL context switch, reset VAO state
    // VAO not share between context
    // shareable: texture, uniform buffer
    // not shareable: FBO, VAO(GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER)
    void resetVAO()
    {
        _bufferBindings[static_cast<int>(BufferType::ARRAY_BUFFER)].reset();
        _bufferBindings[static_cast<int>(BufferType::ELEMENT_ARRAY_BUFFER)].reset();
    }

    void enableVertexAttribArray(GLuint index)
    {
        const auto mask = 1 << index;
        if (!(_attribBits & mask))
        {
            glEnableVertexAttribArray(index);
            _attribBits |= mask;
        }
    }

    void disableVertexAttribArray(GLuint index)
    {
        const auto mask = 1 << index;
        if (_attribBits & mask)
        {
            glDisableVertexAttribArray(index);
            _attribBits &= ~mask;
        }
    }

    void disableUnusedVertexAttribs(uint32_t usedBits)
    {
        if (usedBits != _attribBits)
        {
            uint32_t unusedEnabledBits = _attribBits & ~usedBits;
#if defined(_DEBUG)
            struct VertexBits
            {
                uint16_t bit0 : 1;
                uint16_t bit1 : 1;
                uint16_t bit2 : 1;
                uint16_t bit3 : 1;
                uint16_t bit4 : 1;
                uint16_t bit5 : 1;
                uint16_t bit6 : 1;
                uint16_t bit7 : 1;
                uint16_t bit8 : 1;
                uint16_t bit9 : 1;
                uint16_t bit10 : 1;
                uint16_t bit11 : 1;
                uint16_t bit12 : 1;
                uint16_t bit13 : 1;
                uint16_t bit14 : 1;
                uint16_t bit15 : 1;
                uint16_t reserved;
            };
            VertexBits *usedBitsDeb          = reinterpret_cast<VertexBits*>(&usedBits),
                       *enabledBitsDeb       = reinterpret_cast<VertexBits*>(&_attribBits),
                       *unusedEnabledBitsDeb = reinterpret_cast<VertexBits*>(&unusedEnabledBits);
#endif
            for (auto i = 0; unusedEnabledBits && i < MAX_VERTEX_ATTRIBS; ++i)
            {
                if (unusedEnabledBits & 1)
                    disableVertexAttribArray(i);
                unusedEnabledBits >>= 1;
            }

            assert(usedBits == _attribBits);
        }
    }

    void setVertexAttribDivisor(GLuint index)
    {
        const auto mask = 1 << index;
        if (!(_divisorBits & mask))
        {
            glVertexAttribDivisor(index, 1);
            _divisorBits |= mask;
        }
    }

    void clearVertexAttribDivisor(GLuint index)
    {
        const auto mask = 1 << index;
        if (_divisorBits & mask)
        {
            glVertexAttribDivisor(index, 0);
            _divisorBits &= ~mask;
        }
    }

private:
    GLuint _defaultVAO{0};
    uint32_t _attribBits{0}; // vertexAttribArray bitset
    uint32_t _divisorBits{0}; // divisor bitset
    std::optional<GLuint> _bufferBindings[(int)BufferType::COUNT];
    std::optional<CommonBindState> _textureBindings[MAX_TEXTURE_UNITS];

    std::optional<Viewport> _viewPort;
    std::optional<Winding> _winding;
    std::optional<bool> _depthTest;
    std::optional<bool> _blend;
    std::optional<bool> _scissor;

    std::optional<float> _lineWidth;
    std::optional<GLuint> _frameBufferBind;
    std::optional<BlendEquationSeparateState> _blendEquationSeparate;
    std::optional<BlendFuncSeparateState> _blendFuncSeparate;
    std::optional<ColorMaskState> _colorMask;
    std::optional<GLboolean> _depthMask;
    std::optional<GLenum> _depthFunc;
    std::optional<bool> _stencilTest;
    std::optional<bool> _cullFace;

    std::optional<GLuint> _programBind;
    std::optional<StencilFuncState> _stencilFunc;
    std::optional<StencilFuncState> _stencilFuncFront;
    std::optional<StencilFuncState> _stencilFuncBack;
    std::optional<StencilOperationState> _stencilOp;
    std::optional<StencilOperationState> _stencilOpFront;
    std::optional<StencilOperationState> _stencilOpBack;
    std::optional<GLuint> _stencilMask;
    std::optional<GLuint> _stencilMaskFront;
    std::optional<GLuint> _stencilMaskBack;
    std::optional<GLenum> _activeTexture;
    std::optional<UniformBufferBaseBindState> _uniformBufferState;
};

AX_DLL extern OpenGLState* __gl;

}
