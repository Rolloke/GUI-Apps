/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompareDoc.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompareDoc.cpp $
// CHECKIN:		$Date: 12.07.99 15:08 $
// VERSION:		$Revision: 51 $
// LAST CHANGE:	$Modtime: 12.07.99 14:55 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Systemtime.h"
#include "ImageCompare.h"
#include "MainFrm.h"
#include "ImageCompareDoc.h"
#include "CCommentDlg.h"

#include "ImageRef.h"
#include "ImageRefList.h"

extern CImageCompareApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCompareDoc

IMPLEMENT_DYNCREATE(CImageCompareDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageCompareDoc, CDocument)
	//{{AFX_MSG_MAP(CImageCompareDoc)
	ON_COMMAND(ID_BUTTON_LEFT, OnButtonLeft)
	ON_COMMAND(ID_BUTTON_RIGHT, OnButtonRight)
	ON_COMMAND(ID_BUTTON_IMAGE_OK, OnButtonImageOk)
	ON_COMMAND(ID_BUTTON_IMAGE_WRONG, OnButtonImageWrong)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_LEFT, OnUpdateButtonLeft)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RIGHT, OnUpdateButtonRight)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_IMAGE_OK, OnUpdateButtonImageOk)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_IMAGE_WRONG, OnUpdateButtonImageWrong)
	ON_COMMAND(ID_BUTTON_FIRST, OnButtonFirst)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FIRST, OnUpdateButtonFirst)
	ON_COMMAND(ID_BUTTON_LAST, OnButtonLast)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_LAST, OnUpdateButtonLast)
	ON_COMMAND(ID_BUTTON_SAVE, OnButtonSave)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SAVE, OnUpdateButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CImageCompareDoc::CImageCompareDoc()
{
	m_BufferID1		= 0;
	m_BufferID2		= 0;
	m_ViewID		= 0;
	m_nImageCnt		= 0;
	m_pImageList	= new CImageRefList();
	m_bModify		= FALSE;

	CWK_Profile prof;
	m_sHtmPath		= prof.GetString("Log","ProtocolPath","c:\\protocol");
	 
	ScanWWWRoot();
}
	
