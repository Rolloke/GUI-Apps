// PicturePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PictureLabel.h"
#include "PicturePropPage.h"
#include "CARAPREV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CPicturePropPage 

IMPLEMENT_DYNCREATE(CPicturePropPage, CCaraPropertyPage)

CPicturePropPage::CPicturePropPage() : CCaraPropertyPage(CPicturePropPage::IDD)
{
   //{{AFX_DATA_INIT(CPicturePropPage)
   m_bHalftone = FALSE;
   m_nScaling = -1;
   m_bShiny = FALSE;
   m_nOutput = FALSE;
	m_bFile = FALSE;
	m_strFileName = _T("");
	//}}AFX_DATA_INIT

   m_pLabel         = NULL;
   m_bValuesChanged = false;
   m_bNoFile        = true;
}  

CPicturePropPage::~CPicturePropPage()
{
}

void CPicturePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPicturePropPage)
	DDX_Control(pDX, IDC_PREVIEWSTD, m_Preview);
	DDX_Check(pDX, IDC_PICT_HALFTONE, m_bHalftone);
	DDX_Radio(pDX, IDC_PICT_SCALING0, m_nScaling);
	DDX_Check(pDX, IDC_PICT_SHINY,    m_bShiny);
	DDX_Check(pDX, IDC_PICT_OUTPUT0,  m_nOutput);
	DDX_Check(pDX, IDC_PICT_FILE, m_bFile);
	DDX_Text(pDX, IDC_PICT_FILENAME, m_strFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPicturePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPicturePropPage)
	ON_BN_CLICKED(IDC_PICT_HALFTONE, OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_OUTPUT0,  OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_SCALING0, OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_SCALING1, OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_SCALING2, OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_SHINY,    OnBnClicked)
	ON_BN_CLICKED(IDC_PICT_FILE, OnPictFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPicturePropPage 
BOOL CPicturePropPage::OnSetActive() 
{
   if (m_bValuesChanged)
   {
      UpdateData(false);
      m_bValuesChanged = false;
      if (m_pLabel)
      {
         bool bDibFmt = (m_pLabel->GetFormat() == DIB_FORMAT);
         GetDlgItem(IDC_PICT_OUTPUT0)->EnableWindow(bDibFmt);
         GetDlgItem(IDC_PICT_SHINY)->EnableWindow(bDibFmt);
         GetDlgItem(IDC_PICT_HALFTONE)->EnableWindow(bDibFmt);
         GetDlgItem(IDC_PICT_SCALING0)->EnableWindow(bDibFmt);
         if (bDibFmt && !m_bNoFile)
         {
            GetDlgItem(IDC_PICT_FILENAME)->EnableWindow(m_bFile);
            CString str;
            str.LoadString(IDS_NO_PICTURE);
            GetDlgItem(IDC_PICT_FILE)->EnableWindow((str == m_strFileName) ? false : true);
         }
         else
         {
            GetDlgItem(IDC_PICT_FILE)->EnableWindow(false);
            GetDlgItem(IDC_PICT_FILENAME)->EnableWindow(false);
         }
      }
      m_Preview.SetBkColor(RGB(255,255,255));
   }
   return CPropertyPage::OnSetActive();
}
BOOL CPicturePropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CPicturePropPage::IDD);
   if (m_bNoFile)
   {
      GetDlgItem(IDC_PICT_FILE    )->EnableWindow(false);
      GetDlgItem(IDC_PICT_FILENAME)->EnableWindow(false);
   }
   return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
BOOL CPicturePropPage::OnKillActive() 
{
   int nResult = CCaraPropertyPage::OnKillActive();
   if (nResult & CHANGE_PAGE)
   {
      if (nResult & CHANGES_REJECTED)
         SetLabel(m_pLabel);
      return true;
   }
   else
   {
      CWnd *pFocus  = GetFocus();
      if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}
void CPicturePropPage::OnOK() 
{
   if (m_pLabel)
   {
      m_pLabel->SetScale(m_nScaling);
      m_pLabel->SetSaveInFile((m_bFile != 0) ? true : false);
      if (m_bFile)
      {
         if (m_strFileName.IsEmpty()) return;
         m_strFileName.MakeUpper();
         if (m_strFileName.Find(".BMP") == -1) m_strFileName = m_strFileName + ".BMP";
      }
      m_pLabel->SetFileName(m_strFileName);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CPicturePropPage::OnApply() 
{
   return CCaraPropertyPage::OnApply();
}

void CPicturePropPage::OnBnClicked() 
{
   if (UpdateData(true))
   {
      SetMode();
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CPicturePropPage::SetLabel(CLabel *pl)
{
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CPictureLabel)))
   {
      ASSERT_VALID(pl);
      m_pLabel        = (CPictureLabel*)pl;
      m_bValuesChanged = true;
      m_Preview.SetLabel(pl);

      m_nOldScale = m_nScaling = m_pLabel->GetScaling();

      m_nOldMode = m_pLabel->GetRopMode();
      if ((m_nOldMode==SRCAND)||(m_nOldMode==SRCCOPY  )) m_nOutput = 0;    // normal
      else                                               m_nOutput = 1;    // invertiert
      if ((m_nOldMode==SRCAND)||(m_nOldMode==SRCINVERT)) m_bShiny  = true; // durchsichtig
      else                                               m_bShiny  = false;// nicht durchsichtig

      m_dwOldStretchBltMode = m_pLabel->GetStretchBltMode();
      if (m_dwOldStretchBltMode == HALFTONE) m_bHalftone = true;
      else                                   m_bHalftone = false;
      m_bFile       = m_pLabel->SaveInFile();
      m_strFileName = m_pLabel->GetFileName();
      m_bNoFile     = m_pLabel->m_bNoFile;
   }
   else m_pLabel = NULL;
}


void CPicturePropPage::SetMode()
{
   int nMode;
   if (m_nOutput == 0)                                         // normal
   {
      if (m_bShiny) nMode = SRCAND;                            // und durchsichtig
      else          nMode = SRCCOPY;
   }
   else                                                        // invertiert
   {
      if (m_bShiny) nMode = SRCINVERT;                         // und durchsichtig
      else          nMode = NOTSRCCOPY;
   }
   if (m_pLabel)
   {
      if (m_nScaling == SCALE_PROPORTIONAL)                    // Proportional skalieren
      {
         m_pLabel->SetScale(m_nScaling);
         m_pLabel->CheckScale();
      }
      m_pLabel->SetRopMode(nMode);
      m_pLabel->SetStretchBltMode(m_bHalftone ? HALFTONE : COLORONCOLOR);
   }
}

void CPicturePropPage::OnCancel() 
{
   m_pLabel->SetRopMode(m_nOldMode);
   m_pLabel->SetStretchBltMode(m_dwOldStretchBltMode);
   m_pLabel->SetScale(m_nOldScale);
   CPropertyPage::OnCancel();
}

void CPicturePropPage::OnPictFile() 
{
   UpdateData(true);
   GetDlgItem(IDC_PICT_FILENAME)->EnableWindow(m_bFile);
}
