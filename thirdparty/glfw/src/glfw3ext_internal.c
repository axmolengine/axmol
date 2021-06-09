#include "glfw3ext_internal.h"

HBITMAP _glfwxCreateDIB(HDC hdc, float weight, float height, void** ppvBits, HANDLE hSection)
{
	BITMAPINFOHEADER BIH;
		
	int iSize = sizeof(BITMAPINFOHEADER);
	memset(&BIH, 0, iSize);

	BIH.biSize = iSize;
	BIH.biWidth = (LONG)(weight);
	BIH.biHeight = (LONG)height;
	BIH.biPlanes = 1;
	BIH.biBitCount = 32;
	BIH.biCompression = BI_RGB;

	return CreateDIBSection(
		hdc,
		(BITMAPINFO*)&BIH,
		DIB_RGB_COLORS,
		ppvBits,
		hSection,
		0);
}

