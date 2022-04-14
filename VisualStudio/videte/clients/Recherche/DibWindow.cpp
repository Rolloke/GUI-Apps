// DibWindow.cpp: implementation of the CDibWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "recherche.h"
#include "DibWindow.h"

#include "RechercheDoc.h"
#include "RechercheView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	JDEX_DELIMITER	_T('@')

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDibWindow::CDibWindow()
	: CImageWindow(NULL)
{
	m_pDibData = NULL;
	m_iCameraNr = 0;
}
/////////////////////////////////////////////////////////////////////////////
CDibWindow::~CDibWindow()
{
	WK_DELETE(m_pDibData);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDibWindow, CImageWindow)
	//{{AFX_MSG_MAP(CDibWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CSecID CDibWindow::GetID()
{
	return (DWORD)GetDlgCtrlID();
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::GetTitleText(CDC* pDC)
{
	return m_sPathname;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::IsDibWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
static DWORD sID = 0x10000;
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::Create(const RECT& rect, CRechercheView* pParentWnd)
{
	m_pRechercheView = pParentWnd;

	return CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						sID++);
}
/////////////////////////////////////////////////////////////////////////////
void CDibWindow::OnDraw(CDC* pDC)
{
	CRect rect;
	
	GetVideoClientRect(rect);
	if (m_pDibData)
	{
		m_pDibData->GetDib()->OnDraw(pDC,rect);
	}
	DrawTitle(pDC);
	DrawCinema(pDC);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::LoadFile(const CString& sFileName)
{
	CString sExt;
	int p;

	m_sPathname = sFileName;
	p = sFileName.ReverseFind(_T('.'));
	sExt = sFileName.Mid(p+1);

	if (0==sExt.CompareNoCase(_T("jpx")))
	{
		return LoadJPX(sFileName);
	}
	else if (0==sExt.CompareNoCase(_T("jpg")))
	{
		return LoadJPG(sFileName);
	}
	else if (0==sExt.CompareNoCase(_T("bmp")))
	{
		return LoadDIB(sFileName);
	}
	else if (0==sExt.CompareNoCase(_T("dib")))
	{
		return LoadDIB(sFileName);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::LoadJPG(const CString& sFileName)
{
	CFile file;
	CFileStatus cfs;
	BOOL bRet = FALSE;

	if (m_Jpeg.DecodeJpegFromFile(sFileName))
	{
		HBITMAP hBitmap = m_Jpeg.CreateBitmap();
		if (hBitmap)
		{
			CDib* pDib = new CDib(hBitmap);
			if (pDib)
			{
				m_pDibData = new CDibData(pDib);
				m_sCompression = _T("JPEG");
				UpdateDialogs();
				bRet = TRUE;
			}
			DeleteObject(hBitmap);
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::LoadJPX(const CString& sFileName)
{
	CFile file;
	CFileStatus cfs;
	BOOL bRet = FALSE;
	BYTE bJpxTyp = 0;
	WORD wCount = 0;
	BYTE bPictTyp = 0;
	BYTE bResolution = 0;
	DWORD dwLen = 0;
	CString sData;
	BYTE* pBuffer = NULL;
	CString sInfoString1;
	CString sInfoString2;
	CString sComment;

	WK_TRACE(_T("reading file %s\n"),sFileName);
	if (file.Open(sFileName,CFile::modeRead))
	{
		// DateiTyp lesen
		// jpx typ lesen
		if (1==file.Read(&bJpxTyp,1))
		{
			WK_TRACE(_T("jpx typ = %d\n"),bJpxTyp);
			
			// Anzahl Bilder lesen
			if (2==file.Read(&wCount,2))
			{
				WK_TRACE(_T("Anzahl Bilder = %d\n"),wCount);

				// BildTyp lesen
				if (1==file.Read(&bPictTyp,1))
				{
					WK_TRACE(_T("pict typ = %d\n"),bPictTyp);

					// Bild Auflösung lesen
					if (1==file.Read(&bResolution,1))
					{
						WK_TRACE(_T("resolution = %d\n"),bResolution);

						if (4==file.Read(&dwLen,4))
						{
							WK_TRACE(_T("data len = %d\n"),dwLen);

							// Daten lesen
							if (dwLen==file.Read(sData.GetBuffer(dwLen),dwLen))
							{
								sData.ReleaseBuffer();
								WK_TRACE(_T("data is <%s>\n"),sData);

								// Länge des Bildes lesen
								if (4==file.Read(&dwLen,4))
								{
									WK_TRACE(_T("pict len = %d\n"),dwLen);
									pBuffer = new BYTE[dwLen];
									if (dwLen==file.Read(pBuffer,dwLen))
									{
										bRet = TRUE;
									}
									else
									{
										WK_DELETE_ARRAY(pBuffer);
										dwLen = 0;
										WK_TRACE_ERROR(_T("cannot read pict data\n"));
									}
								}
								else
								{
									WK_TRACE_ERROR(_T("cannot read pict len"));
								}
							}
							else
							{
								sData.ReleaseBuffer();
								WK_TRACE_ERROR(_T("cannot read data\n"));
							}
						}
						else
						{
							WK_TRACE_ERROR(_T("cannot read data len"));
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot read resolution\n"));
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot read pict typ\n"));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read number of pictures\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot read typ\n"));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot open file %s\n"),sFileName);
	}

	if (bRet)
	{
		// successfully read
		WK_TRACE(_T("%s successfully read analyzing data\n"),sFileName);

		int p;
		p = sData.Find(JDEX_DELIMITER);
		if (p!=-1)
		{
			sInfoString1 = sData.Left(p);
			sData = sData.Mid(p+1);
			p = sData.Find(JDEX_DELIMITER);
			if (p!=-1)
			{
				sInfoString2 = sData.Left(p);
				sData = sData.Mid(p+1);
				p = sData.Find(JDEX_DELIMITER);
				if (p!=-1)
				{
					sComment = sData.Left(p);
					sData = sData.Mid(p+1);
				}
			}
		}
		WK_TRACE(_T("InfoString1 <%s>\n"),sInfoString1);
		WK_TRACE(_T("InfoString2 <%s>\n"),sInfoString2);
		WK_TRACE(_T("Kommentar <%s>\n"),sComment);
		WK_TRACE(_T("Data <%s>%d\n"),sData,sData.GetLength());

		if (m_Jpeg.DecodeJpegFromMemory(pBuffer, dwLen))
		{
			HBITMAP hBitmap = m_Jpeg.CreateBitmap();
			if (hBitmap)
			{
				CDib* pDib = new CDib(hBitmap);
				if (pDib)
					m_pDibData = new CDibData(pDib,	sInfoString1,sInfoString2,sComment);

				DeleteObject(hBitmap);
			}
		}
		DateTimeFromInfoString(sInfoString1);
		m_Fields.FromString(sData);
		m_sCompression = _T("JPEG");
		UpdateDialogs();
		WK_DELETE_ARRAY(pBuffer);
	}
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::LoadDIB(const CString& sFileName)
{
	m_pDibData = new CDibData(new CDib(sFileName));
	m_sCompression = _T("Bitmap");
	UpdateDialogs();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
UINT CDibWindow::GetToolBarID()
{
	return IDR_DIB;
}
/////////////////////////////////////////////////////////////////////////////
void CDibWindow::PopupMenu(CPoint pt)
{
	CMenu menu;
	CMenu* pMenu;
	menu.LoadMenu(IDR_CONTEXT);
	pMenu = menu.GetSubMenu(5);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());		
}
/////////////////////////////////////////////////////////////////////////////
void CDibWindow::OnSetWindowSize()
{
	UpdateDialogs();
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::FormatPicture(BOOL bTabbed/* = FALSE*/)
{
	return m_pDibData->GetPicture();
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::FormatData(const CString& sD,const CString& sS)
{
	return StringReplace(m_pDibData->GetData(),_T(","),sD);
}
/////////////////////////////////////////////////////////////////////////////
int CDibWindow::GetCameraNr()
{
	return m_iCameraNr;
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::GetDate()
{
	return m_sDate;
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::GetTime()
{
	return m_sTime;
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::GetCompression()
{
	return m_sCompression;
}
/////////////////////////////////////////////////////////////////////////////
CString CDibWindow::FormatComment()
{
	return m_pDibData->GetComment();
}
/////////////////////////////////////////////////////////////////////////////
int	CDibWindow::GetRecNo()
{
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
int CDibWindow::GetRecCount()
{
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
CSize CDibWindow::GetPictureSize()
{
	CSize s(0,0);
	if (m_pDibData)
	{
		s.cx = m_pDibData->GetDib()->GetWidth();
		s.cy = m_pDibData->GetDib()->GetHeight();
		if (s.cx>700 && s.cy<300)
		{
			s.cy *=2;
		}
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CDibWindow::DateTimeFromInfoString(const CString& sInfoString1)
{
	int p;
	CString s;
	TCHAR szamera[] = _T("amera:");
	TCHAR szatum[] = _T("atum:");
	TCHAR szeit[] = _T("eit:");

	p = sInfoString1.Find(szatum);
	if (p!=-1)
	{
		s = sInfoString1.Mid(p+sizeof(szatum)-1);
		s.TrimLeft();
		int iDay,iMonth,iYear;
		iDay = iMonth = iYear = 0;
		if (3 == _stscanf(s,_T("%d.%d.%d"),&iDay,&iMonth,&iYear))
		{
			m_sDate.Format(_T("%02d.%02d.%d"),iDay,iMonth,iYear);
		}
	}

	p = sInfoString1.Find(szeit);
	if (p!=-1)
	{
		s = sInfoString1.Mid(p+sizeof(szeit)-1);
		s.TrimLeft();
		int iHour,iMinute,iSecond;
		iHour = iMinute = iSecond = 0;
		if (3 == _stscanf(s,_T("%d:%d:%d"),&iHour,&iMinute,&iSecond))
		{
			m_sTime.Format(_T("%02d:%02d:%02d"),iHour,iMinute,iSecond);
		}
	}

	p = sInfoString1.Find(szamera);
	if (p!=-1)
	{
		s = sInfoString1.Mid(p+sizeof(szamera)-1);
		s.TrimLeft();
		int iCameraNr = 0;
		if (1 == _stscanf(s,_T("%d"),&iCameraNr))
		{
			m_iCameraNr = iCameraNr;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString	CDibWindow::GetName()
{
	return m_sPathname;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDibWindow::Navigate(int nRecNr,int nRecCount/* = 0*/)
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CDibWindow::PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim /*= FALSE*/)
{
	int ret;
	if (!bUseDim)
	{
		rect.bottom = rect.top + (rect.Width() * 9) / 12;
	}
	m_pDibData->GetDib()->OnDraw(pDC,rect);
	ret = rect.Height();
	return ret;
}
