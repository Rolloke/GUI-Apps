// PushSource.h: interface for the CPushSource class.
// 
//////////////////////////////////////////////////////////////////////
//  CPushSource: (A DirectShow (DirectX 8.0) filter)
//
//  Purpose: This filter is a basic Transform Filter to process video
//        
//  Usage: 
//  1.  Register the filter. 
//      regsvr32 PushSource.ax
//
//  2.  Insert the filter into the graph to process video data (live 
//      capture or video files)
//      a.  Use the GraphEdt.exe (in DirectX SDK) to build a graph 
//      b.  To use this filter in App, include iPushSource.h for
//          definition of interfaces and build the graph in the App.
///////////////////////////////////////////////////////////////////////
#pragma once

class CDirectBufferAllocater;
class CPushSourceStream;

#include "iPushSource.h"

class CPushSourceClock :public CBaseReferenceClock
{
public:
	CPushSourceClock(LPUNKNOWN pUnk, HRESULT *phr, CAMSchedule * pShed):
		CBaseReferenceClock( _T("PushSourceClock"), pUnk, phr, pShed)
	{
	};
	virtual ~CPushSourceClock() {};
};

class CPushSource :
      public CPersistStream,        // Persist the object
      public CSource,				// Derive from SOURCE Filter
      public IPushSource,			// Interface of CPushSource
      public ISpecifyPropertyPages,	// Having property page
		public IQualityControl,		// QualityControl sink
		public IAMFilterMiscFlags,	// Identifies the filter as Source
//		public IAMDroppedFrames,	// 
		public IKsPropertySet		// 
{
public:

	friend class CDirectBufferAllocater;
	friend class CPushSourceStream;

	// Constructor && Deconstructor
	static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	CPushSource(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
	virtual ~CPushSource();

	DECLARE_IUNKNOWN
	// Reveals the interface the filter support
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);


	// Decide the buffersize of the direct allocator
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);
	// Get the delivered media type
    virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);  // List pos. 0-n
    virtual HRESULT CheckMediaType(const CMediaType *);
	HRESULT	SetCurrentMediaType(const AM_MEDIA_TYPE*pMT);


	// CPersistStream stuff -- Save the config persistently
	// These functions are used to serialize object, (like save or load
	// graph file (*.grf) in GraphEdt.exe. It will be automaticly called 
	// when you save filter to file.
	HRESULT WriteToStream(IStream *pStream);
	HRESULT ReadFromStream(IStream *pStream);
	int     SizeMax() { return 2 * STR_MAX_LENGTH; }

	// CPushSource Functions
	HRESULT ReleaseMediaBuffer(BYTE*);
	PushSourceProperties* FindMSP(void*, bool bRemove=false);
	PushSourceProperties* FindMSPbyNumber(DWORD nSampleNumber, bool bRemove=false);
	HRESULT CreateDirectAllocator();
	void    ReleaseDirectAllocator();
	void    ReleaseAllBuffers();
	HRESULT GetDeliveryBuffer(IMediaSample **, REFERENCE_TIME *, REFERENCE_TIME *, DWORD);
	HRESULT RecommitDirectAllocator();
	void	  ReleasePins();
	HRESULT InitPins();
	BOOL    DoGenerateTimeStamps() {return m_PushSourceParams.m_bGenerateTimeStamps;};
	IReferenceClock* GetClock() {return m_pClock;};
	BOOL    IsLiveSource() {return m_PushSourceParams.m_bLiveSource;};
	BOOL    DoInternalRM() {return m_PushSourceParams.m_bInternalRM;};
	BOOL    GetOffsetInMS() {return m_PushSourceParams.m_nOffsetInMS;};
	HWND    GetPropertyPageHWND() { return m_hWndPropertyPage;};
	BOOL    PreferOwnAllocator() {return m_PushSourceParams.m_bPreferOwnAllocator;};

	// --- IMediaFilter Interface
	STDMETHODIMP SetSyncSource(IReferenceClock *pClock);

	// --- IBaseFilter Interface
	STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause();
	STDMETHODIMP Stop();
	STDMETHODIMP GetState(DWORD, FILTER_STATE *);
	STDMETHODIMP NotifyFilterState(DWORD, FILTER_STATE *);

	// ISpecifyPropertyPages interface
	STDMETHODIMP GetPages(CAUUID *pPages);

	// IPushSource interface to custom the filter
	STDMETHODIMP GetParams(PushSourceParams *irp);
	STDMETHODIMP SetParams(PushSourceParams *irp);

	STDMETHODIMP SetBuffer(PushSourceProperties*);
	STDMETHODIMP SetBufferType(int, RELEASE_BUFFER_FUNCTION, long, ALLOCATOR_PROPERTIES*);
	STDMETHODIMP SetMediaType(AM_MEDIA_TYPE*);

   STDMETHODIMP GetBuffer(PushSourceProperties *);
   STDMETHODIMP GetBufferType(int &, ALLOCATOR_PROPERTIES*);
   STDMETHODIMP GetSourceMediaType(AM_MEDIA_TYPE **);

   STDMETHODIMP SetPropertyPageHWND(HWND);
	STDMETHODIMP SetStreamOffset(REFERENCE_TIME rt);
	STDMETHODIMP GetStreamOffset(REFERENCE_TIME *prt);

	// IQualityControl interface
   STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q);
   STDMETHODIMP SetSink(IQualityControl *piqc);
