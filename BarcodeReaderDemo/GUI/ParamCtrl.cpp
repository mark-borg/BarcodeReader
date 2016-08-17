// ParamCtrl.cpp : implementation file
//

#include "../stdafx.h"
#include "ParamCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CParamCtrl::mStyle=0;
char CParamCtrl::mfilename[MAX_PATH];

/////////////////////////////////////////////////////////////////////////////
// CParamCtrl

CParamCtrl::CParamCtrl(DWORD style) : mImg(0), mpCamera(0)
{
	mStyle = style;
}

CParamCtrl::~CParamCtrl()
{
}


BEGIN_MESSAGE_MAP(CParamCtrl, CStatic)
	//{{AFX_MSG_MAP(CParamCtrl)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamCtrl message handlers

void CParamCtrl::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

	cvNamedWindowEx( mName, GetSafeHwnd(), mStyle );

	if (!(mStyle & PC_NO_CAMERA_CONNECT))
		mpCamera = new NumberedFileCamera( (const char*)mVideoPath, (const char*)mTrainingPath );
	
	if (mpCamera)
	{
		mpCamera->set_output_greyscale();
		if ( ! mpCamera->open_source() )
		{
			AfxMessageBox("Error starting camera");
			return;
		}
		if (mImg)
			cvReleaseImage( &mImg );

		// get an initial image - will be used as template
		mImg = mpCamera->acquire_image();
		if (!mImg)
		{
			AfxMessageBox("Error acquiring image");
			return;
		}
		cvShowImageEx(GetSafeHwnd(), mImg);
	}
}

void CParamCtrl::ResizeWindow(SIZE* size)
{
	CRect rect, prect;
	GetParent()->GetWindowRect(&prect);
	GetWindowRect(&rect);
	cvMoveWindowEx(GetSafeHwnd(), rect.left-prect.left, 
		rect.top-prect.top-GetSystemMetrics(SM_CYSMCAPTION), 
		0, 0, size);
}

void CParamCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (mImg)
	{
		cvPaintImage(GetSafeHwnd(), dc.GetSafeHdc());
	}
}

void CParamCtrl::OnDestroy() 
{
	if (mpCamera)
	{
		if (mImg)
			cvReleaseImage( &mImg );
		mpCamera->close_source();
		delete mpCamera;
		mpCamera = 0;
	}
	CStatic::OnDestroy();
}
