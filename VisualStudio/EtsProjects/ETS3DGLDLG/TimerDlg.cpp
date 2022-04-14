// TimerDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "TimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTimerDlg 

CTimerDlg::CTimerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerDlg)
	//}}AFX_DATA_INIT
   m_Date.cDate        = GetCurrentTime();
   m_Date.cTime        = GetCurrentTime();
   m_Date.strTimerPath = _T("");
   m_Date.strMessage   = _T("");
   m_nActiveDate       = -1;
   m_bChanged          = false;
}


void CTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerDlg)
	DDX_Control(pDX, IDC_TIMER_LIST, m_cList);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TIMER_PATH, m_Date.strTimerPath);
	DDX_MonthCalCtrl(pDX, IDC_TIMER_CALENDAR, m_Date.cDate);
	DDX_DateTimeCtrl(pDX, IDC_TIMER_CLOCK, m_Date.cTime);
	DDX_Text(pDX, IDC_TIMER_MESSAGE, m_Date.strMessage);
}


BEGIN_MESSAGE_MAP(CTimerDlg, CDialog)
	//{{AFX_MSG_MAP(CTimerDlg)
	ON_NOTIFY(MCN_GETDAYSTATE, IDC_TIMER_CALENDAR, OnGetdaystateTimerCalendar)
	ON_BN_CLICKED(IDC_BTN_TIMER_PATH, OnBtnTimerPath)
	ON_BN_CLICKED(IDC_TIMER_BTN_SET, OnTimerBtnSet)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_TIMER_LIST, OnGetdispinfoTimerList)
	ON_EN_KILLFOCUS(IDC_TIMER_MESSAGE, OnKillfocusTimerMessage)
	ON_NOTIFY(NM_CLICK, IDC_TIMER_LIST, OnClickTimerList)
	ON_BN_CLICKED(IDC_TIMER_BTN_NEW, OnTimerBtnNew)
	ON_BN_CLICKED(IDC_TIMER_BTN_DELETE, OnTimerBtnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTimerDlg 

BOOL CTimerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   CString string;
   m_cList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   string.LoadString(IDS_T_NO);
   m_cList.InsertColumn(0, string, LVCFMT_LEFT, 30);
   string.LoadString(IDS_T_DATE);
   m_cList.InsertColumn(1, string, LVCFMT_LEFT, 50);
   string.LoadString(IDS_T_TIME);
   m_cList.InsertColumn(2, string, LVCFMT_LEFT, 50);
   string.LoadString(IDS_T_MSG);
   m_cList.InsertColumn(3, string, LVCFMT_LEFT, 150);

   m_cList.SetItemCount(m_pListDates->GetCount()+1);

   SelectActiveDate();
   UpdateActiveDate();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTimerDlg::OnOK() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   POSITION pos = m_pListDates->GetHeadPosition();
   CString strSection;
   int i = 1;
   while (pos)
   {
      StructOfDates *pDate = (StructOfDates*) m_pListDates->GetNext(pos);
      if (pDate)
      {
         strSection.Format(DATE_SECTION, i++);
         pApp->WriteProfileInt(   strSection, DATE_TIME, pDate->cTime.GetTime());
         pApp->WriteProfileInt(   strSection, DATE_DATE, pDate->cDate.GetTime());
         pApp->WriteProfileInt(   strSection, DATE_FLAG, pDate->dwFlags);
         pApp->WriteProfileString(strSection, DATE_MSG , pDate->strMessage);
         pApp->WriteProfileString(strSection, DATE_PATH, pDate->strTimerPath);
      }
   }
	CDialog::OnOK();
}

void CTimerDlg::OnGetdaystateTimerCalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	*pResult = 0;
}

void CTimerDlg::OnBtnTimerPath() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->GetFolderPath(m_Date.strTimerPath, NULL, FOLDERPATH_RETURN_FILES))
   {
      CDataExchange dx(this, false);
   	DDX_Text(&dx, IDC_CPF_DESTINATION_PATH, m_Date.strTimerPath);
   }
}

void CTimerDlg::OnTimerBtnSet() 
{
	SaveDate();
}

