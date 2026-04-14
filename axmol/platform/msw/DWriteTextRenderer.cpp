/****************************************************************************
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "axmol/platform/msw/DWriteTextRenderer.h"
#include "axmol/platform/msw/ComPtr.h"
#include "ntcvt/ntcvt.hpp"
#include <algorithm>
#include <cstring>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace ax
{

// Convert alignment flags (compatible with original BitmapDC)
static void parseNativeTextAlign(int align, DWRITE_TEXT_ALIGNMENT& hAlign, DWRITE_PARAGRAPH_ALIGNMENT& vAlign)
{
    int hori = align & 0x0F;
    int vert = (align >> 4) & 0x0F;
    switch (hori)
    {
    case 1:
        hAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;  // left
    case 2:
        hAlign = DWRITE_TEXT_ALIGNMENT_TRAILING;
        break;  // right
    case 3:
        hAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
        break;  // center
    default:
        hAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
    }
    switch (vert)
    {
    case 1:
        vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        break;  // top
    case 2:
        vAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
        break;  // bottom
    case 3:
        vAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        break;  // middle
    default:
        vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        break;
    }
}

static inline D2D1::ColorF toD2DColor(const ax::Color32& color)
{
    // Axmol Color32 is RGBA, but Direct2D render targets use BGRA (DXGI_FORMAT_B8G8R8A8_UNORM),
    // so swap R and B when converting to ColorF to ensure correct colors.
    return D2D1::ColorF(color.b / 255.0f, color.g / 255.0f, color.r / 255.0f, color.a / 255.0f);
}

// Minimal IDWriteTextRenderer that writes glyph outlines into a PathGeometry
class GlyphRunToGeometryTextRenderer : public IDWriteTextRenderer
{
public:
    GlyphRunToGeometryTextRenderer(ID2D1Factory* d2dFactory) : _d2dFactory(d2dFactory)
    {
        if (_d2dFactory)
            _d2dFactory->AddRef();
    }

    ~GlyphRunToGeometryTextRenderer()
    {
        for (auto g : _geometries)
            SafeRelease(g);
        SafeRelease(_geometryGroup);
        SafeRelease(_d2dFactory);
    }

    // IUnknown
    IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
    {
        if (ppvObject == nullptr)
            return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteTextRenderer))
        {
            *ppvObject = static_cast<IDWriteTextRenderer*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
    IFACEMETHOD_(ULONG, AddRef)() { return InterlockedIncrement(&_refCount); }
    IFACEMETHOD_(ULONG, Release)()
    {
        ULONG newCount = InterlockedDecrement(&_refCount);
        if (newCount == 0)
            delete this;
        return newCount;
    }

    ID2D1Geometry* fetchGeometryGroup()
    {
        if (!_geometries.empty())
        {
            _d2dFactory->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, _geometries.data(), (UINT)_geometries.size(),
                                             &_geometryGroup);
        }
        return _geometryGroup;
    }

    IFACEMETHOD(DrawGlyphRun)(void*,
                              FLOAT x,
                              FLOAT y,
                              DWRITE_MEASURING_MODE,
                              const DWRITE_GLYPH_RUN* glyphRun,
                              const DWRITE_GLYPH_RUN_DESCRIPTION*,
                              IUnknown*)
    {
        if (!glyphRun || !glyphRun->fontFace)
            return E_FAIL;

        ID2D1PathGeometry* pathGeom = nullptr;
        HRESULT hr                  = _d2dFactory->CreatePathGeometry(&pathGeom);
        if (FAILED(hr))
            return hr;

        ID2D1GeometrySink* sink = nullptr;
        hr                      = pathGeom->Open(&sink);
        if (FAILED(hr))
        {
            pathGeom->Release();
            return hr;
        }

        hr = glyphRun->fontFace->GetGlyphRunOutline(
            glyphRun->fontEmSize, glyphRun->glyphIndices, glyphRun->glyphAdvances, glyphRun->glyphOffsets,
            glyphRun->glyphCount, glyphRun->isSideways, glyphRun->bidiLevel % 2, sink);

        sink->Close();
        sink->Release();

        if (SUCCEEDED(hr))
        {
            D2D1::Matrix3x2F transform            = D2D1::Matrix3x2F::Translation(x, y);
            ID2D1TransformedGeometry* transformed = nullptr;
            if (SUCCEEDED(_d2dFactory->CreateTransformedGeometry(pathGeom, transform, &transformed)))
            {
                _geometries.push_back(transformed);
            }
        }

        pathGeom->Release();
        return hr;
    }

    IFACEMETHOD(DrawUnderline)(void*, FLOAT, FLOAT, const DWRITE_UNDERLINE*, IUnknown*) { return S_OK; }
    IFACEMETHOD(DrawStrikethrough)(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) { return S_OK; }
    IFACEMETHOD(DrawInlineObject)(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) { return S_OK; }

    IFACEMETHOD(IsPixelSnappingDisabled)(void* /*clientDrawingContext*/, BOOL* isDisabled)
    {
        if (isDisabled == nullptr)
            return E_POINTER;
        *isDisabled = TRUE;  // disable pixel snapping for accurate geometry
        return S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(void* /*clientDrawingContext*/, DWRITE_MATRIX* transform)
    {
        if (transform == nullptr)
            return E_POINTER;
        // Return identity transform since we handle positioning ourselves
        transform->m11 = 1.0f;
        transform->m12 = 0.0f;
        transform->m21 = 0.0f;
        transform->m22 = 1.0f;
        transform->dx  = 0.0f;
        transform->dy  = 0.0f;
        return S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(void* /*clientDrawingContext*/, FLOAT* pixelsPerDip)
    {
        if (pixelsPerDip == nullptr)
            return E_POINTER;
        *pixelsPerDip = 1.0f;  // assume 96 DPI for simplicity
        return S_OK;
    }

private:
    LONG _refCount{1};
    ID2D1Factory* _d2dFactory;
    std::vector<ID2D1Geometry*> _geometries;
    ID2D1GeometryGroup* _geometryGroup{nullptr};
};

DWriteTextRenderer& DWriteTextRenderer::sharedTextRenderer()
{
    static DWriteTextRenderer instance;
    return instance;
}

DWriteTextRenderer::DWriteTextRenderer()
{
    std::ignore = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    initialize();
}

DWriteTextRenderer::~DWriteTextRenderer()
{
    releaseRenderTarget();
    SafeRelease(_textFormat);
    SafeRelease(_dwriteFactory);
    SafeRelease(_d2dFactory);
    SafeRelease(_wicFactory);
    CoUninitialize();
}

bool DWriteTextRenderer::initialize()
{
    HRESULT hr;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                             reinterpret_cast<IUnknown**>(&_dwriteFactory));
    if (FAILED(hr))
        return false;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&_d2dFactory);
    if (FAILED(hr))
        return false;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_wicFactory));
    if (FAILED(hr))
        return false;
    _initialized = true;
    return true;
}

