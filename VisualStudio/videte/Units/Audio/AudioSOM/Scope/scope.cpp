//------------------------------------------------------------------------------
// File: Scope.cpp
//
// Desc: DirectShow sample code - illustration of an audio oscilloscope.  It
//       shows the waveform graphically as the audio is received by the filter.
//       The filter is a renderer that can be placed wherever the normal
//       runtime renderer goes.  It has a single input pin that accepts a 
//       number of different audio formats and renders the data appropriately.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//
// Summary
//
// This is an audio oscilloscope renderer - we are basically an audio renderer.
// When we are created we also create a class to handle the scope window
// whose constructor creates a worker thread; when it is destroyed it will
// also terminate the worker thread. On that worker thread a window is handled
// that shows the audio waveform for data sent to us. The data is kept
// in a circular buffer that loops when sufficient data has been received.
// We support a number of different audio formats such as 8-bit mode and stereo.
//
//
// Demonstration Instructions
//
// (To really sure of this demonstration the machine must have a sound card)
//
// Start GraphEdit, which is available in the SDK DXUtils folder. Drag and drop
// an MPEG, AVI or MOV file into the tool and it will be rendered. Then go to
// the filters in the graph and find the filter (box) titled "Audio Renderer"
// This is the filter we will be replacing with this oscilloscope renderer.
// Then click on the box and hit DELETE. After that go to the Graph menu and
// select "Insert Filters", from the dialog box that pops up find and select
// "Oscilloscope", then dismiss the dialog. Back in the graph layout find the
// output pin of the filter that was connected to the input of the audio
// renderer you just deleted, right click and select "Render". You should
// see it being connected to the input pin of the oscilloscope you inserted
//
// Click Run on GraphEdit and you'll see a waveform for the audio soundtrack...
//
//
// Files
//
// icon1.ico            The icon for the oscilloscope window
// resource.h           Microsoft Visual C++ generated file
// scope.cpp            The main filter and window implementations
// scope.def            What APIs the DLL imports and exports
// scope.h              Window and filter class definitions
// scope.mak            Visual C++ generated makefile
// scope.rc             Dialog box template for our window
//
//
// Base classes we use
//
// CBaseInputPin        A generic input pin we use for the filter
// CCritSec             A wrapper class around a critical section
// CBaseFilter          The generic DirectShow filter object
//
//

#include <stdio.h>
#include <streams.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <initguid.h>
#include <wxdebug.h>
#include <mmreg.h>
#define _USE_MATH_DEFINES 1
#include <math.h>
#include "scope.h"
#include "resource.h"
#include ".\scope.h"

#define DONT_USE_ETSBIND 1
#include "CEtsMathClass.h"

#define MIN_FFT_ORDER 5    // 2^5  = 64
#define MAX_FFT_ORDER 15   // 2^16 = 65536

#define MODE_BOTH    0
#define MODE_LEFT    1
#define MODE_RIGHT   2
#define MODE_X_Y     3
#define MODE_Y_X     4
// Setup data


#define REINIT_FFT          0x00000001
#define REINIT_MODE         0x00000002
#define REINIT_FFT_FILTER   0x00000004
#define REINIT_ALL          (REINIT_FFT|REINIT_MODE|REINIT_FFT_FILTER)

#define TBU_SEND_POS  9

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Audio,           // Major type
    &MEDIASUBTYPE_NULL          // Minor type
};


const AMOVIESETUP_PIN sudPins  =
{
    L"Input",                   // Pin string name
    FALSE,                      // Is it rendered
    FALSE,                      // Is it an output
    FALSE,                      // Allowed zero pins
    FALSE,                      // Allowed many
    &CLSID_NULL,                // Connects to filter
    L"Output",                  // Connects to pin
    1,                          // Number of pins types
    &sudPinTypes } ;            // Pin information


const AMOVIESETUP_FILTER sudScope =
{
    &CLSID_Scope,               // Filter CLSID
    L"Oscilloscope",            // String name
    MERIT_DO_NOT_USE,           // Filter merit
    1,                          // Number pins
    &sudPins                    // Pin details
};


// List of class IDs and creator functions for class factory

CFactoryTemplate g_Templates []  = {
    { L"Oscilloscope"
    , &CLSID_Scope
    , CScopeFilter::CreateInstance
    , NULL
    , &sudScope }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// ETSMAT
HMODULE g_hETSMathModule = NULL;
PF_REALFFT_FLOAT g_pfnRealFFT;
//
// CreateInstance
//
// This goes in the factory template table to create new instances
//
CUnknown * WINAPI CScopeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    return new CScopeFilter(pUnk, phr);

} // CreateInstance


//
// Constructor
//
// Create the filter, scope window, and input pin
//
#pragma warning(disable:4355 4127)


CScopeFilter::CScopeFilter(LPUNKNOWN pUnk,HRESULT *phr) :
    CBaseFilter(NAME("Oscilloscope"), pUnk, (CCritSec *) this, CLSID_Scope),
    m_Window(NAME("Oscilloscope"), this, phr)
{
    ASSERT(phr);

    // Create the single input pin
    m_pInputPin = new CScopeInputPin(this,phr,L"Scope Input Pin");
    if(m_pInputPin == NULL)
    {
        if (phr)
            *phr = E_OUTOFMEMORY;
    }
} // (Constructor)


//
// Destructor
//
CScopeFilter::~CScopeFilter()
{
    // Delete the contained interfaces

    ASSERT(m_pInputPin);
    delete m_pInputPin;
    m_pInputPin = NULL;

} // (Destructor)


//
// GetPinCount
//
// Return the number of input pins we support
//
int CScopeFilter::GetPinCount()
{
    return 1;

} // GetPinCount


//
// GetPin
//
// Return our single input pin - not addrefed
//
CBasePin *CScopeFilter::GetPin(int n)
{
    // We only support one input pin and it is numbered zero

    ASSERT(n == 0);
    if(n != 0)
    {
        return NULL;
    }

    return m_pInputPin;

} // GetPin


//
// JoinFilterGraph
//
// Show our window when we join a filter graph
//   - and hide it when we are annexed from it
//
STDMETHODIMP CScopeFilter::JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName)
{
    HRESULT hr = CBaseFilter::JoinFilterGraph(pGraph, pName);
    if(FAILED(hr))
    {
        return hr;
    }

    // Hide or show the scope as appropriate

    if(pGraph == NULL)
    {
        m_Window.InactivateWindow();
    }
    else
    {
        m_Window.ActivateWindow();
    }

    return hr;

} // JoinFilterGraph


//
// Stop
//
// Switch the filter into stopped mode.
//
STDMETHODIMP CScopeFilter::Stop()
{
    CAutoLock lock(this);

    if(m_State != State_Stopped)
    {
        // Pause the device if we were running
        if(m_State == State_Running)
        {
            HRESULT hr = Pause();
            if(FAILED(hr))
            {
                return hr;
            }
        }

        DbgLog((LOG_TRACE,1,TEXT("Stopping....")));

        // Base class changes state and tells pin to go to inactive
        // the pin Inactive method will decommit our allocator which
        // we need to do before closing the device

        HRESULT hr = CBaseFilter::Stop();
        if(FAILED(hr))
        {
            return hr;
        }
    }

    return NOERROR;

} // Stop


//
// Pause
//
// Override Pause to stop the window streaming
//
STDMETHODIMP CScopeFilter::Pause()
{
    CAutoLock lock(this);

    // Check we can PAUSE given our current state

    if(m_State == State_Running)
    {
        m_Window.StopStreaming();
    }

    // tell the pin to go inactive and change state
    return CBaseFilter::Pause();

} // Pause


//
// Run
//
// Overriden to start the window streaming
//
STDMETHODIMP CScopeFilter::Run(REFERENCE_TIME tStart)
{
    CAutoLock lock(this);
    HRESULT hr = NOERROR;
    FILTER_STATE fsOld = m_State;

    // This will call Pause if currently stopped

    hr = CBaseFilter::Run(tStart);
    if(FAILED(hr))
    {
        return hr;
    }

    m_Window.ActivateWindow();

    if(fsOld != State_Running)
    {
        m_Window.StartStreaming();
    }

    return NOERROR;

} // Run


//
// Constructor
//
CScopeInputPin::CScopeInputPin(CScopeFilter *pFilter,
                               HRESULT *phr,
                               LPCWSTR pPinName) :
    CBaseInputPin(NAME("Scope Input Pin"), pFilter, pFilter, phr, pPinName)
{
    m_pFilter = pFilter;

} // (Constructor)


//
// Destructor does nothing
//
CScopeInputPin::~CScopeInputPin()
{
} // (Destructor)


//
// BreakConnect
//
// This is called when a connection or an attempted connection is terminated
// and allows us to reset the connection media type to be invalid so that
// we can always use that to determine whether we are connected or not. We
// leave the format block alone as it will be reallocated if we get another
// connection or alternatively be deleted if the filter is finally released
//
HRESULT CScopeInputPin::BreakConnect()
{
    // Check we have a valid connection

    if(m_mt.IsValid() == FALSE)
    {
        // Don't return an error here, because it could lead to 
        // ASSERT failures when rendering media files in GraphEdit.
        return S_FALSE;
    }

    m_pFilter->Stop();

    // Reset the CLSIDs of the connected media type

    m_mt.SetType(&GUID_NULL);
    m_mt.SetSubtype(&GUID_NULL);
    return CBaseInputPin::BreakConnect();

} // BreakConnect


