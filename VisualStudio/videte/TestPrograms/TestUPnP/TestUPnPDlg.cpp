// TestUPnPDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "TestUPnP.h"
#include "TestUPnPDlg.h"

#include <Upnp.h>
#include ".\testupnpdlg.h"

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



class CUPnPDeviceFinderCallback : public IUPnPDeviceFinderCallback
{
public:
    CUPnPDeviceFinderCallback()
    {
        m_lRefCount = 0;
    };
    
    STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppvObject)
    {
        HRESULT hr = S_OK;
        
        if(NULL == ppvObject)
        {
            hr = E_POINTER;
        }
        else
        {
            *ppvObject = NULL;
        }
        
        if(SUCCEEDED(hr))
        {
            if(IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IUPnPDeviceFinderCallback))
            {
                *ppvObject = static_cast<IUPnPDeviceFinderCallback*>(this);
                AddRef();
            }
            else
            {
                hr = E_NOINTERFACE;
            }
        }
        
        return hr;
    };
    
    STDMETHODIMP_(ULONG) AddRef()
    {
        return ::InterlockedIncrement(&m_lRefCount);
    };
    
    STDMETHODIMP_(ULONG) Release()
    {
        LONG lRefCount = ::InterlockedDecrement(&m_lRefCount);
        if(0 == lRefCount)
        {
            delete this;
        }
        
        return lRefCount;
    };
    
    STDMETHODIMP DeviceAdded(LONG lFindData, IUPnPDevice* pDevice)
    {
        HRESULT hr = S_OK;
        
 		CString sTxt = _T("Device Added: udn: "), sLF(_T("\n")), sC(_T(", "));
		BSTR bstr;
        hr = pDevice->get_Description(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sC;
            ::SysFreeString(bstr);
        }
		hr = pDevice->get_SerialNumber(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
        }
        hr = pDevice->get_PresentationURL(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
        }
/*
        hr = pDevice->get_FriendlyName(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
		}
		hr = pDevice->get_UniqueDeviceName(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
        }
        hr = pDevice->get_ModelName(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sC;
            ::SysFreeString(bstr);
        }
        hr = pDevice->get_ModelURL(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
        }
        hr = pDevice->get_ManufacturerName(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sC;
            ::SysFreeString(bstr);
        }
		hr = pDevice->get_ManufacturerURL(&bstr);
        if (SUCCEEDED(hr))
        {
			sTxt += bstr + sLF;
            ::SysFreeString(bstr);
        }
*/
		WK_TRACE(sTxt);
        return hr;
        
    };
    
    STDMETHODIMP DeviceRemoved(LONG lFindData, BSTR bstrUDN)
    {
        WK_TRACE(_T("Device Removed: udn: %s"), CString(bstrUDN));
        return S_OK;
    };
    
    STDMETHODIMP SearchComplete(LONG lFindData)
    {
        WK_TRACE(_T("Search Complete\n"));
        PostQuitMessage(0);
        return S_OK;
    };
    
    
    
private:
    LONG m_lRefCount;
    
};

HRESULT FindUPnPDevice(BSTR TypeURI)
{
    HRESULT hr = S_OK;
    
    IUPnPDeviceFinderCallback* pUPnPDeviceFinderCallback = new CUPnPDeviceFinderCallback();
    if(NULL != pUPnPDeviceFinderCallback)
    {
        pUPnPDeviceFinderCallback->AddRef();
        IUPnPDeviceFinder* pUPnPDeviceFinder;
        hr = CoCreateInstance(CLSID_UPnPDeviceFinder, NULL, CLSCTX_INPROC_SERVER, 
            IID_IUPnPDeviceFinder, reinterpret_cast<void**>(&pUPnPDeviceFinder));
        if(SUCCEEDED(hr))
        {
            LONG lFindData;
            hr = pUPnPDeviceFinder->CreateAsyncFind(TypeURI, 0, pUPnPDeviceFinderCallback, &lFindData);
            if(SUCCEEDED(hr))
            {
                hr = pUPnPDeviceFinder->StartAsyncFind(lFindData);
                if(SUCCEEDED(hr))
                {
                    // STA threads must pump messages
                    MSG Message;
                    BOOL bGetMessage = 0;
                    while(bGetMessage = GetMessage(&Message, NULL, 0, 0) && -1 != bGetMessage)
                    {
                        DispatchMessage(&Message);      
                    }
                }
                pUPnPDeviceFinder->CancelAsyncFind(lFindData);
            }
            pUPnPDeviceFinder->Release();
        }
        pUPnPDeviceFinderCallback->Release();
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
   
    return hr;
}



// CTestUPnPDlg Dialogfeld

CTestUPnPDlg::CTestUPnPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestUPnPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pUpnpThread = NULL;
}

void CTestUPnPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEARCH, m_cComboSearch);
	DDX_Control(pDX, IDC_EDT_REPORT, m_cReport);
}

BEGIN_MESSAGE_MAP(CTestUPnPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBnClickedBtnClear)
END_MESSAGE_MAP()


