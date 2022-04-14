// AudioTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AudioTest.h"
#include "AudioTestDlg.h"

#include <KS.h>
#include <KSMEDIA.h>
#include <atlbase.h>
#include "ksproxy.h"

#include "common\mfcutil.h"
#include "common\dshowutil.h"
#include "include\dmoreg.h"
#include "common\namedguid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HR_EXCEPTION(HR) if (FAILED(hr=HR))	throw HRESULT_ERROR_AT_POS(hr);
#define HR_REPORT(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr), false);
#define HR_MSG_BOX(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr));

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDlg dialog

CAudioTestDlg::CAudioTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioTestDlg)
	m_bTest = FALSE;
	m_nCapture = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pGB         = NULL;
	m_pCaptureGB  = NULL;
	m_pMC         = NULL;
	m_pME         = NULL;
	m_pCapture    = NULL;
	m_Prov.AddRef();
}

void CAudioTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioTestDlg)
	DDX_Control(pDX, IDC_COMBO_INPUT_CHANNEL, m_cInputChannels);
	DDX_Control(pDX, IDC_COMBO_RENDER_DEV, m_cRenderer);
	DDX_Control(pDX, IDC_COMBO_COMPRESSOR_DEV, m_cCompressor);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_DEV, m_cCapture);
	DDX_Check(pDX, IDC_BTN_TEST, m_bTest);
	DDX_CBIndex(pDX, IDC_COMBO_CAPTURE_DEV, m_nCapture);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAudioTestDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_TEST, OnBtnTest)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_DEV, OnSelchangeComboCaptureDev)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT_CHANNEL, OnSelchangeComboInputChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDlg message handlers

BOOL CAudioTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	HRESULT hr = CoInitialize(NULL);

	ICreateDevEnum *pSysDevEnum = NULL;
   hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (pSysDevEnum)
	{
		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioInputDeviceCategory, NULL, &m_cCapture);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cCapture);
		}
		else
		{
			m_cCapture.SetCurSel(0);
		}

		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioCompressorCategory, NULL, &m_cCompressor);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cCompressor);
		}
		else
		{
			m_cCompressor.SetCurSel(m_cCompressor.FindString(-1, "mpeg"));
		}

		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioRendererCategory, NULL, &m_cRenderer);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cRenderer);
		}
		else
		{
			m_cRenderer.SetCurSel(0);
		}
		RELEASE_OBJECT(pSysDevEnum);
	}
	
	try
	{
		InitGraphBuilder();
	}
	catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters();
	}
	OnSelchangeComboCaptureDev();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAudioTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAudioTestDlg::OnDestroy() 
{
	ReleaseFilters();
	ClearFilterListWithMoniker(m_cCapture);
	ClearFilterListWithMoniker(m_cCompressor);
	ClearFilterListWithMoniker(m_cRenderer);

	CoUninitialize();
	CDialog::OnDestroy();
	
	
}

void CAudioTestDlg::InitGraphBuilder()
{
	if (m_pGB == NULL)
	{
		HRESULT hr;
		// Create the  graph builder.
		HR_EXCEPTION(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB));
		// Create the capture graph builder.
		HR_EXCEPTION(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pCaptureGB));
		// Associate the filter graph with the capture graph builder
		HR_EXCEPTION(m_pCaptureGB->SetFiltergraph(m_pGB));						
		// Query the Media Control Interface
		HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC));
		// Query the Media Event Interface
		HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME));
		HR_REPORT(m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));
		IObjectWithSite* pObjectWithSite = NULL;
		hr = m_pGB->QueryInterface(IID_IObjectWithSite, (void**)&pObjectWithSite);
		if (SUCCEEDED(hr))
		{
			// Use the IObjectWithSite pointer to specify our key provider object.
			// The filter graph manager will use this pointer to call
			// QueryService to do the unlocking.
			// If the unlocking succeeds, then we can build our graph.
			hr = pObjectWithSite->SetSite((IUnknown *) (IServiceProvider *) &m_Prov);
			pObjectWithSite->Release();
		}
		IMediaFilter *pMF;
		hr = m_pGB->QueryInterface(IID_IMediaFilter, (void**)&pMF);
		if (SUCCEEDED(hr))
		{
			pMF->SetSyncSource(NULL);
			pMF->Release();
		}
	}
}

void CAudioTestDlg::ReleaseFilters()
{
	ReleaseGraphFilters();

	if (m_pME)
	{
		m_pME->SetNotifyWindow(NULL, 0, 0);
	}
	
	RELEASE_OBJECT(m_pMC);
	RELEASE_OBJECT(m_pME);

	RELEASE_OBJECT(m_pGB);
	RELEASE_OBJECT(m_pCaptureGB);
}

/////////////////////////////////////////////////////////////////////////////
void CAudioTestDlg::ReportError(ErrorStruct*ps, bool bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
		str.Format(_T("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n"), ps->m_sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
#ifdef _DEBUG
		TRACE(str);
#endif		
		if (bMsgBox)
		{
			AfxMessageBox(str, MB_ICONERROR|MB_OK);
		}
	}
	delete ps;
}