//
// CheckMediaType
//
// Check that we can support a given proposed type
//
HRESULT CScopeInputPin::CheckMediaType(const CMediaType *pmt)
{
    CheckPointer(pmt,E_POINTER);

    WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pmt->Format();

    if(pwfx == NULL)
        return E_INVALIDARG;

    // Reject non-PCM Audio type

    if(pmt->majortype != MEDIATYPE_Audio)
    {
        return E_INVALIDARG;
    }

    if(pmt->formattype != FORMAT_WaveFormatEx)
    {
        return E_INVALIDARG;
    }

    if(   pwfx->wFormatTag != WAVE_FORMAT_PCM
       && pwfx->wFormatTag != WAVE_FORMAT_IEEE_FLOAT)
    {
        return E_INVALIDARG;
    }

    return NOERROR;

} // CheckMediaType


//
// SetMediaType
//
// Actually set the format of the input pin
//
HRESULT CScopeInputPin::SetMediaType(const CMediaType *pmt)
{
   CheckPointer(pmt,E_POINTER);
   CAutoLock lock(m_pFilter);

   // Pass the call up to my base class

   HRESULT hr = CBaseInputPin::SetMediaType(pmt);
   if(SUCCEEDED(hr))
   {
      WAVEFORMATEX *pwf = (WAVEFORMATEX *) pmt->Format();

      m_pFilter->m_Window.m_nChannels = pwf->nChannels;
      m_pFilter->m_Window.m_nSamplesPerSec = pwf->nSamplesPerSec;
      m_pFilter->m_Window.m_nBitsPerSample = pwf->wBitsPerSample;
      m_pFilter->m_Window.m_nBlockAlign = pwf->nBlockAlign;

      m_pFilter->m_Window.m_MaxValue = 128;
      m_pFilter->m_Window.m_nIndex = 0;

      if(!m_pFilter->m_Window.AllocWaveBuffers())
         return E_FAIL;

      // Reset the horizontal scroll bar
      m_pFilter->m_Window.SetMediaType();
   }

   return hr;

} // SetMediaType


//
// Active
//
// Implements the remaining IMemInputPin virtual methods
//
HRESULT CScopeInputPin::Active(void)
{
    return NOERROR;

} // Active


//
// Inactive
//
// Called when the filter is stopped
//
HRESULT CScopeInputPin::Inactive(void)
{
    return NOERROR;

} // Inactive


//
// Receive
//
// Here's the next block of data from the stream
//
HRESULT CScopeInputPin::Receive(IMediaSample * pSample)
{
    // Lock this with the filter-wide lock
    CAutoLock lock(m_pFilter);

    // If we're stopped, then reject this call
    // (the filter graph may be in mid-change)
    if(m_pFilter->m_State == State_Stopped)
    {
        return E_FAIL;
    }

    // Check all is well with the base class
    HRESULT hr = CBaseInputPin::Receive(pSample);
    if(FAILED(hr))
    {
        return hr;
    }

    // Send the sample to the video window object for rendering
    return m_pFilter->m_Window.Receive(pSample);

} // Receive

HWND hwndTT = NULL;                 // handle to the ToolTip control
    char strTT[30] = "This is your ToolTip string.";
    char strTT1[30] = "annother tt";

void CreateMyTooltip (HWND hwnd)
{
                 // struct specifying control classes to register
    INITCOMMONCONTROLSEX iccex; 
          // struct specifying info about tool in ToolTip control
    TOOLINFO ti;
    unsigned int uid = 0;       // for ti initialization
    LPTSTR lptstr = strTT;
    RECT rect;                  // for client area coordinates
    //uid = (unsigned int)::GetDlgItem(hwnd, uid);

    /* INITIALIZE COMMON CONTROLS */
    iccex.dwICC = ICC_BAR_CLASSES;
    iccex.dwSize = sizeof(iccex);
    InitCommonControlsEx(&iccex);

    /* CREATE A TOOLTIP WINDOW */
    hwndTT = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS, "",
        WS_POPUP,		
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, g_hInst,
        NULL);

    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = hwnd;
    ti.hinst = g_hInst;
    ti.uId = uid;
    ti.lpszText = lptstr;
    //ti.lpszText = LPSTR_TEXTCALLBACK;
        // ToolTip control will cover the whole window
    GetClientRect (ti.hwnd, &rect);
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;

    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	

    SendMessage(hwndTT, TTM_TRACKACTIVATE, 1, 0);	
} 

double RectFilter(int, int)
{
    return 1.0;
}

double HammingFilter(int N, int n)
{
    return 0.54 -0.46 * cos(2*M_PI*n / (N - 1));
}

double HanningFilter(int N, int n)
{
    return 0.5 *(1 - cos(2*M_PI*n / (N - 1)));
}

double CosineFilter(int N, int n)
{
    return sin(M_PI * n / (N - 1));
}

double GaussFilter(int N, int n)
{
    double sigma = 0.5;
    double d = ((n-(N-1)*0.5) / (sigma*(N-1)*0.5));
    return exp(-0.5*d*d);
}

double sinc(double d)
{
    return sin(d*M_PI) / (d*M_PI);
}

double LanczosFilter(int N, int n)
{
    return sinc(2*n / (N - 1));
}

//
// CScopeWindow Constructor
//
CScopeWindow::CScopeWindow(TCHAR *pName, CScopeFilter *pRenderer,HRESULT *phr) :
   m_hInstance(g_hInst),
   m_pRenderer(pRenderer),
   m_hThread(INVALID_HANDLE_VALUE),
   m_ThreadID(0),
   m_hwndDlg(NULL),
   m_hwnd(NULL),
   m_hwndTBScroll(NULL),
   m_pPoints1(NULL),
   m_pPoints2(NULL),
   m_nPoints(0),
   m_bStreaming(FALSE),
   m_bActivated(FALSE),
   m_LastMediaSampleSize(0),
   m_pFourier(NULL),
   m_pPF1(NULL),
   m_pPF2(NULL),
   m_pnFrq(NULL),
   m_nMode(MODE_BOTH),
   m_nSamplesPerSec(0)
{
   // Create a thread to look after the window

   
   m_FFT_Filter = RectFilter;

   ASSERT(m_pRenderer);
   g_hETSMathModule = LoadLibrary("ETSMATHC.dll");
   if (g_hETSMathModule)
   {
      g_pfnRealFFT = (PF_REALFFT_FLOAT)GetProcAddress(g_hETSMathModule, MAKEINTRESOURCEA(ETSMATHCLASS_RealFFT_F));
   }

   m_hThread = CreateThread(NULL,                  // Security attributes
                           (DWORD) 0,             // Initial stack size
                           WindowMessageLoop,     // Thread start address
                           (LPVOID) this,         // Thread parameter
                           (DWORD) 0,             // Creation flags
                           &m_ThreadID);          // Thread identifier

    // If we couldn't create a thread the whole thing's off

   ASSERT(m_hThread);
   if(m_hThread == NULL)
   {
      *phr = E_FAIL;
      return;
   }
   m_nOrder = 0;
   m_nIndexOld = 0;
    // Wait until the window has been initialised
   m_SyncWorker.Wait();

   m_AvgTime.SetNoOfAvgValues(20);
} // (Constructor)


//
// Destructor
//
CScopeWindow::~CScopeWindow()
{
   // Ensure we stop streaming and release any samples
   StopStreaming();
   InactivateWindow();

   // Tell the thread to destroy the window
   SendMessage(m_hwndDlg, WM_GOODBYE, (WPARAM)0, (LPARAM)0);

   // Make sure it has finished

   ASSERT(m_hThread != NULL);
   WaitForSingleObject(m_hThread,INFINITE);
   CloseHandle(m_hThread);

   SAFE_DELETE(m_pPoints1);
   SAFE_DELETE(m_pPoints2);
   SAFE_DELETE(m_pFourier);
   SAFE_DELETE(m_pPF1);
   SAFE_DELETE(m_pPF2);
   SAFE_DELETE(m_pnFrq);
   if (g_hETSMathModule)
   {
      FreeLibrary(g_hETSMathModule);
   }
} // (Destructor)


//
// ResetStreamingTimes
//
// This resets the latest sample stream times
//
HRESULT CScopeWindow::ResetStreamingTimes()
{
    m_StartSample = 0;
    m_EndSample = 0;

    return NOERROR;

} // ResetStreamingTimes


//
// StartStreaming
//
// This is called when we start running state
//
HRESULT CScopeWindow::StartStreaming()
{
    CAutoLock cAutoLock(this);

    // Are we already streaming

    if(m_bStreaming == TRUE)
    {
        return NOERROR;
    }

    m_bStreaming = TRUE;
    return NOERROR;

} // StartStreaming


//
// StopStreaming
//
// This is called when we stop streaming
//
HRESULT CScopeWindow::StopStreaming()
{
    CAutoLock cAutoLock(this);

    // Have we been stopped already

    if(m_bStreaming == FALSE)
    {
        return NOERROR;
    }

    m_bStreaming = FALSE;
    return NOERROR;

} // StopStreaming


//
// InactivateWindow
//
// Called at the end to put the window in an inactive state
//
HRESULT CScopeWindow::InactivateWindow()
{
    // Has the window been activated
    if(m_bActivated == FALSE)
    {
        return S_FALSE;
    }

    // Now hide the scope window

    ShowWindow(m_hwndDlg,SW_HIDE);
    m_bActivated = FALSE;

    return NOERROR;

} // InactivateWindow