// CTestUPnPDlg Meldungshandler
HRESULT TraverseCollection(IUPnPDevices * pDevices)
{
    IUnknown * pUnk = NULL;
    HRESULT hr = pDevices->get__NewEnum(&pUnk);
    if (SUCCEEDED(hr))
    {
        IEnumVARIANT * pEnumVar = NULL;
        hr = pUnk->QueryInterface(IID_IEnumVARIANT, (void **) &pEnumVar);
        if (SUCCEEDED(hr))
        {
            VARIANT varCurDevice;
            VariantInit(&varCurDevice);
            pEnumVar->Reset();
            // Loop through each device in the collection
            while (S_OK == pEnumVar->Next(1, &varCurDevice, NULL))
            {
                IUPnPDevice * pDevice = NULL;
                IDispatch * pdispDevice = V_DISPATCH(&varCurDevice);
                if (SUCCEEDED(pdispDevice->QueryInterface(IID_IUPnPDevice, (void **) &pDevice)))
                {
                    // Do something interesting with pDevice
                    BSTR bstrName = NULL;
                    if (SUCCEEDED(pDevice->get_FriendlyName(&bstrName)))
                    {
                        OutputDebugStringW(bstrName);
                        SysFreeString(bstrName);
                    }
                }
                VariantClear(&varCurDevice);
            }
            pEnumVar->Release();
        }
        pUnk->Release();
    }
    return hr;
}

BOOL CTestUPnPDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	SetDebuggerWindowHandle(m_cReport.m_hWnd);


	m_cComboSearch.AddString(_T("ssdp:all"));
	m_cComboSearch.AddString(_T("upnp:rootdevice"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:DigitalSecurityCamera:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:InternetGatewayDevice:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:LANDevice:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:Lighting:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:Luxmeter:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:MediaRenderer:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:MediaServer:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:PowerDevice:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:Reminder:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:device:WANDevice:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:service:DimmingService:1"));
	m_cComboSearch.AddString(_T("urn:schemas-upnp-org:service:Scanner:1"));

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CTestUPnPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
UINT CALLBACK CTestUPnPDlg::UpnpThread(void*p)
{
	CTestUPnPDlg *pThis = (CTestUPnPDlg*)p;
	HRESULT hr = CoInitialize(0);
	if (SUCCEEDED(hr))
	{
		BSTR bstrTypeURI = pThis->m_sSearch.AllocSysString();
		WK_TRACE(_T("Start search: %s\n"), pThis->m_sSearch);
		FindUPnPDevice(bstrTypeURI);
		WK_TRACE(_T("Finnished search\n"), pThis->m_sSearch);
		SysFreeString(bstrTypeURI);
		CoUninitialize();
	}
	pThis->m_pUpnpThread = NULL;
	pThis->GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	return 0;
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CTestUPnPDlg::OnPaint() 
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
HCURSOR CTestUPnPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/////////////////////////////////////////////////////////////////////////////
void CTestUPnPDlg::ReportError(ErrorStruct*ps, BOOL bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str;
		CString sFile(ps->m_sFile);
		str.Format(_T("Error initialising Filters\n%s(%d) : (%x)\n"), sFile, ps->m_nLine, ps->m_hr);
		WK_TRACE(str);
		if (bMsgBox)
		{
			AfxMessageBox(str, MB_ICONERROR|MB_OK);
		}
	}
	delete ps;
}

void CTestUPnPDlg::OnDestroy()
{
	OnBnClickedCancel();
	SetDebuggerWindowHandle(NULL);
	CDialog::OnDestroy();
}

void CTestUPnPDlg::OnBnClickedBtnStart()
{
	int nSel = m_cComboSearch.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_cComboSearch.GetLBText(nSel, m_sSearch);
	}
	else
	{
		m_cComboSearch.GetWindowText(m_sSearch);
	}
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	m_pUpnpThread = AfxBeginThread((AFX_THREADPROC)UpnpThread, (void*)this);
	if (m_pUpnpThread)
	{
		m_pUpnpThread->m_bAutoDelete = TRUE;
	}
}

void CTestUPnPDlg::OnBnClickedCancel()
{
	if (m_pUpnpThread)
	{
		HANDLE hThread = m_pUpnpThread->m_hThread;
		m_pUpnpThread->PostThreadMessage(WM_QUIT, 0, 0);
		WaitForSingleObject(hThread, INFINITE);
	}
}

void CTestUPnPDlg::OnBnClickedOk()
{
	OnOK();
}


/*
	IUPnPDeviceFinder *pDeviceFinder = NULL;
	IUPnPDevices*pDev = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = CoInitialize(0);
		HRESULT_EXCEPTION(hr);

		hr = CoCreateInstance(CLSID_UPnPDeviceFinder, 
							NULL,
							CLSCTX_INPROC_SERVER,
							IID_IUPnPDeviceFinder,
							(void **) &pDeviceFinder);
		HRESULT_EXCEPTION(hr);
		BSTR            bstrTypeURI = NULL;
//DVDVideo, AVTransport, InternetGatewayDevice,urn:schemas-upnp-org:device:Basic:1.0
//		bstrTypeURI = SysAllocString(L"urn:schemas-upnp-org:device:DigitalSecurityCamera:1");
//		bstrTypeURI = SysAllocString(L"urn:schemas-upnp-org:device:DVDVideo");
//		bstrTypeURI = SysAllocString(L"urn:schemas-upnp-org:device:AVTransport");
		bstrTypeURI = SysAllocString(L"urn:schemas-upnp-org:device:InternetGatewayDevice");
		hr = pDeviceFinder->FindByType(bstrTypeURI, 0, &pDev);
		HRESULT_EXCEPTION(hr);
		TraverseCollection(pDev);
		long lCount;
		hr = pDev->get_Count(&lCount);
		HRESULT_EXCEPTION(hr);
	}
	catch (ErrorStruct *ps)
	{
		ReportError(ps, FALSE);
	}

	RELEASE_OBJECT(pDev);
	RELEASE_OBJECT(pDeviceFinder);

	CoUninitialize();
*/

void CTestUPnPDlg::OnBnClickedBtnClear()
{
	m_cReport.SetWindowText(_T(""));
}