/////////////////////////////////////////////////////////////////////////////
CImageCompareDoc::~CImageCompareDoc()
{
	WK_DELETE (m_pImageList);

	// Buffer freigeben
	IM_Load(m_BufferID1, NULL);
	IM_Load(m_BufferID2, NULL);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareDoc::OnNewDocument()
{
	if (m_pImageList->GetCount() == 0)
	{
		CString sMsg;
		sMsg.LoadString(IDS_NO_FILES_FOUND);
		AfxMessageBox(sMsg, MB_OK);
		return FALSE;
	}

	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// Erstes Bild laden
	LoadImages(m_nImageCnt);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareDoc::InitImagin(HWND hWnd)
{
	m_BufferID1	= IM_GetUniqueBuf();
	m_BufferID2	= IM_GetUniqueBuf();
	m_ViewID	= IM_GetUniqueView();
	
	if (IM_DefineView(hWnd,	m_ViewID, m_BufferID1, 0, 0, 0,	0,
					SCALE_OFF | VIEW_PROGRESSIVE | SCALE_DEST | NO_WNDPROC_HOOK) != 0)
					return FALSE;

	if (IM_DefineView(hWnd, m_ViewID, m_BufferID2, 0, 0, 0,	0,
					SCALE_OFF | VIEW_PROGRESSIVE | SCALE_DEST | NO_WNDPROC_HOOK) != 0)
					return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareDoc::LoadImages(int nIndex)
{
	CImageRef *pImageRef = GetImageRef(nIndex);
	if (!pImageRef)
		return FALSE;

	// Bilder laden
	IM_Load(m_BufferID1, pImageRef->m_sImageName1);
	IM_Load(m_BufferID2, pImageRef->m_sImageName2);
	IM_SetAspect(m_BufferID1, 2, 1);
	IM_SetAspect(m_BufferID2, 2, 1);

	// Redraw auslösen
	UpdateAllViews(NULL);
	CString sMsg, sTitle;

	if (pImageRef->m_ImageStatus == Unknown)
		sMsg.LoadString(IDS_IMAGE_NOT_PROOFED);
	else if (pImageRef->m_ImageStatus == Valid)
		sMsg.LoadString(IDS_IMAGE_VALID);
	else if (pImageRef->m_ImageStatus == Unvalid)
		sMsg.LoadString(IDS_IMAGE_UNVALID);

	sTitle.Format("(%d/%d) %s / %s %s", m_nImageCnt+1,
									  m_pImageList->GetCount(),
									  pImageRef->m_sHostName,
									  pImageRef->m_sCamName,
									  sMsg);
	SetTitle(sTitle);

	SetModifiedFlag(TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareDoc::SaveModified() 
{
	BOOL bRet = TRUE;
	
	if (m_bModify)
	{
		CString sMsg;
		sMsg.LoadString(IDS_NOT_SAVED);
		switch (AfxMessageBox(sMsg, MB_YESNOCANCEL))
		{
			case IDYES:
				OnButtonSave();
				break;
			case IDNO:
				break;
			case IDCANCEL:
				bRet = FALSE;
		}
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonSave() 
{
	CString sUserName = theApp.GetUser()->GetName();
	CSystemTime Time;
	CString sFileName;
	CString sDate, sDateTime;
	FILE *streamWrite;

	Time.GetLocalTime();
	sDate.Format("%s",Time.GetDBDate());

	WK_CreateDirectory(m_sHtmPath);

	sFileName.Format("%s\\%s %s.html", m_sHtmPath, theApp.m_pszAppName,sDate);

	streamWrite	= freopen(sFileName, "wt" , stdout);
 	if (!streamWrite)
		return;

	CString sProtocol,sChecker, sTime;
	sProtocol.LoadString(IDS_CHECK_PROTOCOL);
	sChecker.LoadString(IDS_CHECKER);
	sDateTime = Time.GetDateTime();

	WK_TRACE("------------------------------------------\n");
	WK_TRACE("Prüfprotokoll vom %s\n", sTime);
	WK_TRACE("Prüfer: %s\n", sUserName);


	// HTML-Header ausgeben
	printf("<html>\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n");
	printf("<title>%s %s</title>\n",sProtocol, sDateTime);
//	printf("<title>%s</title>\n",sProtocol);
	printf("</head>\n");
	printf("<body>\n");
	printf("<p><strong>%s %s<br>\n", sProtocol,sDateTime);
	printf("%s: %s</strong></p>\n",sChecker,sUserName);

	printf("\n");
	printf("<table border=\"1\" width=\"100%%\">\n");

	CString sHost,sCamera,sRating;

	sHost.LoadString(IDS_HOST);
	sCamera.LoadString(IDS_CAMERA);
	sRating.LoadString(IDS_RATING);

	printf("<tr>\n");
    printf("<td width=\"33%%\"><strong>%s</strong></td>\n",sHost);
    printf("<td width=\"33%%\"><strong>%s</strong></td>\n",sCamera);
    printf("<td width=\"34%%\"><strong>%s</strong></td>\n",sRating);
	printf("</tr>\n");
	
	for (int nI = 0; nI < m_pImageList->GetCount(); nI++)
	{
		CImageRef *pImageRef = GetImageRef(nI);
		if (pImageRef)
		{
			CString sHostName	= pImageRef->m_sHostName;
			CString sCamName	= pImageRef->m_sCamName;
			IStatus ImageStatus = pImageRef->m_ImageStatus;
			CString sValue		= "";
			switch (ImageStatus)
			{	
				case Valid:
					sValue.LoadString(IDS_GOOD);
					break;
				case Unvalid:
					//sValue.LoadString(IDS_BAD);
					sValue = pImageRef->m_sImageComment; 
					break;
				case Unknown:
					sValue.LoadString(IDS_NOT_CHECKED);
					break;
				default:
					WK_TRACE_ERROR("Unknown ImageStatus\n");
					break;
			}
			printf("<tr>\n");
			printf("<td width=\"33%%\">%s</td>\n", sHostName);
			printf("<td width=\"33%%\">%s</td>\n", sCamName);
			printf("<td width=\"34%%\">%s</td>\n", sValue);
			printf("</tr>\n");

			// Auch ins Logfile schreiben
			WK_TRACE("  %d.) %s/%s: %s\n", nI+1, sHostName, sCamName, sValue);
		}
	}
	
	printf("</table>\n");
	printf("</body>\n");
	printf("</html>\n");

	fclose(streamWrite);
	m_bModify = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonFirst() 
{
	m_nImageCnt = 0;
	LoadImages(m_nImageCnt);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonLeft() 
{
	m_nImageCnt--;
	m_nImageCnt = max(m_nImageCnt, 0);
	
	LoadImages(m_nImageCnt);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonRight() 
{
	m_nImageCnt++;
	m_nImageCnt = min(m_nImageCnt, m_pImageList->GetCount()-1);

	LoadImages(m_nImageCnt);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonLast() 
{
	m_nImageCnt = m_pImageList->GetCount()-1;
	LoadImages(m_nImageCnt);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonImageOk() 
{
	CImageRef *pImageRef = GetImageRef(m_nImageCnt);
	if (pImageRef)
	{
		pImageRef->m_ImageStatus = Valid;
		m_bModify = TRUE;
		OnButtonRight();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnButtonImageWrong() 
{

	CImageRef *pImageRef = GetImageRef(m_nImageCnt);
	if (pImageRef)
	{
		CComment dlg;
		if (dlg.DoModal() == IDOK)
			pImageRef->m_sImageComment	= dlg.GetComment();
		else
			pImageRef->m_sImageComment.LoadString(IDS_BAD);

		pImageRef->m_ImageStatus	= Unvalid;
		m_bModify = TRUE;
		OnButtonRight();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bModify);		
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonFirst(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nImageCnt != 0);
}
/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonLeft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nImageCnt != 0);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonRight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nImageCnt != m_pImageList->GetCount()-1);		
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonLast(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nImageCnt != m_pImageList->GetCount()-1);		
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonImageOk(CCmdUI* pCmdUI) 
{
	CImageRef *pImageRef = GetImageRef(m_nImageCnt);
	if (pImageRef)
		pCmdUI->SetCheck(pImageRef->m_ImageStatus == Valid);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::OnUpdateButtonImageWrong(CCmdUI* pCmdUI) 
{
	CImageRef *pImageRef = GetImageRef(m_nImageCnt);
	if (pImageRef)
		pCmdUI->SetCheck(pImageRef->m_ImageStatus == Unvalid);
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::ScanWWWRoot()
{
	CWK_Profile Prof;
	CHostArray	Hosts;
	CString		sHostPath		= "";	
	CString		sCamName;
	long		hFile			= NULL;
	_finddata_t fileinfo;

	Hosts.Load(Prof);
	CString sHostName;
	
	// Alle Hosts durchsuchen auch den Lokalen
	for (int nI = 0; nI <= Hosts.GetSize(); nI++)
	{
		if (nI != Hosts.GetSize())
		{
			sHostPath.Format("%s\\%08lx", CNotificationMessage::GetWWWRoot(), ((CHost*)Hosts.GetAt(nI))->GetID()); 
			sHostName = Hosts.GetAt(nI)->GetName();
		}
		else
		{
			sHostPath.Format("%s\\%08lx", 
							 CNotificationMessage::GetWWWRoot(), 
							 SECID_LOCAL_HOST); 
			sHostName.Format("Lokale Station (%s)", Prof.GetString("Hosts", "OwnID", "Unkown Name"));
		}

		if ((hFile = _findfirst(sHostPath+"\\Ist\\*.jpg", &fileinfo)) == -1)
		{
			continue;
		}

		do
		{
			sCamName.Empty();
			GetPrivateProfileString("Camera", 
									((CString)fileinfo.name).Left(8), 
									"", 
									sCamName.GetBuffer(255), 
									255, 
									sHostPath+"\\Names.ini");
			sCamName.ReleaseBuffer();
			if (!sCamName.IsEmpty())
			{
				m_pImageList->AddImageRef(sHostName, 
										  sCamName, 
										  sHostPath + "\\" + fileinfo.name, 
										  sHostPath + "\\Ist\\"  + fileinfo.name);
			}
		}while (_findnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

/////////////////////////////////////////////////////////////////////////////
CImageRef* CImageCompareDoc::GetImageRef(int nIndex)
{
	if (!m_pImageList)
		return NULL;

	return m_pImageList->GetImageRef(nIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CImageCompareDoc diagnostics
#ifdef _DEBUG
void CImageCompareDoc::AssertValid() const
{
	CDocument::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CImageCompareDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////

