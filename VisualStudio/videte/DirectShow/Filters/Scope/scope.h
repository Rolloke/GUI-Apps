//------------------------------------------------------------------------------
// File: Scope.h
//
// Desc: DirectShow sample code - header file for audio oscilloscope filter.
//
// Copyright (c) 1992-2002 Microsoft Corporation. All rights reserved.
//------------------------------------------------------------------------------

#pragma once
// { 35919F40-E904-11ce-8A03-00AA006ECB65 }
DEFINE_GUID(CLSID_Scope,
0x35919f40, 0xe904, 0x11ce, 0x8a, 0x3, 0x0, 0xaa, 0x0, 0x6e, 0xcb, 0x65);

class CScopeFilter;
class CScopeWindow;

#define SAFE_DELETE(P) if (P) {delete P; P = NULL;}
// Class supporting the scope input pin

class CScopeInputPin : public CBaseInputPin
{
    friend class CScopeFilter;
    friend class CScopeWindow;

private:

    CScopeFilter *m_pFilter;         // The filter that owns us

public:

    CScopeInputPin(CScopeFilter *pTextOutFilter,
                   HRESULT *phr,
                   LPCWSTR pPinName);
    ~CScopeInputPin();

    // Lets us know where a connection ends
    HRESULT BreakConnect();

    // Check that we can support this input type
    HRESULT CheckMediaType(const CMediaType *pmt);

    // Actually set the current format
    HRESULT SetMediaType(const CMediaType *pmt);

    // IMemInputPin virtual methods

    // Override so we can show and hide the window
    HRESULT Active(void);
    HRESULT Inactive(void);

    // Here's the next block of data from the stream.
    // AddRef it if you are going to hold onto it
    STDMETHODIMP Receive(IMediaSample *pSample);

}; // CScopeInputPin


// This class looks after the management of a window. When the class gets
// instantiated the constructor spawns off a worker thread that does all
// the window work. The original thread waits until it is signaled to
// continue. The worker thread first registers the window class if it
// is not already done. Then it creates a window and sets it's size to
// a default iWidth by iHeight dimensions. The worker thread MUST be the
// one who creates the window as it is the one who calls GetMessage. When
// it has done all this it signals the original thread which lets it
// continue, this ensures a window is created and valid before the
// constructor returns. The thread start address is the WindowMessageLoop
// function. This takes as it's initialisation parameter a pointer to the
// CVideoWindow object that created it, the function also initialises it's
// window related member variables such as the handle and device contexts

// These are the video window styles

const DWORD dwTEXTSTYLES = (WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);
const DWORD dwCLASSSTYLES = (CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT | CS_OWNDC);
const LPTSTR RENDERCLASS = TEXT("OscilloscopeWindowClass\0");
const LPTSTR TITLE = TEXT("Oscilloscope\0");

const int iWIDTH = 320;             // Initial window width
const int iHEIGHT = 240;            // Initial window height
const int WM_GOODBYE (WM_USER + 2); // Sent to close the window

class CScopeWindow : public CCritSec
{
   friend class CScopeInputPin;
   friend class CScopeFilter;

private:

   HINSTANCE m_hInstance;          // Global module instance handle
   CScopeFilter *m_pRenderer;      // The owning renderer object

   HWND m_hwndDlg;                 // Handle for our dialog
   HWND m_hwnd;                    // Handle for the graph window
   HPEN m_hPen1;                   // We use two pens for drawing
   HPEN m_hPen2;                   //  the waveforms in the window
   HPEN m_hPenTicks;               // Used to draw ticks at bottom
   HPEN m_hPenGrid;                // Used to draw grid
   HPEN m_hPenTrigger;             // Used to draw trigger level line
   HBITMAP m_hBitmap;              // Draw all waveforms into here
   HANDLE m_hThread;               // Our worker thread
   DWORD m_ThreadID;               // Worker thread ID

   CAMEvent m_SyncWorker;          // Synchronise with worker thread
   CAMEvent m_RenderEvent;         // Signals sample to render

   LONG m_Width;                   // Client window width
   LONG m_Height;                  // Client window height
   BOOL m_bActivated;              // Has the window been activated

   CRefTime m_StartSample;         // Most recent sample start time
   CRefTime m_EndSample;           // And it's associated end time

   BOOL m_bStreaming;              // Are we currently streaming
   POINT *m_pPoints1;              // Array of points to graph Channel1
   POINT *m_pPoints2;              // Array of points to graph Channel2
   float *m_pFourier;
   POINT *m_pPF1;                  // Array of points to graph FFT Channel1
   POINT *m_pPF2;                  // Array of points to graph FFT Channel2
   int   *m_pnFrq;                 // Array of frequency positions