//
// ActivateWindow
//
// Show the scope window
//
HRESULT CScopeWindow::ActivateWindow()
{
    // Has the window been activated
    if(m_bActivated == TRUE)
    {
        return S_FALSE;
    }

    m_bActivated = TRUE;
    ASSERT(m_bStreaming == FALSE);

    ShowWindow(m_hwndDlg,SW_SHOWNORMAL);
    CreateMyTooltip(m_hwndDlg);
    return NOERROR;

} // ActivateWindow


//
// OnClose
//
// This function handles the WM_CLOSE message
//
BOOL CScopeWindow::OnClose()
{
    InactivateWindow();
    return TRUE;

} // OnClose


typedef struct GainEntry_tag
{
    double GainValue;
    TCHAR GainText[8];
} GainEntry;

GainEntry GainEntries[] =
{
    256.,  TEXT("*256"),
    128.,  TEXT("*128"),
    64.,   TEXT("*64"),
    32.,   TEXT("*32"),
    16.,   TEXT("*16"),
    8.,    TEXT("*8"),
    4.,    TEXT("*4"),
    2.,    TEXT("*2"),
    1.,    TEXT("*1"),
    1./2,  TEXT("/2"),
    1./4,  TEXT("/4"),
    1./8,  TEXT("/8"),
    1./16, TEXT("/16"),
    1./32, TEXT("/32"),
    1./64, TEXT("/64"),
    1./128,TEXT("/128"),
    1./256,TEXT("/256"),
    1./512,TEXT("/512"),
};


#define N_GAINENTRIES (sizeof(GainEntries) / sizeof (GainEntries[0]))
#define GAIN_DEFAULT_INDEX 7

typedef struct TBEntry_tag
{
    int TBDivisor;
    TCHAR *TBText;

} TBEntry;
#undef _T

#ifdef _UNICODE
#define _T((X)) L(X)
#define 
#else
#define _T(X) X
#endif
TBEntry Timebases[] =
{
    50000,  _T("2 uS/Div"),
    20000,  _T("5 uS/Div"),
    10000,  _T("10 uS/Div"),
     5000,  _T("20 uS/Div"),
     2000,  _T("50 uS/Div"),
     1000,  _T("100 uS/Div"),
      500,  _T("200 uS/Div"),
      200,  _T("500 uS/Div"),
      100,  _T("1 mS/Div"),
       50,  _T("2 mS/Div"),
       20,  _T("5 mS/Div"),
       10,  _T("10 mS/Div"),
        5,  _T("20 mS/Div"),
        2,  _T("50 mS/Div"),
        1,  _T("100 mS/Div")
};

#define N_TIMEBASES (sizeof(Timebases) / sizeof (Timebases[0]))
#define TIMEBASE_DEFAULT_INDEX 9


//
// SetControlRanges
//
// Set the scroll ranges for all of the vertical trackbars
//
void CScopeWindow::SetControlRanges()
{
    SendMessage(m_hwndLGain, TBM_SETRANGE, TRUE, MAKELONG(0, N_GAINENTRIES - 1));
    SendMessage(m_hwndLGain, TBM_SETPOS, TRUE, (LPARAM) GAIN_DEFAULT_INDEX);
    SetDlgItemText(m_hwndDlg, IDC_L_GAIN_TEXT, GainEntries[m_LGain].GainText);

    SendMessage(m_hwndLOffset, TBM_SETRANGE, TRUE, MAKELONG(0, m_Height - 1));
    SendMessage(m_hwndLOffset, TBM_SETPOS, TRUE, (LPARAM) m_Height / 2);
    SetDlgItemInt(m_hwndDlg, IDC_L_OFFSET_TEXT, -m_LOffset, TRUE);

    SendMessage(m_hwndRGain, TBM_SETRANGE, TRUE, MAKELONG(0, N_GAINENTRIES - 1));
    SendMessage(m_hwndRGain, TBM_SETPOS, TRUE, (LPARAM) GAIN_DEFAULT_INDEX);
    SetDlgItemText(m_hwndDlg, IDC_R_GAIN_TEXT, GainEntries[m_RGain].GainText);

    SendMessage(m_hwndROffset, TBM_SETRANGE, TRUE, MAKELONG(0, m_Height - 1));
    SendMessage(m_hwndROffset, TBM_SETPOS, TRUE, (LPARAM) m_Height / 2);
    SetDlgItemInt(m_hwndDlg, IDC_R_OFFSET_TEXT, -m_ROffset, TRUE);

    SendMessage(m_hwndTimebase, TBM_SETRANGE, TRUE, MAKELONG(0, N_TIMEBASES - 1));
    SendMessage(m_hwndTimebase, TBM_SETPOS, TRUE, (LPARAM) m_nTimebase);
    SetDlgItemText(m_hwndDlg, IDC_TIMEBASE_TEXT, Timebases[m_nTimebase].TBText);

    SendMessage(m_hwndTBLevel, TBM_SETRANGE, TRUE, MAKELONG(-100, 100));
    SendMessage(m_hwndTBLevel, TBM_SETPOS, TRUE, (LPARAM) m_nTriggerLevel);

} // SetControlRanges


void CScopeWindow::SetMediaType()
{
   SetHorizScrollRange();
   UpdateFFTparams(TRUE);
   UpdateControls(REINIT_FFT);
}

//
// SetHorizScrollRange
//
// The horizontal scrollbar handles scrolling through the 1 second circular buffer
//
void CScopeWindow::SetHorizScrollRange()
{
   int PointsToDisplay, nRange;

   if (m_nOrder)
   {
      PointsToDisplay = 1 << m_nOrder;
   }
   else
   {
      PointsToDisplay = m_nPoints / Timebases [m_nTimebase].TBDivisor;
   }

   nRange = m_nPoints - PointsToDisplay - 1;
   SendMessage(m_hwndTBScroll, TBM_SETRANGEMIN, FALSE, (LPARAM)0);
   SendMessage(m_hwndTBScroll, TBM_SETRANGEMAX, FALSE, (LPARAM)nRange);
   SendMessage(m_hwndTBScroll, TBM_SETPAGESIZE, 0, (LPARAM)PointsToDisplay);

   if (m_fFreeze)
   {
      if (m_TBScroll > nRange)
      {
         SendMessage(m_hwndTBScroll, TBM_SETPOS, TRUE, (LPARAM) 0);
         m_TBScroll = nRange;
      }
   }
   else
   {
      SendMessage(m_hwndTBScroll, TBM_SETPOS, TRUE, (LPARAM) nRange);
      m_TBScroll = 0;
   }
   TCHAR szFormat[80];
   switch(m_nBitsPerSample + m_nChannels)
   {
      case 9:
         // Mono, 8-bit
         lstrcpy(szFormat, TEXT("M-8-\0"));
         break;

      case 10:
         // Stereo, 8-bit
         lstrcpy(szFormat, TEXT("S-8-\0"));
         break;

      case 17:
         // Mono, 16-bit
         lstrcpy(szFormat, TEXT("M-16-\0"));
         break;

      case 18:
         // Stereo, 16-bit
         lstrcpy(szFormat, TEXT("S-16-\0"));
         break;
      case 25:
         // Mono, 24-bit
         lstrcpy(szFormat, TEXT("M-24-\0"));
         break;
      case 26:
         // Stereo, 24-bit
         lstrcpy(szFormat, TEXT("S-24-\0"));
         break;
      case 33:
         // Mono, 32-bit
         lstrcpy(szFormat, TEXT("M-32f-\0"));
         break;
      case 34:
         // Stereo, 32-bit
         lstrcpy(szFormat, TEXT("S-32f-\0"));
         break;

      default:
         lstrcpy(szFormat, TEXT(" \0"));
         SetDlgItemText(m_hwndDlg, IDC_FORMAT, szFormat);
         return;

   } // End of format switch

   TCHAR szSamplingFreq[80];
   wsprintf(szSamplingFreq, TEXT("%d\0"), m_nSamplesPerSec);
   lstrcat(szFormat, szSamplingFreq);
   SetDlgItemText(m_hwndDlg, IDC_FORMAT, szFormat);

} // SetHorizScrollRange

void CScopeWindow::UpdateFFTparams(BOOL bAll)
{
   CAutoLock cAutoLock(m_pRenderer);
   SAFE_DELETE(m_pFourier);
   SAFE_DELETE(m_pPF1);
   SAFE_DELETE(m_pPF2);
   if (m_nOrder)
   {
      int i, nSize = 1 << m_nOrder;
      double dFlow = (double)m_nSamplesPerSec / (double)nSize; 
      double dTemp = dFlow * (double)nSize;  // upper Frequency
      double dStep = dFlow;                  // step value 
      dTemp = log10(dTemp*0.5);                  // log10 of upper frq
      dTemp = (double)(m_Width) / dTemp;     // factor for integer steps
      m_pFourier = new float[nSize];
      nSize /= 2;
      m_pPF1 = new POINT[nSize+1];
      m_pPF1[0].x = m_pPF1[0].y = 0;
      for (i=1; i<=nSize; i++)
      {
         m_pPF1[i].x = (LONG)(log10(dFlow)*dTemp+0.5);
         // todo implement linear division
         //m_pPF1[i].x = i;
         dFlow += dStep;
      }
      dFlow -= dStep;
      if(m_nChannels == 2)
      {
         m_pPF2 = new POINT[nSize+1];
         m_pPF2[0].x = m_pPF2[0].y = 0;
         for (i=1; i<=nSize; i++)
         {
            m_pPF2[i].x = m_pPF1[i].x;
         }
      }
   }

   if (bAll)
   {
      double dFrq, dFstep, dTemp, dFmax = m_nSamplesPerSec*0.5;
      int i, nSize = (int)((log(dFmax)-1.0)*10.0);
      dTemp = log10(dFmax);                  // log10 of upper frq

      dTemp = (double)(m_Width) / dTemp;     // factor for integer steps
      SAFE_DELETE(m_pnFrq);
      m_pnFrq = new int[nSize];
      dFstep = dFrq = 10;
      for (i=0; dFrq<dFmax && i<nSize; i++)
      {
         m_pnFrq[i] = (LONG)(log10(dFrq)*dTemp+0.5);
         dFrq += dFstep;
         if ((i+1)%9 == 0)
         {
            dFstep *= 10;
         }
      }
      m_nFrq = i;
   }
}

