// RemoteTextReceiverCtrl.cpp : Implementation of the CRemoteTextReceiverCtrl ActiveX Control class.

#include "stdafx.h"
#include "RemoteTextReceiver.h"
#include "RemoteTextReceiverCtrl.h"
#include "RemoteTextReceiverPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define IDT_FIRST_UPDATE 100

void CReceivingSocket::OnReceive(int nErrorCode)
{
   TCHAR szText[1024];
   int len = Receive(szText, 1023*sizeof(TCHAR));
   szText[len] = 0;
   CEdit*pEdit = (CEdit*)m_pCtrl;
   pEdit->SetSel(-1, -1);
   pEdit->ReplaceSel(szText);
   m_pCtrl->WriteTextToLogfile(szText);
}

void CReceivingSocket::OnAccept(int nErrorCode)
{
}


IMPLEMENT_DYNCREATE(CRemoteTextReceiverCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CRemoteTextReceiverCtrl, COleControl)
    ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
    ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
   ON_WM_TIMER()
   ON_WM_CREATE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CRemoteTextReceiverCtrl, COleControl)
   DISP_STOCKPROP_CAPTION()
   DISP_STOCKPROP_FORECOLOR()
   DISP_STOCKPROP_BACKCOLOR()
   DISP_STOCKPROP_BORDERSTYLE()
   DISP_STOCKPROP_ENABLED()
   DISP_STOCKPROP_FONT()
   DISP_STOCKPROP_APPEARANCE()
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CRemoteTextReceiverCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CRemoteTextReceiverCtrl, 1)
    PROPPAGEID(CRemoteTextReceiverPropPage::guid)
END_PROPPAGEIDS(CRemoteTextReceiverCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRemoteTextReceiverCtrl, "REMOTETEXTRECEIV.RemoteTextReceivCtrl.1",
    0x1b0f0bfd, 0x8a71, 0x4564, 0xad, 0x20, 0xbd, 0xeb, 0x1c, 0xc9, 0xa7, 0x96)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CRemoteTextReceiverCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DRemoteTextReceiver =
        { 0x884AE4FA, 0x5170, 0x48F7, { 0x8B, 0x90, 0x7A, 0x60, 0x5E, 0x62, 0x29, 0x4C } };
const IID BASED_CODE IID_DRemoteTextReceiverEvents =
        { 0x426F7FB8, 0xBE1C, 0x4414, { 0xAE, 0x3C, 0x3C, 0x68, 0x1A, 0xF9, 0xFC, 0x88 } };



// Control type information

static const DWORD BASED_CODE _dwRemoteTextReceiverOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRemoteTextReceiverCtrl, IDS_REMOTETEXTRECEIVER, _dwRemoteTextReceiverOleMisc)



// CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CRemoteTextReceiverCtrl

BOOL CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    // TODO: Verify that your control follows apartment-model threading rules.
    // Refer to MFC TechNote 64 for more information.
    // If your control does not conform to the apartment-model rules, then
    // you must modify the code below, changing the 6th parameter from
    // afxRegApartmentThreading to 0.

    if (bRegister)
    {
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_REMOTETEXTRECEIVER,
            IDB_REMOTETEXTRECEIVER,
            0,//afxRegApartmentThreading,
            _dwRemoteTextReceiverOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    }
    else
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// Licensing strings

static const TCHAR BASED_CODE _szLicFileName[] = _T("RemoteTextReceiver.lic");

static const WCHAR BASED_CODE _szLicString[] =
    L"Copyright (c) 2007 ";



// CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::VerifyUserLicense -
// Checks for existence of a user license

BOOL CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::VerifyUserLicense()
{
    return TRUE;
    //return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName, _szLicString);
}



// CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::GetLicenseKey -
// Returns a runtime licensing key

BOOL CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrlFactory::GetLicenseKey(DWORD dwReserved,
    BSTR FAR* pbstrKey)
{
    if (pbstrKey == NULL)
        return FALSE;

    *pbstrKey = SysAllocString(_szLicString);
    return (*pbstrKey != NULL);
}



// CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrl - Constructor

CRemoteTextReceiverCtrl::CRemoteTextReceiverCtrl() :
   m_Receiver(this)
{
    InitializeIIDs(&IID_DRemoteTextReceiver, &IID_DRemoteTextReceiverEvents);
    m_nPort = 0;
}



// CRemoteTextReceiverCtrl::~CRemoteTextReceiverCtrl - Destructor

CRemoteTextReceiverCtrl::~CRemoteTextReceiverCtrl()
{
    // TODO: Cleanup your control's instance data here.
}

void CRemoteTextReceiverCtrl::WriteTextToLogfile(LPCTSTR s)
{
    if (m_LogFile.m_hFile != CFile::hFileNull)
    {
        m_LogFile.WriteString(s);
    }
}


// CRemoteTextReceiverCtrl::OnDraw - Drawing function

void CRemoteTextReceiverCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    if (!pdc)
        return;

    DoSuperclassPaint(pdc, rcBounds);
}



// CRemoteTextReceiverCtrl::DoPropExchange - Persistence support

void CRemoteTextReceiverCtrl::DoPropExchange(CPropExchange* pPX)
{
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);
    PX_String(pPX, "URL"    , m_sUrl    , NULL);
    PX_String(pPX, "LogFile", m_sLogFile, NULL);
    PX_ULong( pPX, "Port"   , m_nPort   , 0);
    PX_String(pPX, "Text"   , m_sText    , NULL);
}



// CRemoteTextReceiverCtrl::OnResetState - Reset control to default state

void CRemoteTextReceiverCtrl::OnResetState()
{
    COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}



// CRemoteTextReceiverCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CRemoteTextReceiverCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = _T("EDIT");
    //cs.style |= ES_MULTILINE|ES_READONLY|ES_NOHIDESEL|ES_AUTOHSCROLL|ES_AUTOVSCROLL;
    cs.style |= ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL;
    return COleControl::PreCreateWindow(cs);
}



// CRemoteTextReceiverCtrl::IsSubclassedControl - This is a subclassed control

BOOL CRemoteTextReceiverCtrl::IsSubclassedControl()
{
    return TRUE;
}



// CRemoteTextReceiverCtrl::OnOcmCommand - Handle command messages

LRESULT CRemoteTextReceiverCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
    WORD wNotifyCode = HIWORD(wParam);
#else
    WORD wNotifyCode = HIWORD(lParam);
#endif

    // TODO: Switch on wNotifyCode here.

    return 0;
}



// CRemoteTextReceiverCtrl message handlers


void CRemoteTextReceiverCtrl::OnTimer(UINT nIDEvent)
{
    if (!m_sText.IsEmpty())
    {
        SetWindowText(m_sText);
    }
    KillTimer(IDT_FIRST_UPDATE);
    COleControl::OnTimer(nIDEvent);
}

int CRemoteTextReceiverCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (theApp.m_bSocketInitialized)
    {
        if (m_Receiver.Create(0))
        {
            if (m_Receiver.Connect(m_sUrl, m_nPort))
            {
            }
        }
    }
    if (!m_sLogFile.IsEmpty())
    {
        m_LogFile.Open(m_sLogFile, CFile::typeText|CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate);
    }
    SetTimer(IDT_FIRST_UPDATE, 100, NULL);

    return 0;
}