void CAudioTestDlg::OnBtnTest() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_BTN_TEST, m_bTest);
	if (m_bTest)
	{
		IBaseFilter *pCompressor= NULL;
		IBaseFilter *pRenderer  = NULL;
		IPin        *pPin = NULL;
		IMoniker    *pMoniker;
		HRESULT      hr;
		
		try
		{
			if (m_pCapture == NULL) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

			pMoniker = (IMoniker*)m_cCompressor.GetItemData(m_cCompressor.GetCurSel());
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
			HR_EXCEPTION(AddFilterToGraph(m_pGB, &pCompressor, pMoniker, L"Compressor"));

			HR_EXCEPTION(ConnectFilters(m_pGB, m_pCapture, 0, pCompressor, 0));
			HR_EXCEPTION(GetPin(pCompressor, PINDIR_OUTPUT, 0, &pPin));

			pMoniker = (IMoniker*)m_cRenderer.GetItemData(m_cRenderer.GetCurSel());
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
			HR_EXCEPTION(AddFilterToGraph(m_pGB, &pRenderer, pMoniker, L"Renderer"));

			HR_EXCEPTION(m_pCaptureGB->RenderStream(NULL, &MEDIATYPE_Audio, pPin, NULL, pRenderer));

			HR_EXCEPTION(m_pMC->Run());
		}
		catch(ErrorStruct *ps)
		{
			ReportError(ps);
			dx.m_bSaveAndValidate = FALSE;
			m_bTest               = FALSE;
			DDX_Check(&dx, IDC_BTN_TEST, m_bTest);
			ReleaseGraphFilters(FALSE);
		}
		RELEASE_OBJECT(pCompressor);
		RELEASE_OBJECT(pPin);
		RELEASE_OBJECT(pRenderer);
	}
	else
	{
		ReleaseGraphFilters(FALSE);
	}
}

void CAudioTestDlg::OnSelchangeComboCaptureDev() 
{
	int nSel = m_cCapture.GetCurSel();
	if (nSel != m_nCapture)
	{
		HRESULT             hr = 0;
		IAMAudioInputMixer *pAMaim = NULL;
		IPin               *pPinIn = NULL;
		int                 n, nIn;
		ReleaseGraphFilters();

		hr = AddFilterToGraph(m_pGB, &m_pCapture, (IMoniker*)m_cCapture.GetItemData(nSel), L"Capture");
		n = 0;
		while (SUCCEEDED(hr))
		{
			hr = GetPin(m_pCapture, PINDIR_INPUT, n++, &pPinIn);		// Query pins of the capture and the wave splitter device
			if (SUCCEEDED(hr))
			{
				PIN_INFO pi;
				hr = pPinIn->QueryPinInfo(&pi);								// Query Pin Info
				if (SUCCEEDED(hr))
				{
					CString str(pi.achName);
					nIn = m_cInputChannels.AddString(str);
				}
				else
				{
					ReportError(HRESULT_ERROR_AT_POS(hr));
				}
				RELEASE_OBJECT(pi.pFilter);
				if (nIn != CB_ERR)
				{
					hr = pPinIn->QueryInterface(IID_IAMAudioInputMixer, (void**)&pAMaim);
					if (SUCCEEDED(hr))												// Query Input Mixer
					{
						m_cInputChannels.SetItemData(nIn, (DWORD)pAMaim);
					}
				}
				RELEASE_OBJECT(pPinIn);
			}
		}
		m_nCapture = nSel;
		m_cInputChannels.SetCurSel(m_cInputChannels.FindString(-1, "Mi"));
		OnSelchangeComboInputChannel();
	}
}

void CAudioTestDlg::ResetInputChannels()
{
	int i, nCount = m_cInputChannels.GetCount();
	for (i=0; i<nCount; i++)
	{
		IAMAudioInputMixer *pAMaim = (IAMAudioInputMixer*)m_cInputChannels.GetItemData(i);
		RELEASE_OBJECT(pAMaim);
	}
	m_cInputChannels.ResetContent();
}

void CAudioTestDlg::OnSelchangeComboInputChannel() 
{
	int nSel = m_cInputChannels.GetCurSel();
	if (nSel != CB_ERR)
	{
		IAMAudioInputMixer* pAMaim = (IAMAudioInputMixer*)m_cInputChannels.GetItemData(nSel);
		pAMaim->put_Enable(TRUE);
		double dLevel;
		HRESULT hr = pAMaim->get_MixLevel(&dLevel);
		if (SUCCEEDED(hr) && dLevel < 0.5)
		{
			dLevel = 1.0;
			pAMaim->put_MixLevel(dLevel);
		}
	}	
}

void CAudioTestDlg::ReleaseGraphFilters(BOOL bCaptureToo)
{
	if (m_pGB)
	{
		HRESULT       hr;
		IEnumFilters *pEnum = NULL;
		IBaseFilter  *pFilter = NULL;
		ULONG         cFetched;
		CPtrList      Filters;

		if (m_pMC)
		{
			m_pMC->StopWhenReady();
		}

		hr = m_pGB->EnumFilters(&pEnum);
		if (SUCCEEDED(hr))
		{
			while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
			{
				if (pFilter == m_pCapture)
				{
					RELEASE_OBJECT(pFilter);
				}
				else
				{
					Filters.AddTail(pFilter);
				}
			}
			pEnum->Release();
		}

		if (bCaptureToo)
		{
			ResetInputChannels();
			if (m_pCapture) m_pGB->RemoveFilter(m_pCapture);
			RELEASE_OBJECT(m_pCapture);
		}

		while (Filters.GetCount())
		{
			pFilter = (IBaseFilter*)Filters.RemoveHead();
			m_pGB->RemoveFilter(pFilter);
			RELEASE_OBJECT(pFilter);
		}
	}
}