   int    m_nPoints;               // Size of m_pPoints[1|2]
   int    m_nIndex;                // Index of last sample written
   int    m_nIndexOld;             // Index of last Index position after processing
   int    m_LastMediaSampleSize;   // Size of last MediaSample
   int    m_nFrq;                  // Array size of frequency positions

   int m_nChannels;                // number of active channels
   int m_nSamplesPerSec;           // Samples per second
   int m_nBitsPerSample;           // Number bits per sample
   int m_nBlockAlign;              // Alignment on the samples
   int m_MaxValue;                 // Max Value of the POINTS array

   int m_LGain;                    // Left channel control settings
   int m_LOffset;                  //  And likewise its offset
   int m_RGain;                    // Right channel control settings
   int m_ROffset;                  //  And likewise its offset
   int m_nTimebase;                // Timebase settings
   int m_nTriggerLevel;            // Trigger level
   int m_nMode;                    // 2 channel modes

   BOOL m_fFreeze;                 // Flag to signal we're UI frozen
   BOOL m_fAGC;                    // Flag Automatic Gain Control
   BOOL m_fAFC;                    // Flag Automatic Time domain control
   BOOL m_fTriggerChannel;         // Trigger channel selector
   int m_TBScroll;                 // Holds position in scroll range
   int m_nOrder;

   // Hold window handles to controls

   HWND m_hwndLGain;
   HWND m_hwndLOffset;
   HWND m_hwndRGain;
   HWND m_hwndROffset;
   HWND m_hwndTimebase;
   HWND m_hwndTBLevel;
   HWND m_hwndTBScroll;

   BOOL m_fTriggerPosZeroCrossing;

   // These create and manage a video window on a separate thread

   HRESULT UninitialiseWindow();
   HRESULT InitialiseWindow();
   HRESULT MessageLoop();
   void    SetMediaType();

   static DWORD __stdcall WindowMessageLoop(LPVOID lpvThreadParm);
   static BOOL CALLBACK ScopeDlgProcS(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
   BOOL ScopeDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   // Maps windows message loop into C++ virtual methods

   // Called when we start and stop streaming
   HRESULT ResetStreamingTimes();

   // Window message handlers
   BOOL OnClose();
   BOOL OnPaint();

   // Draw the waveform
   void ClearWindow(HDC hdc);
   BOOL AllocWaveBuffers(void);
   void SearchForPosZeroCrossing(int StartPoint, int * IndexEdge);
   void CopyWaveform(IMediaSample *pMediaSample);

   void DrawPartialWaveform(HDC hdc,
                           int IndexStart,
                           int IndexEnd,
                           int ViewportStart,
                           int ViewportEnd);

   void DrawWaveform(void);
   void SetControlRanges();
   void SetHorizScrollRange();
   void ProcessVertScrollCommands(WPARAM wParam, LPARAM lParam);
   void ProcessHorizScrollCommands(WPARAM wParam, LPARAM lParam);
   void RealFFT_float(int nStart, BOOL bLeft);
   void UpdateFFTparams(BOOL);
   void UpdateControls(DWORD dwReInit);
   void EnableDlgItem(UINT nID, BOOL bEnable);

public:

   // Constructors and destructors

   CScopeWindow(TCHAR *pName, CScopeFilter *pRenderer, HRESULT *phr);
   virtual ~CScopeWindow();

   HRESULT StartStreaming();
   HRESULT StopStreaming();
   HRESULT InactivateWindow();
   HRESULT ActivateWindow();

   // Called when the input pin receives a sample
   HRESULT Receive(IMediaSample * pIn);

}; // CScopeWindow


// This is the COM object that represents the oscilloscope filter

class CScopeFilter : public CBaseFilter, public CCritSec
{

public:
    // Implements the IBaseFilter and IMediaFilter interfaces

    DECLARE_IUNKNOWN


    STDMETHODIMP Stop();
    STDMETHODIMP Pause();
    STDMETHODIMP Run(REFERENCE_TIME tStart);

public:

    CScopeFilter(LPUNKNOWN pUnk,HRESULT *phr);
    virtual ~CScopeFilter();

    // Return the pins that we support
    int GetPinCount();
    CBasePin *GetPin(int n);

    // This goes in the factory template table to create new instances
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN, HRESULT *);

    STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);

private:

    // The nested classes may access our private state
    friend class CScopeInputPin;
    friend class CScopeWindow;

    CScopeInputPin *m_pInputPin;   // Handles pin interfaces
    CScopeWindow m_Window;         // Looks after the window

}; // CScopeFilter

