// NotificationPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "NotificationPage.h"

#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Subitem defines des ListControl
#define SUB_ITEM_NAME		0
#define SUB_ITEM_MESSAGE	1
#define SUB_ITEM_URL		2

/////////////////////////////////////////////////////////////////////////////
// CNotificationPage dialog
CNotificationPage::CNotificationPage(CSVView* pParent) : CSVPage(CNotificationPage::IDD)
{
	m_pParent = pParent;
	m_pNotifications = pParent->GetDocument()->GetNotifications();
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pSelectedNotification = NULL;
	m_iSelectedItem = -1;
	m_iKind = 0;

	//{{AFX_DATA_INIT(CNotificationPage)
	m_sName = _T("");
	m_sMessage = _T("");
	m_sURL = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}


void CNotificationPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotificationPage)
	DDX_Control(pDX, IDC_RADIO_URL, m_radioURL);
	DDX_Control(pDX, IDC_RADIO_MESSAGE, m_radioMessage);
	DDX_Control(pDX, IDC_EDIT_URL, m_editURL);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
	DDX_Control(pDX, IDC_NAME_NOTIFICATION, m_editName);
	DDX_Control(pDX, IDC_NOTIFICATION, m_Notification);
	DDX_Text(pDX, IDC_NAME_NOTIFICATION, m_sName);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_sMessage);
	DDX_Text(pDX, IDC_EDIT_URL, m_sURL);
	DDX_Radio(pDX, IDC_RADIO_MESSAGE, m_iKind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotificationPage, CSVPage)
	//{{AFX_MSG_MAP(CNotificationPage)
	ON_EN_CHANGE(IDC_NAME_NOTIFICATION, OnChangeName)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_NOTIFICATION, OnItemchangedNotification)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT_MESSAGE, OnChangeEditMessage)
	ON_EN_CHANGE(IDC_EDIT_URL, OnChangeEditUrl)
	ON_BN_CLICKED(IDC_RADIO_MESSAGE, OnRadioMessage)
	ON_BN_CLICKED(IDC_RADIO_URL, OnRadioUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotificationPage message handlers

BOOL CNotificationPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[3];
	int i;

	m_Notification.GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_NAME)->GetWindowText(s[0]);
	GetDlgItem(IDC_RADIO_MESSAGE)->GetWindowText(s[1]);
	s[2].LoadString(IDS_FILE);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = (rect.Width() -3)/ 3;

	for (i = 0; i < 3; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_Notification.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}
	
	FillNotificationListBox();

	int	c = m_Notification.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	EnableExceptNew();


	LONG dw = ListView_GetExtendedListViewStyle(m_Notification.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_Notification.m_hWnd,dw);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationPage::CanNew()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationPage::CanDelete()
{
	return (m_Notification.GetItemCount()>0) && (m_pSelectedNotification != NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnNew()
{
	CNotificationMessage* pNM = m_pNotifications->AddNotificationMessage();
	CString s;

	s.LoadString(IDS_NEW_NOTIFICATION);
	pNM->SetName(s);
	int i = InsertItem(pNM);
	SelectItem(i);
	SetModified(TRUE,TRUE);
	EnableExceptNew();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
int	CNotificationPage::InsertItem(CNotificationMessage* pNM)
{
	LV_ITEM lvis;
	CString sName;
	int i,c;

	sName = pNM->GetName();

	c = m_Notification.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = sName.GetBuffer(0);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pNM;
	lvis.iItem = c;
	lvis.iSubItem = SUB_ITEM_NAME;

	i = m_Notification.InsertItem(&lvis);
	sName.ReleaseBuffer();

	m_Notification.SetItemText(i,1,pNM->GetMessage());
	m_Notification.SetItemText(i,2,pNM->GetURL());
	
	return i;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationPage::CheckActivationsWithNotification(CSecID nID)
{
	// first check wether there is any Activation (CInputToOutput) with
	// notification
	CInputGroup* pInputGroup;
	CInput* pInput;
	CInputToOutput* pInputToOutput;
	int i,j,k,c,d,e;
	BOOL bShowMB = TRUE;

	c = m_pInputList->GetSize();
	for (i=0;i<c;i++)
	{
		pInputGroup = m_pInputList->GetGroupAt(i);
		d = pInputGroup->GetSize();
		for (j=0;j<d;j++)
		{
			pInput = pInputGroup->GetInput(j);
			e = pInput->GetNumberOfInputToOutputs();
			for (k=e-1;k>=0;k--)
			{
				pInputToOutput = pInput->GetInputToOutput(k);
				if (pInputToOutput->GetNotificationID() == nID)
				{
					// we still have one
					if (bShowMB)
					{
						if (IDYES==AfxMessageBox(IDP_DELETE_NOTIFICATION_WITH_ACTIVATION,MB_YESNO))
						{
							bShowMB = FALSE;
						}
						else
						{
							return FALSE;
						}
					}
					pInput->DeleteInputToOutput(pInputToOutput);
				}
			} // inputstooutputs
		} // inputs
	}//groups
	if (bShowMB==FALSE)
	{
		m_pInputList->Save(GetProfile(),IsLocal());
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedNotification==NULL))
	{
		return;
	}

#ifdef _DEBUG
	if (m_pSelectedNotification->GetID().GetSubID() == 0xffff)
	{
		return;
	}
#endif

	if (CheckActivationsWithNotification(m_pSelectedNotification->GetID()))
	{
		m_pNotifications->DeleteNotificationMessage(m_pSelectedNotification);
		m_pSelectedNotification = NULL;
		// calc new selection
		int	newCount = m_Notification.GetItemCount()-1;	// pre-subtract before DeleteItem
		int i = m_iSelectedItem;	// new selection

		// range check
		if (i > newCount-1 )
		{	// outside ?
			i=newCount-1;
		}
		else if (newCount==0)
		{	// last one ?
			i = -1;
		}

		if (m_Notification.DeleteItem(m_iSelectedItem))	// remove from listCtrl
		{
			SelectItem(i);
			SetModified(TRUE,TRUE);
			EnableExceptNew();
			m_Notification.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::SaveChanges()
{
	WK_TRACE_USER(_T("Benachrichtigungen-Einstellungen wurden geändert\n"));
	UpdateData();
	m_pNotifications->Save(GetProfile());
	GenerateHTMLFile();
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::GenerateHTMLFile()
{
	CString sFileName;
	CFile file;
	CFileException cfe;

	sFileName.Format(_T("%s\\notification.htm"),theApp.m_sLocalWWW);

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sText = _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">") HTML_LINEFEED;
		sText.Write(file);

		sL.LoadString(IDS_NOTIFICATIONPAGE);
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_NOTIFICATION, sL);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"),sL, HTML_LINEFEED);
		sText.Write(file);

		CNotificationMessage* pNM;
		int i,c;
		c = m_pNotifications->GetSize();
		for (i=0;i<c;i++)
		{
			pNM = m_pNotifications->GetAt(i);
			if (pNM)
			{
				if (pNM->GetURL().IsEmpty())
				{
					sText.Format(_T("<a href=\"%08lx.htm\"> %s </a><br>%s"),
						pNM->GetID(), pNM->GetName(), HTML_LINEFEED);
					GenerateHTMLFile(pNM);
				}
				else
				{
					sText.Format(_T("<a href=\"%s\"> %s </a><br>%s"),
						pNM->GetURL(), pNM->GetName(), HTML_LINEFEED);
				}
				sText.Write(file);
			}
		}
		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open input file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::GenerateHTMLFile(CNotificationMessage* pNM)
{
	CString sFileName;

	sFileName.Format(_T("%s\\%lx.htm"),theApp.m_sLocalWWW,pNM->GetID().GetID());

	CFile file;
	CFileException cfe;

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sText = _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">") HTML_LINEFEED;
		sText.Write(file);

		sL.LoadString(IDS_NOTIFICATION);
		sL += _T(" ") + pNM->GetName();
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_NOTIFICATION, sL);
		sText.Write(file);


		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"),sL, HTML_LINEFEED);
		sText.Write(file);

		if (!pNM->GetMessage().IsEmpty())
		{
			sText.Format(_T("%s<br>%s"),pNM->GetMessage(), HTML_LINEFEED);
			sText.Write(file);
		}
		
		sText.Format(_T("<hr>%s"), HTML_LINEFEED);
		sText.Write(file);

		sL.LoadString(IDS_NOTIFICATIONPAGE);
		sText.Format(_T("<a href=\"notification.htm\">%s</a><br>%s"), sL, HTML_LINEFEED);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open notification file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::ClearPageExceptList()
{
	m_iSelectedItem = -1;
	m_pSelectedNotification = NULL;
	m_sName.Empty();
	m_sMessage.Empty();
	m_sURL.Empty();
	m_iKind = 0;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::CancelChanges()
{
	ClearPageExceptList();
	m_pNotifications->Load(GetProfile());
	FillNotificationListBox();
	int	c = m_Notification.GetItemCount();
	SelectItem((c>0) ? 0 : -1);
	EnableExceptNew();
	if (c>=0) {
		m_Notification.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationPage::IsDataValid()
{
	CNotificationMessage* pNM;
	for (int i=0 ; i<m_pNotifications->GetSize() ; i++) 
	{
		pNM = m_pNotifications->GetAt(i);
		if (pNM)
		{
#ifdef _DEBUG
			if (pNM->GetID().GetSubID() == 0xffff)
			{
				continue;
			}
#endif
			if (pNM->GetURL().IsEmpty() && pNM->GetMessage().IsEmpty())
			{
				CString sMsg;
				sMsg.Format(IDP_NOTIFICATION_NO_MESSAGE_NOR_URL, pNM->GetName());
				AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::EnableExceptNew()
{
	BOOL bEnable;
	int c;
	c = m_Notification.GetItemCount();
	bEnable = (c>0);
	m_Notification.EnableWindow(bEnable);
#ifdef _DEBUG
	if (   m_pSelectedNotification
		 && m_pSelectedNotification->GetID().GetSubID() == 0xffff)
	{
		bEnable = FALSE;
	}
#endif
	m_editName.EnableWindow(bEnable);
	m_radioMessage.EnableWindow(bEnable);
	m_radioURL.EnableWindow(bEnable);

	m_editMessage.EnableWindow(bEnable && m_iKind==0);
	m_editURL.EnableWindow(bEnable && m_iKind==1);
	m_btnBrowse.EnableWindow(bEnable && m_iKind==1);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::SelectItem(int i)
{
	if (i>=0)
	{
		CString sTyp;
		m_iSelectedItem = i;
		m_pSelectedNotification = (CNotificationMessage*)m_Notification.GetItemData(m_iSelectedItem);
		m_sName = m_pSelectedNotification->GetName();
		m_sMessage = m_pSelectedNotification->GetMessage();
		m_sURL = m_pSelectedNotification->GetURL();
		if (!m_sURL.IsEmpty())
		{
			m_iKind = 1;
		}
		// wenn keine URL angegeben, Message annehmen
		else
		{
			m_iKind = 0;
		}
		
#ifdef _DEBUG
		if (m_pSelectedNotification->GetID().GetSubID() == 0xffff)
		{
			m_iKind = -1;
		}
#endif

		m_Notification.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}
	else
	{
		ClearPageExceptList();
	}
	UpdateData(FALSE);
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::FillNotificationListBox()
{
	m_Notification.DeleteAllItems();

	int i,c;
	CNotificationMessage* pNM;

	c = m_pNotifications->GetSize();

	for (i=0; i<c; i++)
	{
		pNM = (CNotificationMessage*)m_pNotifications->GetAt(i);
		InsertItem(pNM);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnItemchangedNotification(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectItem(pNMListView->iItem);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnChangeName() 
{
	UpdateData();

	if (m_pSelectedNotification)
	{
		m_pSelectedNotification->SetName(m_sName);
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedNotification->GetName();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = SUB_ITEM_NAME;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Notification.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnRadioMessage() 
{
	UpdateData();
	// Die URL ist nicht mehr gueltig, deshalb im ListControl austragen
	if (m_pSelectedNotification)
	{
		m_pSelectedNotification->SetURL(_T(""));
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedNotification->GetURL();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = SUB_ITEM_URL;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Notification.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	// Kann ja bereits eine message vorhanden sein, in die ListView eintragen
	OnChangeEditMessage();
	EnableExceptNew();	
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnRadioUrl() 
{
	UpdateData();
	// Die Message ist nicht mehr gueltig, deshalb im ListControl austragen
	if (m_pSelectedNotification)
	{
		m_pSelectedNotification->SetMessage(_T(""));
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedNotification->GetMessage();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = SUB_ITEM_MESSAGE;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Notification.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	// Kann ja bereits eine message vorhanden sein, im ListControl eintragen
	OnChangeEditUrl();
	EnableExceptNew();	
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnChangeEditMessage() 
{
	UpdateData();

	if (m_sMessage.GetLength()>1024)
	{
		AfxMessageBox(IDP_MESSAGE_TOO_LONG,MB_ICONSTOP);
		m_sMessage = m_sMessage.Left(1024);
		m_editMessage.SetSel(0,-1);
		m_editMessage.SetFocus();
		return;
	}

	if (m_pSelectedNotification)
	{
		m_pSelectedNotification->SetMessage(m_sMessage);
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedNotification->GetMessage();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = SUB_ITEM_MESSAGE;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Notification.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	SetModified(TRUE,TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnChangeEditUrl() 
{
	UpdateData();
	int iStart, iEnd;
	m_editURL.GetSel(iStart, iEnd);
	m_sURL = StringReplace(m_sURL, _T('\\'), _T('/'));
	UpdateData(FALSE);
	m_editURL.SetSel(iEnd, iEnd);
	if (m_pSelectedNotification)
	{
		m_pSelectedNotification->SetURL(m_sURL);
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedNotification->GetURL();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = SUB_ITEM_URL;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Notification.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	SetModified(TRUE,TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationPage::OnButtonBrowse() 
{
	COemFileDialog dlg(this);
	CString sLoad,sHTML;

	sLoad.LoadString(IDS_LOAD);
	sHTML.LoadString(IDS_HTML);
	dlg.SetProperties(TRUE,sLoad,sLoad);
	WK_CreateDirectory(CNotificationMessage::GetWWWRoot());
	dlg.SetInitialDirectory(CNotificationMessage::GetWWWRoot());
	dlg.AddFilter(sHTML,_T("htm"));
	if (IDOK==dlg.DoModal())
	{
		m_sURL = dlg.GetPathname();
		UpdateData(FALSE);
		// URL hat sich geaendert
		OnChangeEditUrl();
	}
}