void CScopeWindow::EnableDlgItem(UINT nID, BOOL bEnable)
{
   EnableWindow(GetDlgItem(m_hwndDlg, nID), bEnable);
}

void CScopeWindow::ShowDlgItem(UINT nID, BOOL bShow)
{
    ShowWindow(GetDlgItem(m_hwndDlg, nID), bShow ? SW_SHOW : SW_HIDE);
}

void CScopeWindow::UpdateControls(DWORD dwReInit)
{
   BOOL bStereo      = m_nChannels >= 2;
   BOOL bTime        = m_nOrder    == 0;
   BOOL bNotLissajous = m_nMode     <  MODE_X_Y;
   int i, n, j, nMode, nMax;
   TCHAR sz[256];

   if (dwReInit & REINIT_FFT)
   {
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT, CB_RESETCONTENT, 0, 0);
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT, CB_ADDSTRING, 0, (LPARAM)"Time");
      n = m_nSamplesPerSec;
      for (nMax=-2; n>0; n>>=1, nMax++);

      if (g_pfnRealFFT)
      {
         for (i=MIN_FFT_ORDER; i<nMax; i++)
         {
            n = 1 << i;
            wsprintf(sz, TEXT("%d"), n);
            SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT, CB_ADDSTRING, 0, (LPARAM)sz);
         }
      }
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT, CB_SETCURSEL, 0, 0);
   }

   if (dwReInit & REINIT_MODE)
   {
      nMode = 0;
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_MODE, CB_RESETCONTENT, 0, 0);
      n = LoadString(m_hInstance, IDS_COMBO_MODE, sz, 255);
      for (i=j=0; i<n; i++)
      {
         if (sz[i]==';')
         {
            sz[i] = 0;
            SendDlgItemMessage(m_hwndDlg, IDC_COMBO_MODE, CB_ADDSTRING, 0, (LPARAM)&sz[j]);
            j = i+1;
            nMode++;
            if (!bTime && nMode == MODE_X_Y)
            {
               break;
            }
         }
      }
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_MODE, CB_SETCURSEL, 0, 0);
   }

   if (dwReInit & REINIT_FFT_FILTER)
   {
      nMode = 0;
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT_FILTER, CB_RESETCONTENT, 0, 0);
      n = LoadString(m_hInstance, IDS_COMBO_FFT_FILTER, sz, 255);
      for (i=j=0; i<n; i++)
      {
         if (sz[i]==';')
         {
            sz[i] = 0;
            SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT_FILTER, CB_ADDSTRING, 0, (LPARAM)&sz[j]);
            j = i+1;
            nMode++;
         }
      }
      SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT_FILTER, CB_SETCURSEL, 0, 0);
   }

   EnableWindow(m_hwndTimebase, bTime);
   EnableDlgItem(IDC_TXT_TIME_BASE, bTime);
   EnableDlgItem(IDC_TIMEBASE_TEXT, bTime);

   ShowDlgItem(IDC_RD_TRIGGER_C1, bTime);
   ShowDlgItem(IDC_RD_TRIGGER_C2, bTime);
   ShowDlgItem(IDC_COMBO_FFT_FILTER, !bTime);
   EnableDlgItem(IDC_COMBO_FFT_FILTER, !bTime);

   EnableWindow(m_hwndTBScroll, m_fFreeze && bNotLissajous);

   EnableDlgItem(IDC_COMBO_MODE, bStereo);
   EnableDlgItem(IDC_TXT_MODE, bStereo);
   EnableDlgItem(IDC_RD_TRIGGER_C2, bStereo);

   EnableDlgItem(IDC_COMBO_FFT, bNotLissajous);
   EnableDlgItem(IDC_TRIGGER, bNotLissajous);
   EnableDlgItem(IDC_CK_TRIGGER_EDGE, bNotLissajous);
   EnableDlgItem(IDC_TB_LEVEL, bNotLissajous);
   EnableDlgItem(IDC_TIMEBASE, bNotLissajous);
}
//
// ProcessHorizScrollCommands
//
// Called when we get a horizontal scroll bar message
//
void CScopeWindow::ProcessHorizScrollCommands(WPARAM wParam, LPARAM lParam)
{
   int pos, max, nOld = m_TBScroll;
   int command = LOWORD(wParam);

   if (command != TB_ENDTRACK &&
      command != TB_THUMBTRACK &&
      command != TB_LINEDOWN &&
      command != TB_LINEUP &&
      command != TB_PAGEUP &&
      command != TB_PAGEDOWN)
   {
      return;
   }

   ASSERT(IsWindow((HWND) lParam));

   pos = (int) SendMessage((HWND) lParam, TBM_GETPOS, 0, 0L);
   max = (int) SendMessage((HWND) lParam, TBM_GETRANGEMAX, 0, 0L);
   if((HWND) lParam == m_hwndTBScroll)
   {
      m_TBScroll = max - pos;
   }
   if (nOld != m_TBScroll)
   {
      if (m_nOrder)
      {
         int  nFFT = 1 << m_nOrder;
         int nIndexFFT = m_nIndex - nFFT - m_TBScroll;
         if (nIndexFFT < 0)
         {
            nIndexFFT += m_nPoints;
         }
         RealFFT_float(nIndexFFT, TRUE);
         if (m_pPoints2)
         {
            RealFFT_float(nIndexFFT, FALSE);
         }

      }
      OnPaint();
   }

} // ProcessHorizScrollCommands


//
// ProcessVertScrollCommands
//
// Called when we get a vertical scroll bar message
//
void CScopeWindow::ProcessVertScrollCommands(WPARAM wParam, LPARAM lParam)
{
   int pos;
   int command = LOWORD(wParam);

   switch(command)
   {
      case TB_ENDTRACK: case TB_THUMBTRACK:
      case TB_LINEDOWN: case TB_LINEUP:
      case TB_PAGEUP:   case TB_PAGEDOWN:
      case TBU_SEND_POS:
         break;
      default:
         return;
   }

   ASSERT(IsWindow((HWND) lParam));

   if (command == TBU_SEND_POS)
   {
      pos = HIWORD(wParam);
      SendMessage((HWND) lParam, TBM_SETPOS, 1, pos);
   }
   else
   {
      pos = (int) SendMessage((HWND) lParam, TBM_GETPOS, 0, 0L);
   }

   if((HWND) lParam == m_hwndLGain)
   {
      m_LGain = pos;
      SetDlgItemText(m_hwndDlg, IDC_L_GAIN_TEXT, GainEntries[m_LGain].GainText);
   }
   else if((HWND) lParam == m_hwndLOffset)
   {
      m_LOffset = pos - m_Height / 2;
      SetDlgItemInt(m_hwndDlg, IDC_L_OFFSET_TEXT, -m_LOffset, TRUE);
   }
   else if((HWND) lParam == m_hwndRGain)
   {
      m_RGain = pos;
      SetDlgItemText(m_hwndDlg, IDC_R_GAIN_TEXT, GainEntries[m_RGain].GainText);
   }
   else if((HWND) lParam == m_hwndROffset)
   {
      m_ROffset = pos - m_Height / 2;
      SetDlgItemInt(m_hwndDlg, IDC_R_OFFSET_TEXT, -m_ROffset, TRUE);
   }
   else if((HWND) lParam == m_hwndTimebase)
   {
      m_nTimebase = pos ;
      SetDlgItemText(m_hwndDlg, IDC_TIMEBASE_TEXT, Timebases[m_nTimebase].TBText);
      SetHorizScrollRange();
   }
   else if((HWND) lParam == m_hwndTBLevel)
   {
      m_nTriggerLevel = -pos;
   }

   OnPaint();

} // ProcessVertScrollCommands


