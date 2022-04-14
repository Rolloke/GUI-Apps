// InputDeactivationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "InputDeactivationDialog.h"
#include "ObjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDeactivationDialog dialog
CInputDeactivationDialog::CInputDeactivationDialog(CObjectView* pParent, CServer* pServer, CSecID id)
	: CWK_Dialog(IDD_INPUT_DEACTIVATION, pParent)
{
	m_pParent           = pParent;
	m_pServer           = pServer;
	m_wServerID			= pServer->GetID();
	m_InputID           = id;
	m_pAlarmOffSpans    = NULL;
	m_iSelectedItemIndex= -1;
	m_bChanged			= FALSE;

	//{{AFX_DATA_INIT(CInputDeactivationDialog)
	//}}AFX_DATA_INIT
	Create(IDD_INPUT_DEACTIVATION, pParent);
}
/////////////////////////////////////////////////////////////////////////////
CInputDeactivationDialog::~CInputDeactivationDialog()
{
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDeactivationDialog)
	DDX_Control(pDX, IDOK, m_Button_OK);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_Button_New);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_Button_Delete);
	DDX_Control(pDX, IDC_STOP_TIME, m_ctrlStopTime);
	DDX_Control(pDX, IDC_START_TIME, m_ctrlStartTime);
	DDX_Control(pDX, IDC_STOP_DATE, m_ctrlStopDate);
	DDX_Control(pDX, IDC_START_DATE, m_ctrlStartDate);
	DDX_Control(pDX, IDC_LIST_DEACTIVATIONS, m_ListSpans);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CInputDeactivationDialog, CDialog)
	//{{AFX_MSG_MAP(CInputDeactivationDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_START_DATE, OnDatetimechangeStartDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_START_TIME, OnDatetimechangeStartTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STOP_DATE, OnDatetimechangeStopDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STOP_TIME, OnDatetimechangeStopTime)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEACTIVATIONS, OnClickListDeactivations)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEACTIVATIONS, OnDblclkListDeactivations)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DEACTIVATIONS, OnRclickListDeactivations)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_DEACTIVATIONS, OnRdblclkListDeactivations)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEACTIVATIONS, OnItemchangedListDeactivations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CInputDeactivationDialog message handlers
