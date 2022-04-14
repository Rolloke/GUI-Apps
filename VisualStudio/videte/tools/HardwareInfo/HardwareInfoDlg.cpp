// HardwareInfoDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "HardwareInfo.h"
#include "HardwareInfoDlg.h"

#include "strmif.h"

//#include "..\..\DirectShow\Common\dshowutil.h"

#undef TRACE
#define TRACE DebugOutput

#define SAFE_RELEASE(x)  if(x) {x->Release(); x=0;}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialogfelddaten
   enum { IDD = IDD_ABOUTBOX };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHardwareInfoDlg Dialogfeld



CHardwareInfoDlg::CHardwareInfoDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CHardwareInfoDlg::IDD, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_pLocator = NULL;
    m_pServices = NULL;
}

CHardwareInfoDlg::~CHardwareInfoDlg()   // Standarddestruktor
{
   SAFE_RELEASE(m_pServices);
   SAFE_RELEASE(m_pLocator);     
   CoUninitialize();
}

void CHardwareInfoDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_EDT_OUT, m_edtOut);
}

BEGIN_MESSAGE_MAP(CHardwareInfoDlg, CDialog)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CHardwareInfoDlg Meldungshandler

BOOL CHardwareInfoDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Hinzufügen des Menübefehls "Info..." zum Systemmenü.

   // IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
   //  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
   SetIcon(m_hIcon, TRUE);         // Großes Symbol verwenden
   SetIcon(m_hIcon, FALSE);      // Kleines Symbol verwenden
   CString sError, sDescription;

   HRESULT hres;
   try
   {
      hres =  CoInitializeEx(0, COINIT_MULTITHREADED); // Initialize COM.
      if (FAILED(hres)) throw hres;

      hres =  CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);
      if (FAILED(hres)) throw hres;

       hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_pLocator);
      if (FAILED(hres)) throw hres;

      // Connect to the root\default namespace with the current user.
      hres = m_pLocator->ConnectServer(_bstr_t(L"root\\cimv2"), NULL, NULL, 0, NULL, 0, 0, &m_pServices);
      if (FAILED(hres)) throw hres;

       // Set the proxy so that impersonation of the client occurs.
      hres = CoSetProxyBlanket(m_pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,NULL,
         RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
      if (FAILED(hres)) throw hres;
   }
   catch (HRESULT hError)
   {
      SAFE_RELEASE(m_pServices);
      SAFE_RELEASE(m_pLocator);     
//      GetErrorStrings(hError, sError, sDescription);
      TRACE(_T("Error:%x, %s:%s\n"),hError, sError, sDescription);
   }
   if (m_pServices)
   {
      IEnumWbemClassObject *pEnum = 0;
      int i=0, n=0;
      BSTR bstrClases[15];
      bstrClases[n++] = L"Win32_LogicalDisk";   // 1
      bstrClases[n++] = L"Win32_Fan";
      bstrClases[n++] = L"Win32_HeatPipe";
      bstrClases[n++] = L"Win32_Refrigeration";
      bstrClases[n++] = L"Win32_TemperatureProbe";
      bstrClases[n++] = L"Win32_Keyboard";
      bstrClases[n++] = L"Win32_PointingDevice";
      bstrClases[n++] = L"Win32_CDROMDrive";
      bstrClases[n++] = L"Win32_Bus";
      bstrClases[n++] = L"Win32_DeviceBus";
      bstrClases[n++] = L"Win32_IDEController";
      bstrClases[n++] = L"Win32_MotherboardDevice";
      bstrClases[n++] = L"Win32_OnBoardDevice";
      bstrClases[n++] = L"Win32_Processor";
      bstrClases[n++] = L"Win32_SoundDevice";
      //n=0;
      for (i=0; i<n; i++)
      {
         TRACE(_T("%s\n"), CString(bstrClases[i]));
         hres = m_pServices->CreateInstanceEnum(bstrClases[i], WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);
         if (SUCCEEDED(hres))
         {
            ULONG uTotal = 0;
            BSTR pstrName = NULL;
            VARIANT   vValue;
            LONG    lFlavor;
            CIMTYPE    cimtype;

            // Retrieve the objects in the result set.
            for (;;)
            {
               IWbemClassObject *pObj = 0;
               ULONG uReturned = 0;

               hres = pEnum->Next(
                  0,                  // Time out
                  1,                  // One object
                  &pObj,
                  &uReturned
                  );

               uTotal += uReturned;

               if (uReturned == 0)
                  break;
               // Use the object.

               VariantInit(&vValue);
               cimtype = 0;
               lFlavor = 0;
               pstrName = _bstr_t(L"__PATH");
               pObj->Get(pstrName, 0, &vValue, &cimtype, &lFlavor);

               CString sName(pstrName);
               CString sValue;
               switch (vValue.vt)
               {
                  case VT_BSTR: sValue = CString(vValue.bstrVal); break;
                  case VT_I4: sValue.Format(_T("%d"), vValue.intVal);
               }
               TRACE(_T("Path:%s, type:%d, Flavor:%x\n"), sValue, cimtype, lFlavor);
               VariantClear(&vValue);
               SysFreeString(pstrName);
                 
               pObj->Release();    // Release objects not owned.            
            }

            // All done.
            pEnum->Release();
         }
         else
         {
//            GetErrorStrings(hres, sError, sDescription);
         }
      }

      IWbemContext *pContext = NULL;
       hres = CoCreateInstance(CLSID_WbemContext, 0, CLSCTX_INPROC_SERVER, IID_IWbemContext, (LPVOID*)&pContext);

      IWbemClassObject *pclObj = NULL;
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_VoltageProbe"), WBEM_FLAG_RETURN_WBEM_COMPLETE, pContext, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_TemperatureProbe"), WBEM_FLAG_DIRECT_READ, NULL, &pclObj, NULL);
      hres = m_pServices->GetObject(_bstr_t(L"Win32_Environment"), WBEM_FLAG_DIRECT_READ, NULL, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_BIOS"), WBEM_FLAG_DIRECT_READ, NULL, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_Bus"), WBEM_FLAG_DIRECT_READ, NULL, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_Fan"), WBEM_FLAG_RETURN_WBEM_COMPLETE, pContext, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_LogicalDisk"), 0, 0, &pclObj, NULL);