bool DWriteTextRenderer::setFont(std::wstring_view fontName, int nSize, bool enableBold)
{
    if (!_initialized)
        return false;
    if (fontName.empty())
        fontName = L"Segoe UI"sv;  // fallback system font
    float newSize = (nSize > 0) ? (float)nSize : 16.0f;

    // Skip if no change
    if (_textFormat && _fontName == fontName && _fontSize == newSize && _bold == enableBold)
        return true;

    // Release old text format
    SafeRelease(_textFormat);

    // Create new IDWriteTextFormat
    HRESULT hr = _dwriteFactory->CreateTextFormat(
        fontName.data(), nullptr, enableBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, newSize, L"", &_textFormat);
    if (FAILED(hr))
    {
        AXLOGW("Failed to create text format for font '{}': hr=0x{:08X}", ntcvt::from_chars(fontName),
               static_cast<UINT>(hr));
        return false;
    }

    _fontName = fontName;
    _fontSize = newSize;
    _bold     = enableBold;
    return true;
}

bool DWriteTextRenderer::createRenderTarget(int width,
                                            int height,
                                            const FontDefinition& textDefinition,
                                            bool& premultipliedAlpha)
{
    if (width <= 0 || height <= 0)
        return false;

    premultipliedAlpha = true;

    if (_renderTarget && _bitmapWidth == width && _bitmapHeight == height)
        return true;  // reuse existing target of same size

    releaseRenderTarget();

    // Create a pixel format and initial its format
    // and alphaMode fields.
    // https://docs.microsoft.com/en-gb/windows/win32/direct2d/supported-pixel-formats-and-alpha-modes#supported-formats-for-id2d1devicecontext
    D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    props.pixelFormat                   = pixelFormat;

    HRESULT hr =
        _wicFactory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &_wicBitmap);
    if (FAILED(hr))
        return false;

    // Create D2D render target from WIC bitmap
    hr = _d2dFactory->CreateWicBitmapRenderTarget(_wicBitmap, props, &_renderTarget);
    if (FAILED(hr))
    {
        SafeRelease(_wicBitmap);
        return false;
    }

    // Use grayscale antialiasing (similar to GDI)
    _renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    // Create solid color brush (white, will be multiplied by text color later)
    hr = _renderTarget->CreateSolidColorBrush(toD2DColor(textDefinition._fontFillColor), &_textBrush);
    if (FAILED(hr))
    {
        releaseRenderTarget();
        return false;
    }

    _bitmapWidth  = width;
    _bitmapHeight = height;
    return true;
}