BOOL CInputDeactivationDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();

	//------------------------------  Liste formatieren
	CString sHeader;
	int iColWidth;
	LVCOLUMN LVColumn;

	CRect rect;
	m_ListSpans.GetClientRect(rect);

	iColWidth = (int)(rect.Width() / 2);

	sHeader.LoadString(IDS_VON);

	LVColumn.mask       = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	LVColumn.pszText    = (LPTSTR) LPCTSTR(sHeader);				//Column Header
	LVColumn.fmt        = LVCFMT_LEFT;						//Column Alignment
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size
	LVColumn.cx			= iColWidth;						//Column width

	m_ListSpans.InsertColumn(0,&LVColumn);

	sHeader.LoadString(IDS_BIS);
	LVColumn.pszText    = (LPTSTR) LPCTSTR(sHeader);				//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size

	m_ListSpans.InsertColumn(1,&LVColumn);

	LONG dw = ListView_GetExtendedListViewStyle(m_ListSpans.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_ListSpans.m_hWnd,dw);

	m_ctrlStartDate.SetFormat(_T("ddd, dd. MMM yyy"));
	m_ctrlStopDate.SetFormat(_T("ddd, dd. MMM yyy"));

	SetActualTime();	  
	CheckTimeSpan();

	//------------------------------  AlarmOffSpans holen
	
	CIPCInputVision *pInput = m_pServer->GetInput();

	CString sInputName = pInput->GetInputNameFromID(m_InputID);
	SetDlgItemText(IDC_TXT_INPUT,sInputName);

	m_pAlarmOffSpans = pInput->GetAlarmOffSpans();
	if(m_pAlarmOffSpans)
	{
		UpdateListSpans(FALSE);
//		OnChangeListEntry();
	}

	EnableControls();

	ShowWindow(SW_SHOW);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::PostNcDestroy() 
{
	m_pParent->m_pInputDeactivationDialog = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnOK() 
{
	// TODO set spans !
	CIPCInputVision *pInput = m_pServer->GetInput();
	pInput->RequestSetAlarmOffSpans(m_InputID, *m_pAlarmOffSpans);

	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnButtonNew() 
{
	CSystemTime SystemTime, SystemDate;
	CString sDateTime;

	//CheckTimeSpan();

				//-------------------------------------   Startzeit

	m_ctrlStartTime.GetTime(&SystemTime);			
	m_ctrlStartDate.GetTime(&SystemDate);

	CSystemTime SystemStartDateTime(SystemDate.GetDay(),
									SystemDate.GetMonth(),
									SystemDate.GetYear(),
									SystemTime.GetHour(),
									SystemTime.GetMinute(),
									SystemTime.GetSecond());

	sDateTime=SystemStartDateTime.GetDateTime(); 
	
			 //------------------------------------   Stopzeit	

	m_ctrlStopTime.GetTime(&SystemTime);			
	m_ctrlStopDate.GetTime(&SystemDate);

	CSystemTime SystemStopDateTime (SystemDate.GetDay(),
									SystemDate.GetMonth(),
									SystemDate.GetYear(),
									SystemTime.GetHour(),
									SystemTime.GetMinute(),
									SystemTime.GetSecond());
	
	sDateTime=SystemStopDateTime.GetDateTime();

	//neuer Eintrag ins Array
	CSystemTimeSpan *pSystemTimeSpan = new CSystemTimeSpan(SystemStartDateTime,
													         SystemStopDateTime);
	m_pAlarmOffSpans->Add(pSystemTimeSpan);
	UpdateListSpans(TRUE);

	m_bChanged = TRUE;
	EnableControls();
	m_ListSpans.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnButtonDelete() 
{
	if(m_iSelectedItemIndex == -1)
	{
		SetActualTime();
		return;
	}
	if(m_ListSpans.GetItemCount()==0)
	{
		SetActualTime();
		return;
	}

	CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);
	
	m_pAlarmOffSpans->RemoveAt(m_iSelectedItemIndex);
	WK_DELETE(pTimespan);

	if(m_pAlarmOffSpans->GetSize() == 0)
	{
		SetActualTime();
	}

	UpdateListSpans(FALSE);
	
	if (m_iSelectedItemIndex>m_ListSpans.GetItemCount()-1)
	{
	   m_iSelectedItemIndex = m_ListSpans.GetItemCount()-1;
	}
	m_ListSpans.SetItemState(m_iSelectedItemIndex,
							 LVIS_SELECTED|LVIS_FOCUSED,
							 0x000F);
	m_ListSpans.SetFocus();
	m_bChanged = TRUE;
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::UpdateListSpans(BOOL bAddItem)
{
	if(bAddItem) //Über Button New hinzugefügtes Element wird an die Liste angehängt
	{
		CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_pAlarmOffSpans->GetSize()-1);

		CSystemTime StartTime = pTimespan->GetStartTime();
		CSystemTime StopTime  = pTimespan->GetEndTime();

		int c = m_ListSpans.GetItemCount();
		m_iSelectedItemIndex = m_ListSpans.InsertItem (LVIF_TEXT|LVIF_STATE,
														c,
														StartTime.GetDateTime(),
														LVIS_SELECTED|LVIS_FOCUSED,
														0,0,0);
		m_ListSpans.SetItemText(m_iSelectedItemIndex,1,StopTime.GetDateTime());
	}
	else		 //Gesamte Listenansicht wir aktualisiert
	{
		 m_ListSpans.DeleteAllItems();
		 for(int i=0; i<m_pAlarmOffSpans->GetSize();i++)
		 {
			CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(i);

			CSystemTime StartTime = pTimespan->GetStartTime();
			CSystemTime StopTime  = pTimespan->GetEndTime();

			if (i == 0)
			{
				m_iSelectedItemIndex = m_ListSpans.InsertItem (LVIF_TEXT|LVIF_STATE,
															   i,
															   StartTime.GetDateTime(),
															   LVIS_SELECTED|LVIS_FOCUSED,
															   0,0,0);
				m_ctrlStartDate.SetTime(StartTime);
				m_ctrlStartTime.SetTime(StartTime);	

				m_ctrlStopDate.SetTime(StopTime);
				m_ctrlStopTime.SetTime(StopTime);
			}
			else
			{
				m_ListSpans.InsertItem (i,StartTime.GetDateTime());
			}
			m_ListSpans.SetItemText(i,1,StopTime.GetDateTime());
		 }
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputDeactivationDialog::CheckTimeSpan(int iCtrl)

// iCtrl = 1 : StartTime wurde geändert
// iCtrl = 2 : StopTime wurde geändert
// iCtrl = 3 : StartDate wurde geändert
// iCtrl = 4 : StopDate wurde geändert

{
	long lRet;						//Plausibilitätsprüfung der Start- und Endzeit
	SYSTEMTIME start,end,current;
	FILETIME   fStart,fEnd,fcurrent;
	CTime timestart,timeend, CurrentTime ;

	CSystemTime SystemTime,
				SystemDate;


	
	if(iCtrl !=4 )
	{
		
		m_ctrlStartTime.GetTime(&SystemTime);			
		m_ctrlStartDate.GetTime(&SystemDate);
		
		CSystemTime SystemStartDateTime(SystemDate.GetDay(),
										SystemDate.GetMonth(),
										SystemDate.GetYear(),
										SystemTime.GetHour(),
										SystemTime.GetMinute(),
										SystemTime.GetSecond());
								
		//------------------------------------   Stopzeit	

		m_ctrlStopTime.GetTime(&SystemTime);			

	//Version 2.1: 
		//1. Begrenzung des Start-/Endpunktes einer Zeitspanne auf den gleichen Tag
		m_ctrlStopDate.SetTime(&SystemDate);	

		CSystemTime SystemStopDateTime (SystemDate.GetDay(),
										SystemDate.GetMonth(),
										SystemDate.GetYear(),
										SystemTime.GetHour(),
										SystemTime.GetMinute(),
										SystemTime.GetSecond());
		timestart= SystemStartDateTime.GetCTime();
		timeend  = SystemStopDateTime.GetCTime();
	}
	else if(iCtrl != 3)
	{
		m_ctrlStopTime.GetTime(&SystemTime);			
		m_ctrlStopDate.GetTime(&SystemDate);

		CSystemTime SystemStopDateTime(SystemDate.GetDay(),
										SystemDate.GetMonth(),
										SystemDate.GetYear(),
										SystemTime.GetHour(),
										SystemTime.GetMinute(),
										SystemTime.GetSecond());
								
		//------------------------------------   Stopzeit	
		
		m_ctrlStartTime.GetTime(&SystemTime);			
		
		//Version 2.1: 
		//1. Begrenzung des Start-/Endpunktes einer Zeitspanne auf den gleichen Tag
		m_ctrlStartDate.SetTime(&SystemDate);	
		
		CSystemTime SystemStartDateTime(SystemDate.GetDay(),
										SystemDate.GetMonth(),
										SystemDate.GetYear(),
										SystemTime.GetHour(),
										SystemTime.GetMinute(),
										SystemTime.GetSecond());
		timestart= SystemStartDateTime.GetCTime();
		timeend  = SystemStopDateTime.GetCTime();
	}


	CurrentTime=CTime::GetCurrentTime();

	timestart  .GetAsSystemTime(start);
	timeend    .GetAsSystemTime(end);
	CurrentTime.GetAsSystemTime(current);
	
	SystemTimeToFileTime(&start,&fStart);
	SystemTimeToFileTime(&end,&fEnd);
	SystemTimeToFileTime(&current,&fcurrent);

	//----------------------------------------------------------

	lRet = CompareFileTime(&fcurrent,&fStart);//Vergleich Startzeit, Aktuelle Zeit

	if(lRet == -1)     //fcurrent < fStart
	{
	}
	else if(lRet == 0) //fcurrent == fStart
	{
	}
	else if(lRet == 1) //fcurrent > fStart
	{
	//	if(m_iSelectedItemIndex == -1)
		{
			m_ctrlStartDate.SetTime(&CurrentTime);
			m_ctrlStartTime.SetTime(&CurrentTime);
			CurrentTime.GetAsSystemTime(start);
		}
	//	else
		{
	//		return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	//----------------------------------------------------------


 	lRet = CompareFileTime(&fcurrent,&fEnd);//Vergleich Endzeit, Aktuelle Zeit
 
 	if(lRet == -1)     //fcurrent < fEnd
 	{
 	}
 	else if(lRet == 0) //fcurrent == fEnd
 	{
 	}
 	else if(lRet == 1) //fcurrent > fEnd
 	{
 	//	if(m_iSelectedItemIndex == -1)
 		{
 			m_ctrlStopDate.SetTime(&CurrentTime);
 			m_ctrlStopTime.SetTime(&CurrentTime);	
 			CurrentTime.GetAsSystemTime(end);
 		}
 	//	else
 		{
 	//		return FALSE;
 		}
 	}
 	else
 	{
 		return FALSE;
 	}

 
	//----------------------------------------------------------
		
	CurrentTime.GetAsSystemTime(current);//Vergleich Startzeit, Endzeit
	
	SystemTimeToFileTime(&start,&fStart);
	SystemTimeToFileTime(&end,&fEnd);

	lRet = CompareFileTime(&fStart,&fEnd);

	if(lRet == -1)     //fStart < fEnd
	{
	//Version 2.1: 
	//2.Begrenzung jeder Zeitspanne auf max. 10 Stunden

		if(iCtrl != 2) // Startzeit wurde geändert
		{
			if(((end.wHour-start.wHour)>=10)) //Ausserhalb des Zeitrahmens
			{
				start.wHour = (WORD)((int)start.wHour + ((int)end.wHour - (int)(start.wHour + 10)));

				if(((end.wHour-start.wHour)==10)) //Check der Minuten
				{
					if(start.wMinute < end.wMinute)
					{
						start.wMinute = end.wMinute;
					}
					if(start.wMinute == end.wMinute)
					{
						if(start.wSecond < end.wSecond) // Check der Sekunden
						{
							start.wSecond = end.wSecond;
						}
					}
				}
				m_ctrlStartDate.SetTime(&start);
				m_ctrlStartTime.SetTime(&start);
			}
		}
		else if(iCtrl != 1) // Endzeit wurde geändert
		{
			if(((end.wHour-start.wHour)>=10))  //Ausserhalb des Zeitrahmens
			{
				end.wHour = (WORD)((int)end.wHour - ((int)end.wHour - (int)(start.wHour + 10)));
				if(((end.wHour-start.wHour)==10)) //Check der Minuten
				{
					if(end.wMinute > start.wMinute)
					{
						end.wMinute = start.wMinute;
					}
					if(end.wMinute == start.wMinute)
					{
						if(start.wSecond < end.wSecond) // Check der Sekunden
						{
							end.wSecond = start.wSecond;
						}
					}
				}
				m_ctrlStopDate.SetTime(&end);
				m_ctrlStopTime.SetTime(&end);	
			} 
		}
	}

	else if(lRet == 1) //fStart > fEnd
	{
	  	start.wSecond++;
		m_ctrlStopDate.SetTime(&start);
		m_ctrlStopTime.SetTime(&start);	
	}

	if(lRet == 0) //fStart == fEnd
	{
	  	start.wSecond++;
		m_ctrlStopDate.SetTime(&start);
		m_ctrlStopTime.SetTime(&start);	
	}
 	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::SetActualTime()
{
	CTime t = CTime::GetCurrentTime();
	
	m_ctrlStartDate.SetTime(&t);
	m_ctrlStartTime.SetTime(&t);	

	m_ctrlStopDate.SetTime(&t);
	m_ctrlStopTime.SetTime(&t);	
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::EnableControls()
{
	BOOL bEnable = (   (m_iSelectedItemIndex != -1)
					&& (m_pAlarmOffSpans->GetSize() != 0)
					);
	m_ListSpans.EnableWindow(bEnable);
	m_ctrlStopTime.EnableWindow(bEnable);
	m_ctrlStartTime.EnableWindow(bEnable);
	m_ctrlStopDate.EnableWindow(FALSE);
	m_ctrlStartDate.EnableWindow(FALSE);

	m_Button_New.EnableWindow(TRUE);
	bEnable = (m_pAlarmOffSpans->GetSize() != 0);
	m_Button_Delete.EnableWindow(bEnable);
	m_Button_OK.EnableWindow(m_bChanged);
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_iSelectedItemIndex == -1)  //Kein Listenelement ausgewählt
	{
		CheckTimeSpan(3);
		return;
	}
	CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);
	ChangeTimeSpan(pTimespan,3);
	EnableControls();
 	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_iSelectedItemIndex == -1)  //Kein Listenelement ausgewählt
	{
		CheckTimeSpan(1);
		return;
	}
	CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);
	ChangeTimeSpan(pTimespan,1);
	EnableControls();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnDatetimechangeStopDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_iSelectedItemIndex == -1)  //Kein Listenelement ausgewählt
	{
		CheckTimeSpan(4);
		return;
	}
	CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);
	ChangeTimeSpan(pTimespan,4);
	EnableControls();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnDatetimechangeStopTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_iSelectedItemIndex == -1)  //Kein Listenelement ausgewählt
	{
		CheckTimeSpan(2);
		return;
	}
	CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);
	ChangeTimeSpan(pTimespan,2);
	EnableControls();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::ChangeTimeSpan(CSystemTimeSpan *pSystemTimeSpan,int iCtrl)
{
	//Änderung einer in der Liste markierten Zeitspanne
				//-------------------------------------   Startzeit

	CheckTimeSpan(iCtrl);
	CSystemTime SystemTime,
				SystemDate;

	m_ctrlStartTime.GetTime(&SystemTime);			
	m_ctrlStartDate.GetTime(&SystemDate);
	
	CSystemTime SystemStartDateTime(SystemDate.GetDay(),
									SystemDate.GetMonth(),
									SystemDate.GetYear(),
									SystemTime.GetHour(),
									SystemTime.GetMinute(),
									SystemTime.GetSecond());
						
				//------------------------------------   Stopzeit	
	
	m_ctrlStopTime.GetTime(&SystemTime);			
	m_ctrlStopDate.GetTime(&SystemDate);

	CSystemTime SystemStopDateTime (SystemDate.GetDay(),
									SystemDate.GetMonth(),
									SystemDate.GetYear(),
									SystemTime.GetHour(),
									SystemTime.GetMinute(),
									SystemTime.GetSecond());

	pSystemTimeSpan->SetStartTime(SystemStartDateTime);	
	pSystemTimeSpan->SetEndTimes(SystemStopDateTime);	
	
	m_pAlarmOffSpans->SetAt(m_iSelectedItemIndex,pSystemTimeSpan);

	SystemTime = pSystemTimeSpan->GetStartTime();
	m_ListSpans.SetItemText(m_iSelectedItemIndex,0, SystemTime.GetDateTime());

	SystemTime = pSystemTimeSpan->GetEndTime();
	m_ListSpans.SetItemText(m_iSelectedItemIndex,1, SystemTime.GetDateTime());

	m_bChanged = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CInputDeactivationDialog::OnClickListDeactivations(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("Item %i\n"), pNMListView->iItem);
	if (pNMListView->iItem != m_iSelectedItemIndex)
	{
		OnChangeListEntry(pNMListView->iItem);
	}
	*pResult = 0;
}

void CInputDeactivationDialog::OnDblclkListDeactivations(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("Item %i\n"), pNMListView->iItem);
	if (pNMListView->iItem != m_iSelectedItemIndex)
	{
		OnChangeListEntry(pNMListView->iItem);
	}
	*pResult = 0;
}

