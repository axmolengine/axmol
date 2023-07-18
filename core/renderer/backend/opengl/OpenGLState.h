#pragma once

#include <optional>

#include "base/Types.h"
#include "platform/GL.h"
#include "UtilsGL.h"
#include "renderer/backend/Enums.h"

// Inline opengl state set calls

NS_AX_BACKEND_BEGIN

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

struct TextureBindState
{
    TextureBindState(GLenum t, GLuint h) : target(t), handle(h) {}
    inline bool equals(GLenum t, GLuint h) const { return this->target == t && this->handle == h; }
    GLenum target;
    GLuint handle;
};

struct UniformBufferBindState
{
    UniformBufferBindState(GLenum i, GLuint h) : index(i), handle(h) {}
    inline bool equals(GLenum i, GLuint h) const { return this->index == i && this->handle == h; }

    GLuint index;
    GLuint handle;
};

struct OpenGLState
{
    template <typename _Left>
    static inline void try_enable(GLenum target, _Left& opt)
    {
        if (opt)
            return;
        opt = true;
        glEnable(target);
    }
    template <typename _Left>
    static inline void try_disable(GLenum target, _Left& opt)
    {
        if (!opt)
            return;
        opt = false;
        glDisable(target);
    }
    template <typename _Func, typename _Left, typename _Right>
    static inline void try_call(_Func&& func, _Left& opt, _Right&& v)
    {
        if (opt == v)
            return;
        opt = v;
        func(v);
    }
    template <typename _Func, typename _Left, typename _Right, typename... _Args>
    static inline void try_callf(_Func&& func, _Left& opt, _Right&& v, _Args&&... args)
    {
        if (opt == v)
            return;
        opt = v;
        func(args...);
    }
    template <typename _Func, typename _Left, typename _Right>
    static inline void try_callu(_Func&& func, GLenum target, _Left& opt, _Right&& v)
    {
        if (opt == v)
            return;
        opt = v;
        func(target, v);
    }
    template <typename _Func, typename _Left, typename... _Args>
    static inline void try_callx(_Func&& func, _Left& opt, _Args&&... args)
    {
        if (opt && (*opt).equals(args...))
            return;
        opt.emplace(args...);
        func(args...);
    }

    template <typename _Func, typename _Left, typename... _Args>
    static inline void try_callxu(_Func&& func, GLenum upvalue, _Left& opt, _Args&&... args)
    {
        if (opt && (*opt).equals(args...))
            return;
        opt.emplace(args...);
        func(upvalue, args...);
    }


    using CullMode = backend::CullMode;
    using Winding  = backend::Winding;
    using UtilsGL  = backend::UtilsGL;

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
    void bindTexture(GLenum target, GLuint handle) { try_callx(glBindTexture, _textureBind, target, handle); }
    void bindArrayBuffer(GLuint v) { try_callu(glBindBuffer, GL_ARRAY_BUFFER, _arrayBufferBind, v); }
    void bindElementBuffer(GLuint v) { try_callu(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _elementBufferBind, v); }
    void bindUniformBufferBase(GLuint index, GLuint handle)
    {
        try_callxu(glBindBufferBase, GL_UNIFORM_BUFFER, _uniformBufferState, index, handle);
    }

private:
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
    std::optional<TextureBindState> _textureBind;
    std::optional<GLuint> _arrayBufferBind;
    std::optional<GLuint> _elementBufferBind;
    std::optional<UniformBufferBindState> _uniformBufferState;
};

AX_DLL extern OpenGLState __gl;

NS_AX_BACKEND_END
