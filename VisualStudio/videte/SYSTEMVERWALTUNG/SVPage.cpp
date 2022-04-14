// SVPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "SVPage.h"
#include "mainfrm.h"

#include "SVDoc.h"
#include "SVView.h"
#include ".\svpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSVPage property page
IMPLEMENT_DYNCREATE(CSVPage, CDialog)
/////////////////////////////////////////////////////////////////////////////
CSVPage::CSVPage(UINT nIDTemplate) : CWK_Dialog(nIDTemplate)
{
	m_pParent          = NULL;
	m_bModified        = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CSVPage::~CSVPage()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSVPage, CWK_Dialog)
	//{{AFX_MSG_MAP(CSVPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CWK_Profile& CSVPage::GetProfile()
{
	CSVDoc* pDoc = m_pParent->GetDocument();
	return pDoc->GetProfile();
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dongle& CSVPage::GetDongle()
{
	CSVDoc* pDoc = m_pParent->GetDocument();
	return *pDoc->GetDongle();
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// don't call base class implementation 
	// it will eat frame window accelerators

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	if (CWK_Dialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SetModified(BOOL bModified/*=TRUE*/, BOOL bServerInit/*=TRUE*/)
{
	m_bModified = bModified;
	if (m_bModified && bServerInit)
	{
		m_pParent->m_bServerInit = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::IsDataValid()
{
	// should be overridden by subclasses
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnItemChangedStatusList(CStatusList* /* pStatusList */, int /* nListItem */)
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnStatusChangedStatusList(int nDlgItemID, int nListItem)
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::Initialize()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnNew()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnDelete()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnCancel() 
{
	if (m_bModified) 
	{
		CancelChanges();
		SetModified(FALSE,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnOK()
{
	if (m_bModified) 
	{
		if (IsDataValid())
		{
			SaveChanges();
			SetModified(FALSE,FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SaveChanges()
{
	// should be overridden by subclasses
}
////////////////////////////////////////////////////////////////////////////
void CSVPage::CancelChanges()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::DisableAll()
{
	CWnd* pItem = GetWindow(GW_CHILD);

	while (pItem && IsChild(pItem) ) 
	{
		// Haben wir noch ein Child?
		pItem->EnableWindow(FALSE);
		pItem = pItem->GetNextWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::ActivateTooltips(BOOL bActivate)
{
	if (m_ToolTip.m_hWnd)
	{
		m_ToolTip.Activate(bActivate);
		SendMessageToDescendants(m_sEnableTooltipMessage,bActivate);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SaveInputsAndOutputsAfterCheckingActivations()
{
	CInputList*		pInputList	= m_pParent->GetDocument()->GetInputs();
	COutputList*	pOutputList	= m_pParent->GetDocument()->GetOutputs();
	CMediaList*		pMediaList	= m_pParent->GetDocument()->GetMedias();

	// first check whether there is any Activation (CInputToOutput)
	// with an output, that no longer exists
	CInputGroup* pInputGroup;
	CInput* pInput;
	CInputToOutput* pInputToOutput;
	int i, iSize, j, jSize, k, kSize;

	iSize = pInputList->GetSize();
	for (i=0 ; i<iSize ; i++)
	{
		pInputGroup = pInputList->GetGroupAt(i);
		jSize = pInputGroup->GetSize();
		for (j=0 ; j<jSize ; j++)
		{
			pInput = pInputGroup->GetInput(j);
			kSize = pInput->GetNumberOfInputToOutputs();
			for (k=kSize-1 ; k>=0 ; k--)
			{
				pInputToOutput = pInput->GetInputToOutput(k);
				CSecID id = pInputToOutput->GetOutputOrMediaID();
				// does this output still exists
				// if not, delete activation
				if (id!=SECID_NO_ID)
				{
					if (   pOutputList->GetOutputByID(id) == NULL
						&& pMediaList->GetMediaByID(id) == NULL)
					{
						pInput->DeleteInputToOutput(pInputToOutput);
					}
				}
			} // inputstooutputs
		} // inputs
	}//groups

	// save the inputs and outputs
	pInputList->Save(GetProfile(),FALSE);
	pOutputList->Save(GetProfile(),FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   NMHDR *pNf = (NMHDR*) lParam;
	if (pNf->code == NM_CUSTOMDRAW)
   {
      int i;
      NMTTCUSTOMDRAW *pNcd = (NMTTCUSTOMDRAW*)lParam;
      if (pNcd->nmcd.dwDrawStage == CDDS_PREPAINT)
      {
/*
         i = 0;
         TOOLINFO ti;
         ti.cbSize = sizeof(TOOLINFO);
         m_ToolTip.SendMessage(TTM_GETTOOLINFO, 0, (LPARAM)&ti);
         ti.cbSize = sizeof(TOOLINFO);
*/
      }
      else if (pNcd->nmcd.dwDrawStage & CDDS_ITEM)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_PREERASE)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_POSTPAINT)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_POSTERASE)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_ITEMPOSTERASE)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
      {
         i = 1;
      }
      else if (pNcd->nmcd.dwDrawStage == CDDS_ITEMPREERASE)
      {
         i = 1;
      }
      
//      pNcd->uDrawFlags |= DT_CENTER;
   }
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}
/////////////////////////////////////////////////////////////////////////////
int CSVPage::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{

   return CWnd::OnToolHitTest(point, pTI);
}
/////////////////////////////////////////////////////////////////////////////
int CSVPage::GetNoOfSubDlgs()
{
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dialog *CSVPage::GetSubDlg(int)
{
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SetSelections(int nSelection)
{
	CWK_Dialog *pDlg;
	int i, nDlgs = GetNoOfSubDlgs();
	for (i=0; i< nDlgs; i++)
	{
		pDlg = GetSubDlg(i);
		if (pDlg)
		{
			pDlg->SetSelection(nSelection);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CSVPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWK_Dialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnIdleSVPage()
{
	// may be overridden by subclasses
}
/*********************************************************************************************
 Class      : CSVPage
 Function   : CheckActivationsWithOutput
 Description: Checks the activations with this output and or deletes or requests for deletion
              in dependency to the parameter [nCheckOptions].

 Parameters:   
  outID       : (E): SecID of the Output  (CSecID)
  nIDMsg      : (E): Message ID for the Messagebox  (int)
  nCheckOption: (E): Options for checking the activations  (int)
		* SVP_CO_CHECK       : Checks and requests for deletion (MessageID required)
		* SVP_CO_CHECK_SILENT: Checks and deletes silent
		* SVP_CO_CHECK_ONLY  : Checks only

 Result type: (TRUE, FALSE)  (BOOL)
        * TRUE, if there was no activation left.
        * FALSE, if there are still activations.
		
 created: February, 11 2004
 <TITLE CheckActivationsWithOutput>
*********************************************************************************************/
BOOL CSVPage::CheckActivationsWithOutput(CSecID outID, int nIDMsg, int nCheckOption)
{
	// first check wether there is any Activation (CInputToOutput) with
	// output
	CInputGroup* pInputGroup;
	CInput* pInput;
	CInputToOutput* pInputToOutput;
	int i,j,k,c,d,e;
	BOOL bShowMB = TRUE;
	BOOL bSave   = FALSE;
	
	CInputList *pInputList = m_pParent->GetDocument()->GetInputs();

	c = pInputList->GetSize();
	for (i=0;i<c;i++)
	{
		pInputGroup = pInputList->GetGroupAt(i);
		d = pInputGroup->GetSize();
		for (j=0;j<d;j++)
		{
			pInput = pInputGroup->GetInput(j);
			e = pInput->GetNumberOfInputToOutputs();
			for (k=e-1;k>=0;k--)
			{
				pInputToOutput = pInput->GetInputToOutput(k);
				if (pInputToOutput->GetOutputOrMediaID() == outID)
				{
					// we still have one
					if (nCheckOption == SVP_CO_CHECK_ONLY)
					{
						return FALSE;
					}
					else if (bShowMB)
					{
						if (nCheckOption == SVP_CO_CHECK_SILENT)
						{
							bShowMB = FALSE;
							bSave	= TRUE;
						}
						else if (IDYES==AfxMessageBox(nIDMsg, MB_YESNO))
						{
							bShowMB = FALSE;
						}
						else
						{
							return FALSE;
						}
					}
					pInput->SetDeleteInputToOutputs(outID);
				}
			} // inputstooutputs
		} // inputs
	}//groups
	if (bSave)
	{
		pInputList->Save(GetProfile(),IsLocal());
	}

	return TRUE;
}
/*********************************************************************************************
 Class      : CSVPage
 Function   : CheckActivationsWithInputGroup
 Description: Checks the activations with all inputs in the group and or deletes or
              requests for deletion in dependency to the parameter [nCheckOptions].

 Parameters:   
  outID       : (E): SecID of the Output  (CSecID)
  nCheckOption: (E): Options for checking the activations  (int)
		* SVP_CO_CHECK       : Checks and requests for deletion 
		* SVP_CO_CHECK_SILENT: Checks and deletes silent
		* SVP_CO_CHECK_ONLY  : Checks only

 Result type: (TRUE, FALSE)  (BOOL)
        * TRUE, if there was no activation left.
        * FALSE, if there are still activations.
		
 created: February, 11 2004
 <TITLE CheckActivationsWithOutput>
*********************************************************************************************/
BOOL CSVPage::CheckActivationsWithInputGroup(CSecID idGroup, int nCheckOption)
{
	BOOL bSave   = FALSE;
	CInputList *pInputList = m_pParent->GetDocument()->GetInputs();
	CInputGroup*pInputGroup = pInputList->GetGroupByID(idGroup);

	if (pInputGroup)
	{
		CString sMsg, sFormat, sNames;
		sFormat.LoadString(IDS_DELETE_INPUT);
		CPtrList list;
		CInput*pInput;
		int i, nSize = pInputGroup->GetSize();
		for (i=0; i<nSize; i++)
		{
			pInput = pInputGroup->GetInput(i);
			if (pInput->GetNumberOfInputToOutputs())
			{
				if (nCheckOption == SVP_CO_CHECK_ONLY)
				{
					return FALSE;
				}
				else if (nCheckOption == SVP_CO_CHECK_SILENT)
				{
					pInput->SetDeleteInputToOutputs(pInput->GetID());
					bSave = TRUE;
				}
				else
				{
					sNames += _T(" * ") + pInput->GetName() + _T("\n");
					list.AddHead(pInput);
				}
			}
		}
		if (list.GetCount())
		{
			sMsg.Format(sFormat, sNames);
			if (IDYES==AfxMessageBox(sMsg, MB_YESNO))
			{
				POSITION pos = list.GetHeadPosition();
				while (pos)
				{
					pInput = (CInput*)list.GetNext(pos);
					pInput->SetDeleteInputToOutputs(pInput->GetID());
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	if (bSave)
	{
		pInputList->Save(GetProfile(),IsLocal());
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	return TRUE;
}
/*********************************************************************************************
Class      : CSVPage
Function   : DeleteProcessesOfExtensionCards
Description: Deletes all processes (e.g. video outputs) which are connected to the
			 extension card to delete.
Parameters:   
pProcessList: (E): Process list of the System Managment  (CProcessList*)
pOutputList : (E): Output list of the System Managment (COutputList*)
sGroupName  : (E): Name of the extension card (CString)

Result type: (Void)

created: July, 05 2005
<TITLE DeleteProcessesOfExtensionCards>
*********************************************************************************************/
void CSVPage::DeleteProcessesOfExtensionCards(CProcessList* pProcessList, 
											  COutputList* pOutputList,
											  CString sGroupName)
{
	if(pProcessList)
	{
		BOOL bSave = FALSE;
		int iCount = pProcessList->GetSize();
		CProcessList ProcessesToDelete; //make sure its an array
		CProcess* pProcess = NULL;
		for (int j=iCount-1 ; j>=0 ; j--)
		{
			pProcess = pProcessList->GetAt(j);
			if (pProcess && pProcess->IsVideoOut())
			{
				// Get data from process
				CString	sName;
				CProcess::VOutSubType subType;
				CSecID idGroupCard;
				DWORD dwOutput;
				CString sCameras;
				DWORD dwDwellTime;
				pProcess->GetVideoOut(sName, subType, idGroupCard, dwOutput, sCameras, dwDwellTime);
				
				if(pOutputList)
				{
					COutputGroup* pGroup = pOutputList->GetGroupByID(idGroupCard);
					if(pGroup)
					{
						CString sName = pGroup->GetName();
						if (0 == sName.Find(sGroupName))
						{
							pProcessList->DeleteProcess(pProcess);
							bSave = TRUE;
						}
					}
				}
			}
		} // for

		if(bSave)
		{
			pProcessList->Save(GetProfile());
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnVScroll(CScrollBar&wndScrollBar, UINT nSBCode, UINT nPos, int nSize, int&iScroll) 
{
	int nPageSize = GetPageSize(wndScrollBar);
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		iScroll++;
		if (iScroll>=(nSize - nPageSize)) 
		{
			iScroll = nSize - nPageSize;
		}
		break;
	case SB_LINEUP:
		iScroll--;
		if (iScroll < 0) 
		{
			iScroll = 0;
		}
		break;
	case SB_PAGEDOWN:
		iScroll += nPageSize;
		if (iScroll >= (nSize - nPageSize)) 
		{
			iScroll = nSize - nPageSize;
		}
		break;
	case SB_PAGEUP:
		iScroll -= nPageSize;
		if (iScroll < 0) 
		{
			iScroll = 0;
		}
		break;
	case SB_TOP:
		iScroll = 0;
		break;
	case SB_BOTTOM:
		iScroll = nSize - nPageSize;
		break;
	case SB_THUMBTRACK:
		iScroll = nPos;
		if (iScroll>=(nSize - nPageSize)) 
		{
			iScroll = nSize- nPageSize;
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SetPageSize(CScrollBar&wndScrollBar, int iSize)
{
	SCROLLINFO ScrollInfo;

	wndScrollBar.GetScrollInfo(&ScrollInfo);
	ScrollInfo.nPage = iSize;
	wndScrollBar.SetScrollInfo(&ScrollInfo, TRUE);
}
/////////////////////////////////////////////////////////////////////////////
int CSVPage::GetPageSize(CScrollBar&wndScrollBar)
{
	SCROLLINFO ScrollInfo = {0};

	wndScrollBar.GetScrollInfo(&ScrollInfo);

	return ScrollInfo.nPage;
}