void DWriteTextRenderer::releaseRenderTarget()
{
    SafeRelease(_textBrush);
    SafeRelease(_renderTarget);
    SafeRelease(_wicBitmap);
    _bitmapWidth = _bitmapHeight = 0;
}

bool DWriteTextRenderer::drawText(std::string_view text,
                                  const FontDefinition& textDefinition,
                                  unsigned int align,
                                  bool& hasPremultipliedAlpha,
                                  SIZE& extent,
                                  tlx::byte_buffer& outData)
{
    if (!_initialized || text.empty())
        return false;

    auto fontName = ntcvt::from_chars(textDefinition._fontName, CP_UTF8);
    if (!setFont(fontName, textDefinition._fontSize, false))
        return false;

    // Convert text to UTF-16
    std::wstring wtext = ntcvt::from_chars(text, CP_UTF8);
    if (wtext.empty())
        return false;

    // Create text layout
    ComPtr<IDWriteTextLayout> textLayout;
    const float maxWidth  = (extent.cx > 0) ? (float)extent.cx : 32767.f;
    const float maxHeight = (extent.cy > 0) ? (float)extent.cy : 32767.f;
    const auto wrapMode   = textDefinition._enableWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP;

    // Parse text alignment
    DWRITE_TEXT_ALIGNMENT hAlign;
    DWRITE_PARAGRAPH_ALIGNMENT vAlign;
    parseNativeTextAlign(align, hAlign, vAlign);

    DWRITE_TEXT_METRICS metrics;
    HRESULT hr = _dwriteFactory->CreateTextLayout(wtext.c_str(), (UINT32)wtext.size(), _textFormat, maxWidth, maxHeight,
                                                  textLayout.GetAddressOf());
    if (FAILED(hr))
        return false;
    textLayout->SetTextAlignment(hAlign);
    textLayout->SetParagraphAlignment(vAlign);
    textLayout->SetWordWrapping(wrapMode);

    // Handle overflow == 2 (shrink font to fit)
    enum OverflowMode
    {
        SHRINK = 2,
    };
    if (textDefinition._overflow == OverflowMode::SHRINK && extent.cx > 0 && extent.cy > 0) [[unlikely]]
    {
        const DWRITE_TEXT_RANGE entireRange{0, (UINT32)wtext.size()};

        int low     = 1;
        int high    = textDefinition._fontSize;
        int bestFit = high;

        while (low <= high)
        {
            int mid = (low + high) / 2;

            hr = textLayout->SetFontSize(mid, entireRange);
            AX_BREAK_IF(FAILED(hr));

            hr = textLayout->GetMetrics(&metrics);
            AX_BREAK_IF(FAILED(hr));

            if (metrics.width <= maxWidth && metrics.height <= maxHeight)
            {
                bestFit = mid;
                low     = mid + 1;
            }
            else
            {
                high = mid - 1;
            }
        }
        hr = textLayout->SetFontSize(bestFit, entireRange);
        if (SUCCEEDED(hr))
            hr = textLayout->GetMetrics(&metrics);
    }
    else
    {
        hr = textLayout->GetMetrics(&metrics);
    }

    if (FAILED(hr))
        return false;

    // Determine final render size
    int renderWidth  = (extent.cx > 0) ? extent.cx : (int)ceil(metrics.widthIncludingTrailingWhitespace);
    int renderHeight = (extent.cy > 0) ? extent.cy : (int)ceil(metrics.height);

    if (extent.cx <= 0 && extent.cy <= 0)
    {
        hr = textLayout->SetMaxWidth(static_cast<float>(renderWidth));
        if (SUCCEEDED(hr))
            hr = textLayout->SetMaxHeight(static_cast<float>(renderHeight));
    }
    else if (extent.cx <= 0)
        hr = textLayout->SetMaxWidth(static_cast<float>(renderWidth));
    else if (extent.cy <= 0)
        hr = textLayout->SetMaxHeight(static_cast<float>(renderHeight));

    if (FAILED(hr))
        return false;

    if (!createRenderTarget(renderWidth, renderHeight, textDefinition, hasPremultipliedAlpha))
        return false;

    // Draw text
    _renderTarget->BeginDraw();
    _renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));  // transparent background
    _textBrush->SetColor(toD2DColor(textDefinition._fontFillColor));

    // Using zero drawing origin based on alignment
    constexpr D2D1_POINT_2F origin = {0, 0};

    // If stroke enabled, create stroke brush
    ID2D1SolidColorBrush* strokeBrush{nullptr};
    const bool strokeEnabled = textDefinition._stroke._strokeEnabled && textDefinition._stroke._strokeSize > 0.0f;
    // If stroke requested, build geometry and draw outline (and fill)
    if (!strokeEnabled)  // Normal fill
    {
        _renderTarget->DrawTextLayout(origin, textLayout.Get(), _textBrush);
    }
    else  // Fill with stroke(outline) effect
    {
        _renderTarget->CreateSolidColorBrush(toD2DColor(textDefinition._stroke._strokeColor), &strokeBrush);

        GlyphRunToGeometryTextRenderer* geomRenderer = new GlyphRunToGeometryTextRenderer(_d2dFactory);
        textLayout->Draw(nullptr, geomRenderer, 0.0f, 0.0f);
        ID2D1Geometry* geom = geomRenderer->fetchGeometryGroup();
        if (geom)
        {
            D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Translation(origin.x, origin.y);
            _renderTarget->SetTransform(transform);

            if (strokeBrush)
                _renderTarget->DrawGeometry(geom, strokeBrush, textDefinition._stroke._strokeSize);

            if (_textBrush)
                _renderTarget->FillGeometry(geom, _textBrush);

            _renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        }

        geomRenderer->Release();

        // Release temporaries
        SafeRelease(strokeBrush);
    }

    hr = _renderTarget->EndDraw();

    // Update output size
    extent.cx = renderWidth;
    extent.cy = renderHeight;

    // Copy pixel data from WIC bitmap to output buffer (32-bit BGRA, premultiplied alpha)
    bool ok = SUCCEEDED(hr);
    if (ok)
    {
        WICRect rcCopy = {0, 0, _bitmapWidth, _bitmapHeight};
        outData.resize(static_cast<size_t>(_bitmapWidth) * _bitmapHeight * 4);

        constexpr UINT bpp = 32;
        UINT rowPitch      = (_bitmapWidth * bpp + 7) / 8;
        HRESULT hr         = _wicBitmap->CopyPixels(&rcCopy, rowPitch, (UINT)outData.size(), outData.data());
        if (FAILED(hr))
        {
            AXLOGW("Failed to copy pixels from WIC bitmap: hr=0x{:08X}", static_cast<UINT>(hr));
        }
    }

    return ok;
}

}  // namespace ax
