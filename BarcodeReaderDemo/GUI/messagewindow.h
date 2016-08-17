#pragma once

// MessageWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageWindow window

class CMessageWindow : public CEdit
{
// Construction
public:
	CMessageWindow();

// Attributes
public:

// Operations
public:

	CMessageWindow& operator<<(const char* psz)
		{ AddMessage(psz); return *this; }
	
	CMessageWindow& operator<<(double psz)
		{ char tmp[255]; sprintf(tmp, "%.3f", psz); AddMessage(tmp); return *this; }

	CMessageWindow& operator<<(int psz)
		{ char tmp[255]; sprintf(tmp, "%d", psz); AddMessage(tmp); return *this; }
	
	void AddMessage(const char* psz);
	void SetMessage(const char* psz);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageWindow)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMessageWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMessageWindow)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
