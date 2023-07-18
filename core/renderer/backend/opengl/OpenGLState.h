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

#define AX_GURAD_SET(x, y)         \
    if (x && x == (y)) \
        return;                    \
    x = (y)

#define AX_GURAD_SET_EQ(x, ...)                           \
    if (x && x.value().equals(##__VA_ARGS__)) \
        return;                                           \
    x.emplace(##__VA_ARGS__)

struct OpenGLState
{
    using CullMode = backend::CullMode;
    using Winding  = backend::Winding;
    using UtilsGL  = backend::UtilsGL;

    void viewport(const Viewport& v)
    {
        AX_GURAD_SET(_viewPort, v);
        glViewport(v.x, v.y, v.width, v.height);
    }
    void winding(Winding v)
    {
        AX_GURAD_SET(_winding, v);
        glFrontFace(UtilsGL::toGLFrontFace(v));
    }
    void enableDepthTest()
    {
        AX_GURAD_SET(_depthTest, true);
        glEnable(GL_DEPTH_TEST);
    }
    void disableDepthTest()
    {
        AX_GURAD_SET(_depthTest, false);
        glDisable(GL_DEPTH_TEST);
    }
    void enableBlend()
    {
        AX_GURAD_SET(_blend, true);
        glEnable(GL_BLEND);
    }
    void disableBlend() {
        AX_GURAD_SET(_blend, false);
        glDisable(GL_BLEND);
    }
    void enableScissor(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        AX_GURAD_SET(_scissor, true);
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    }
    void disableScissor() {
        AX_GURAD_SET(_scissor, false);
        glDisable(GL_SCISSOR_TEST);
    }
    void lineWidth(float v)
    {
        AX_GURAD_SET(_lineWidth, v);
        glLineWidth(v);
    }
    void bindFrameBuffer(GLuint v)
    {
        AX_GURAD_SET(_frameBufferBind, v);
        glBindFramebuffer(GL_FRAMEBUFFER, v);
    }
    void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
    {
        AX_GURAD_SET_EQ(_blendEquationSeparate, modeRGB, modeAlpha);
        glBlendEquationSeparate(modeRGB, modeAlpha);
    }
    void blendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
    {
        AX_GURAD_SET_EQ(_blendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
        glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    }
    void colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
    {
        AX_GURAD_SET_EQ(_colorMask, r, g, b, a);
        glColorMask(r, g, b, a);
    }
    void depthMask(GLboolean v)
    {
        AX_GURAD_SET(_depthMask, v);
        glDepthMask(v);
    }
    void depthFunc(GLenum v)
    {
        AX_GURAD_SET(_depthFunc, v);
        glDepthFunc(v);
    }
    void enableStencilTest()
    {
        AX_GURAD_SET(_stencilTest, true);
        glEnable(GL_STENCIL_TEST);
    }
    void disableStencilTest()
    {
        AX_GURAD_SET(_stencilTest, false);
        glDisable(GL_STENCIL_TEST);
    }
    void enableCullFace(GLenum mode)
    {
        AX_GURAD_SET(_cullFace, true);
        glEnable(GL_CULL_FACE);
        glCullFace(mode);
    }
    void disableCullFace()
    {
        AX_GURAD_SET(_cullFace, false);
        glDisable(GL_CULL_FACE);
    }
    void useProgram(GLuint v)
    {
        AX_GURAD_SET(_programBind, v);
        glUseProgram(v);
    }

    void stencilFunc(GLenum func, GLint ref, GLuint mask)
    {
        AX_GURAD_SET_EQ(_stencilFunc, func, ref, mask);
        glStencilFunc(func, ref, mask);
    }

    void stencilFuncFront(GLenum func, GLint ref, GLuint mask)
    {
        AX_GURAD_SET_EQ(_stencilFuncFront, func, ref, mask);
        glStencilFuncSeparate(GL_FRONT, func, ref, mask);
    }
    void stencilFuncBack(GLenum func, GLint ref, GLuint mask)
    {
        AX_GURAD_SET_EQ(_stencilFuncBack, func, ref, mask);
        glStencilFuncSeparate(GL_BACK, func, ref, mask);
    }

    void stencilOp(GLenum fail, GLenum zfail, GLenum zpass)
    {
        AX_GURAD_SET_EQ(_stencilOp, fail, zfail, zpass);
        glStencilOp(fail, zfail, zpass);
    }
    void stencilOpFront(GLenum fail, GLenum zfail, GLenum zpass)
    {
        AX_GURAD_SET_EQ(_stencilOpFront, fail, zfail, zpass);
        glStencilOpSeparate(GL_FRONT, fail, zfail, zpass);
    }
    void stencilOpBack(GLenum fail, GLenum zfail, GLenum zpass)
    {
        AX_GURAD_SET_EQ(_stencilOpBack, fail, zfail, zpass);
        glStencilOpSeparate(GL_BACK, fail, zfail, zpass);
    }

    void stencilMask(GLuint v)
    {
        AX_GURAD_SET(_stencilMask, v);
        glStencilMask(v);
    }
    void stencilMaskFront(GLuint v)
    {
        AX_GURAD_SET(_stencilMaskFront, v);
        glStencilMaskSeparate(GL_FRONT, v);
    }
    void stencilMaskBack(GLuint v)
    {
        AX_GURAD_SET(_stencilMaskBack, v);
        glStencilMaskSeparate(GL_BACK, v);
    }
    void activeTexture(GLenum v)
    {
        AX_GURAD_SET(_activeTexture, v);
        glActiveTexture(v);
    }
    void bindTexture(GLenum target, GLuint handle)
    {
        AX_GURAD_SET_EQ(_textureBind, target, handle);

        glBindTexture(target, handle);
    }
    void bindArrayBuffer(GLuint v)
    {
        AX_GURAD_SET(_arrayBufferBind, v);
        glBindBuffer(GL_ARRAY_BUFFER, v);
    }
    void bindElementBuffer(GLuint v)
    {
        AX_GURAD_SET(_elementBufferBind, v);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v);
    }
    void bindUniformBufferBase(GLuint index, GLuint handle)
    {
        AX_GURAD_SET_EQ(_uniformBufferState, index, handle);
        glBindBufferBase(GL_UNIFORM_BUFFER, index, handle);
    }

private:
    std::optional<Viewport> _viewPort;
    std::optional<Winding> _winding;
    std::optional<bool> _depthTest;
    std::optional<bool> _blend;
    std::optional<bool> _scissor;
    // std::optional<ScissorRect> _scissorRect;

    std::optional<float> _lineWidth;
    std::optional<GLuint> _frameBufferBind;
    std::optional<BlendEquationSeparateState> _blendEquationSeparate;
    std::optional<BlendFuncSeparateState> _blendFuncSeparate;
    std::optional<ColorMaskState> _colorMask;
    std::optional<GLboolean> _depthMask;
    std::optional<GLenum> _depthFunc;
    std::optional<bool> _stencilTest;
    std::optional<bool> _cullFace;
    // std::optional<GLenum> _cullFaceMode;
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
