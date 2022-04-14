// PanelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MPEG4Play2.h"
#include "PanelDlg.h"

#include "MPEG4Play2Doc.h"
#include ".\paneldlg.h"
//#include "MPEG4Play2View.h"



// CPanelDlg dialog

IMPLEMENT_DYNAMIC(CPanelDlg, CDialog)
CPanelDlg::CPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelDlg::IDD, pParent)
{
	m_strFilename = _T("");

	Create(IDD);
}

CPanelDlg::~CPanelDlg()
{
}

void CPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, OnBnClickedButtonQuit)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_NEXTPIC, OnBnClickedButtonNextPic)
	ON_BN_CLICKED(IDC_BUTTON_PREVPIC, OnBnClickedButtonPrevPic)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_END, OnBnClickedButtonEnd)
END_MESSAGE_MAP()


// CPanelDlg message handlers

void CPanelDlg::OnCancel(void)
{
	// Panel-Dialog zerstoeren, da modeless dialog box
	DestroyWindow();
}

void CPanelDlg::PostNcDestroy(void)
{
	// this loeschen, da modeless dialog box
	delete this;
}

void CPanelDlg::OnBnClickedButtonQuit()
{
	// gesamte Anwendung beenden
	GetParent()->DestroyWindow();
}


void CPanelDlg::OnBnClickedButtonOpen()
{
	// Unterscheidung: Datei wird ueber FileDialog oder ueber
	// Parameteruebergabe bei Programmstart geoeffnet
	if(m_strFilename == _T(""))
	{	// ...Dateioeffnung ueber FileDialog

		((CMPEG4Play2Doc*)(((CFrameWnd*)GetParent())->GetActiveDocument()))->SetOpenFileViaDialog(TRUE);

		// File-Dialog erstellen
		// - Instanz erstellen 
		CFileDialog fileDlg(TRUE, 
							NULL, 
							NULL, 
							OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, 
							_T("raw MPEG4 Video (*.m4v)|*.m4v|"), 
							this);

		// - m_ofn structure initialisieren
		fileDlg.m_ofn.lpstrTitle = _T("MPEG4-Datei-Auswahl");

		// - DoModal
		if ( fileDlg.DoModal() == IDOK)
		{
			if(((CFrameWnd*)GetParent())->GetActiveDocument()->OnOpenDocument(fileDlg.GetPathName()))
			{
				((CFrameWnd*)GetParent())->GetActiveView()->OnInitialUpdate();

				((CFrameWnd*)GetParent())->CenterWindow();
				((CFrameWnd*)GetParent())->ShowWindow(SW_SHOW);
				// Fenstertitel setzen
				((CFrameWnd*)GetParent())->GetActiveDocument()->SetTitle(fileDlg.GetPathName());
				((CFrameWnd*)GetParent())->UpdateWindow();
				
				// Panel neu positionieren
				CRect rectPanel;
				GetWindowRect(rectPanel);
				CRect rectStreamWnd;
				((CFrameWnd*)GetParent())->GetWindowRect(rectStreamWnd);
				MoveWindow(rectStreamWnd.right-rectPanel.Width(),
					       rectStreamWnd.bottom,
						   rectPanel.Width(),
						   rectPanel.Height());

				// Status aller Buttons setzen
				GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
				GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
				GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
				GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();
				GetDlgItem(IDC_BUTTON_START)->EnableWindow();
				GetDlgItem(IDC_BUTTON_END)->EnableWindow();
				GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
			}
			else
			{
				AfxMessageBox(_T("error reading file\n"), MB_ICONSTOP);
			}
		}
	}
	else
	{	// ...Dateioeffnung per Parameteruebergabe bei Programmstart
		if(((CFrameWnd*)GetParent())->GetActiveDocument()->OnOpenDocument(m_strFilename))
		{
			((CFrameWnd*)GetParent())->GetActiveView()->OnInitialUpdate();

			((CFrameWnd*)GetParent())->CenterWindow();
			((CFrameWnd*)GetParent())->ShowWindow(SW_SHOW);
			// Fenstertitel setzen
			((CFrameWnd*)GetParent())->GetActiveDocument()->SetTitle(m_strFilename);
			((CFrameWnd*)GetParent())->UpdateWindow();

			// Panel neu positionieren
			CRect rectPanel;
			GetWindowRect(rectPanel);
			CRect rectStreamWnd;
			((CFrameWnd*)GetParent())->GetWindowRect(rectStreamWnd);
			MoveWindow(rectStreamWnd.right-rectPanel.Width(),
				       rectStreamWnd.bottom,
				       rectPanel.Width(),
				       rectPanel.Height());

			// Status aller Buttons setzen
			GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
			GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
			GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();
			GetDlgItem(IDC_BUTTON_START)->EnableWindow();
			GetDlgItem(IDC_BUTTON_END)->EnableWindow();
			GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();

			m_strFilename = _T("");
		}
	}
}



