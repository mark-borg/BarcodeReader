// TCDC.cpp: implementation of the CTCDC class.
//
//////////////////////////////////////////////////////////////////////

void CTDIBDC::Stretch(BYTE* pBuffer, int w, int h)
{
	ASSERT(mBPP == 24);
	Pixel24* dst = static_cast<Pixel24*> ((void*)pbitsBuffer);
	Pixel24* src = static_cast<Pixel24*> ((void*)pBuffer);

	register int y = 0, x, width;
	
	const register WIDTH = m_rect.Width();
	const register dy = (h << 16) / m_rect.Height();
	const register dx = (w << 16) / WIDTH;
	
	register int height = m_rect.Height();
	while (height > 0)
	{
		x = 0;
		width = WIDTH;
		while (width > 0)
		{
			dst->red	= ((Pixel24*)&src[(y >> 16) * w])[x >> 16].red;
			dst->green	= ((Pixel24*)&src[(y >> 16) * w])[x >> 16].green;
			dst->blue	= ((Pixel24*)&src[(y >> 16) * w])[x >> 16].blue;
			++dst;
			x += dx;
			--width;
		}

		y += dy;
		--height;
	} 
	::SetDIBitsToDevice(GetSafeHdc(), 0, 0, m_rect.Width(), m_rect.Height(), 0, 0, 0, m_rect.Height(), 
							pbitsBuffer, &bminfo, DIB_RGB_COLORS);
}

