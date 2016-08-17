// barcodereaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "barcodereaderapp.h"
#include "barcodereaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarcodereaderDlg dialog

CBarcodereaderDlg::CBarcodereaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBarcodereaderDlg::IDD, pParent), mpCamera(0)
{
	//{{AFX_DATA_INIT(CBarcodereaderDlg)
	m_sample = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mStep = 0;
}

void CBarcodereaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBarcodereaderDlg)
	DDX_Control(pDX, IDC_RESULT, m_result);
	DDX_Control(pDX, IDC_MESSAGES, m_messages);
	DDX_Radio(pDX, IDC_SAMPLE_1, m_sample);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBarcodereaderDlg, CDialog)
	//{{AFX_MSG_MAP(CBarcodereaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarcodereaderDlg message handlers

BOOL CBarcodereaderDlg::OnInitDialog()
{
	m_result.SetStyle(PC_NO_CAMERA_CONNECT|PC_STRETCH_IMAGE);
	m_result.SetName("Result");

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	m_messages << "BarcodeReader initialised successfully.";
	m_messages << "Press <Start> to perform barcode analysis.";

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBarcodereaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBarcodereaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBarcodereaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#	define VIDEO_PATH_1	    "data\\ean13"
#	define TEST_FILE_MASK_1	"%s\\%03.03d.jpg"

#	define VIDEO_PATH_2	    "data\\upc-a"
#	define TEST_FILE_MASK_2	"%s\\%03.03d.jpg"

bool CBarcodereaderDlg::DoMain()
{
	m_messages << "Started ...";
	UpdateData();

	if (m_sample==0)
	{
		mVideoPath    = VIDEO_PATH_1;
		mTrainingPath = TEST_FILE_MASK_1;
	}
	else
	{
		mVideoPath    = VIDEO_PATH_2;
		mTrainingPath = TEST_FILE_MASK_2;
	}

	// use command-line path (if any)
	CBarcodereaderApp* app = (CBarcodereaderApp*) AfxGetApp();
	if ( app )
	{
		if ( !app->cmd_line_params.m_strFileName.IsEmpty() )
		{
			mVideoPath = app->cmd_line_params.m_strFileName + mVideoPath;
		}
	}
	
	m_messages << "Image source: NumberedFileCamera@" << mVideoPath;
	UpdateData();


	if (mpCamera)
		delete mpCamera;
	
	mpCamera = new NumberedFileCamera( (const char*)mVideoPath, (const char*)mTrainingPath );
//	mpCamera = new NumberedFileCamera( VIDEO_PATH, TEST_FILE_MASK );

	mpCamera->set_output_greyscale();
	if ( ! mpCamera->open_source() )
		return false;

	IplImage* img = NULL;
	IplImage* res = NULL;
	IplImage* res3 = NULL;

	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f, 0, 3 );
			
//	BarcodeReader::set_debug();
	BarcodeReader::set_max_scans( 70 );
	BarcodeReader::set_min_correct_scans( 1 );

	bool processedImage = false;

	while( ( img = mpCamera->acquire_image() ) != NULL )
	{
		m_result.ShowImage( img );

		
		string value;
		bool ok = BarcodeReader::run( img, value );


		res = cvCloneImage( img );
		res3 = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 3 );
		cvSet( res, CV_RGB( 0,0,0 ) );
		if ( ok )
		{
			cvPutText( res, ( "OK  " + value ).c_str(), cvPoint( 10, 50 ), &font, CV_RGB( 255,255,255 ) );
			overlay_result( img, res, res3, 1 );
		
			string s = "OK: Found " + ( ( m_sample == 0 ? " EAN13" : "UPC-A" ) + ( " barcode " + value ) );
			m_messages << s.c_str();
		}
		else
		{
			cvLine( res, cvPoint( 0, 0 ), cvPoint( res->width-1, res->height-1 ), CV_RGB( 255,255,255 ), 10 );
			cvLine( res, cvPoint( 0, res->height-1 ), cvPoint( res->width-1, 0 ), CV_RGB( 255,255,255 ), 10 );
			overlay_result( img, res, res3, 0 );

			string s = "ERR: No barcode found!";
			m_messages << s.c_str();
		}
		m_result.ShowImage( res3 );

		while (mStep==1)
		{
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		cvReleaseImage( &res );
		cvReleaseImage( &res3 );
		cvReleaseImage( &img );
		if (mStep==2)
			break;
		if (mStep==4)
			break;
		mStep=1;

		processedImage = true;
	}

	if ( ! processedImage )
		AfxMessageBox( "Images missing!! Please re-install the demo program." );

	mpCamera->close_source();
	if (mStep==4)
		mStep=0;
	return true;
}

void CBarcodereaderDlg::OnOK() 
{
	if (mStep==1)
	{
		mStep = 0;
		return;
	}
	if (mStep==2)
		return;
	mStep=1;
	GetDlgItem(IDOK)->SetWindowText("Step");
	if (!DoMain())
		m_messages << "Error starting barcode reader!";
	else
		m_messages << "Finished.";
	mStep=0;
	GetDlgItem(IDOK)->SetWindowText("Start");
}

void CBarcodereaderDlg::OnCancel() 
{
	mStep=2;	
	CDialog::OnCancel();
}

void CBarcodereaderDlg::OnStop() 
{
	mStep=4;	
}


