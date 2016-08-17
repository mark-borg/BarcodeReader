// MessageWindow.cpp : implementation file
//

#include "../stdafx.h"
#include "MessageWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageWindow

CMessageWindow::CMessageWindow()
{
}

CMessageWindow::~CMessageWindow()
{
}


BEGIN_MESSAGE_MAP(CMessageWindow, CEdit)
	//{{AFX_MSG_MAP(CMessageWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageWindow message handlers

void CMessageWindow::PreSubclassWindow() 
{
	CEdit::PreSubclassWindow();
	SetReadOnly();
}

void CMessageWindow::AddMessage(const char* psz)
{
	CString spsz;
	GetWindowText(spsz);
	
	SetSel(spsz.GetLength(), spsz.GetLength());
	
	spsz=psz;
	spsz+="\r\n";
	
	ReplaceSel(spsz);
	
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CMessageWindow::SetMessage(const char* psz)
{
	SetWindowText(psz);
}