//
// InitialiseWindow
//
// This is called by the worker window thread after it has created the main
// window and it wants to initialise the rest of the owner objects window
// variables such as the device contexts. We execute this function with the
// critical section still locked.
//
HRESULT CScopeWindow::InitialiseWindow()
{
   RECT rR;

   // Initialise the window variables
   m_hwnd = GetDlgItem(m_hwndDlg, IDC_SCOPEWINDOW);

   // Quick sanity check
   ASSERT(m_hwnd != NULL);

   m_fTriggerPosZeroCrossing = 1;
   m_nTimebase     = TIMEBASE_DEFAULT_INDEX;
   m_nTriggerLevel = 0;
   m_fFreeze       = 0;
   m_fAGC          = 0;
   m_fAFC          = 0;
   m_fTriggerChannel = 0;

   m_LGain         = GAIN_DEFAULT_INDEX;
   m_RGain         = GAIN_DEFAULT_INDEX;
   m_LOffset       = 0;
   m_ROffset       = 0;

   m_TBScroll      = 0;

   GetWindowRect(m_hwnd, &rR);
   m_Width = rR.right - rR.left;
   m_Height = rR.bottom - rR.top;

   m_hwndLGain     =   GetDlgItem(m_hwndDlg, IDC_L_GAIN);
   m_hwndLOffset   =   GetDlgItem(m_hwndDlg, IDC_L_OFFSET);

   m_hwndRGain     =   GetDlgItem(m_hwndDlg, IDC_R_GAIN);
   m_hwndROffset   =   GetDlgItem(m_hwndDlg, IDC_R_OFFSET);

   m_hwndTimebase =    GetDlgItem(m_hwndDlg, IDC_TIMEBASE);

   m_hwndTBLevel  =    GetDlgItem(m_hwndDlg, IDC_TB_LEVEL);
   m_hwndTBScroll =    GetDlgItem(m_hwndDlg, IDC_SLIDER_POS);

   SetControlRanges();

   CheckDlgButton(m_hwndDlg, IDC_FREEZE, m_fFreeze); // check state
   CheckDlgButton(m_hwndDlg, IDC_TRIGGER, m_fTriggerPosZeroCrossing != 0);
   CheckDlgButton(m_hwndDlg, IDC_RD_TRIGGER_C1, m_fTriggerChannel == 0);
   CheckDlgButton(m_hwndDlg, IDC_RD_TRIGGER_C2, m_fTriggerChannel != 0);

   UpdateControls(REINIT_ALL);

   m_hPen1 = CreatePen(PS_SOLID, 0, RGB(0, 0xff, 0));
   m_hPen2 = CreatePen(PS_SOLID, 0, RGB(0x40, 0x40, 0xff));
   m_hPenTicks = CreatePen(PS_SOLID, 0, RGB(0, 128, 128));
   m_hPenGrid = CreatePen(PS_DOT, 0, RGB(64, 128, 128));
   m_hPenTrigger = CreatePen(PS_DASHDOTDOT, 0, RGB(64, 128, 128));

   HDC hdc = GetDC(NULL);
   m_hBitmap = CreateCompatibleBitmap(hdc, m_Width, m_Height);
   ReleaseDC(NULL, hdc);

   return NOERROR;

} // InitialiseWindow


//
// UninitialiseWindow
//
// This is called by the worker window thread when it receives a WM_GOODBYE
// message from the window object destructor to delete all the resources we
// allocated during initialisation
//
HRESULT CScopeWindow::UninitialiseWindow()
{
    // Reset the window variables
    DeleteObject(m_hPen1);
    DeleteObject(m_hPen2);
    DeleteObject(m_hPenTicks);
    DeleteObject(m_hPenTrigger);
    DeleteObject(m_hBitmap);

    m_hwnd = NULL;
    return NOERROR;

} // UninitialiseWindow


//
// ScopeDlgProc
//
// The Scope window is actually a dialog box, and this is its window proc.
// The only thing tricky about this is that the "this" pointer to the
// CScopeWindow is passed during the WM_INITDIALOG message and is stored
// in the window user data. This lets us access methods in the class
// from within the dialog.
//
BOOL CALLBACK CScopeWindow::ScopeDlgProcS(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

   // Get the window long that holds our owner pointer
   if (uMsg == WM_INITDIALOG)
   {
      SetWindowLong(hDlg, (DWORD) GWL_USERDATA, (LONG) lParam);
      return TRUE;
   }
   else
   {
      CScopeWindow *pScopeWindow = (CScopeWindow *) GetWindowLong(hDlg, GWL_USERDATA);
#ifdef _DEBUG
      switch(uMsg)
      {
         case WM_PAINT: case WM_CLOSE: case WM_GOODBYE: case WM_ERASEBKGND:
            ASSERT(pScopeWindow != NULL);
         break;
      }
#endif
      return pScopeWindow->ScopeDlgProc(hDlg, uMsg, wParam, lParam);
   }
} // ScopeDlgProc

BOOL CScopeWindow::ScopeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_COMMAND:
         switch(LOWORD(wParam))
         {
               case IDOK:
               case IDCANCEL:
                  EndDialog(hDlg, 0);
                  return TRUE;

               case IDC_FREEZE:
                  m_fFreeze = (BOOL) IsDlgButtonChecked(hDlg,IDC_FREEZE);
                  if (!m_fFreeze)
                  {
                     SendMessage(m_hwndTBScroll, TBM_SETPOS, TRUE, (LPARAM) SendMessage(m_hwndTBScroll, TBM_GETRANGEMAX, 0, 0L));
                     m_TBScroll = 0;
                  }
                  UpdateControls(0);
                  DrawWaveform();
                  break;

               case IDC_TRIGGER:
                  m_fTriggerPosZeroCrossing =
                  (BOOL) IsDlgButtonChecked(hDlg,IDC_TRIGGER);
                  if (m_fTriggerPosZeroCrossing)
                  {
                     m_fTriggerPosZeroCrossing = IsDlgButtonChecked(hDlg,IDC_CK_TRIGGER_EDGE) ? -1 : 1;
                  }
                  DrawWaveform();
                  break;

               case IDC_CK_AGC:
                  m_fAGC = IsDlgButtonChecked(hDlg,IDC_CK_AGC);
                  break;

               case IDC_CK_AFC:
                  m_fAFC = IsDlgButtonChecked(hDlg,IDC_CK_AFC);
                  break;

               case IDC_RD_TRIGGER_C1: case IDC_RD_TRIGGER_C2:
                  m_fTriggerChannel = IsDlgButtonChecked(hDlg, IDC_RD_TRIGGER_C2);
                  DrawWaveform();
                  break;

               case IDC_CK_TRIGGER_EDGE:
                  if (m_fTriggerPosZeroCrossing)
                  {
                     m_fTriggerPosZeroCrossing = IsDlgButtonChecked(hDlg,IDC_CK_TRIGGER_EDGE) ? -1 : 1;
                  }
                  DrawWaveform();
                  break;

               case IDC_COMBO_FFT:
                  if (HIWORD(wParam)==CBN_SELCHANGE)
                  {
                     int nOld = m_nOrder;
                     int nSel = SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT, CB_GETCURSEL, 0, 0);
                     if (nSel == 0) m_nOrder = 0;
                     else
                     {
                        m_nOrder = nSel + MIN_FFT_ORDER;
                     }
                     if (nOld != m_nOrder)
                     {
                        SetHorizScrollRange();
                        UpdateFFTparams(FALSE);
                        UpdateControls(REINIT_MODE);
                     }
                  }break;

               case IDC_COMBO_MODE:
                  if (HIWORD(wParam)==CBN_SELCHANGE)
                  {
                     int nOldMode = m_nMode;
                     m_nMode = SendDlgItemMessage(m_hwndDlg, IDC_COMBO_MODE, CB_GETCURSEL, 0, 0);
                     if (  nOldMode >= MODE_X_Y
                        && m_nMode  <  MODE_X_Y)
                     {
                        int j;
                        for(j=0; j<m_nSamplesPerSec; j++)
                           m_pPoints1[j].x = j;
                     }
                     UpdateControls(0);
                     DrawWaveform();
                  }break;

               case IDC_COMBO_FFT_FILTER:
                  if (HIWORD(wParam)==CBN_SELCHANGE)
                  {
                     int nSel = SendDlgItemMessage(m_hwndDlg, IDC_COMBO_FFT_FILTER, CB_GETCURSEL, 0, 0);
                     switch(nSel)
                     {
                     case 1: m_FFT_Filter = HammingFilter; break;
                     case 2: m_FFT_Filter = HanningFilter; break;
                     case 3: m_FFT_Filter = CosineFilter; break;
                     case 4: m_FFT_Filter = GaussFilter; break;
                     //case 5: m_FFT_Filter = LanczosFilter; break;
                     default:
                     case 0: m_FFT_Filter = RectFilter; break;
                     }
                     DrawWaveform();
                  }break;
               default:
                  break;
         }break;

      case WM_VSCROLL:
         ProcessVertScrollCommands(wParam, lParam);
         break;

      case WM_HSCROLL:
         ProcessHorizScrollCommands(wParam, lParam);
         break;

      case WM_PAINT:
         OnPaint();
         break;
      case WM_ERASEBKGND:
         return 0;
      // We stop WM_CLOSE messages going any further by intercepting them
      // and then setting an abort signal flag in the owning renderer so
      // that it knows the user wants to quit. The renderer can then
      // go about deleting it's interfaces and the window helper object
      // which will eventually cause a WM_DESTROY message to arrive. To
      // make it look as though the window has been immediately closed
      // we hide it and then wait for the renderer to catch us up
      case WM_CLOSE:
         OnClose();
         return (LRESULT) 0;

         // We receive a WM_GOODBYE window message (synchronously) from the
         // window object destructor in which case we do actually destroy
         // the window and complete the process in the WM_DESTROY message

      case WM_GOODBYE:
         UninitialiseWindow();
         PostQuitMessage(FALSE);
         EndDialog(hDlg, 0);
         return (LRESULT) 0;

      default:
         break;
   }

   return (LRESULT) 0;
}


