// MSDNCollection.cpp : implementation file
//

#include "stdafx.h"
#include "MSDNEdit.h"
#include "MSDNCollection.h"
#include "MSDNEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSDNCollection dialog


CMSDNCollection::CMSDNCollection(CWnd* pParent /*=NULL*/)
	: CDialog(CMSDNCollection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSDNCollection)
	m_nNum     = 0;
	//}}AFX_DATA_INIT
   m_pPtrList = NULL;
   m_pDocCol  = NULL;
   m_bChanged = false;
}


void CMSDNCollection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSDNCollection)
	DDX_Control(pDX, IDC_COL_SPIN, m_cSpin);
	//}}AFX_DATA_MAP
   if (m_pDocCol)
   {
      DDX_Text(pDX, IDC_COL_EDT_NUMBER, m_pDocCol->nNum);
	   DDX_Text(pDX, IDC_COL_EDT_PATH, m_pDocCol->strName);
   }
}


BEGIN_MESSAGE_MAP(CMSDNCollection, CDialog)
	//{{AFX_MSG_MAP(CMSDNCollection)
	ON_EN_CHANGE(IDC_COL_EDT_NUM, OnChangeColEdtNum)
	ON_EN_CHANGE(IDC_COL_EDT_NUMBER, OnChange)
	ON_BN_CLICKED(IDC_BTN_COL_DELETE, OnBtnColDelete)
	ON_BN_CLICKED(IDC_BTN_COL_NEW, OnBtnColNew)
	ON_EN_CHANGE(IDC_COL_EDT_PATH, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSDNCollection message handlers

BOOL CMSDNCollection::OnInitDialog() 
{
   int nCount = 0;
   if (m_pPtrList)
   {
      nCount = m_pPtrList->GetSize();
      if (nCount > 0)
      {
         m_nNum = 1;
         m_pDocCol = (MSDNDocCol*)m_pPtrList->GetAt(m_nNum-1);
      }
   }
	CDialog::OnInitDialog();
   m_cSpin.SetRange(1, nCount);
   CDataExchange dx(this, false);
 	DDX_Text(&dx, IDC_COL_EDT_NUM, m_nNum);

   SendDlgItemMessage(IDC_BTN_COL_NEW   , BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ICON_NEW)   , IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
   SendDlgItemMessage(IDC_BTN_COL_DELETE, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ICON_DELETE), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMSDNCollection::OnChangeColEdtNum() 
{
   if (m_nNum < 1) return;
   CDataExchange dx(this, true);
 	DDX_Text(&dx, IDC_COL_EDT_NUM, m_nNum);
   if (m_pPtrList)
   {
      SaveChanged();
      m_pDocCol = (MSDNDocCol*)m_pPtrList->GetAt(m_nNum-1);
      UpdateData(false);
   }
}

void CMSDNCollection::OnChange() 
{
   m_bChanged = true;	
}

void CMSDNCollection::OnBtnColDelete() 
{
   int nSize = m_pPtrList->GetSize();
   if ((nSize > 0) && (AfxMessageBox(IDS_DELETE_ITEM, MB_ICONQUESTION|MB_YESNO) == IDYES))
   {
      m_pPtrList->RemoveAt(m_nNum-1);
      delete m_pDocCol;
      GetParent()->SendMessage(WM_COMMAND, ID_FILE_CHANGE_DOC_INFO, (LPARAM)m_hWnd);
      m_pDocCol = NULL;
      nSize--;
      m_bChanged = false;
      if (nSize > 0)
      {
         m_cSpin.SetRange(1, nSize);
         m_nNum = 1;
         CDataExchange dx(this, false);
	      DDX_Text(&dx, IDC_COL_EDT_NUM, m_nNum);
         OnChangeColEdtNum();
      }
      else
      {
         m_nNum = 0;
      }
   }
}

void CMSDNCollection::OnBtnColNew() 
{
   SaveChanged();
   m_pDocCol = new MSDNDocCol;
   m_pPtrList->Add(m_pDocCol);
   int nSize = m_pPtrList->GetSize();
   m_cSpin.SetRange(1, nSize);
   m_nNum = nSize;
   UpdateData(false);
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_COL_EDT_NUM, m_nNum);
   GetParent()->SendMessage(WM_COMMAND, ID_FILE_CHANGE_DOC_INFO, (LPARAM)m_hWnd);
}

void CMSDNCollection::SaveChanged()
{
   if (m_bChanged)
   {
      if (AfxMessageBox(IDS_SAVE_CHANGED_ITEMS, MB_ICONQUESTION|MB_YESNO) == IDYES)
      {
         GetParent()->SendMessage(WM_COMMAND, ID_FILE_CHANGE_DOC_INFO, (LPARAM)m_hWnd);
         UpdateData();
      }
      m_bChanged = false;
   }
}

void CMSDNCollection::OnCancel() 
{
   SaveChanged();	
	CDialog::OnCancel();
}
