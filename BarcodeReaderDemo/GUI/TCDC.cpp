// TCDC.cpp: implementation of the CTCDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCDC::CTCDC()
{
}

CTCDC::CTCDC(CDC* dc, int x, int y, int w, int h)
{
	m_rect = CRect(x, y, w, h);
	CreateCompatibleDC(dc);
	m_bitmap.CreateCompatibleBitmap(dc, m_rect.Width(), m_rect.Height());
	m_oldbmp = (HBITMAP)SelectObject(m_bitmap);
}

CTCDC::CTCDC(CDC* dc, CRect *rect)
{
	CreateCompatibleDC(dc);
	m_bitmap.CreateCompatibleBitmap(dc, rect->Width(), rect->Height());
	m_oldbmp = (HBITMAP)SelectObject(m_bitmap);
	m_rect = *rect;
}

CTDIBDC::CTDIBDC(CDC* dc, const char* filename)
{
	dwCompression = BI_RGB;
	mStyle = 0;
	
	CDC memDC;
	BITMAP bmp;

	HBITMAP hbmp = (HBITMAP)::LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);

	if (!hbmp)
	{
		m_bitmap = 0;
		AfxMessageBox("Error loading image");
		return;
	}
	memDC.CreateCompatibleDC(NULL);

	::GetObject(hbmp, sizeof(bmp), &bmp);
	HBITMAP oldBmp = (HBITMAP)memDC.SelectObject(hbmp);

	Make(dc, 0, 0, bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel);
	
	BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(oldBmp);

	DeleteObject(hbmp);
}

void CTCDC::Destroy()
{
	SelectObject(m_oldbmp);
	m_bitmap.DeleteObject();
	DeleteDC();
}

CTCDC::~CTCDC()
{
	Destroy();
}

CTDIBDC::CTDIBDC(DWORD comp)
{ dwCompression = comp; mStyle=0;
}

void CTDIBDC::Destroy()
{
	SelectObject(m_oldbmp);
	DeleteObject(m_bitmap);
	DeleteDC();
    DrawDibClose(hDrawDib);
}

CTDIBDC::~CTDIBDC()
{
	Destroy();
}

void CTDIBDC::Make(CDC* dc, int x, int y, int w, int h, int bpp)
{
	ZeroMemory(&bminfo, sizeof(bminfo));
	bminfo.bmiHeader.biCompression = dwCompression;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biSize = sizeof(bminfo.bmiHeader);
	mBPP = bpp;
	m_rect = CRect(x, y, w, h);
	
	CreateCompatibleDC(dc);

	DoDIBSection();
	
	m_oldbmp = (HBITMAP)SelectObject(m_bitmap);
	
	hDrawDib = DrawDibOpen();
}

bool CTDIBDC::DoDIBSection()
{
	bminfo.bmiHeader.biBitCount = mBPP;
	bminfo.bmiHeader.biHeight = m_rect.Height();
	bminfo.bmiHeader.biWidth = m_rect.Width();
	bminfo.bmiHeader.biSizeImage = m_rect.Width() * m_rect.Height() * ((mBPP + 7)/8);
	
	// Create a 32 bits depth bitmap and select it into the memory DC 
	BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
			sizeof(BITMAPINFOHEADER),	// biSize 
			m_rect.Width(),				// biWidth; 
			m_rect.Height(),			// biHeight; 
			1,							// biPlanes; 
			mBPP,						// biBitCount 
			dwCompression,						// biCompression; 
			0,							// biSizeImage; 
			0,							// biXPelsPerMeter; 
			0,							// biYPelsPerMeter; 
			0,							// biClrUsed; 
			0							// biClrImportant; 
	};
	pbitsBuffer = 0;
	m_bitmap = ::CreateDIBSection(GetSafeHdc(), (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbitsBuffer, NULL, 0);
    GdiFlush();
	return (m_bitmap != NULL);
}