//
// MessageLoop
//
// This is the standard windows message loop for our worker thread. It sits
// in a normal processing loop dispatching messages until it receives a quit
// message, which may be generated through the owning object's destructor
//
HRESULT CScopeWindow::MessageLoop()
{
    MSG Message;        // Windows message structure
    DWORD dwResult;     // Wait return code value

    HANDLE hWait[] = { (HANDLE) m_RenderEvent };

    // Enter the modified message loop

    while(TRUE)
    {
        // We use this to wait for two different kinds of events, the first
        // are the normal windows messages, the other is an event that will
        // be signaled when a sample is ready

        dwResult = MsgWaitForMultipleObjects((DWORD) 1,     // Number events
            hWait,         // Event handle
            FALSE,         // Wait for either
            INFINITE,      // No timeout
            QS_ALLINPUT);  // All messages

        // Has a sample become ready to render
        if(dwResult == WAIT_OBJECT_0)
        {
            DrawWaveform();
        }

        // Process the thread's window message

        while(PeekMessage(&Message,NULL,(UINT) 0,(UINT) 0,PM_REMOVE))
        {
            // Check for the WM_QUIT message
 
            if(Message.message == WM_QUIT)
            {
                return NOERROR;
            }
            switch (Message.message)
            {
              case TTN_GETDISPINFO:
                {
                  NMTTDISPINFO *pdi = ( NMTTDISPINFO *) Message.lParam;
                  pdi->lpszText = "tool tip";
                }
              break;
            }

            // Send the message to the window procedure
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
    }

} // MessageLoop


#pragma warning(disable:4702)

//
// WindowMessageLoop
//
// This creates a window and processes it's messages on a separate thread
//
DWORD __stdcall CScopeWindow::WindowMessageLoop(LPVOID lpvThreadParm)
{
    CScopeWindow *pScopeWindow;     // The owner renderer object

    // Cast the thread parameter to be our owner object
    pScopeWindow = (CScopeWindow *) lpvThreadParm;

    pScopeWindow->m_hwndDlg =
        CreateDialogParam(pScopeWindow->m_hInstance,    // Handle of app instance
        MAKEINTRESOURCE(IDD_SCOPEDIALOG),               // Dialog box template
        NULL,                                           // Handle of owner window
        (DLGPROC) ScopeDlgProcS,                        // Address of dialog procedure
        (LONG) pScopeWindow                             // Initialization value
        );

    if(pScopeWindow->m_hwndDlg != NULL)
    {
        // Initialise the window, then signal the constructor that it can
        // continue and then unlock the object's critical section and
        // process messages

        pScopeWindow->InitialiseWindow();
    }

    pScopeWindow->m_SyncWorker.Set();

    if(pScopeWindow->m_hwndDlg != NULL)
    {
        pScopeWindow->MessageLoop();
    }

    ExitThread(TRUE);
    return TRUE;

} // WindowMessageLoop


#pragma warning(default:4702)


//
// OnPaint
//
// WM_PAINT message
//
BOOL CScopeWindow::OnPaint()
{
    DrawWaveform();
    return TRUE;

} // OnPaint


//
// ClearWindow
//
// Clear the scope to black and draw the center tickmarks
//
void CScopeWindow::ClearWindow(HDC hdc)
{
   int y = m_Height / 2;

   SetMapMode(hdc, MM_TEXT);
   SetWindowOrgEx(hdc, 0, 0, NULL);
   SetViewportOrgEx(hdc, 0, 0, NULL);
   SetBkMode(hdc, TRANSPARENT);

   // Paint the entire window black
   PatBlt(hdc,            // Handle of device context
         (INT) 0,        // x-coord of upper-left corner
         (INT) 0,        // y-coord of upper-left corner
         m_Width,        // Width of rectangle to be filled
         m_Height,       // Height of rectangle to be filled
         BLACKNESS);     // Raster operation code

   // Draw the horizontal line
   SelectObject(hdc, m_hPenTicks);
   MoveToEx(hdc, 0, y, NULL);
   LineTo(hdc, m_Width, y);

   SelectObject(hdc, m_hPenGrid);
   // Draw the tickmarks
   float inc = (float) m_Width / 10;
   int pos, j;
   int TickPoint;

   if (m_nOrder)
   {
      TCHAR sz[16];
      int n = 10;
      SelectObject(hdc, GetWindowFont(m_hwndLGain));
      SetTextColor(hdc, RGB(64, 128, 128));
      for(j = 0; j < m_nFrq; j++)
      {
         TickPoint = m_Height / 2;
         pos = (int) m_pnFrq[j];
         MoveToEx(hdc, pos, y + TickPoint, NULL);
         LineTo(hdc, pos, y - TickPoint);
         if ((j+1)%9 == 0 || j == 0)
         {
            wsprintf(sz, TEXT("%d"), n);
            TextOut(hdc, pos, y, sz, strlen(sz));
            n *= 10;
         }
      }
   }
   else
   {
      for(j = 1; j < 10; j++)
      {
         TickPoint = m_Height / 2;
         pos = (int) (j * inc);
         MoveToEx(hdc, pos, y + TickPoint, NULL);
         LineTo(hdc, pos, y - TickPoint);
      }
   }
   inc = (float) m_Height / 10;
   for(j = 1; j < 10; j++)
   {
      if (j==5) continue;
      pos = (int) (j * inc);
      MoveToEx(hdc, 0, pos, NULL);
      LineTo(hdc, m_Width, pos);
   }
} // ClearWindow


//
// DrawPartialWaveform
//
// Draw a part of the Oscilloscope waveform - IndexStart and IndexEnd
// are pointers into the m_pPoints array (in LOGICAL COORDINATES)
// while ViewpointStart and ViewpointEnd are in SCREEN COORDINATES
//
void CScopeWindow::DrawPartialWaveform(HDC hdc,
                                       int IndexStart,
                                       int IndexEnd,
                                       int ViewportStart,
                                       int ViewportEnd)
{
   int nVPex, nPoints;
   int nViewportWidth = ViewportEnd - ViewportStart;
   int nYorg = 0;
   POINT *pP2 = NULL, *pP = m_pPoints1 + IndexStart;

   // Origin at lower left, x increases up, y increases to right
   SetMapMode(hdc, MM_ANISOTROPIC);
   if (m_nOrder)
   {
      pP = m_pPF1;
      nPoints = (1 << (m_nOrder-1));
      nVPex = IndexEnd - IndexStart;
   }
   else if (m_nMode == MODE_X_Y || m_nMode == MODE_Y_X)
   {
      pP2 = m_pPoints2 + IndexStart;
      nPoints = IndexEnd - IndexStart;
      nVPex = (int) (m_MaxValue*2 / GainEntries[m_RGain].GainValue);
      nYorg = nVPex >> 1;
      IndexStart = -nYorg + MulDiv(nVPex, m_ROffset*2, m_Height);
      nYorg = m_LOffset;
   }
   else
   {
      nVPex = nPoints = IndexEnd - IndexStart;
      ASSERT(IndexStart + nPoints < m_nPoints);
   }

   if (   m_nChannels == 1
       || m_nMode != MODE_RIGHT)
   {
      SetWindowOrgEx(hdc, IndexStart, nYorg, NULL);
      SetWindowExtEx(hdc, nVPex, (int) (m_MaxValue / GainEntries[m_LGain].GainValue), NULL);
      SetViewportExtEx(hdc, nViewportWidth, -m_Height / 2, NULL);
      SetViewportOrgEx(hdc, ViewportStart, m_LOffset + m_Height / 2, NULL);

      SelectObject(hdc, m_hPen1);
      if (m_nMode == MODE_X_Y)
      {
         int i;
         MoveToEx(hdc, pP->y, pP2->y, NULL);
         for (i=1; i<nPoints; i++)
         {
            LineTo(hdc, pP[i].y, pP2[i].y);
         }
      }
      else if (m_nMode == MODE_Y_X)
      {
         int i;
         MoveToEx(hdc, pP2->y, pP->y, NULL);
         for (i=1; i<nPoints; i++)
         {
            LineTo(hdc, pP2[i].y, pP[i].y);
         }
      }
      else if (m_nOrder)
      {
         int i;
         for (i=1; i<nPoints; i++)
         {
            MoveToEx(hdc, pP[i].x, 0, NULL);
            LineTo(hdc, pP[i].x, pP[i].y);
         }
      }
      else
      {
         Polyline(hdc, pP, nPoints + 1);
      }
   }

   if(   m_pPoints2
      && (m_nMode == MODE_RIGHT || m_nMode == MODE_BOTH))
   {
      SetWindowOrgEx(hdc, IndexStart, 0, NULL);
      SetWindowExtEx(hdc, nVPex, (int) (m_MaxValue / GainEntries[m_RGain].GainValue), NULL);
      SetViewportExtEx(hdc, nViewportWidth, -m_Height / 2, NULL);
      SetViewportOrgEx(hdc, ViewportStart, m_ROffset + m_Height / 2, NULL);
      pP = m_nOrder > 0 ? m_pPF1 : (m_pPoints2 + IndexStart);

      SelectObject(hdc, m_hPen2);
      if (m_nOrder)
      {
         int i;
         for (i=1; i<nPoints; i++)
         {
            MoveToEx(hdc, pP[i].x, 0, NULL);
            LineTo(hdc, pP[i].x, pP[i].y);
         }
      }
      else
      {
         Polyline(hdc, pP, nPoints + 1);
      }
   }

   if (   m_fTriggerPosZeroCrossing 
       && !m_fFreeze
       && m_nMode < MODE_X_Y)
   {
      SetViewportOrgEx(hdc, ViewportStart, m_Height / 2, NULL);
      int nLevel =(int) (m_MaxValue*m_nTriggerLevel / (GainEntries[m_LGain].GainValue*100.0));
      SelectObject(hdc, m_hPenTrigger);
      MoveToEx(hdc, IndexStart, nLevel, NULL);
      LineTo(hdc, IndexEnd, nLevel);
   }
} // DrawPartialWaveform


//
// DrawWaveform
//
// Draw the full Oscilloscope waveform
//
void CScopeWindow::DrawWaveform(void)
{
   if(m_pPoints1 == NULL)
      return;

   CAutoLock lock(m_pRenderer);

   HDC hdc = GetWindowDC(m_hwnd);  // WindowDC has clipping region
   HDC hdcT = CreateCompatibleDC(hdc);
   SelectObject(hdcT, m_hBitmap);
   SaveDC(hdcT);
   ClearWindow(hdcT);

   int StartOffset;
   int IndexEdge;
   int IndexStart1=0, IndexEnd1=0;
   int IndexStart2, IndexEnd2;
   int PointsToDisplay=0, PointsToDisplay1=0, PointsToDisplay2;
   int ViewportBreak=0;
   int OffsetTimeMS = 0;
   BOOL fWraps = 0;                // If segment to display wraps around 0

   if (m_nOrder)
   {
      IndexStart2 = 0;
      IndexEnd2   = m_pPF1[(1 << (m_nOrder-1)) - 1].x;
   }
   else if (m_nMode >= MODE_X_Y)
   {
      IndexStart2 = m_nIndexOld;
      IndexEnd2   = m_nIndex;
      fWraps      = (IndexEnd2 < IndexStart2);
      char sz[64];
      wsprintf(sz, "%d -> %d\n", m_nIndexOld, m_nIndex);
      OutputDebugStr(sz);
      if(fWraps)
      {
         IndexStart1 = IndexStart2;
         IndexEnd1 = m_nPoints - 1;
         IndexStart2 = 0;
         IndexEnd2 = m_nIndex;
         PointsToDisplay1 = IndexEnd1 - IndexStart1;
      }
   }
   else
   {
      PointsToDisplay = m_nPoints / Timebases [m_nTimebase].TBDivisor;

      StartOffset = (m_nIndex - 1) - PointsToDisplay;
      if(StartOffset < 0)
         StartOffset += m_nPoints;

      if(   m_fTriggerPosZeroCrossing 
         && m_fFreeze == FALSE
         && m_nMode < MODE_X_Y)
      {
         SearchForPosZeroCrossing(StartOffset, &IndexEdge);
         IndexStart2 = IndexEdge;
      }
      else
      {
         IndexStart2 = StartOffset;
      }

      IndexStart2 -= m_TBScroll;
      if(IndexStart2 < 0 && m_fFreeze)
         IndexStart2 += m_nPoints;

      IndexEnd2 = IndexStart2 + PointsToDisplay;    // can be negative
      if(IndexEnd2 > m_nIndex)
         OffsetTimeMS = MulDiv(m_nIndex + (m_nPoints - IndexEnd2), 1000, m_nSamplesPerSec);
      else
         OffsetTimeMS = MulDiv(m_nIndex - IndexEnd2, 1000, m_nSamplesPerSec);

      fWraps = (IndexEnd2 >= m_nPoints);
      if(fWraps)
      {
         IndexStart1 = IndexStart2;
         IndexEnd1 = m_nPoints - 1;
         IndexStart2 = 0;
         IndexEnd2 -= m_nPoints;
         PointsToDisplay1 = IndexEnd1 - IndexStart1;
      }
   }

   PointsToDisplay2 = IndexEnd2 - IndexStart2;

   if(fWraps)
   {
      ViewportBreak = (int) (m_Width * (float) PointsToDisplay1 / PointsToDisplay);

      // Draw the first section (from the end of the POINT array)
      DrawPartialWaveform(hdcT,
         IndexStart1, IndexEnd1,     // Index start, Index end
         0, ViewportBreak);          // Window start, Window end

      // Draw the second section (from the beginning of the POINT array)
      DrawPartialWaveform(hdcT,
         IndexStart2, IndexEnd2,     // Index start, Index end
         ViewportBreak, m_Width);    // Window start, Window end
   }
   else
   {
      DrawPartialWaveform(hdcT,
         IndexStart2, IndexEnd2,     // Index start, Index end
         0, m_Width);                // Window start, Window end
   }

   SetMapMode(hdcT, MM_TEXT);
   SetWindowOrgEx(hdcT, 0, 0, NULL);
   SetViewportOrgEx(hdcT, 0, 0, NULL);

   BitBlt(hdc,        // Handle of destination device context
         0,          // x-coordinate of upper-left corner
         0,          // y-coordinate of upper-left corner
         m_Width,    // Wwidth of destination rectangle
         m_Height,   // Height of destination rectangle
         hdcT,       // Handle of source device context
         0,          // x-coordinate of source rectangle
         0,          // y-coordinate of source rectangle
         SRCCOPY);   // Raster operation code

   RestoreDC(hdc, -1);
   DeleteDC(hdcT);
   GdiFlush();

   ReleaseDC(m_hwnd, hdc);

   TCHAR szT[80];
   // Show the size of the last buffer received
   wsprintf(szT, TEXT("%d\0"), m_LastMediaSampleSize);
   SetDlgItemText(m_hwndDlg, IDC_BUFSIZE, szT);
   // Show the timestamps
   LONG mSStart;
   LONG mSEnd = m_EndSample.Millisecs();

   CRefTime rt;
   m_pRenderer->StreamTime(rt);

   // Delta is the difference between the last sample received and
   // the current sample playing according to the StreamTime
   LONG mSDelta = mSEnd - rt.Millisecs();
   wsprintf(szT, TEXT("%d.%d\0"), mSDelta / 1000, abs(mSDelta) % 1000);
   SetDlgItemText(m_hwndDlg, IDC_TS_DELTA, szT);

   if (m_fFreeze)
   {
      // Display the begin and end times of the sweep
      mSEnd = OffsetTimeMS;
      mSStart = mSEnd - MulDiv(PointsToDisplay, 1000, m_nSamplesPerSec);

      wsprintf(szT, TEXT("%d.%d\0"), mSStart / 1000, abs(mSStart) % 1000);
      SetDlgItemText(m_hwndDlg, IDC_TS_START, szT);

      wsprintf(szT, TEXT("%d.%d\0"), mSEnd / 1000, abs(mSEnd) % 1000);
      SetDlgItemText(m_hwndDlg, IDC_TS_LAST, szT);
   }
} // DrawWaveform


//
// AllocWaveBuffers
//
// Allocate a 1 second buffer for each channel
// This is only called when the format changes
// Return TRUE if allocations succeed
//
BOOL CScopeWindow::AllocWaveBuffers()
{
   int j;
   TCHAR szT[80];
   SAFE_DELETE(m_pPoints1);
   SAFE_DELETE(m_pPoints2);
   m_nPoints = 0;

   m_nPoints = m_nSamplesPerSec;
   // Show the size of the last buffer received
   wsprintf(szT, TEXT("%d\0"), m_nSamplesPerSec);
   SetDlgItemText(m_hwndDlg, IDC_BUFSIZE, szT);

   m_pPoints1 = new POINT[m_nPoints];
   if(m_pPoints1)
   {
      for(j = 0; j < m_nPoints; j++)
         m_pPoints1[j].x = j;
   }

   if(m_nChannels == 2)
   {
      m_pPoints2 = new POINT[m_nPoints];
      if(m_pPoints2)
      {
         for(j = 0; j < m_nPoints; j++)
               m_pPoints2[j].x = j;
      }
   }

   // Return TRUE if allocations succeeded
   ASSERT((m_pPoints1 != NULL) && ((m_nChannels == 2) ? (m_pPoints2 != NULL) : TRUE));
   return ((m_pPoints1 != NULL) && ((m_nChannels == 2) ? (m_pPoints2 != NULL) : TRUE));
} // AllocWaveBuffers


//
// SearchForPosZeroCrossing
//
// Searches backward for a positive going zero crossing in the waveform
//
void CScopeWindow::SearchForPosZeroCrossing(int StartPoint, int * IndexEdge, int *pnOffset/*=NULL*/)
{
   if(StartPoint < 0)
   {
      StartPoint = m_nPoints + StartPoint;
   }
   int cur, last, j;
   int nEdge = (int) (m_MaxValue*m_nTriggerLevel / (GainEntries[m_LGain].GainValue*100.0));
   POINT * pPoints = m_pPoints1;
   if (m_fTriggerChannel)
   {
      pPoints = m_pPoints2;
   }

   *IndexEdge = StartPoint;

   last = pPoints[StartPoint].y;

   if (m_fTriggerPosZeroCrossing < 0)
   {
      for(j=0; j<m_nPoints; j++)
      {
         if(--StartPoint < 0)
            StartPoint = m_nPoints - 1;

         cur = pPoints[StartPoint].y;
         if(cur > nEdge && last <= nEdge)
         {
            *IndexEdge = StartPoint;
            break;
         }
         last = cur;
      }
   }
   else
   {
      for(j=0; j<m_nPoints; j++)
      {
         if(--StartPoint < 0)
            StartPoint = m_nPoints - 1;

         cur = pPoints[StartPoint].y;
         if(cur < nEdge && last >= nEdge)
         {
            *IndexEdge = StartPoint;
            break;
         }
         last = cur;
      }
   }

} // SearchForPosZeroCrossing


//
// CopyWaveform
//
// Copy the current MediaSample into a POINT array so we can use GDI
// to paint the waveform.  The POINT array contains a 1 second history
// of the past waveform.  The "Y" values are normalized to a range of
// +128 to -127 within the POINT array.
//
void CScopeWindow::CopyWaveform(IMediaSample *pMediaSample)
{
    static const int n7bit  = (1 << 7) - 1;
    static const int n15bit = (1 << 15) - 1;
    static const int n23bit = (1 << 23) - 1;
    struct s24bit
    {
        unsigned long n24b:24;
    };
    BYTE *pWave;                // Pointer to image data
    int  nBytes;
    int  nSamplesPerChan;
    int  nIndexOld = m_nIndex;

    ASSERT(pMediaSample);
    if (!pMediaSample)
        return;

    pMediaSample->GetPointer(&pWave);
    ASSERT(pWave != NULL);

    nBytes = pMediaSample->GetActualDataLength();
    nSamplesPerChan = nBytes / m_nBlockAlign;
    if (m_nMode >= MODE_X_Y)
    {
        m_nIndexOld = m_nIndex;
    }

    switch(m_nBitsPerSample + m_nChannels)
    {
        BYTE * pb;
        WORD * pw;

        case 9:
        {   // Mono, 8-bit
            pb = pWave;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int)*pb++ - n7bit;  // Make zero centered
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        case 10:
        {   // Stereo, 8-bit
            pb = pWave;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int)*pb++ - n7bit; // Make zero centered
                m_pPoints2[m_nIndex].y = (int)*pb++ - n7bit;
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        case 17:
        { // Mono, 16-bit
            pw = (WORD *) pWave;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int) ((short) *pw++);
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        case 18:
        { // Stereo, 16-bit
            pw = (WORD *)pWave;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int) ((short) *pw++);
                m_pPoints2[m_nIndex].y = (int) ((short) *pw++);
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        case 25:
        { // Mono, 24-bit
            pb = pWave;
            int n;
            while(nSamplesPerChan--)
            {
                n = (int)((s24bit*)pb)->n24b - n23bit;
                m_pPoints1[m_nIndex].y = n >> 8;
                pb+=3;
                if(++m_nIndex == m_nSamplesPerSec)
                    m_nIndex = 0;
            }
            break;
        }

        case 26:
        { // Stereo, 24-bit
            int n;
            pb = pWave;
            while(nSamplesPerChan--)
            {
                n = (int)((s24bit*)pb)->n24b - n23bit;
                m_pPoints1[m_nIndex].y = n >> 8;
                pb+=3;
                n = (int)((s24bit*)pb)->n24b - n23bit;
                m_pPoints2[m_nIndex].y = n >> 8;
                pb+=3;
                if(++m_nIndex == m_nSamplesPerSec)
                    m_nIndex = 0;
            }
            break;
        }
        case 33:
        { // Mono, 16-bit
            float *pf = (float*) pWave;
            double factor = n15bit;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int)(factor * *pf++);
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        case 34:
        { // Stereo, 16-bit
            float *pf = (float*) pWave;
            double factor = n15bit;
            while(nSamplesPerChan--)
            {
                m_pPoints1[m_nIndex].y = (int)(factor * *pf++);
                m_pPoints2[m_nIndex].y = (int)(factor * *pf++);
                if(++m_nIndex == m_nSamplesPerSec)
                m_nIndex = 0;
            }
            break;
        }

        default:
            ASSERT(0);
            break;

    } // End of format switch

    if (m_fAGC)
    { 
        double dGain, dThreshold = 256.0;
        int i, n1, nSize, n2 = 0;
        int  nMin = 0, nMax = 0;
        int n = sizeof(GainEntries) / sizeof(GainEntry);
        if (nIndexOld < m_nIndex)
        {
            n1 = m_nIndex;
        }
        else
        {
            n1 = m_nSamplesPerSec;
            n2 = m_nIndex;
        }

        for (i=nIndexOld; i<n1; i++)
        {
            nMin = min(nMin, m_pPoints1[i].y);
            nMax = max(nMax, m_pPoints1[i].y);
        }

        for (i=0 ; i<n2; i++)
        {
            nMin = min(nMin, m_pPoints1[i].y);
            nMax = max(nMax, m_pPoints1[i].y);
        }

        nSize = nMax - nMin;
        for (i=0; i<n; i++)
        {
            dGain = GainEntries[i].GainValue * nSize;
            if (dGain < dThreshold)
            {
                if (m_LGain != i)
                {
                    PostMessage(m_hwndDlg, WM_VSCROLL, MAKELONG(TBU_SEND_POS, i), (LPARAM)m_hwndLGain);
                }
                break;
            }
        }

        if (m_nChannels > 1)
        {
            for (i=nIndexOld; i<n1; i++)
            {
                nMin = min(nMin, m_pPoints2[i].y);
                nMax = max(nMax, m_pPoints2[i].y);
            }

            for (i=0 ; i<n2; i++)
            {
                nMin = min(nMin, m_pPoints2[i].y);
                nMax = max(nMax, m_pPoints2[i].y);
            }
            nSize = nMax - nMin;
            for (i=0; i<n; i++)
            {
                dGain = GainEntries[i].GainValue * nSize;
                if (dGain < dThreshold)
                {
                    if (m_RGain != i)
                    {
                        PostMessage(m_hwndDlg, WM_VSCROLL, MAKELONG(TBU_SEND_POS, i), (LPARAM)m_hwndRGain);
                    }
                    break;
                }
            }
        }
    }

    if (m_fAFC)
    {
        int nP1, nP2, j, n = 0, nFrq;
        //int j, n = 0, nFrq;
        SearchForPosZeroCrossing(m_nIndex, &nP1);
        SearchForPosZeroCrossing(nP1-1, &nP2);
        nP1 += m_nPoints;
        nP2 += m_nPoints;
        n = nP1 - nP2;
        if (n>0)
        {
            m_AvgTime.AddValue(n);
            int nAvg = (int)m_AvgTime.GetAverageT();
            if (abs(n - nAvg) > 4)
            {
                n = nAvg;
            }
            n = nAvg;
        }
        if (n > 0)
        {
          nFrq = m_nSamplesPerSec / n; 
          n = sizeof(Timebases) / sizeof(TBEntry);
          for (j=0; j<n; j++)
          {
              if (nFrq > Timebases[j].TBDivisor)
              {
                  if (j != m_nTimebase)
                  {
                      PostMessage(m_hwndDlg, WM_VSCROLL, MAKELONG(TBU_SEND_POS, j), (LPARAM)m_hwndTimebase);
                  }
                  break;
              }
          }
        }
    }
} // CopyWaveform


//
// Receive
//
// Called when the input pin receives another sample.
// Copy the waveform to our circular 1 second buffer
//
HRESULT CScopeWindow::Receive(IMediaSample *pSample)
{
    CheckPointer(pSample,E_POINTER);
    CAutoLock cAutoLock(this);
    ASSERT(pSample != NULL);

    // Has our UI been frozen

    if(m_fFreeze)
    {
        return NOERROR;
    }

    pSample->GetTime((REFERENCE_TIME*)&m_StartSample, (REFERENCE_TIME*)&m_EndSample);

    // Ignore zero-length samples
    if((m_LastMediaSampleSize = pSample->GetActualDataLength()) == 0)
        return NOERROR;

    if(m_bStreaming == TRUE)
    {
         CopyWaveform(pSample);     // Copy data to our circular buffer
         if (m_nOrder)
         {
            int  nFFT = 1 << m_nOrder;
            if (abs(m_nIndex - m_nIndexOld) >= nFFT)
            {
               int nIndexFFT = m_nIndex - nFFT - 1;
               if(m_fTriggerPosZeroCrossing && !m_fFreeze)
               {
                  SearchForPosZeroCrossing(nIndexFFT, &nIndexFFT);
               }
               if (nIndexFFT < 0)
               {
                  nIndexFFT += m_nPoints;
               }
               RealFFT_float(nIndexFFT, TRUE);
               if (m_pPoints2)
               {
                  RealFFT_float(nIndexFFT, FALSE);
               }
               m_nIndexOld = m_nIndex;
               SetEvent(m_RenderEvent);    // Set an event to display the
            }
         }
         else
         {
            SetEvent(m_RenderEvent);    // Set an event to display the
         }
        // new data on another thread
        return NOERROR;
    }

    return NOERROR;

} // Receive

void CScopeWindow::RealFFT_float(int nStart, BOOL bLeft)
{
   if (g_pfnRealFFT)
   {
      int j, k, n, nSamples = m_nSamplesPerSec;;
      float *pdFourier = m_pFourier;
      POINT *p = bLeft ? m_pPoints1 : m_pPoints2;
      n    = 1<<m_nOrder;
      for (j=0, k=nStart; j<n; j++)
      {
         pdFourier[j] = (float)(m_FFT_Filter(n, j) * p[k++].y);
         // TODO! Apply filter here (Hamming, Hanning)
         if (k==nSamples)
         {
            k = 0;
         }
      }

      g_pfnRealFFT(0, m_nOrder, pdFourier, NULL, NULL);

      n >>= 1;
      p = bLeft ? m_pPF1 : m_pPF2;
      for (j=1, k=0; j<=n; j++, k+=2)
      {
         p[j].y = (LONG)(hypot(pdFourier[k], pdFourier[k+1])+0.5);
      }
   }
}

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

//
// DllRegisterServer
//
// Handles DLL registry
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);

} // DllUnregisterServer


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
   return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

