// HTMLmapProperties.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "HTMLmapProperties.h"
#include <oemgui/OemFileDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_ZOOM_VAL   10
#define MAX_ZOOM_VAL 1000

/////////////////////////////////////////////////////////////////////////////
// CHTMLmapProperties dialog
CHTMLmapProperties::CHTMLmapProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CHTMLmapProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHTMLmapProperties)
	m_sPictureFileName = _T("");
	m_sPictureSize = _T("");
	m_sTitle = _T("");
	m_nZoomValue = 100;
	//}}AFX_DATA_INIT
	m_szPicture.cx = 0;
	m_szPicture.cy = 0;
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHTMLmapProperties)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Text(pDX, IDC_EDT_HM_PICTURE_FILENAME, m_sPictureFileName);
	DDV_MinChars(pDX, m_sPictureFileName, 0, IDC_TXT_HM_PICTURE_FILE);
	DDX_Text(pDX, IDC_EDT_HM_PICTURE_SIZE, m_sPictureSize);
	DDX_Text(pDX, IDC_EDT_HM_TITLE, m_sTitle);
	DDV_MinChars(pDX, m_sTitle, 0, IDC_TXT_HM_TITLE);
	DDX_Text(pDX, IDC_EDT_HM_ZOOM, m_nZoomValue);
	//}}AFX_DATA_MAP

	DDV_MinMaxInt(pDX, m_nZoomValue, MIN_ZOOM_VAL, MAX_ZOOM_VAL);

	if (!pDX->m_bSaveAndValidate)
	{
		CString str, sText;
		CMonitorInfo mi;
		int i, n = mi.GetNrOfMonitors();
		CRect rc;
		for (i=0; i<n; i++)
		{
			mi.GetMonitorRect(i, rc);
			str.Format(_T("%d.:%d x %d"), i+1,  rc.Width(), rc.Height());
			sText += str;
			if (i<n-1) sText += _T(", ");
		}
		DDX_Text(pDX, IDC_EDT_HM_MONITOR_SIZE, sText);
	}
