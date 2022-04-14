// ErrorMessageDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "ErrorMessageDlg.h"
#include "CustErr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CErrorMessageDlg 


CErrorMessageDlg::CErrorMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrorMessageDlg)
	m_nErrorMessage = 0;
	m_strErrorMessage = _T("");
	//}}AFX_DATA_INIT
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME6);
}


void CErrorMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorMessageDlg)
	//}}AFX_DATA_MAP
   GetErrorNumber();
	DDX_Text(pDX, IDC_EDT_ERRRO_MESSAGE, m_strErrorMessage);
}


BEGIN_MESSAGE_MAP(CErrorMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CErrorMessageDlg)
	ON_EN_KILLFOCUS(IDC_EDT_ERROR_NUMBER, OnKillfocusEdtErrorNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CErrorMessageDlg 

void CErrorMessageDlg::OnKillfocusEdtErrorNumber() 
{
   GetErrorNumber();
   GetErrorMessage();
}

void CErrorMessageDlg::GetErrorNumber()
{
   CString str;
   char    *cStop;
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_ERROR_NUMBER, str);
   if (!str.IsEmpty())
      m_nErrorMessage = strtoul(str, &cStop, 16);
}

void CErrorMessageDlg::GetErrorMessage()
{
   if (m_nErrorMessage & CUSTOMER_CODE_FLAG)
   {
      CString strLoad;
      int     nID = 0, nModule = 0;
      m_strErrorMessage.LoadString(IDS_MODULE);

      switch (m_nErrorMessage & PRODUCT_RANGE)
      {
         case ETS_BASE_MODULE:
         {
            strLoad.LoadString(IDS_BASE_MODULE);
            m_strErrorMessage += strLoad;
            m_strErrorMessage += '\n';
            switch (m_nErrorMessage & MODULE_RANGE)
            {
               case MODULE_ETSDIV:  strLoad.LoadString(IDS_ETSDIV); nModule = 0x1000; break;
               case MODULE_ETS3DGL: strLoad.LoadString(IDS_ETS3DGL); nModule = 0x2000; break;
               default: strLoad.Empty(); break;
            }
         }break;
         case PRODUCT_CARA_MODULE:
         {
            strLoad.LoadString(IDS_CARA_MODULE);
            m_strErrorMessage += strLoad;
            m_strErrorMessage += "\n\r";
            switch (m_nErrorMessage & MODULE_RANGE)
            {
               case MODULE_CARAWALK: strLoad.LoadString(IDS_CARAWALK); nModule = 0x1800; break;
               case MODULE_CARABOX:  strLoad.LoadString(IDS_CARABOX); nModule = 0x2800; break;
               case MODULE_CARASDB:  strLoad.LoadString(IDS_CARASDB); nModule = 0x3800; break;
               default: strLoad.Empty(); break;
            }
         } break;
         case PRODUCT_CAPE_MODULE:
         {
            strLoad.LoadString(IDS_CAPE_MODULE);
         } break;
         default:
            strLoad.LoadString(IDS_UNKNOWN_MODULE);
            m_strErrorMessage += strLoad;
            m_strErrorMessage += "\n\r";
            break;
      }

      m_strErrorMessage += strLoad;

      if ((m_nErrorMessage & CODE_RANGE) >= 0x0020)         // Errorcode speziell für ein Modul
      {
         switch (m_nErrorMessage & CODE_RANGE)
         {
            case 0:  nID = IDS_FUNCTION_LOAD_ERROR;
            default: nID = m_nErrorMessage & CODE_RANGE;
         }
      }
      else
      {
         nID = (m_nErrorMessage & CODE_RANGE) | nModule;
      }

      if (strLoad.LoadString(nID))
      {
         m_strErrorMessage += strLoad;
         CDataExchange dx(this, false);
   	   DDX_Text(&dx, IDC_EDT_ERRRO_MESSAGE, m_strErrorMessage);
      }
   }
}

BOOL CErrorMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

CErrorMessageDlg::~CErrorMessageDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}

#endif // _DEBUG

