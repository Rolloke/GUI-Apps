// PropPageVariable.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "PropPageVariable.h"
#include "SVPropertySheet.h"

#include "SVPage.h"
#include "SVTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NOT_SAVED 1
/////////////////////////////////////////////////////////////////////////////
// CPropPageVariable property page
IMPLEMENT_DYNCREATE(CPropPageVariable, CPropertyPage)
/////////////////////////////////////////////////////////////////////////////

CPropPageVariable::CPropPageVariable() : CPropertyPage(CPropPageVariable::IDD)
{
	//{{AFX_DATA_INIT(CPropPageVariable)
	m_strWizardText = _T("");
	m_strWizardHeading = _T("");
	//}}AFX_DATA_INIT

	m_bCreated            = false;
	m_bDeleted            = false;
	m_bVisited            = false;
	m_bDummy			  = false;

	// Modified in Init(..)
	m_nWizardPageID       = 0;
	m_nWizardTextID       = 0;

	m_lParam              = NULL;
	m_pfnInit             = NULL;
	m_pfnSetActive        = NULL;
	m_pfnNext             = NULL;

	// Flags
	m_bCreateNew          = false;
	m_bAllowNew           = false;

	m_bDeleteExisting     = false;
	m_bAllowDelete        = false;

	m_bInitializeOnNext   = false;
	m_bInitializeOnFinish = false;

	m_bDontVisitTwice	  = false;
	m_bPageIDisSecID      = false;

	m_bDisablePage        = true;
	m_bDisableTree        = true;
}
/////////////////////////////////////////////////////////////////////////////
CPropPageVariable::~CPropPageVariable()
{
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageVariable)
	DDX_Text(pDX, IDC_STAT_PPVAR_TEXT, m_strWizardText);
	DDX_Text(pDX, IDC_STATIC_PPVAR_HEADING, m_strWizardHeading);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPropPageVariable, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageVariable)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_GET_WPAGE_PARAM, OnGetPageParam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CSVPropertySheet* CPropPageVariable::GetSVPropertySheet()
{
	CWnd *pWnd = GetParent();
	ASSERT_KINDOF(CSVPropertySheet, pWnd);
	return (CSVPropertySheet*)pWnd;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPageVariable message handlers
BOOL CPropPageVariable::OnInitDialog() 
{
	if (m_nWizardTextID)
	{
		m_strWizardText.LoadString(m_nWizardTextID);
	}
	CPropertyPage::OnInitDialog();
	CFont *pFont = GetFont();
	if (CSkinDialog::UseGlobalFonts())
	{
		pFont = CSkinDialog::GetDialogItemGlobalFont();
	}
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfWeight  = FW_BOLD;
		lf.lfHeight -= 2;
		if (m_HeadingFont.m_hObject != NULL)
		{
			m_HeadingFont.DeleteObject();
		}
		m_HeadingFont.CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_PPVAR_HEADING)->SetFont(&m_HeadingFont);
	}

	GetSVPropertySheet()->SendMessage(WM_USER, WM_SET_WIZARD_SIZE);
	if (m_pfnInit)
	{
		if (!m_pfnInit(this, m_lParam))
		{
			GetSVPropertySheet()->PressLastButton();
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageVariable::OnSetActive() 
{
	DWORD dwFlags = PSWIZB_NEXT|PSWIZB_BACK;
	CSVPropertySheet *pSheet = GetSVPropertySheet();
	
	if (pSheet->IsDeleting())									// On destruction
	{
		return CPropertyPage::OnSetActive();					// return here
	}

	int nActivePage = pSheet->GetActiveIndex();                 // Aktive Eigenschaftsseite ermitteln
	if ((m_bVisited && m_bDontVisitTwice) ||                    // Schon geändert bzw. besucht ? 
		(nActivePage == 0))                                     // erste Eigenschaftsseite ?
	{
		dwFlags &= ~PSWIZB_BACK;
	}
	if (nActivePage == pSheet->GetPageCount()-1)                // letzte Eigenschaftsseite ?
	{
		dwFlags |= PSWIZB_FINISH;
	}
	pSheet->SetWizardButtons(dwFlags);                          // Buttons en/disablen
    LRESULT lSelected = 0;
																// Dialogseite auswählen
	if (m_sPageTextID.IsEmpty())
	{
		lSelected = pSheet->GetSVView()->SendMessage(WM_SELECT_PAGE, m_nWizardPageID, (LPARAM)((m_bPageIDisSecID) ? SELECT_FROM_SEC_ID:SELECT_FROM_STRING_ID));
	}
	else
	{
		lSelected = pSheet->GetSVView()->SendMessage(WM_SELECT_PAGE, (WPARAM)LPCTSTR(m_sPageTextID), (LPARAM)SELECT_FROM_STRING);
	}

	CSVTree *pTree  =  pSheet->GetSVView()->GetSVTree();
	CSVPage *pPage  =  pSheet->GetSVView()->GetSVPage();        // neu gesetzte Dialogseite !

	if (   lSelected == 0
		|| pTree == NULL
		|| pPage == NULL)
	{
		if (m_pfnInit == NULL)
		{
			pSheet->PressLastButton();
		}
		return 1;
	}
	if (m_strWizardHeading.IsEmpty())
	{
		HTREEITEM hItem = pTree->GetSelectedItem();
		if (hItem)
		{
			m_strWizardHeading = pTree->GetItemText(hItem);
			SetDlgItemText(IDC_STATIC_PPVAR_HEADING, m_strWizardHeading);
		}
	}
	if (m_bCreateNew &&                                         // neuen Datensatz erstellen ?
		!m_bCreated &&                                          // wenn er noch nicht erstellt wurde
		pPage->CanNew())                                        // und einer erstellt werden kann
	{
		pPage->OnNew();
		m_bCreated = true;                                       // erstellt auf true setzen
	}

	if (m_pfnSetActive)
	{
		if (!m_pfnSetActive(this, m_lParam))
		{
			pSheet->PressLastButton();
		}
	}

	pPage->EnableWindow(!m_bDisablePage);
	//   pTree->EnableWindow(!m_bDisableTree);
	pTree->DisableMouse(m_bDisableTree);

	pPage->SetFocus();
	return CPropertyPage::OnSetActive();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageVariable::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::OnCancel() 
{
	CPropertyPage::OnCancel();                                  // Abbrechen
	CSVPage *pPage = GetSVPropertySheet()->GetSVView()->GetSVPage();
	if (pPage) pPage->OnCancel();                               // Undo aufrufen
	GetParent()->PostMessage(WM_CLOSE);                         // WM_CLOSE senden, da nicht modal
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageVariable::OnWizardFinish() 
{
//	BOOL bReturn = CPropertyPage::OnWizardFinish();
	BOOL bReturn = OnWizardNext();                              // Änderungen prüfen und speichern
	GetSVPropertySheet()->SetLastButton(ID_WIZFINISH);
	if (bReturn == NOT_SAVED) return FALSE;                     // wenn nicht gespeichert: Finish abbrechen

	CSVPage *pPage = GetSVPropertySheet()->GetSVView()->GetSVPage();
	if (pPage && m_bInitializeOnFinish)                         // Server initialisieren ?
	{
		pPage->Initialize();
	}

   GetParent()->PostMessage(WM_CLOSE);                         // WM_CLOSE senden, da nicht modal
   return TRUE;                                                // finished
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPropPageVariable::OnWizardBack() 
{
	GetSVPropertySheet()->SetLastButton(ID_WIZBACK);
	CSVPage *pPage = GetSVPropertySheet()->GetSVView()->GetSVPage();
	if (pPage && pPage->IsModified())
	{
		pPage->OnCancel();                                       // Änderungen verwerfen
		m_bCreated = false;                                      // erstellt auf false setzen
		m_bDeleted = false;                                      // gelöscht auf false setzen
	}
	return CPropertyPage::OnWizardBack();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPropPageVariable::OnWizardNext() 
{
	GetSVPropertySheet()->SetLastButton(ID_WIZNEXT);
	CSVPage *pPage = GetSVPropertySheet()->GetSVView()->GetSVPage();
	bool bOK =  false;

	if (m_pfnNext) m_pfnNext(this, m_lParam);

	if (m_bDeleteExisting &&                                    // bestehenden Datensatz löschen ?
		!m_bDeleted &&                                          // wenn er noch nicht gelöscht wurde
		pPage->CanDelete())                                     // und gelöscht werden kann
	{
		pPage->OnDelete();
		m_bDeleted = true;                                      // gelöscht auf true setzen
	}

	if (pPage->IsModified())                                    // Dialogseite geändert
	{
		pPage->OnOK();                                          // Änderungen prüfen und speichern
		bOK = true;                                             // OnOk aufgerufen
	}
	if (pPage->IsModified())                                    // Nicht gespeichert ?
	{
		pPage->SetFocus();                                      // Focus auf Dialogseite setzen
		return NOT_SAVED;
	}
	else                                                        // gespeichert
	{
		if (bOK)                                                // OK ?
		{
			if (m_bInitializeOnNext)
			{
				pPage->Initialize();                            // Initialisieren
			}
			m_bVisited = true;
		}
		return CPropertyPage::OnWizardNext();                   // nächste Eigenschaftseite aufrufen
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPropPageVariable::OnGetPageParam(WPARAM wParam, LPARAM lParam)
{
	bool *pbReturn = (bool*) lParam;
	CSVPage *pPage = GetSVPropertySheet()->GetSVView()->GetSVPage();
	if (pPage)
	{
		switch (wParam)
		{
			case ALLOW_NEW:    
				*pbReturn = m_bAllowNew && pPage->CanNew();
				break;
			case ALLOW_DELETE:
				*pbReturn = m_bAllowDelete && pPage->CanDelete();
				break;
			case ALLOW_CANCEL:
				*pbReturn = (m_bAllowNew || m_bAllowDelete) && pPage->IsModified();
				break;
			case ALLOW_SAVE:
				*pbReturn = (m_bAllowNew || m_bAllowDelete) && pPage->IsModified();
				break;
			case ALLOW_INIT:
				*pbReturn = (m_bInitializeOnFinish || m_bInitializeOnNext);
				break;
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::Init(int nPageID, int nTextID, 
                             DWORD dwFlags        /*=0*/,
                             LPARAM lParam        /*=NULL*/,
                             VARIABLE_PP_FC pfnI  /*=NULL*/,
                             VARIABLE_PP_FC pfnSA /*=NULL*/,
                             VARIABLE_PP_FC pfnN  /*=NULL*/)
{
	m_nWizardPageID = nPageID;
	m_nWizardTextID = nTextID;

	m_lParam       = lParam;
	m_pfnInit      = pfnI;
	m_pfnSetActive = pfnSA;
	m_pfnNext      = pfnN;

	if (dwFlags & CREATE_NEW_DATA)      m_bCreateNew          = true;
	if (dwFlags & ALLOW_NEW_DATA)       m_bAllowNew           = true;

	if (dwFlags & DELETE_EXISTING_DATA) m_bDeleteExisting     = true;
	if (dwFlags & ALLOW_DELETE_DATA)    m_bAllowDelete        = true;

	if (dwFlags & INIT_ON_NEXT)         m_bInitializeOnNext   = true;
	if (dwFlags & INIT_ON_FINISH)       m_bInitializeOnFinish = true;

	if (dwFlags & PAGEID_IS_SEC_ID)     m_bPageIDisSecID      = true;
	if (dwFlags & DONT_VISIT_TWICE)     m_bDontVisitTwice	  = true;

	if (dwFlags & ENABLE_PAGE)          m_bDisablePage        = false;
	if (dwFlags & ENABLE_TREE)          m_bDisableTree		  = false;
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::SetPageTextID(LPCTSTR sPageTextID)
{
	m_sPageTextID = sPageTextID;
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::OnDestroy() 
{
	CWnd *pWnd;
	for (int i=IDC_DEF_ITEM0; i<=IDC_DEF_ITEM10; i++)
	{
		pWnd = GetDlgItem(i);
		if (pWnd) delete pWnd;
		else      break;
	}
	CPropertyPage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
int CPropPageVariable::GetPageID()
{
	return m_nWizardPageID;
}
/////////////////////////////////////////////////////////////////////////////
void CPropPageVariable::SetPageID(int nWizardPageID)
{
	m_nWizardPageID = nWizardPageID;
}
/////////////////////////////////////////////////////////////////////////////
int	CPropPageVariable::GetTextID()
{
	return m_nWizardTextID;
}