void CInputDeactivationDialog::OnRclickListDeactivations(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("Item %i\n"), pNMListView->iItem);
	if (pNMListView->iItem != m_iSelectedItemIndex)
	{
		OnChangeListEntry(pNMListView->iItem);
	}
	*pResult = 0;
}

void CInputDeactivationDialog::OnRdblclkListDeactivations(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("Item %i\n"), pNMListView->iItem);
	if (pNMListView->iItem != m_iSelectedItemIndex)
	{
		OnChangeListEntry(pNMListView->iItem);
	}
	*pResult = 0;
}

void CInputDeactivationDialog::OnChangeListEntry(int iNewSelection)
{
	if (   (iNewSelection != -1)
		&& (iNewSelection != m_iSelectedItemIndex)
		)
	{
		m_iSelectedItemIndex=iNewSelection;
		m_ListSpans.SetItemState(iNewSelection,
								 LVIS_SELECTED|LVIS_FOCUSED,
								 LVIS_SELECTED|LVIS_FOCUSED);

		CSystemTimeSpan* pTimespan = m_pAlarmOffSpans->GetAt(m_iSelectedItemIndex);

		CSystemTime StartTime = pTimespan->GetStartTime();
		CSystemTime StopTime  = pTimespan->GetEndTime();

		m_ctrlStartDate.SetTime(StartTime);
		m_ctrlStartTime.SetTime(StartTime);	

		m_ctrlStopDate.SetTime(StopTime);
		m_ctrlStopTime.SetTime(StopTime);
	}
	else
	{
		m_ListSpans.SetItemState(m_iSelectedItemIndex,
								 LVIS_SELECTED|LVIS_FOCUSED,
								 LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CInputDeactivationDialog::OnItemchangedListDeactivations(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("Item %i\n"), pNMListView->iItem);
	if (pNMListView->iItem != m_iSelectedItemIndex)
	{
		OnChangeListEntry(pNMListView->iItem);
	}
	*pResult = 0;
}
