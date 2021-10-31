// IRcodeDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "IRcodeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIRcodeDlg 


CIRcodeDlg::CIRcodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIRcodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIRcodeDlg)
	m_strDevice = _T("");
	//}}AFX_DATA_INIT

   m_hIcon           = (HICON)INVALID_HANDLE_VALUE;
}


void CIRcodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIRcodeDlg)
	DDX_Control(pDX, IDC_IR_CODE_LIST, m_cCodeList);
	DDX_Text(pDX, IDC_IR_EDT_DEVICE, m_strDevice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIRcodeDlg, CDialog)
	//{{AFX_MSG_MAP(CIRcodeDlg)
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_IR_CODE_LIST, OnBeginlabeleditIrCodeList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_IR_CODE_LIST, OnEndlabeleditIrCodeList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_IR_CODE_LIST, OnGetdispinfoIrCodeList)
	ON_NOTIFY(NM_CLICK, IDC_IR_CODE_LIST, OnClickIrCodeList)
	ON_BN_CLICKED(IDC_IR_BTN_LOAD, OnIrBtnLoad)
	ON_BN_CLICKED(IDC_IR_BTN_SAVE, OnIrBtnSave)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_IR_CODE_LIST, OnCustomDraw)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDHELP, CDialog::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CIRcodeDlg 

BOOL CIRcodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   int nOrder[2] = { 1, 0};
   int nCount = CODE_PARAMETER+CODE_COMMANDS/2+CODE_TRACK/2;

   CString string;
   m_cCodeList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   string.LoadString(IDS_IR_CODES);
   m_cCodeList.InsertColumn(0, string,  LVCFMT_RIGHT, 100);

   string.LoadString(IDS_IR_CODE_DESCR);
   m_cCodeList.InsertColumn(1, string,  LVCFMT_LEFT, 170);

   m_cCodeList.SendMessage(LVM_SETCOLUMNORDERARRAY, 2, (LPARAM)&nOrder);

   m_cCodeList.DeleteAllItems();
   m_cCodeList.SetItemCount(nCount);

   if (m_hIcon != INVALID_HANDLE_VALUE)
      SetIcon(m_hIcon, TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CIRcodeDlg::OnBeginlabeleditIrCodeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   switch (pDispInfo->item.iItem)
   {
      case 0:
      {
         *pResult = 1;
      } break;
      default :*pResult = 0; break;
   }
	
}

void CIRcodeDlg::OnEndlabeleditIrCodeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   *pResult = 0;
   if ((pDispInfo->item.mask & LVIF_TEXT) && (pDispInfo->item.pszText != NULL))
   {
      switch (pDispInfo->item.iItem)
      {
         case 3: case 4: case 20: case 21: case 22: case 23: case 24: case 25: // Hex
         {
            int n, c = sscanf(pDispInfo->item.pszText, "%02x", &n);
            if (c == 1)
            {
            	*pResult = 1;
               m_cCD_CodeParameter[pDispInfo->item.iItem] = n;
            }
         } break;
         case 26:
         {
            int i = 0, j=0;
            BYTE c = 0;
            if (pDispInfo->item.pszText[j++] == ':')
            {
               for (i=0; i<8; i++)
               {
                  if (pDispInfo->item.pszText[j++] == '1') c |= (1<<i);
                  if (pDispInfo->item.pszText[j++] != ':') break;
               }
            }
            if (i == 8)
            {
               m_cCD_CodeParameter[pDispInfo->item.iItem] = c;
            	*pResult = 1;
            }
         } break;
         default:
         if ((pDispInfo->item.iItem >= CODE_PARAMETER) && (pDispInfo->item.iItem < (CODE_PARAMETER+5)))
         {
            int i = (pDispInfo->item.iItem-CODE_PARAMETER)*2;
            int n1, n2, c = sscanf(pDispInfo->item.pszText, "%02x%02x", &n1, &n2);
            if (c == 2)
            {
            	*pResult = 1;
               m_cCD_CodeCommands[i  ] = n1;
               m_cCD_CodeCommands[i+1] = n2;
            }
         } 
         else if ((pDispInfo->item.iItem >= CODE_PARAMETER+5) && (pDispInfo->item.iItem < (CODE_PARAMETER+14)))
         {
            int i = (pDispInfo->item.iItem-(CODE_PARAMETER+5))*2;
            int n1, n2, c = sscanf(pDispInfo->item.pszText, "%02x%02x", &n1, &n2);
            if (c == 2)
            {
            	*pResult = 1;
               m_cCD_CodeTrack[i  ] = n1;
               m_cCD_CodeTrack[i+1] = n2;
            }
         } 
         else if (pDispInfo->item.iItem < CODE_PARAMETER)      // int
         {
            int i, nlen = strlen(pDispInfo->item.pszText);
            for (i=0; i<nlen; i++)
               if (!isdigit(pDispInfo->item.pszText[i])) break;
            if (i==nlen)
            {
               m_cCD_CodeParameter[pDispInfo->item.iItem] = atoi(pDispInfo->item.pszText);
            	*pResult = 1;
            }
         } break;
      }

   }
}