/*
	// IAMDroppedFrames interface
	STDMETHODIMP GetAverageFrameSize(long *plAverageSize);
	STDMETHODIMP GetDroppedInfo(long lSize, long *plArray, long *plNumCopied);
	STDMETHODIMP GetNumDropped(long *plDropped);
	STDMETHODIMP GetNumNotDropped(long *plNotDropped);
*/
	// IKsPropertySet interface
	STDMETHODIMP Set(REFGUID guidPropSet,DWORD dwPropID, LPVOID pInstanceData,DWORD cbInstanceData, LPVOID pPropData,DWORD cbPropData);
	STDMETHODIMP Get(REFGUID guidPropSet,DWORD dwPropID,LPVOID pInstanceData,DWORD cbInstanceData,LPVOID pPropData,DWORD cbPropData,DWORD *pcbReturned);
   STDMETHODIMP QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport);

	// This is the only method of IPersist
	STDMETHODIMP GetClassID(CLSID *pClsid);

	// IAMFilterMiscFlags  interface
	STDMETHODIMP_(ULONG) GetMiscFlags(void);

protected:
	// Save the configuration into registry
	BOOL    saveConfig();
	// Read Config from registry
	void    readConfig(); 

	// Parameters controling the filter behavior
	PushSourceParams m_PushSourceParams;
	
	CDirectBufferAllocater *m_pDirectAllocator;
	ALLOCATOR_PROPERTIES    m_allocprops;
	PushSourceProperties  *m_pSampleProperties;
	CGenericList<PushSourceProperties>m_BufferList;
	CGenericList<PushSourceProperties>m_SentBufferList;
	
	RELEASE_BUFFER_FUNCTION m_pReleaseBufferFunction;
	long                    m_lParamRBF;
	int                     m_nBufferType;
	AM_MEDIA_TYPE*			m_pCurrentMediaType;
	CGenericList<AM_MEDIA_TYPE>m_MediaTypes;
	BOOL					m_bReCommit;
	BOOL					m_bBufferChanged;
	IQualityControl			*m_pQSink;
	REFERENCE_TIME			m_rtStartTime;
	HWND					m_hWndPropertyPage;
	CPushSourceClock		*m_pRefClock;
	DWORD					m_nNextSampleNumber;
	BOOL					m_bIsStopping;
	int						m_nSampleToLate;
	int						m_nSampleToEarly;
	int                     m_nAdvices;
private:
	static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam);
	static HWND				gm_hWndThread;
	static _TCHAR			gm_sWndName[];
	static const _TCHAR		gm_sModuleName[];
public:
	static HWND GetThreadWnd() { return gm_hWndThread; }
	static void Trace(LPCTSTR str);
};