//      hres = m_pServices->GetObject(_bstr_t(L"Win32_LogicalDisk.DeviceID=\"d:\""), 0, 0, &pclObj, NULL);
//      GetErrorStrings(hres, sError, sDescription);
      if (pclObj)
      {
         hres = pclObj->BeginEnumeration(0);
         BSTR pstrName = NULL;
         VARIANT   vValue;
         LONG    lFlavor;
         CIMTYPE    cimtype;
         while (SUCCEEDED(hres))
         {
            VariantInit(&vValue);
            hres = pclObj->Next(0, &pstrName, &vValue, &cimtype, &lFlavor);
            if (pstrName == NULL)
            {
               break;
            }
            CString sName(pstrName);
            CString sValue;
            CString sVariant;
            switch (vValue.vt)
            {
               case VT_BSTR: sValue = CString(vValue.bstrVal); sVariant = _T("VT_BSTR"); break;
               case VT_I4: sValue.Format(_T("%d"), vValue.intVal);  sVariant = _T("VT_I4");break;
               case VT_NULL: sValue.Format(_T("%d"), vValue.intVal);  sVariant = _T("VT_NULL");break;
               case (VT_ARRAY+VT_BSTR):
               {
                  sVariant = _T("VT_ARRAY+VT_BSTR");
                  BSTR str;
                  long l = 0;
                  for (l=0; l<(long)vValue.parray->cbElements; l++)
                  {
                     hres = SafeArrayGetElement(vValue.parray, &l, &str);
                     if (SUCCEEDED(hres))
                     {
                        sName += _T(", ");
                        sName += CString(str);
                     }
                  }
               }break;
            }
            TRACE(_T("%s:%s, type:%d, Variant:%s (%d)\n"), sName, sValue, cimtype, sVariant, vValue.vt);
            VariantClear(&vValue);
            SysFreeString(pstrName);
         }

         IWbemClassObject *pInParams   = NULL;
         IWbemClassObject *pOutParams = NULL;
         HRESULT hr = NULL;
         BSTR pName = NULL;

         hr = pclObj->BeginMethodEnumeration(0);

         while (! FAILED(hr) && WBEM_S_NO_MORE_DATA != hr)
         {
            hr = pclObj->NextMethod(0, &pName, &pInParams, &pOutParams); 
            SysFreeString(pName); 
            SAFE_RELEASE(pInParams);
            SAFE_RELEASE(pOutParams);
         }
      }

      SAFE_RELEASE(pContext);