void CIRcodeDlg::OnGetdispinfoIrCodeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 0:
         {
            switch (pDispInfo->item.iItem)
            {
               case 0:
               if ((m_cCD_CodeParameter[0]>=0) && (m_cCD_CodeParameter[0]<=2))
               {
                  ::LoadString(AfxGetInstanceHandle(), m_cCD_CodeParameter[0]+IDS_IR_TYPE_SPACE, pDispInfo->item.pszText, pDispInfo->item.cchTextMax);
               } break;
               case 3: case 4: case 20: case 21: case 22: case 23: case 24: case 25: // Hex
               {
                  ::wsprintf(pDispInfo->item.pszText, "%02x", m_cCD_CodeParameter[pDispInfo->item.iItem]); break;
               } break;
               case 26:
               {
                  int i, j=0;
                  pDispInfo->item.pszText[j++] = ':';
                  for (i=0; i<8; i++)
                  {
                     pDispInfo->item.pszText[j++] = (m_cCD_CodeParameter[pDispInfo->item.iItem] & (1<<i)) ? '1' : '0';
                     pDispInfo->item.pszText[j++] = ':';
                  }
                  pDispInfo->item.pszText[j] = 0;
               } break;
               default:
               if ((pDispInfo->item.iItem >= CODE_PARAMETER) && (pDispInfo->item.iItem < (CODE_PARAMETER+5)))
               {
                  int i = (pDispInfo->item.iItem-CODE_PARAMETER)*2;
                  ::wsprintf(pDispInfo->item.pszText, "%02x%02x", m_cCD_CodeCommands[i], m_cCD_CodeCommands[i+1]);
               } 
               else if ((pDispInfo->item.iItem >= CODE_PARAMETER+5) && (pDispInfo->item.iItem < (CODE_PARAMETER+14)))
               {
                  int i = (pDispInfo->item.iItem-(CODE_PARAMETER+5))*2;
                  ::wsprintf(pDispInfo->item.pszText, "%02x%02x", m_cCD_CodeTrack[i], m_cCD_CodeTrack[i+1]);
               } 
               else if (pDispInfo->item.iItem < CODE_PARAMETER)                                      // int
               {
                  ::wsprintf(pDispInfo->item.pszText, "%d", m_cCD_CodeParameter[pDispInfo->item.iItem]); break;
               } break;
            }
         }break;
         case 1:
            ::LoadString(AfxGetInstanceHandle(), pDispInfo->item.iItem+IDS_IR_CODE1, pDispInfo->item.pszText, pDispInfo->item.cchTextMax); break;
      }
   }
	
	*pResult = 0;
}

void CIRcodeDlg::OnClickIrCodeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LVHITTESTINFO lvHTI;

   GetCursorPos(&lvHTI.pt);
   m_cCodeList.ScreenToClient(&lvHTI.pt);
   m_cCodeList.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);

   switch (lvHTI.iItem)
   {
      case 0:
      {
         m_cCD_CodeParameter[0]++;
         if (m_cCD_CodeParameter[0] > 2) m_cCD_CodeParameter[0] = 0;
         m_cCodeList.Update(lvHTI.iItem);
      } break;
      default: break;
   }
	
	*pResult = 0;
}