void CPanelDlg::OnBnClickedButtonPlay()
{
	((CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument())->StartThread();

	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}

void CPanelDlg::OnBnClickedButtonStop()
{
	CMPEG4Play2Doc* pDoc;

	pDoc = (CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument();

	// Thread stoppen
	pDoc->StopThread();
    
	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == 0)		// Streamanfang erreicht?
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == pDoc->GetStreamEndPos())	// Streamende erreicht?
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();

	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_END)->EnableWindow();
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}

void CPanelDlg::OnBnClickedButtonNextPic()
{
	CMPEG4Play2Doc* pDoc;

	pDoc = (CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument();

	// naechstes Bild anzeigen
	pDoc->ShowNextPicture();

	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == 0)		// Streamanfang erreicht?
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == pDoc->GetStreamEndPos())	// Streamende erreicht?
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();
	
	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_END)->EnableWindow();
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}

void CPanelDlg::OnBnClickedButtonPrevPic()
{
	CMPEG4Play2Doc* pDoc;

	pDoc = (CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument();

	// vorheriges Bild anzeigen
	pDoc->ShowPrevPicture();

	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == 0)		// Streamanfang erreicht?
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();

	//if(ftell(pDoc->GetFile()) == pDoc->GetStreamEndPos())	// Streamende erreicht?
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow(FALSE);
	//else
	//	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();

	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_END)->EnableWindow();
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}

BOOL CPanelDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	// Button-Icons setzen
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_OPEN);
	((CButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_PLAY);
	((CButton*)GetDlgItem(IDC_BUTTON_PLAY))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_STOP);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_PREVPIC);
	((CButton*)GetDlgItem(IDC_BUTTON_PREVPIC))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_NEXTPIC);
	((CButton*)GetDlgItem(IDC_BUTTON_NEXTPIC))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_START);
	((CButton*)GetDlgItem(IDC_BUTTON_START))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_END);
	((CButton*)GetDlgItem(IDC_BUTTON_END))->SetIcon(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_ICON_QUIT);
	((CButton*)GetDlgItem(IDC_BUTTON_QUIT))->SetIcon(hIcon);

	//CRect rect;
	//GetWindowRect(&rect);
	//SetWindowPos(NULL,
	//             ::GetSystemMetrics(SM_CXSCREEN)-abs(rect.right-rect.left),
	//			 ::GetSystemMetrics(SM_CYSCREEN)-34-abs(rect.top-rect.bottom),
	//			 -1,
	//			 -1,
	//			 SWP_NOSIZE | SWP_NOZORDER);

	SetWindowPos(NULL,
	             0,
				 0,
				 -1,
				 -1,
				 SWP_NOSIZE | SWP_NOZORDER);

	// ToolTips erstellen
	m_ToolTips.Create(this);
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_OPEN), _T("Open file")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_PLAY), _T("Play")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_STOP), _T("Stop")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_PREVPIC), _T("Step back")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_NEXTPIC), _T("Step forward")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_START), _T("Go to start")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_END), _T("Go to end")); 
	m_ToolTips.AddTool((CButton*)GetDlgItem(IDC_BUTTON_QUIT), _T("Quit")); 

	return TRUE;   // return TRUE unless you set the focus to a control
}

void CPanelDlg::OnBnClickedButtonStart()
{
	((CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument())->GoToStart();

	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_END)->EnableWindow();
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}

void CPanelDlg::OnBnClickedButtonEnd()
{
	((CMPEG4Play2Doc*)((CFrameWnd*)GetParent())->GetActiveDocument())->GoToEnd();

	// Status aller Buttons setzen
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_NEXTPIC)->EnableWindow();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_END)->EnableWindow();
	GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow();
}


void CPanelDlg::SetFilename(CString strFilename)
{
	m_strFilename = strFilename;
}


CString CPanelDlg::GetFilename()
{
	return m_strFilename;
}


BOOL CPanelDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_MOUSEMOVE:
		m_ToolTips.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