void CTimerDlg::OnGetdispinfoTimerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   POSITION       pos;
   StructOfDates *pDate = NULL;

   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem != 0)
      {
         pos = m_pListDates->FindIndex(pDispInfo->item.iItem);
         if (pos) pDate = (StructOfDates*)m_pListDates->GetAt(pos);
         if (pDate == NULL) pDate = &m_Date;
      }
      switch (pDispInfo->item.iSubItem)
      {
         case 0: wsprintf(pDispInfo->item.pszText, "%d", pDispInfo->item.iItem+1); break;
         case 1:
         {
            CString str = pDate->cDate.Format(IDS_T_DATE_FMT);
            strncpy(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
         } break;
         case 2:
         {
            CString str = pDate->cTime.Format(IDS_T_TIME_FMT);
            strncpy(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
         } break;
         case 3:strncpy(pDispInfo->item.pszText, pDate->strMessage, pDispInfo->item.cchTextMax);break;
      }
   }
	*pResult = 0;
}

void CTimerDlg::OnKillfocusTimerMessage() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_TIMER_MESSAGE, m_Date.strMessage);
}

void CTimerDlg::OnClickTimerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LVHITTESTINFO lvHTI;

   GetCursorPos(&lvHTI.pt);
   m_cList.ScreenToClient(&lvHTI.pt);

   m_cList.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if ((lvHTI.iItem != -1) && (m_nActiveDate != lvHTI.iItem))
   {
      SaveDate();
      m_nActiveDate = lvHTI.iItem;
      UpdateActiveDate();
   }
	
	*pResult = 0;
}

void CTimerDlg::CopyDate(StructOfDates &sGet, StructOfDates &sSet)
{
   sGet.cDate        = sSet.cDate;
   sGet.cTime        = sSet.cTime;
   sGet.dwFlags      = sSet.dwFlags;
   sGet.strMessage   = sSet.strMessage;
   sGet.strTimerPath = sSet.strTimerPath;
}

void CTimerDlg::SaveDate()
{
   if (m_bChanged)
   {
      StructOfDates *pDate = NULL;
      POSITION       pos   = NULL;

      pos = m_pListDates->FindIndex(m_nActiveDate);      // Alte Daten Speichern
      if (pos) pDate = (StructOfDates*)m_pListDates->GetAt(pos);
      if (pDate && (AfxMessageBox(IDS_T_SAVE_PARAMS, MB_YESNO|MB_ICONQUESTION)==IDOK) && UpdateData(true))
      {
         CopyDate(*pDate, m_Date);
      }
      m_bChanged = false;
   }
}

void CTimerDlg::OnTimerBtnNew() 
{
   CDataExchange dx(this, false);
   m_Date.strTimerPath.Empty();
   m_Date.strMessage.Empty();
	DDX_Text(&dx, IDC_TIMER_PATH, m_Date.strTimerPath);
	DDX_Text(&dx, IDC_TIMER_MESSAGE, m_Date.strMessage);
   dx.m_bSaveAndValidate = true;
   DDX_MonthCalCtrl(&dx, IDC_TIMER_CALENDAR, m_Date.cDate);
	DDX_DateTimeCtrl(&dx, IDC_TIMER_CLOCK, m_Date.cTime);

   StructOfDates *pNew = new StructOfDates;
   CopyDate(*pNew, m_Date);
   m_pListDates->AddHead(pNew);
   m_cList.SetItemCount(m_pListDates->GetCount());
   m_nActiveDate = m_pListDates->GetCount()-1;
   SelectActiveDate();
}

void CTimerDlg::OnTimerBtnDelete() 
{
   StructOfDates *pDate = NULL;
   POSITION       pos   = NULL;

   pos = m_pListDates->FindIndex(m_nActiveDate);      // Alte Daten Speichern
   if (pos)
   {
      StructOfDates *pDel = (StructOfDates*)m_pListDates->GetAt(pos);
      if (pDel)
      {
	      m_pListDates->RemoveAt(pos);
         delete pDel;
         m_cList.SetItemCount(m_pListDates->GetCount());
         m_nActiveDate = m_pListDates->GetCount()-1;
         SelectActiveDate();
         UpdateActiveDate();
      }
   }
}

void CTimerDlg::SelectActiveDate()
{
   if (m_nActiveDate != -1)
   {
      m_cList.SetHotItem(m_nActiveDate);
      m_cList.SetItemState(m_nActiveDate, 0, 0);
      m_cList.SetSelectionMark(m_nActiveDate);
   }
}

void CTimerDlg::UpdateActiveDate()
{
   StructOfDates *pDate = NULL;
   POSITION       pos   = NULL;
   pos = m_pListDates->FindIndex(m_nActiveDate);      // Alte Daten Speichern
   if (pos)
   {
      StructOfDates *pDate = (StructOfDates*)m_pListDates->GetAt(pos);
      if (pDate)
      {
         CopyDate(m_Date, *pDate);
         UpdateData(false);
      }
   }
}