void CIRcodeDlg::OnIrBtnLoad() 
{
   if (((CAC_COMApp*)AfxGetApp())->DoFileDialog(false, false, IDR_FILE_TYPEIRC, m_strFileName))
   {
      ReadData(m_strFileName);
      if (m_hWnd)
      {
         UpdateData(false);
         m_cCodeList.RedrawItems(0, m_cCodeList.GetItemCount()-1);
      }
   }
}

void CIRcodeDlg::OnIrBtnSave() 
{
   if (UpdateData(true))
   {
      if (m_strFileName.IsEmpty()) m_strFileName.LoadString(AFX_IDS_UNTITLED);
      if (((CAC_COMApp*)AfxGetApp())->DoFileDialog(true, false, IDR_FILE_TYPEIRC, m_strFileName))
      {
         WriteData(m_strFileName);
      }
   }
}

void CIRcodeDlg::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NMLVCUSTOMDRAW	*pNmCustDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	*pResult = 0;
   switch(pNmCustDraw->nmcd.dwDrawStage) 
   {
      case CDDS_PREPAINT: 
         *pResult = CDRF_NOTIFYITEMDRAW;
         break;
      case CDDS_ITEMPREPAINT:
         if ((pNmCustDraw->nmcd.dwItemSpec >= CODE_PARAMETER) && (pNmCustDraw->nmcd.dwItemSpec < (CODE_PARAMETER+5)))
         { 
            pNmCustDraw->clrText   = GetSysColor(COLOR_INFOTEXT);
            pNmCustDraw->clrTextBk = GetSysColor(COLOR_INFOBK);
            *pResult = CDRF_NEWFONT;
         }break;
   }
}

bool CIRcodeDlg::ReadData(CString &strFileName)
{
	TRY
	{
      CFile file(strFileName, CFile::modeRead);
      CArchive ar(&file, CArchive::load);
      DWORD dwIdent[2] = {ACIR_TYPE, 200};
      ar.Read(dwIdent, sizeof(DWORD)*2);
      if ((dwIdent[0] == ACIR_TYPE) && (dwIdent[1] == 200))
      {
         ar >> m_strDevice;
         ar.Read(m_cCD_CodeParameter, sizeof(BYTE)*CODE_PARAMETER);
         ar.Read(m_cCD_CodeCommands , sizeof(BYTE)*CODE_COMMANDS);
         ar.Read(m_cCD_CodeTrack    , sizeof(BYTE)*CODE_TRACK);
      }
      return true;
   }
	CATCH_ALL(e)
	{
      MSG msg = {AfxGetMainWnd()->m_hWnd, 0, (WPARAM)LPCTSTR(m_strFileName), 0, 0, {0, 0}};
		AfxGetThread()->ProcessWndProcException(e, &msg);
	}
	END_CATCH_ALL
   return false;
}

bool CIRcodeDlg::WriteData(CString &strFileName)
{
	TRY
	{
      CFile file(strFileName, CFile::modeCreate|CFile::modeWrite);
      CArchive ar(&file, CArchive::store);
      DWORD dwIdent[2] = {ACIR_TYPE, 200};
      ar.Write(dwIdent, sizeof(DWORD)*2);
      ar << m_strDevice;
      ar.Write(m_cCD_CodeParameter, sizeof(BYTE)*CODE_PARAMETER);
      ar.Write(m_cCD_CodeCommands , sizeof(BYTE)*CODE_COMMANDS);
      ar.Write(m_cCD_CodeTrack    , sizeof(BYTE)*CODE_TRACK);
      return true;
   }
	CATCH_ALL(e)
	{
      MSG msg = {AfxGetMainWnd()->m_hWnd, 0, (WPARAM)LPCTSTR(m_strFileName), 0, 0, {0, 0}};
		AfxGetThread()->ProcessWndProcException(e, &msg);
	}
	END_CATCH_ALL
   return false;
}

LRESULT CIRcodeDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->dwContextId == 0)
   {
      CDialog::OnHelp();
      return true;
   }

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      CDialog::WinHelp(HID_BASE_CONTROL + pHI->iCtrlId, HELP_CONTEXT);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }

   return true;
}
