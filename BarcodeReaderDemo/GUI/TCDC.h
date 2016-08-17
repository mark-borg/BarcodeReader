// TCDC.h: interface for the CTCDC class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vfw.h>

class CTCDC : public CDC  
{
public:
	CTCDC();
	CTCDC(CDC* dc, CRect *rect);
	CTCDC(CDC* dc, int x, int y, int w, int h);
	virtual ~CTCDC();
	__forceinline void Draw(CDC* dc, RECT &r)
		{ dc->BitBlt(r.left, r.top, r.right - r.left, r.bottom - r.top, this, r.left, r.top, SRCCOPY);
		}

	__forceinline void Draw(CDC* dc, CRect *rect)
		{ dc->BitBlt(0, 0, rect->Width(), rect->Height(), this, 0, 0, SRCCOPY);
		}
	__forceinline void Draw(CDC* dc, int x, int y, int w, int h)
		{ dc->BitBlt(x, y, w, h, this, 0, 0, SRCCOPY);
		}
	__forceinline void Draw(CDC* dc)
		{ dc->BitBlt(0, 0, m_rect.Width(), m_rect.Height(), this, 0, 0, SRCCOPY);
		}
	
	__forceinline void Copy(CDC* dc)
		{ BitBlt(0, 0, m_rect.Width(), m_rect.Height(), dc, 0, 0, SRCCOPY);
		}
	CRect m_rect;
	CBitmap m_bitmap;
	HBITMAP m_oldbmp;
protected:
	void Destroy();
};


class CTDIBDC : public CDC  
{
public:
	CTDIBDC(DWORD comp = BI_RGB);

	CTDIBDC(CDC* dc, const char* filename);
	
	CTDIBDC(CDC* dc, int x, int y, int w, int h, int bpp = 24, DWORD comp = BI_RGB)
	{ dwCompression = comp; Make(dc, x, y, w, h, bpp); mStyle = 0;
	}

	CTDIBDC(CDC* dc, CRect *rect, int bpp = 24, DWORD comp = BI_RGB)
	{ dwCompression = comp; Make(dc, rect->left, rect->top, rect->Width(), rect->Height(), bpp); mStyle = 0;
	}

	virtual ~CTDIBDC();
	__forceinline void Draw(CDC* dc, RECT &r)
		{ dc->BitBlt(r.left, r.top, r.right - r.left, r.bottom - r.top, this, r.left, r.top, SRCCOPY);
		}

	__forceinline void Draw(CDC* dc, CRect *rect)
		{ dc->BitBlt(0, 0, rect->Width(), rect->Height(), this, 0, 0, SRCCOPY);
		}
	__forceinline void Draw(CDC* dc, int x, int y, int w, int h)
		{ dc->BitBlt(x, y, w, h, this, 0, 0, SRCCOPY);
		}
	__forceinline void Draw(CDC* dc)
		{ dc->BitBlt(0, 0, m_rect.Width(), m_rect.Height(), this, 0, 0, SRCCOPY);
		}
	
	__forceinline void Copy(CDC* dc)
		{ BitBlt(0, 0, m_rect.Width(), m_rect.Height(), dc, 0, 0, SRCCOPY);
		}

	__forceinline void StretchDraw(CTDIBDC* dc, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
		{ DrawDibDraw(hDrawDib, dc->GetSafeHdc(), 
                x1, y1, w1, h1, (BITMAPINFOHEADER*)&bminfo, pbitsBuffer, x2, y2, w2, h2, mStyle);
		}

	__forceinline void ClipDraw(CTDIBDC* dc, int x, int y, int w, int h)
		{ DrawDibDraw(hDrawDib, GetSafeHdc(), 
                0, 0, m_rect.Width(), m_rect.Height(), (BITMAPINFOHEADER*)&dc->bminfo, dc->pbitsBuffer, x, y, w, h, 0);
 		}

	__forceinline void StretchDraw(CDC* dc, int x, int y, int w, int h)
		{ DrawDibDraw(hDrawDib, dc->GetSafeHdc(), 
                x, y, w, h, (BITMAPINFOHEADER*)&bminfo, pbitsBuffer, 0, 0, m_rect.Width(), m_rect.Height(), mStyle);
		}
	__forceinline void CopyBuffer(BYTE* pBuf)
		{ pbitsBuffer=pBuf;//CopyMemory(pbitsBuffer, pBuf, bminfo.bmiHeader.biSizeImage);
		}
	__forceinline void StretchDraw(BYTE* pBuffer, int x, int y, int w, int h)
		{ 
			DrawDibDraw(hDrawDib, GetSafeHdc(), 
                0, 0,m_rect.Width(), m_rect.Height(), (BITMAPINFOHEADER*)&bminfo, pBuffer, x, y, w - x, h - y, mStyle);
		}
	__forceinline void StretchDrawEx(BYTE* pBuffer, int w, int h)
		{ DrawDibDraw(hDrawDib, GetSafeHdc(), 
                0, 0, -1, -1, (BITMAPINFOHEADER*)&bminfo, pBuffer, 0, 0, w, h, mStyle);
		}

	__forceinline void DrawBegin(int x, int y, int w, int h)
		{ DrawDibBegin(hDrawDib, GetSafeHdc(), x, y, (BITMAPINFOHEADER*)&bminfo, w, h,DDF_BUFFER);
		}

	__forceinline bool IsLoaded()
		{ return ((m_bitmap) ? true : false); }

	void SetStyle(DWORD s)
		{ mStyle = s; }
	CRect m_rect;
	HBITMAP m_bitmap;
	HBITMAP m_oldbmp;

	BITMAPINFO bminfo;
	int mBPP;
	VOID* pbitsBuffer; 
	HDRAWDIB hDrawDib;
	DWORD dwCompression;
protected:
	DWORD mStyle;
	bool DoDIBSection();
	void Make(CDC* dc, int x, int y, int w, int h, int bpp);
	void Destroy();
};

//#include "TCDC.inl"