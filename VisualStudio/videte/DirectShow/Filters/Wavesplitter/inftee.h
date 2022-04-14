//------------------------------------------------------------------------------
// File: InfTee.h
//
// Desc: DirectShow sample code - header file for infinite tee filter.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "iwavesplitter.h"
#include "tchar.h"

//#ifndef __INFTEE__
//#define __INFTEE__

class CWaveSplitter;
class CWaveOutputPin;

// class for the Tee filter's Input pin

class CWaveInputPin : public CBaseInputPin, public IWaveSplitter
{
   friend class CWaveOutputPin;
   CWaveSplitter *m_pSplit;                  // Main filter object
   BOOL m_bInsideCheckMediaType;  // Re-entrancy control

public:

   // Constructor and destructor
   CWaveInputPin(TCHAR *pObjName,
                CWaveSplitter *pTee,
                HRESULT *phr,
                LPCWSTR pPinName);

   DECLARE_IUNKNOWN;

#ifdef DEBUG
   ~CWaveInputPin();
#endif
   STDMETHODIMP NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly);

   // Pass through calls downstream
   STDMETHODIMP EndOfStream();
   STDMETHODIMP BeginFlush();
   STDMETHODIMP EndFlush();
   STDMETHODIMP NewSegment(
                   REFERENCE_TIME tStart,
                   REFERENCE_TIME tStop,
                   double dRate);

   // Handles the next block of data from the stream
   STDMETHODIMP Receive(IMediaSample *pSample);

	 
   // Used to check the input pin connection
   HRESULT CheckMediaType(const CMediaType *pmt);
   HRESULT SetMediaType(const CMediaType *pmt);
   HRESULT BreakConnect();

   // Reconnect outputs if necessary at end of completion
   virtual HRESULT CompleteConnect(IPin *pReceivePin);


	// IWaveSplitter interface functions
   STDMETHODIMP SetThreshold(float);
	STDMETHODIMP SetMediaSampleCheckFnc(MEDIASAMPLE_FUNCTION, long);
	STDMETHODIMP SetPinNumber(int nPin, int nNumber);
   STDMETHODIMP SetDeliver(int);

private:
   STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
};


// Class for the Tee filter's Output pins.

class CWaveOutputPin : public CBaseOutputPin
{
   friend class CWaveInputPin;
   friend class CWaveSplitter;

   CWaveSplitter *m_pSplit;                  // Main filter object pointer
   IUnknown    *m_pPosition;      // Pass seek calls upstream
   BOOL  m_bHoldsSeek;             // Is this the one seekable stream
   COutputQueue *m_pOutputQueue;  // Streams data to the peer pin
   BOOL  m_bInsideCheckMediaType;  // Re-entrancy control
//   LONG  m_cOurRef;                // We maintain reference counting
	IMemAllocator *m_pIMemAllocator;
	CMemAllocator *m_pAllocator;
	int   m_nPinNumber;
	bool  m_bSynchronized;
public:

   // Constructor and destructor

   CWaveOutputPin(TCHAR *pObjName,
                   CWaveSplitter *pTee,
                   HRESULT *phr,
                   LPCWSTR pPinName,
                   INT PinNumber);


    ~CWaveOutputPin();

   // Override to expose IMediaPosition
   STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppvoid);
   // Override since the life time of pins and filters are not the same
   STDMETHODIMP_(ULONG) NonDelegatingAddRef();
   STDMETHODIMP_(ULONG) NonDelegatingRelease();

   // Override to enumerate media types
   STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **ppEnum);

   // Check that we can support an output type
   HRESULT CheckMediaType(const CMediaType *pmt);
   HRESULT SetMediaType(const CMediaType *pmt);

   // Negotiation to use our input pins allocator
   HRESULT DecideAllocator(IMemInputPin *pPin, IMemAllocator **ppAlloc);
   HRESULT DecideBufferSize(IMemAllocator *pMemAllocator,
                              ALLOCATOR_PROPERTIES * ppropInputRequest);

   // Used to create output queue objects
   HRESULT Active();
   HRESULT Inactive();

   // Overriden to create and destroy output pins
   HRESULT CompleteConnect(IPin *pReceivePin);

   // Overriden to pass data to the output queues
   HRESULT Deliver(IMediaSample *pMediaSample);
   HRESULT DeliverEndOfStream();
   HRESULT DeliverBeginFlush();
   HRESULT DeliverEndFlush();
   HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);


   // Overriden to handle quality messages
   STDMETHODIMP Notify(IBaseFilter *pSender, Quality q);
};


// Class for the Tee filter

class CWaveSplitter: public CCritSec, public CBaseFilter
{
   friend class CWaveInputPin;
   friend class CWaveOutputPin;

public:
   static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
   CWaveSplitter(TCHAR *pName,LPUNKNOWN pUnk,HRESULT *hr);
   virtual ~CWaveSplitter();

   // Function needed for the class factory

   // Send EndOfStream if no input connection
	STDMETHODIMP SetSyncSource(IReferenceClock *);
   STDMETHODIMP Run(REFERENCE_TIME tStart);
   STDMETHODIMP Pause();
   STDMETHODIMP Stop();

   CBasePin *GetPin(int n);
   int GetPinCount();

protected:
    // The following manage the list of output pins
   void InitOutputPinsList(bool bDelete);
   CWaveOutputPin *GetPinNFromList(int n);
   CWaveOutputPin *CreateNextOutputPin(CWaveSplitter *pTee);
   void DeleteOutputPin(CWaveOutputPin *pPin);
   int GetNumFreePins();

private:
    // Let the pins access our internal state
   typedef CGenericList <CWaveOutputPin> COutputList;

   CWaveInputPin m_Input;			  // Declare an input pin.
   INT m_NumOutputPins;            // Current output pin count
   COutputList m_OutputPinsList;   // List of the output pins
   INT m_NextOutputPinNumber;      // Increases monotonically.
   LONG m_lCanSeek;                // Seekable output pin
   IMemAllocator *m_pAllocator;    // Allocator from our input pin
	short m_nThresholdLevel;		  // Schwellwert in Prozent
	bool  m_bActivated;				  // Filter aktiviert
	int   m_nDeliver;					  // Samples weiterleiten
	
	MEDIASAMPLE_FUNCTION m_pCheckFnc;// Checkfunction for the delivered Samples
	long  m_lParamCheckFnc;

private:
	static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam);
	static HWND          gm_hWndThread;
	static const _TCHAR  gm_sModuleName[];
public:
	static void Trace(LPCTSTR sFormat, ...);
};

//#endif // __INFTEE__

