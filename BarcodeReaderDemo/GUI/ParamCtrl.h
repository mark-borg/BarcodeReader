#pragma once

// ParamCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParamCtrl window


#define PC_STRETCH_IMAGE     0x00
#define PC_RESIZE_TO_IMAGE   0x01
#define PC_NO_STRETCH_IMAGE  0x02
#define PC_NO_CAMERA_CONNECT 0x04

class CParamCtrl : public CStatic
{
// Construction
public:
	CParamCtrl(DWORD style=(PC_STRETCH_IMAGE|PC_NO_CAMERA_CONNECT));

// Attributes
public:

// Operations
public:
	void ResizeWindow(SIZE* size);

	void SetPath(const char* video, const char* train)
		{ mVideoPath=video, mTrainingPath=train;
		}
	void SetName(const char* name)
		{ mName = name; }
	void SetStyle(DWORD style)
		{ mStyle = style; }
	void ShowImage(IplImage* Img)
		{ mImg = Img; cvShowImageEx(GetSafeHwnd(), mImg); }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParamCtrl();

	// Generated message map functions
protected:
	CString mName, mVideoPath, mTrainingPath;
	NumberedFileCamera *mpCamera;
	static DWORD mStyle;
	static char mfilename[MAX_PATH];
	CRect	mRect;
	IplImage* mImg;
	//{{AFX_MSG(CParamCtrl)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
