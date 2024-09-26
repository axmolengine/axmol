/****************************************************************************
Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

The MIT License (MIT)

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

Based upon code from the DirectX Tool Kit by Microsoft Corporation,
obtained from https://directxtk.codeplex.com
****************************************************************************/
#include "platform/winrt/WICImageLoader-winrt.h"
#include "platform/winrt/WinRTUtils.h"
#include "ntcvt/ntcvt.hpp"

namespace ax
{

#if AX_USE_WIC

IWICImagingFactory* WICImageLoader::_wicFactory = NULL;

namespace {

WICConvert g_WICConvert[] =
{
	// Note target GUID in this conversion table must be one of those directly supported by cocos2d-x

	{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

	{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

	{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

	{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT
	{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT

	{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

	{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat24bppRGB }, // DXGI_FORMAT_R16G16B16A16_FLOAT

	//#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	//    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	//    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	//    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	//#endif

	// We don't support n-channel formats
};

class SimpleFileStream : public IStream
{
    std::unique_ptr<IFileStream> _fileStream;
    long _cRef;
    IFileStream::Mode _mode;

public:
    explicit SimpleFileStream(std::string_view path, IFileStream::Mode mode) noexcept : _cRef(1), _mode(mode)
    {
        _fileStream = FileUtils::getInstance()->openFileStream(path, mode);
    }

    virtual ~SimpleFileStream()
    {
        if (_fileStream)
            _fileStream->close();
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (__uuidof(IStream) == riid)
        {
            *ppv = static_cast<IStream*>(this);
        }
        else
        {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();

        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&_cRef); }

    STDMETHODIMP_(ULONG) Release() override
    {
        LONG cRef = InterlockedDecrement(&_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    HRESULT Read(void* pv, ULONG cb, ULONG* pcbRead) override
    {
        if (!_fileStream || _mode == IFileStream::Mode::WRITE)
            return STG_E_ACCESSDENIED;

        if (!pv)
            return STG_E_INVALIDPOINTER;

        const auto result = _fileStream->read(pv, cb);
        if (result < 0)
            return STG_E_ACCESSDENIED;

        if (result < cb)
            return S_FALSE;

        if (pcbRead)
            *pcbRead = result;

        return S_OK;
    }

    HRESULT Write(const void* pv, ULONG cb, ULONG* pcbWritten) override
    {
        if (!_fileStream || _mode == IFileStream::Mode::READ)
            return STG_E_ACCESSDENIED;

        if (!pv)
            return STG_E_INVALIDPOINTER;

        const auto result = _fileStream->write(pv, cb);
        if (result < 0)
        {
            return STG_E_CANTSAVE;
        }

        if (pcbWritten)
            *pcbWritten = result;

        return S_OK;
    }

    HRESULT Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition) override
    {
        if (!_fileStream || (dwOrigin != STREAM_SEEK_CUR && dwOrigin != STREAM_SEEK_SET && dwOrigin != STREAM_SEEK_END))
            return STG_E_INVALIDFUNCTION;

        const auto result =
            _fileStream->seek(dlibMove.QuadPart, dwOrigin);  // dwOrigin is compatible with SEEK_SET, SEEK_CUR, SEEK_END

        if (result < 0)
            return STG_E_INVALIDFUNCTION;

        if (plibNewPosition)
            plibNewPosition->QuadPart = result;

        return S_OK;
    }

    HRESULT SetSize(ULARGE_INTEGER libNewSize) override
    {
        if (_fileStream != nullptr && _fileStream->resize(libNewSize.QuadPart))
        {
            return S_OK;
        }

        return STG_E_INVALIDFUNCTION;
    }

    HRESULT CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten) override
    {
        return E_NOTIMPL;
    }

    HRESULT Commit(DWORD grfCommitFlags) override { return S_OK; }
    HRESULT Revert() override { return E_NOTIMPL; }
    HRESULT LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override { return E_NOTIMPL; }
    HRESULT UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override { return E_NOTIMPL; }
    HRESULT Stat(STATSTG* pstatstg, DWORD grfStatFlag) override { return E_NOTIMPL; }
    HRESULT Clone(IStream** ppstm) override { return E_NOTIMPL; }
};

}

WICImageLoader::WICImageLoader() :
	_width(0),
	_height(0),
	_dataLen(0),
	_bpp(0),
	_data(0)
{
	memset((void*)&_format, 0, sizeof(_format));
}

WICImageLoader::~WICImageLoader()
{
	if(_data != NULL && _dataLen > 0) {
		delete[] _data;
		_data = NULL;
	}
}

bool WICImageLoader::decodeImageData(const uint8_t* blob, size_t size)
{
    bool bRet = false;
    HRESULT hr = E_FAIL;

	IWICStream* pWicStream = NULL;
	IWICImagingFactory* pWicFactory = getWICFactory();

	if(NULL != pWicFactory)
	{
		hr = pWicFactory->CreateStream(&pWicStream);
	}

	if(SUCCEEDED(hr))
	{
		hr = pWicStream->InitializeFromMemory(const_cast<WICInProcPointer>(blob), static_cast<DWORD>(size));
	}

	IWICBitmapDecoder* pDecoder = NULL;

	if(SUCCEEDED(hr))
	{
		hr = pWicFactory->CreateDecoderFromStream(pWicStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}

	bRet = processImage(pDecoder);

	SafeRelease(&pWicStream);
	SafeRelease(&pDecoder);

	return bRet;
}

bool WICImageLoader::processImage(IWICBitmapDecoder* pDecoder)
{
    HRESULT hr = E_FAIL;
	IWICBitmapFrameDecode* pFrame = NULL;

	if(NULL != pDecoder)
	{
		hr = pDecoder->GetFrame(0, &pFrame);
	}

	if(SUCCEEDED(hr))
	{
		hr = pFrame->GetPixelFormat(&_format);
	}

	IWICFormatConverter* pConv = NULL;

	if(SUCCEEDED(hr))
	{
		hr = convertFormatIfRequired(pFrame, &pConv);
	}

	if(SUCCEEDED(hr))
	{
		_bpp = getBitsPerPixel(_format);

		if(NULL != pConv)
		{
			hr = pConv->GetSize((UINT*)&_width, (UINT*)&_height);
		}
		else
		{
			hr = pFrame->GetSize((UINT*)&_width, (UINT*)&_height);
		}
	}

	assert(_bpp > 0);
	assert(_width > 0 && _height > 0);

	if(SUCCEEDED(hr))
	{
		size_t rowPitch = (_width * _bpp + 7) / 8;
		_dataLen = rowPitch * _height;
		_data = new (std::nothrow) BYTE[_dataLen];

		if(NULL != pConv)
		{
			hr = pConv->CopyPixels(NULL, static_cast<UINT>(rowPitch), static_cast<UINT>(_dataLen), _data);
		}
		else
		{
			hr = pFrame->CopyPixels(NULL, static_cast<UINT>(rowPitch), static_cast<UINT>(_dataLen), _data);
		}
	}

	SafeRelease(&pFrame);
	SafeRelease(&pConv);
	return SUCCEEDED(hr);
}

HRESULT WICImageLoader::convertFormatIfRequired(IWICBitmapFrameDecode* pFrame, IWICFormatConverter** ppConv)
{
	*ppConv = NULL;

	if(	(memcmp(&_format, &GUID_WICPixelFormat8bppGray, sizeof(WICPixelFormatGUID)) == 0) ||
		(memcmp(&_format, &GUID_WICPixelFormat8bppAlpha, sizeof(WICPixelFormatGUID)) == 0) ||
		(memcmp(&_format, &GUID_WICPixelFormat24bppRGB, sizeof(WICPixelFormatGUID)) == 0) ||
		(memcmp(&_format, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID)) == 0))
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;
	IWICImagingFactory* pFactory = getWICFactory();
	IWICFormatConverter* pConv = NULL;

	if(NULL != pFactory)
	{
		hr = pFactory->CreateFormatConverter(&pConv);
	}

	WICPixelFormatGUID destFormat = GUID_WICPixelFormat32bppRGBA; // Fallback to RGBA 32-bit format which is supported by all devices

	for( size_t i=0; i < _countof(g_WICConvert); ++i )
	{
		if ( memcmp( &g_WICConvert[i].source, &_format, sizeof(WICPixelFormatGUID) ) == 0 )
		{
			memcpy( &destFormat, &g_WICConvert[i].target, sizeof(WICPixelFormatGUID) );
			break;
		}
	}

	BOOL bCanConv = FALSE;

	if(SUCCEEDED(hr))
	{
		hr = pConv->CanConvert(_format, destFormat, &bCanConv);
	}

	if(SUCCEEDED(hr) && bCanConv == TRUE)
	{
		hr = pConv->Initialize(pFrame, destFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
	}

	if(SUCCEEDED(hr))
	{
		memcpy(&_format, &destFormat, sizeof(WICPixelFormatGUID));
		*ppConv = pConv;
	}

	return SUCCEEDED(hr);
}

size_t WICImageLoader::getBitsPerPixel(WICPixelFormatGUID format)
{
    HRESULT hr = E_FAIL;

	IWICImagingFactory* pfactory = getWICFactory();

	IWICComponentInfo* pCInfo = NULL;

	if(pfactory != NULL)
	{
		hr = pfactory->CreateComponentInfo(format, &pCInfo);
	}

	WICComponentType cType;

	if(SUCCEEDED(hr))
	{
		hr = pCInfo->GetComponentType(&cType);
	}

	IWICPixelFormatInfo* pPInfo = NULL;

	if(SUCCEEDED(hr) && cType == WICPixelFormat)
	{
		hr = pCInfo->QueryInterface(IID_IWICPixelFormatInfo, (void**)&pPInfo);
	}

	UINT bpp = 0;

	if(SUCCEEDED(hr))
	{
		hr = pPInfo->GetBitsPerPixel(&bpp);
	}

	SafeRelease(&pCInfo);
	SafeRelease(&pPInfo);
	return bpp;
}

int WICImageLoader::getHeight()
{
	return _height;
}

int WICImageLoader::getWidth()
{
	return _width;
}

size_t WICImageLoader::getImageData(WICInProcPointer rawData, size_t dataLen)
{
	if(dataLen < _dataLen)
		return 0;

	memcpy(rawData, _data, _dataLen);

	return _dataLen;
}

size_t WICImageLoader::getImageDataSize()
{
	return _dataLen;
}

WICPixelFormatGUID WICImageLoader::getPixelFormat()
{
	return _format;
}

bool WICImageLoader::encodeImageData(std::string_view path, const uint8_t* data, size_t dataLen, WICPixelFormatGUID pixelFormat, int width, int height, GUID containerFormat)
{
	assert(data != NULL);
	assert(dataLen > 0 && width > 0 && height > 0);

	IWICImagingFactory* pFact = getWICFactory();

	HRESULT hr = E_FAIL;
    IStream* outStream  = new SimpleFileStream(path, IFileStream::Mode::WRITE);
	IWICStream* pStream = NULL;

	if (NULL != pFact) {
		hr = pFact->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr)) {
        hr = pStream->InitializeFromIStream(outStream);
	}

	IWICBitmapEncoder* pEnc = NULL;

	if (SUCCEEDED(hr)) {
		hr = pFact->CreateEncoder(containerFormat, NULL, &pEnc);
	}

	if (SUCCEEDED(hr)) {
		hr = pEnc->Initialize(pStream, WICBitmapEncoderNoCache);
	}

	IWICBitmapFrameEncode* pFrame = NULL;
	IPropertyBag2* pProp = NULL;

	if (SUCCEEDED(hr)) {
		hr = pEnc->CreateNewFrame(&pFrame, &pProp);
	}

	if (SUCCEEDED(hr)) {
		hr = pFrame->Initialize(pProp);
	}

	if (SUCCEEDED(hr)) {
		hr = pFrame->SetSize(width, height);
	}

	if (SUCCEEDED(hr)) {
		WICPixelFormatGUID targetFormat = pixelFormat;
		hr = pFrame->SetPixelFormat(&targetFormat);

		if (targetFormat != pixelFormat) {
			hr = E_INVALIDARG;
		}
	}

	if (SUCCEEDED(hr)) {
		size_t bpp = getBitsPerPixel(pixelFormat);
		size_t stride = (width * bpp + 7) / 8;

		hr = pFrame->WritePixels(height, static_cast<UINT>(stride), static_cast<UINT>(dataLen), const_cast<BYTE*>(data));
	}

	if (SUCCEEDED(hr)) {
		hr = pFrame->Commit();
	}

	if (SUCCEEDED(hr)) {
		hr = pEnc->Commit();
	}

    SafeRelease(&outStream);
	SafeRelease(&pStream);
	SafeRelease(&pEnc);
	SafeRelease(&pFrame);
	SafeRelease(&pProp);
	return SUCCEEDED(hr);
}

IWICImagingFactory* WICImageLoader::getWICFactory()
{
	if(NULL == _wicFactory)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

		if(SUCCEEDED(hr))
		{
			hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&_wicFactory);
		}

		if(FAILED(hr))
		{
			SafeRelease(&_wicFactory);
		}
	}

	return _wicFactory;
}

#endif

}