/*
      // making a query
      BSTR Language = SysAllocString(L"WQL");
      BSTR Query = SysAllocString(L"SELECT * FROM Win32_Fan");

       // Initialize the IEnumWbemClassObject pointer.
      IEnumWbemClassObject *pEnum = 0;

      // Issue the query.
      hres = m_pServices->ExecQuery(Language,Query, WBEM_FLAG_FORWARD_ONLY, 0, &pEnum);
       SysFreeString(Query);
      SysFreeString(Language);
      if (SUCCEEDED(hres))
      {
         ULONG uTotal = 0;

         // Retrieve the objects in the result set.
         for (;;)
         {
            IWbemClassObject *pObj = 0;
            ULONG uReturned = 0;

            hres = pEnum->Next(
               0,                  // Time out
               1,                  // One object
               &pObj,
               &uReturned
               );

            uTotal += uReturned;

            if (uReturned == 0)
               break;
            // Use the object.
            hres = pObj->BeginEnumeration(0);
            BSTR pstrName = NULL;
            VARIANT   vValue;
            LONG    lFlavor;
            CIMTYPE    cimtype;
            while (SUCCEEDED(hres))
            {
               VariantInit(&vValue);
               hres = pObj->Next(0, &pstrName, &vValue, &cimtype, &lFlavor);
               if (pstrName == NULL)
               {
                  break;
               }
               CString sName(pstrName);
               CString sValue;
               switch (vValue.vt)
               {
                  case VT_BSTR: sValue = CString(vValue.bstrVal); break;
                  case VT_I4: sValue.Format(_T("%d"), vValue.intVal);
               }
               TRACE(_T("%s:%s, type:%d\n"), sName, sValue, cimtype);
               VariantClear(&vValue);
               SysFreeString(pstrName);
            }

              
            // ...
              
            // Release it.
            // ===========
              
            pObj->Release();    // Release objects not owned.            
         }

         // All done.
         pEnum->Release();
      }
*/
   }
   return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CHardwareInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CHardwareInfoDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // Gerätekontext zum Zeichnen

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Symbol in Clientrechteck zentrieren
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Symbol zeichnen
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CHardwareInfoDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

void CHardwareInfoDlg::DebugOutput(TCHAR *tszErr, ...)
{
   CString sErrOut;
   LPTSTR tszErrOut = sErrOut.GetBufferSetLength(1024);

   va_list valist;

   va_start(valist,tszErr);
   wvsprintf(tszErrOut, tszErr, valist);
   sErrOut.ReleaseBuffer();
   sErrOut.Replace(_T("\n"), _T("\r\n"));

   m_edtOut.SendMessage(EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
   m_edtOut.SendMessage(EM_REPLACESEL, (WPARAM)0, (LPARAM)LPCTSTR(sErrOut));
   va_end (valist);
}
