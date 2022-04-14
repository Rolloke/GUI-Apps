// EditTimeFormatDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LogView.h"
#include "EditTimeFormatDlg.h"


// EditTimeFormatDlg-Dialogfeld

IMPLEMENT_DYNAMIC(EditTimeFormatDlg, CDialog)

EditTimeFormatDlg::EditTimeFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditTimeFormatDlg::IDD, pParent)
{

}

EditTimeFormatDlg::~EditTimeFormatDlg()
{
}

void EditTimeFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TIME_FORMAT_LIST, m_List);
}


BEGIN_MESSAGE_MAP(EditTimeFormatDlg, CDialog)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_TIME_FORMAT_LIST, &EditTimeFormatDlg::OnLvnGetdispinfoTimeFormatList)
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TIME_FORMAT_LIST, &EditTimeFormatDlg::OnLvnBeginlabeleditTimeFormatList)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TIME_FORMAT_LIST, &EditTimeFormatDlg::OnLvnEndlabeleditTimeFormatList)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()


// EditTimeFormatDlg-Meldungshandler

BOOL EditTimeFormatDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
	CRect rc;
	m_List.GetClientRect(&rc);
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_List.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());
    m_List.SetItemCount(theApp.mFormats.GetCount()+1);
    EnableToolTips(true);

    return TRUE;  // return TRUE unless you set the focus to a control
    // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

BOOL EditTimeFormatDlg::OnToolTipNotify(UINT /* id */, NMHDR *pNMHDR, LRESULT *pResult)
{
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   UINT nID = (UINT)pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = (UINT)::GetDlgCtrlID((HWND)pNMHDR->idFrom);
   }

   AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
   CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
   pToolTip->SendMessage(TTM_SETMAXTIPWIDTH , 0, 300);

   if (mToolTipText.LoadString(nID))
   {
       pTTT->lpszText = (LPTSTR)LPCTSTR(mToolTipText);
   }
   else if (nID != 0)
   {// will be zero on a separator

      pTTT->lpszText = MAKEINTRESOURCE(nID);
      pTTT->hinst = AfxGetResourceHandle();
      return(TRUE);
   }


   *pResult = 0;

   return TRUE;    // message was handled

}

void EditTimeFormatDlg::OnLvnGetdispinfoTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if(pDispInfo->item.mask & LVIF_TEXT)
	{
		if (pDispInfo->item.iSubItem == 0)
		{
            if (pDispInfo->item.iItem < theApp.mFormats.GetCount())
            {
			    const CString &str = theApp.mFormats.GetAt(pDispInfo->item.iItem);
			    _tcsncpy(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
            }
            else
            {
		        _tcsncpy(pDispInfo->item.pszText, _T("..."), pDispInfo->item.cchTextMax);
            }
		}
	}
    *pResult = 0;
}

void EditTimeFormatDlg::OnLvnBeginlabeleditTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    if (pDispInfo->item.iItem >= theApp.GetFixedFormats())
    {
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}

void EditTimeFormatDlg::OnLvnEndlabeleditTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    if (pDispInfo->item.cchTextMax)
    {
        CString str(pDispInfo->item.pszText);
        if (str.GetLength() && str != "...")
        {
            if (pDispInfo->item.iItem == theApp.mFormats.GetCount())
            {
                theApp.mFormats.Add(str);
                m_List.SetItemCount(theApp.mFormats.GetCount()+1);
            }
            else
            {
                theApp.mFormats[pDispInfo->item.iItem] = str;
            }
        }
        else
        {
            theApp.mFormats.RemoveAt(pDispInfo->item.iItem);
            m_List.SetItemCount(theApp.mFormats.GetCount()+1);
        }
    }
    *pResult = 0;
}
