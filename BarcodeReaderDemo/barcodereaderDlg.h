// barcodereaderDlg.h : header file
//

#pragma once

#include "gui\MessageWindow.h"
#include "gui\ParamCtrl.h"
//#include "barcodereaderlib.h"

/////////////////////////////////////////////////////////////////////////////
// CBarcodereaderDlg dialog

class CBarcodereaderDlg : public CDialog, public BarcodeReader
{
// Construction
public:
	CBarcodereaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBarcodereaderDlg)
	enum { IDD = IDD_BARCODEREADER_DIALOG };
	CParamCtrl	m_result;
	CMessageWindow	m_messages;
	int		m_sample;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarcodereaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString mVideoPath, mTrainingPath;
	HICON m_hIcon;
	int mStep;
	NumberedFileCamera *mpCamera;
	bool DoMain();

	// Generated message map functions
	//{{AFX_MSG(CBarcodereaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
