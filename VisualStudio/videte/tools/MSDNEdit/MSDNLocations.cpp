// MSDNLocations.cpp : implementation file
//

#include "stdafx.h"
#include "MSDNEdit.h"
#include "MSDNLocations.h"
#include "MSDNEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSDNLocations dialog


CMSDNLocations::CMSDNLocations(CWnd* pParent /*=NULL*/)
	: CDialog(CMSDNLocations::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSDNLocations)
	m_nNum = 0;
	//}}AFX_DATA_INIT
   m_pPtrList = NULL;
   m_pDocLoc  = NULL;
   m_bChanged = false;
}


void CMSDNLocations::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSDNLocations)
	DDX_Control(pDX, IDC_LOC_SPIN, m_cSpin);
	//}}AFX_DATA_MAP
   if (m_pDocLoc)
   {
	   DDX_Text(pDX, IDC_LOC_EDT_NUMBER, m_pDocLoc->nLocColNum);
	   DDX_Text(pDX, IDC_LOC_EDT_NAME, m_pDocLoc->strLocName);
	   DDX_Text(pDX, IDC_LOC_EDT_TITLE, m_pDocLoc->strTitleString);
	   DDX_Text(pDX, IDC_LOC_EDT_VOLUME, m_pDocLoc->strVolume);
	   DDX_Text(pDX, IDC_LOC_EDT_PATH, m_pDocLoc->strLocPath);
   }
}


BEGIN_MESSAGE_MAP(CMSDNLocations, CDialog)
	//{{AFX_MSG_MAP(CMSDNLocations)
	ON_EN_CHANGE(IDC_LOC_EDT_NUM, OnChangeLocEdtNum)
	ON_EN_CHANGE(IDC_LOC_EDT_NUMBER, OnChange)
	ON_BN_CLICKED(IDC_BTN_LOC_NEW, OnBtnLocNew)
	ON_BN_CLICKED(IDC_BTN_LOC_DELETE, OnBtnLocDelete)
	ON_EN_CHANGE(IDC_LOC_EDT_PATH, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSDNLocations message handlers

BOOL CMSDNLocations::OnInitDialog() 
{
   int nCount = 0;
   if (m_pPtrList)
   {
      nCount = m_pPtrList->GetSize();
      if (nCount > 0)
      {
         m_nNum = 1;
         m_pDocLoc = (MSDNDocLoc*)m_pPtrList->GetAt(m_nNum-1);
      }
   }
	CDialog::OnInitDialog();
   m_cSpin.SetRange(1, nCount);
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_LOC_EDT_NUM, m_nNum);
   
   SendDlgItemMessage(IDC_BTN_LOC_NEW   , BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ICON_NEW)   , IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
   SendDlgItemMessage(IDC_BTN_LOC_DELETE, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ICON_DELETE), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMSDNLocations::OnChangeLocEdtNum() 
{
   if (m_nNum < 1) return;
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_LOC_EDT_NUM, m_nNum);
   if (m_pPtrList)
   {
      SaveChanged();
      m_pDocLoc = (MSDNDocLoc*)m_pPtrList->GetAt(m_nNum-1);
      UpdateData(false);
   }
	
}

void CMSDNLocations::OnChange() 
{
   m_bChanged = true;	
}

void CMSDNLocations::OnBtnLocNew() 
{
   SaveChanged();
   m_pDocLoc = new MSDNDocLoc;
   m_pPtrList->Add(m_pDocLoc);
   int nSize = m_pPtrList->GetSize();
   m_cSpin.SetRange(1, nSize);
   m_nNum = nSize;
   UpdateData(false);
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_LOC_EDT_NUM, m_nNum);
   GetParent()->SendMessage(WM_COMMAND, ID_FILE_CHANGE_DOC_INFO, (LPARAM)m_hWnd);
}

void CMSDNLocations::OnBtnLocDelete() 
{
   int nSize = m_pPtrList->GetSize();
   if ((nSize > 0) && (AfxMessageBox(IDS_DELETE_ITEM, MB_ICONQUESTION|MB_YESNO) == IDYES))
   {
      m_pPtrList->RemoveAt(m_nNum-1);
      delete m_pDocLoc;
      m_pDocLoc = NULL;
      nSize--;
      m_bChanged = false;
      GetParent()->SendMessage(WM_COMMAND, ID_FILE_CHANGE_DOC_INFO, (LPARAM)m_hWnd);
      if (nSize > 0)
      {
         m_cSpin.SetRange(1, nSize);
         m_nNum = 1;
         CDataExchange dx(this, false);
	      DDX_Text(&dx, IDC_LOC_EDT_NUM, m_nNum);
         OnChangeLocEdtNum();
      }
      else
      {
         m_nNum = 0;
      }
   }
}

void CMSDNLocations::SaveChanged()
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

void CMSDNLocations::OnCancel() 
{
   SaveChanged();	
	CDialog::OnCancel();
}
