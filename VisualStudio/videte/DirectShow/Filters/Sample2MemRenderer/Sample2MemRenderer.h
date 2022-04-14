// Sample2MemRenderer.h: interface for the CSample2MemRenderer class.
// 
//////////////////////////////////////////////////////////////////////
//  CSample2MemRenderer: (A DirectShow (DirectX 8.0) filter)
//
//  Purpose: This filter is a basic Transform Filter to process video
//        
//  Usage: 
//  1.  Register the filter. 
//      regsvr32 Sample2MemRenderer.ax
//
//  2.  Insert the filter into the graph to process video data (live 
//      capture or video files)
//      a.  Use the GraphEdt.exe (in DirectX SDK) to build a graph 
//      b.  To use this filter in App, include iSample2MemRenderer.h for
//          definition of interfaces and build the graph in the App.
///////////////////////////////////////////////////////////////////////
#pragma once

class CSample2MemRenderer	: 
		public CBaseRenderer,			// Base Renderer
      public CPersistStream,			// Persist the object
      public ISample2MemRenderer,	// Interface of CSample2MemRenderer
      public ISpecifyPropertyPages,	// Having property page
      public IQualityControl,			// Allow throttling
		public IAMFilterMiscFlags		// Identifies the filter as Renderer
{
public:

	DECLARE_IUNKNOWN;
 
	// Constructor && Deconstructor
	static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	CSample2MemRenderer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
	virtual ~CSample2MemRenderer();

	// Reveals the interface the filter support
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	// ISpecifyPropertyPages interface
	STDMETHODIMP GetPages(CAUUID *pPages);

	// ISample2MemRenderer interface to custom the filter
	STDMETHODIMP GetParams(Sample2MemRendererParams *irp);
	STDMETHODIMP SetParams(Sample2MemRendererParams *irp);
	STDMETHODIMP SetReceiveFunction(RECEIVE_FUNCTION, long);
	STDMETHODIMP SetAcceptedMediaTypes(AM_MEDIA_TYPE*, int, BOOL);
	STDMETHODIMP GetAllocatorProperties(ALLOCATOR_PROPERTIES *pAP);

   // IQualityControl methods - Notify allows audio-video throttling
   STDMETHODIMP SetSink( IQualityControl * piqc);
   STDMETHODIMP Notify( IBaseFilter * pSelf, Quality q);

	// IAMFilterMiscFlags  interface
	STDMETHODIMP_(ULONG) GetMiscFlags(void);

// CPersistStream stuff -- Save the config persistently
// These functions are used to serialize object, (like save or load
// graph file (*.grf) in GraphEdt.exe. It will be automaticly called 
// when you save filter to file.
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int     SizeMax() { return 2 * STR_MAX_LENGTH; }
    // This is the only method of IPersist
    STDMETHODIMP GetClassID(CLSID *pClsid);

    // Override CBaseRenderer pure functions
    virtual HRESULT DoRenderSample(IMediaSample *pMediaSample);
    virtual HRESULT CheckMediaType(const CMediaType *);
	 // Override CBaseRenderer functions
    virtual HRESULT SetMediaType(const CMediaType *pmt);
    virtual BOOL ScheduleSample(IMediaSample *pMediaSample);

    virtual void OnRenderStart(IMediaSample *pMediaSample);
    virtual void OnRenderEnd(IMediaSample *pMediaSample);
    virtual void OnReceiveFirstSample(IMediaSample *pMediaSample);
    virtual HRESULT OnStartStreaming();
    virtual HRESULT OnStopStreaming();
    virtual void OnWaitStart();
    virtual void OnWaitEnd();
    virtual void PrepareRender();
    void ThrottleWait();

protected:
    // Save the configuration into registry
    void    saveConfig();
    // Read Config from registry
    void    readConfig(); 

    // Parameters controling the filter behavior
    Sample2MemRendererParams m_Sample2MemRendererParams;
    // Critical Section (used for multi-thread share)
    CCritSec m_Sample2MemRendererLock; 

	long m_nPackageNo;
	RECEIVE_FUNCTION m_pReceiveFnc;
	long             m_lRFParam;
	AM_MEDIA_TYPE  **m_ppMediaTypes;
	AM_MEDIA_TYPE   *m_pRenderedType;
	int              m_nMediaTypes;
	BOOL				  m_bEnumerate;
	ALLOCATOR_PROPERTIES m_AllocProps;

   int m_trThrottle;

};