//	DDV_MinChars(pDX, m_sPictureSize, 0);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHTMLmapProperties, CDialog)
	//{{AFX_MSG_MAP(CHTMLmapProperties)
	ON_BN_CLICKED(IDC_BTN_HM_PICTURE_FILENAME, OnBtnHmPictureFilename)
	ON_EN_CHANGE(IDC_EDT_HM_TITLE, OnChange)
	ON_EN_CHANGE(IDC_EDT_HM_ZOOM, OnChangeZoom)
	ON_EN_KILLFOCUS(IDC_EDT_HM_TITLE, OnKillfocusEdtHmTitle)
	ON_EN_CHANGE(IDC_EDT_HM_PICTURE_FILENAME, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CHTMLmapProperties::OnInitDialog() 
{
	if (!m_sPictureFileName.IsEmpty())
	{
		CheckPictureFile();
	}
	CDialog::OnInitDialog();
	m_cBtnOK.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::OnBtnHmPictureFilename() 
{
	CString sTypes;
	sTypes.LoadString(IDS_PICTURE_FILETYPES);

	COemFileDialog dlg(this);
	CString sOpen,sTitle;
	sTitle.LoadString(IDS_LOAD_PICTURE_FILE);
	sOpen.LoadString(AFX_IDS_OPENFILE);
	dlg.SetProperties(TRUE, sTitle, sOpen);
	if (theApp.m_sMapImagePath.IsEmpty() == FALSE)
	{
		dlg.SetInitialDirectory(theApp.m_sMapImagePath);
	}
	dlg.AddFilter(sTypes, _T("bmp;gif;jpg;png"));

	if (dlg.DoModal() == IDOK)
	{
		m_sPictureFileName = dlg.GetPathname();
		CString sPath, sFilename;
		WK_SplitPath(m_sPictureFileName, sPath, sFilename);
		sPath.TrimRight(_T('\\'));
		theApp.m_sMapImagePath = sPath;
		CheckPictureFile();
		if (m_szPicture.cx)
		{
			OnChange();
			UpdateData(FALSE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::OnChange() 
{
	m_cBtnOK.EnableWindow(TRUE);
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::OnChangeZoom() 
{
	if (m_cBtnOK.m_hWnd)
	{
		m_nZoomValue = GetDlgItemInt(IDC_EDT_HM_ZOOM);
	}
	if (IsBetween(m_nZoomValue, MIN_ZOOM_VAL, MAX_ZOOM_VAL))
	{
		m_sPictureSize.Format(_T("%d x %d => %d x %d"), m_szPicture.cx, m_szPicture.cy, MulDiv(m_szPicture.cx, m_nZoomValue, 100), MulDiv(m_szPicture.cy, m_nZoomValue, 100));
	}
	else
	{
		m_sPictureSize.Empty();
	}
	if (m_cBtnOK.m_hWnd)
	{
		SetDlgItemText(IDC_EDT_HM_PICTURE_SIZE, m_sPictureSize);
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::CheckPictureFile()
{
	CString sPath;
	CFile FilePict;
	
	if (m_sPath.IsEmpty() ||
		 m_sPictureFileName.ReverseFind(_T('\\')) != -1)
	{
		sPath = m_sPictureFileName;
	}
	else 
	{
		sPath = m_sPath + m_sPictureFileName;
	}
	m_szPicture.cx = m_szPicture.cy = 0;
	if (FilePict.Open(sPath, CFile::modeRead))
	{
		char szContent[4096];
		int nLength = FilePict.Read(szContent, sizeof(szContent));
		FilePict.Close();
		unsigned char szPNG[10] = " PNG\r\n \n";
		szPNG[0] = (unsigned char)0x89;
		szPNG[6] = (unsigned char)0x1a;

		if (strncmp(szContent, "GIF8", 4) == 0)						// GIF-Formtat
		{
			m_szPicture.cx = *(WORD*)&szContent[6];
			m_szPicture.cy = *(WORD*)&szContent[8];
		}
		else if (strncmp(szContent, "BM", 2) == 0)					// Bitmap / jpg
		{
			BITMAPINFO *pBmi       = (BITMAPINFO*)&szContent[sizeof(BITMAPFILEHEADER)];
			m_szPicture.cx = pBmi->bmiHeader.biWidth;
			m_szPicture.cy = pBmi->bmiHeader.biHeight;
		}
		else if (strncmp(szContent, (char*)szPNG, 8) == 0)			// PNG
		{
			char *psz = NULL, *psz2;
			for (int i=8; i<nLength-4; i++)
			{
				psz = strstr(&szContent[i], "IHDR");					// find (IHDR) Image header Chunk
				if (psz) break;
			}
			if (psz) 
			{
				psz += 4;
				psz2 = (char*)&m_szPicture.cx;							// 4 Bytes Width
				psz2[3] = psz[0];												// big endian format;
				psz2[2] = psz[1];
				psz2[1] = psz[2];
				psz2[0] = psz[3];
				psz += 4;
				psz2 = (char*)&m_szPicture.cy;							// 4 Bytes Height
				psz2[3] = psz[0];
				psz2[2] = psz[1];
				psz2[1] = psz[2];
				psz2[0] = psz[3];
			}
		}
		else																		// JPEG
		{
			bool  bFound = false, bExif = false;
			BYTE  byVal1, byVal2;
			DWORD dwLen = nLength;
			int   i = 0;
			CSize szSizeExif1(0,0), szSizeExif2(0,0);

			for (DWORD dwI = 0; dwI < dwLen-4; dwI++)
			{
				byVal1 = szContent[dwI];
				byVal2 = szContent[dwI+1];
				if (bExif == false && strncmp(&szContent[dwI], "Exif", 4) == 0)
				{
					bExif = true;												// Exif header gefunden
				}

				if (   (byVal1 == 0xff)										// SOF Header info
					 && (byVal2 == 0xc0		// Baseline DCT
					 ||  byVal2 == 0xc1		// Extended Sequential DCT
					 ||  byVal2 == 0xc2		// Progressive DCT
					 ||  byVal2 == 0xc3		// Lossless (Sequential)
					 ||  byVal2 == 0xcf))	// Differential Lossless (Arithmetic Coding)

				{
					if (dwI <= (dwLen - 8))
					{
						// Da die Bytefolge 'ffc0' nicht nur im SOF-Marker vorkommt, wird hier noch
						// zusätzlich die 'Sample precision' überprüft, die für BaselineDCT 8Bit beträgt
						if (szContent[dwI+4] == 0x08)
						{
							m_szPicture.cy = (int)MAKEWORD(szContent[dwI+6], szContent[dwI+5]);
							m_szPicture.cx = (int)MAKEWORD(szContent[dwI+8], szContent[dwI+7]);
							bFound = true;
							break;
						}
					}
					else
					{
					}
				}
				if (bExif)
				{
					if (byVal1 == 0xa0 && byVal2 == 0x02)				// X-Dimension Tag
					{
						szSizeExif1.cx = (int)MAKEWORD(szContent[dwI+11], szContent[dwI+10]);
					}
					else if (byVal1 == 0xa0 && byVal2 == 0x03)		// Y-Dimension Tag
					{
						szSizeExif1.cy = (int)MAKEWORD(szContent[dwI+11], szContent[dwI+10]);
					}
					else if (byVal1 == 0x02 && byVal2 == 0xa0)		// X-Dimension Tag big endian
					{
						szSizeExif2.cx = (int)MAKEWORD(szContent[dwI+8], szContent[dwI+9]);
					}
					else if (byVal1 == 0x03 && byVal2 == 0xa0)		// Y-Dimension Tag big endian
					{
						szSizeExif2.cy = (int)MAKEWORD(szContent[dwI+8], szContent[dwI+9]);
					}
				}
			}

			bFound = false;
			if (bExif)															// größe aus Exif header ermitteln
			{
				if (szSizeExif1.cx != 0 && szSizeExif1.cy != 0)		// beide Tags small endian
				{
					m_szPicture = szSizeExif1;
				}
				else if (szSizeExif2.cx != 0 && szSizeExif2.cy != 0)// beide Tags big endian
				{
					m_szPicture = szSizeExif2;
				}
			}
			else																	// zusätzliche Info aus JFIF header ermitteln
			{
				for (i=0; i<nLength-4; i++)
				{
					if (strncmp(&szContent[i], "JFIF", 5) == 0)		// find (JFIF) Image header Chunk
					{
						bFound = true;
						break;
					}
				}
			}
			if (bFound)
			{
				i-=2;
//					WORD wLength     = MAKEWORD(szContent[i+1], szContent[i]);
				i+=7;
//					WORD uVersion    = *((WORD*)&szContent[i]);
				i+=2;
				BYTE bUnits      = *((BYTE*)&szContent[i]);
				i+=1;
				WORD wXdensity   = MAKEWORD(szContent[i+1], szContent[i]);
				i+=2;
				WORD wYdensity   = MAKEWORD(szContent[i+1], szContent[i]);
				i+=2;
//					BYTE bXthumbnail = *((BYTE*)&szContent[i]);
				i+=1;
//					BYTE bYthumbnail = *((BYTE*)&szContent[i]);
				if (bUnits == 0)												// Pixel
				{
					if (m_szPicture.cx == 0)
					{
						m_szPicture.cx = wXdensity;
						m_szPicture.cy = wYdensity;
					}
					else
					{
//						ASSERT(m_szPicture.cx == wXdensity);
//						ASSERT(m_szPicture.cy == wYdensity);
					}
				}
				else if (bUnits == 1)										// dpi
				{
					HDC hDC = ::GetDC(m_hWnd);
					if (hDC)
					{
						int nDX = ::GetDeviceCaps(hDC, LOGPIXELSX);
						int nDY = ::GetDeviceCaps(hDC, LOGPIXELSY);
						m_szPicture.cx = MulDiv(m_szPicture.cx, wXdensity, nDX);
						m_szPicture.cy = MulDiv(m_szPicture.cy, wYdensity, nDY);
						::ReleaseDC(m_hWnd, hDC);
					}
				}
				else if (bUnits == 2)										// dpmm
				{
					HDC hDC = ::GetDC(m_hWnd);
					if (hDC)
					{
						int nDX = ::GetDeviceCaps(hDC, HORZRES) / ::GetDeviceCaps(hDC, HORZSIZE);
						int nDY = ::GetDeviceCaps(hDC, VERTRES) / ::GetDeviceCaps(hDC, VERTSIZE);
						m_szPicture.cx = MulDiv(m_szPicture.cx, wXdensity, nDX);
						m_szPicture.cy = MulDiv(m_szPicture.cy, wYdensity, nDY);
						::ReleaseDC(m_hWnd, hDC);
					}
				}
			}
		}
	}
	if (m_szPicture.cx)
	{
		OnChangeZoom();
	}
	else
	{
		m_sPictureSize= _T("UnKnown");
		// error
		WK_TRACE_WARNING(_T("HTMLMap tried to open unknown picture format %s\n"), sPath);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::CheckCopyPictureFile()
{
	int nFind = m_sPictureFileName.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		CString sFileName = m_sPictureFileName.Mid(nFind+1);
		CString sPath = m_sPath + sFileName;
		CopyFile(m_sPictureFileName, sPath, FALSE);
		m_sPictureFileName = sFileName;
	}
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::OnKillfocusEdtHmTitle() 
{
	CDataExchange dx(this, TRUE);
	DDX_Text(&dx, IDC_EDT_HM_TITLE, m_sTitle);
}
//////////////////////////////////////////////////////////////////////////
void CHTMLmapProperties::OnOK() 
{
	if (m_sPictureSize.IsEmpty())
	{
		if (m_nZoomValue < MIN_ZOOM_VAL) m_nZoomValue = MIN_ZOOM_VAL;
		if (m_nZoomValue > MAX_ZOOM_VAL) m_nZoomValue = MAX_ZOOM_VAL;
		SetDlgItemInt(IDC_EDT_HM_ZOOM, m_nZoomValue);
		OnChangeZoom();
	}
	else
	{
		CDialog::OnOK();
	}
}
