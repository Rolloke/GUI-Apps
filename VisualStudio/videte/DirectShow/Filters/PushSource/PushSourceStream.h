//------------------------------------------------------------------------------
// File: pushsourcestream.h
//

#ifndef __CPUSHSOURCESTREAM__
#define __CPUSHSOURCESTREAM__

class CPushSource;

class CPushSourceStream :
	public CSourceStream,
	public CBaseStreamControl,
	public IAMPushSource,
	public IAMStreamConfig,
	public IKsPropertySet
{
	friend class CPushSource;

public:

   CPushSourceStream(TCHAR *pObjectName,
                         HRESULT *phr,
                         CPushSource*pms,
                         LPCWSTR pName, const GUID*, long);
#ifdef UNICODE
   CPushSourceStream(CHAR *pObjectName,
                         HRESULT *phr,
                         CPushSource*pms,
                         LPCWSTR pName, const GUID*, long);
#endif
   virtual ~CPushSourceStream(void);  // virtual destructor ensures derived class destructors are called too.


	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    HRESULT DestroySourceThread(void);
protected:

    CPushSource*m_pFilter;	// The parent of this stream


    // Override this to provide the worker thread a means
    // of processing a buffer
    virtual HRESULT FillBuffer(IMediaSample *pSamp);

    // Called as the thread is created/destroyed - use to perform
    // jobs such as start/stop streaming mode
    // If OnThreadCreate returns an error the thread will exit.
    virtual HRESULT OnThreadCreate(void);
    virtual HRESULT OnThreadDestroy(void);
    virtual HRESULT OnThreadStartPlay(void);


    // ask for buffers of the size appropriate to the agreed media type.
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties);
    virtual HRESULT InitAllocator(IMemAllocator **ppAlloc);
    virtual HRESULT GetDeliveryBuffer(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags);
    virtual HRESULT DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);

    HRESULT CompleteConnect(IPin *pReceivePin);
    HRESULT BreakConnect(void);

//  Worker Thread
public:
    // thread commands
    enum Command {CMD_INIT, CMD_PAUSE, CMD_RUN, CMD_STOP, CMD_EXIT};
    HRESULT Init(void) { return CallWorker(CMD_INIT); }
    HRESULT Exit(void) { return CallWorker(CMD_EXIT); }
    HRESULT Run(void) { return CallWorker(CMD_RUN); }
    HRESULT Pause(void) { return CallWorker(CMD_PAUSE); }
    HRESULT Stop(void) { return CallWorker(CMD_STOP); }

//    void OutputPinNeedsToBeReconnected(void);
	// IAMPushSource interface	
	STDMETHODIMP GetPushSourceFlags(ULONG *pFlags);
	STDMETHODIMP SetPushSourceFlags(ULONG Flags);
	STDMETHODIMP SetStreamOffset(REFERENCE_TIME rtOffset);
	STDMETHODIMP GetStreamOffset(REFERENCE_TIME *prtOffset);
	STDMETHODIMP GetMaxStreamOffset(REFERENCE_TIME *prtMaxOffset);
	STDMETHODIMP SetMaxStreamOffset(REFERENCE_TIME rtMaxOffset);
	STDMETHODIMP GetLatency(REFERENCE_TIME *prtLatency);

	// IKsPropertySet interface
	STDMETHODIMP Set(REFGUID guidPropSet,DWORD dwPropID, LPVOID pInstanceData,DWORD cbInstanceData, LPVOID pPropData,DWORD cbPropData);
	STDMETHODIMP Get(REFGUID guidPropSet,DWORD dwPropID,LPVOID pInstanceData,DWORD cbInstanceData,LPVOID pPropData,DWORD cbPropData,DWORD *pcbReturned);
	STDMETHODIMP QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport);
        
	// IAMStreamConfig interface
	STDMETHODIMP GetFormat(AM_MEDIA_TYPE **pmt);
	STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize);
	STDMETHODIMP GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);
	STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt);

	// Own attributes
	IMemAllocator*GetAllocator() { return (IMemAllocator*) m_pAllocator; }

protected:
	Command GetRequest(void) { return (Command) CAMThread::GetRequest(); }
	BOOL    CheckRequest(Command *pCom) { return CAMThread::CheckRequest( (DWORD *) pCom); }

	// override these if you want to add thread commands
	virtual DWORD ThreadProc(void);  		// the thread function

	virtual HRESULT DoBufferProcessingLoop(void);    // the loop executed whilst running

	void FatalError(HRESULT hr);

	// *
	// * AM_MEDIA_TYPE support
	// *

	// If you support more than one media type then override these 2 functions
	HRESULT GetMediaType(CMediaType *pmt);
	virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);  // List pos. 0-n
    virtual HRESULT SetMediaType(const CMediaType *);

	STDMETHODIMP QueryId(LPWSTR * Id);

	ULONG m_lPushSourceFlags;
	REFERENCE_TIME m_rtStreamOffset;
	REFERENCE_TIME m_rtMaxStreamOffset;
	REFERENCE_TIME m_rtLatency;
	LONG				m_lDeliveredSamples;

	GUID				m_guidPIN_CATEGORY;

//    bool m_fReconnectOutputPin;
};

#endif // __CPUSHSOURCESTREAM__
