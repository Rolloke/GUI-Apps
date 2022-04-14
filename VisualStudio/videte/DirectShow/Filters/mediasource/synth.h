//------------------------------------------------------------------------------
// File: Synth.h
//
// Desc: DirectShow sample code - header file for audio signal generator 
//       source filter.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __AUDIOSYNTH__
#define __AUDIOSYNTH__

//CLSID_MediaSourceFilter
//{1A410C37-0D4E-4694-88F2-FA44E6CF03F5}
DEFINE_GUID(CLSID_MediaSourceFilter,
0x1a410c37, 0xd4e, 0x4694, 0x88, 0xf2, 0xfa, 0x44, 0xe6, 0xcf, 0x3, 0xf5);

//CLSID_MediaSourceFilterPropertyPage
// {78F10056-BB75-4ae9-99E8-1CF363065BBE}
DEFINE_GUID(CLSID_SynthPropertyPage,
0x78f10056, 0xbb75, 0x4ae9, 0x99, 0xe8, 0x1c, 0xf3, 0x63, 0x6, 0x5b, 0xbe);

const double TWOPI = 6.283185308;
const int MaxFrequency = 20000;
const int MinFrequency = 0;
const int DefaultFrequency = 440;       // A-440
const int MaxAmplitude = 100;
const int MinAmplitude = 0;
const int DefaultSweepStart = DefaultFrequency;
const int DefaultSweepEnd = 5000;
const int WaveBufferSize = 16*1024;     // Size of each allocated buffer
                                        // Originally used to be 2K, but at
                                        // 44khz/16bit/stereo you would get
                                        // audio breaks with a transform in the
                                        // middle.

enum Waveforms {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_SAWTOOTH,
    WAVE_SINESWEEP,
    WAVE_LAST           // Always keep this guy last
};

#define WM_PROPERTYPAGE_ENABLE  (WM_USER + 100)

// below stuff is implementation-only....
#ifdef _AUDIOSYNTH_IMPLEMENTATION_

class CMediaSourceStream;
class CMediaSourceFilter;

// -------------------------------------------------------------------------
// CAudioSynth
// -------------------------------------------------------------------------

class CAudioSynth {

public:

    CAudioSynth(
                CCritSec* pStateLock,
                int Frequency = DefaultFrequency,
                int Waveform = WAVE_SINE,
                int iBitsPerSample = 8,
                int iChannels = 1,
                int iSamplesPerSec = 11025,
                int iAmplitude = 100
                );

    ~CAudioSynth();

    // Load the buffer with the current waveform
    void FillPCMAudioBuffer(const WAVEFORMATEX& wfex, BYTE pBuf[], int iSize);

    // Set the "current" format and allocate temporary memory
    HRESULT AllocWaveCache(const WAVEFORMATEX& wfex);

    void GetPCMFormatStructure(WAVEFORMATEX* pwfex);

    STDMETHODIMP get_Frequency(int *Frequency);
    STDMETHODIMP put_Frequency(int  Frequency);
    STDMETHODIMP get_Waveform(int *Waveform);
    STDMETHODIMP put_Waveform(int  Waveform);
    STDMETHODIMP get_Channels(int *Channels);
    STDMETHODIMP put_Channels(int Channels);
    STDMETHODIMP get_BitsPerSample(int *BitsPerSample);
    STDMETHODIMP put_BitsPerSample(int BitsPerSample);
    STDMETHODIMP get_SamplesPerSec(int *SamplesPerSec);
    STDMETHODIMP put_SamplesPerSec(int SamplesPerSec);
    STDMETHODIMP put_SynthFormat(int Channels, int BitsPerSample, int SamplesPerSec);
    STDMETHODIMP get_Amplitude(int *Amplitude);
    STDMETHODIMP put_Amplitude(int  Amplitude);
    STDMETHODIMP get_SweepRange(int *SweepStart, int *SweepEnd);
    STDMETHODIMP put_SweepRange(int  SweepStart, int  SweepEnd);
    STDMETHODIMP get_OutputFormat(SYNTH_OUTPUT_FORMAT *pOutputFormat);
    STDMETHODIMP put_OutputFormat(SYNTH_OUTPUT_FORMAT ofOutputFormat);

private:
    CCritSec* m_pStateLock;

    WORD  m_wChannels;          // The output format's current number of channels.
    WORD  m_wFormatTag;         // The output format.  This can be PCM audio or MS ADPCM audio.
    DWORD m_dwSamplesPerSec;    // The number of samples produced in one second by the synth filter.
    WORD  m_wBitsPerSample;     // The number of bits in each sample.  This member is only valid if the
                                // current format is PCM audio.

    int m_iWaveform;            // WAVE_SINE ...
    int m_iFrequency;           // if not using sweep, this is the frequency
    int m_iAmplitude;           // 0 to 100

    int m_iWaveformLast;        // keep track of the last known format
    int m_iFrequencyLast;       // so we can flush the cache if necessary
    int m_iAmplitudeLast;

    int m_iCurrentSample;       // 0 to iSamplesPerSec-1
    BYTE * m_bWaveCache;        // Wave Cache as BYTEs.  This cache ALWAYS holds PCM audio data.
    WORD * m_wWaveCache;        // Wave Cache as WORDs.  This cache ALWAYS holds PCM audio data.
    int m_iWaveCacheSize;       // how big is the cache?
    int m_iWaveCacheCycles;     // how many cycles are in the cache
    int m_iWaveCacheIndex;

    int m_iSweepStart;           // start of sweep
    int m_iSweepEnd;             // end of sweep

    void CalcCache         (const WAVEFORMATEX& wfex);
    void CalcCacheSine     (const WAVEFORMATEX& wfex);
    void CalcCacheSquare   (const WAVEFORMATEX& wfex);
    void CalcCacheSawtooth (const WAVEFORMATEX& wfex);
    void CalcCacheSweep    (const WAVEFORMATEX& wfex);

};

class CDirectBufferAllocater : public CMemAllocator
{

protected:

    // our pin who created us
    //
public:

	CDirectBufferAllocater(CMediaSourceFilter * pParent, HRESULT *phr ) : CMemAllocator( TEXT("SampleGrabberAllocator"), NULL, phr ) 
	  , m_pFilter(pParent)
	{
	};

	virtual ~CDirectBufferAllocater( );
	// we override this to tell whoever's upstream of us what kind of
	// properties we're going to demand to have
	//

	virtual HRESULT Alloc();
	virtual void Free();
	virtual void ReallyFree();
	BOOL IsCommitted() { return m_bCommitted; };


	STDMETHODIMP SetProperties(ALLOCATOR_PROPERTIES*, ALLOCATOR_PROPERTIES*);

	STDMETHODIMP GetBuffer(IMediaSample **, REFERENCE_TIME *, REFERENCE_TIME *, DWORD);
	// final release of a CMediaSample will call this
	STDMETHODIMP ReleaseBuffer(IMediaSample *);

private:
	 CMediaSourceFilter *m_pFilter;
};



// -------------------------------------------------------------------------
// CMediaSourceFilter
// -------------------------------------------------------------------------
// CMediaSourceFilter manages filter level stuff

class CMediaSourceFilter : 
	public IMediaSource,
   public ISpecifyPropertyPages,
   public CPersistStream,
   public CDynamicSource
{

public:
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	~CMediaSourceFilter();


	DECLARE_IUNKNOWN;

	// override this to reveal our property interface
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	// --- ISpecifyPropertyPages ---

	// return our property pages
	STDMETHODIMP GetPages(CAUUID * pPages);

	// --- IPersistStream Interface

	STDMETHODIMP GetClassID(CLSID *pClsid);
	int SizeMax();
	HRESULT WriteToStream(IStream *pStream);
	HRESULT ReadFromStream(IStream *pStream);
	DWORD   GetSoftwareVersion(void);
	HRESULT CreateDirectAllocator();
	void    ReleaseDirectAllocator();
	void    ReleaseAllBuffers();
	HRESULT GetDeliveryBuffer(IMediaSample **, REFERENCE_TIME *, REFERENCE_TIME *, DWORD);
	HRESULT ReleaseMediaBuffer(BYTE*);
	HRESULT RecommitDirectAllocator();
	MediaSampleProperties* FindMSP(void*, bool bRemove=false);

	// --- IMediaFilter Interface
	STDMETHODIMP SetSyncSource(IReferenceClock *pClock);

	// --- IBaseFilter Interface
	STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause();
	STDMETHODIMP Stop();
	STDMETHODIMP GetState(DWORD, FILTER_STATE *);

	// --- IMediaSource ---
	STDMETHODIMP SetBuffer(MediaSampleProperties*);
	STDMETHODIMP SetBufferType(int, RELEASE_BUFFER_FUNCTION, long, ALLOCATOR_PROPERTIES*);
	STDMETHODIMP SetMediaType(AM_MEDIA_TYPE*);

	STDMETHODIMP get_Frequency(int *Frequency);
	STDMETHODIMP put_Frequency(int Frequency);
	STDMETHODIMP get_Waveform(int *Waveform);
	STDMETHODIMP put_Waveform(int Waveform);
	STDMETHODIMP get_Channels(int *Channels);
	STDMETHODIMP get_BitsPerSample(int *BitsPerSample);
	STDMETHODIMP get_SamplesPerSec(int *SamplesPerSec);
	STDMETHODIMP put_Channels(int Channels);
	STDMETHODIMP put_BitsPerSample(int BitsPersample);
	STDMETHODIMP put_SamplesPerSec(int SamplesPerSec);
	STDMETHODIMP get_Amplitude(int *Amplitude);
	STDMETHODIMP put_Amplitude(int Amplitude);
	STDMETHODIMP get_SweepRange(int *SweepStart, int *SweepEnd);
	STDMETHODIMP put_SweepRange(int  SweepStart, int  SweepEnd);
	STDMETHODIMP get_OutputFormat(SYNTH_OUTPUT_FORMAT *pOutputFormat);
	STDMETHODIMP put_OutputFormat(SYNTH_OUTPUT_FORMAT ofOutputFormat);

	CAudioSynth *m_Synth;           // the current synthesizer

	CDirectBufferAllocater *m_pDirectAllocator;
	ALLOCATOR_PROPERTIES    m_allocprops;
	MediaSampleProperties  *m_pSampleProperties;
	CGenericList<MediaSampleProperties>m_BufferList;
	CGenericList<MediaSampleProperties>m_SentBufferList;
	
	RELEASE_BUFFER_FUNCTION m_pReleaseBufferFunction;
	long                    m_lParamRBF;
	int                     m_nBufferType;
	CMediaType              m_MediaType;
	bool						   m_bReCommit;
	bool						   m_bBufferChanged;

	
private:

	// it is only allowed to to create these objects with CreateInstance
	CMediaSourceFilter(LPUNKNOWN lpunk, HRESULT *phr);

	// When the format changes, reconnect...
	void ReconnectWithNewFormat(void);

};


// -------------------------------------------------------------------------
// CMediaSourceStream
// -------------------------------------------------------------------------
// CMediaSourceStream manages the data flow from the output pin.

class CMediaSourceStream : public CDynamicSourceStream, public IKsPropertySet, public CBaseStreamControl
{

public:

    CMediaSourceStream(HRESULT *phr, CMediaSourceFilter *pParent, LPCWSTR pPinName);
    ~CMediaSourceStream();

	DECLARE_IUNKNOWN;

	// override this to reveal our property interface
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    BOOL ReadyToStop(void) {return FALSE;}

    // stuff an audio buffer with the current format
    HRESULT FillBuffer(IMediaSample *pms);

    // ask for buffers of the size appropriate to the agreed media type.
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    HRESULT GetMediaType(CMediaType *pmt);

    HRESULT CompleteConnect(IPin *pReceivePin);
    HRESULT BreakConnect(void);

    // resets the stream time to zero.
    HRESULT Active(void);

    virtual HRESULT InitAllocator(IMemAllocator **ppAlloc);

    virtual HRESULT GetDeliveryBuffer(IMediaSample ** ppSample,
                                      REFERENCE_TIME * pStartTime,
                                      REFERENCE_TIME * pEndTime,
                                      DWORD dwFlags);

    virtual HRESULT DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);

	// IPin interface
	STDMETHODIMP BeginFlush();
	STDMETHODIMP EndFlush();
	STDMETHODIMP Receive(IMediaSample *);

	// IKsPropertySet interface
	STDMETHODIMP Get(REFGUID, ULONG, LPVOID, ULONG, LPVOID, ULONG, PULONG);
	STDMETHODIMP Set(REFGUID, ULONG, LPVOID, ULONG, LPVOID, ULONG);
	STDMETHODIMP QuerySupported(REFGUID, DWORD, DWORD *);

private:

    void DerivePCMFormatFromADPCMFormatStructure(const WAVEFORMATEX& wfexADPCM, WAVEFORMATEX* pwfexPCM);

    // Access to this state information should be serialized with the filters
    // critical section (m_pFilter->pStateLock ())

    // This lock protects: m_dwTempPCMBufferSize, m_hPCMToMSADPCMConversionStream,
    // m_rtSampleTime, m_fFirstSampleDelivered and m_llSampleMediaTimeStart
    CCritSec    m_cSharedState;     

    CRefTime     m_rtSampleTime;    // The time to be stamped on each sample
    HACMSTREAM m_hPCMToMSADPCMConversionStream;
    DWORD m_dwTempPCMBufferSize;
    bool m_fFirstSampleDelivered;
    LONGLONG m_llSampleMediaTimeStart;
	 GUID     m_guidPinCategory;
    CAudioSynth *m_Synth;           // the current synthesizer

    CMediaSourceFilter *m_pParent;
	 BOOL m_fLastSampleDiscarded;
};

#endif // _AUDIOSYNTH_IMPLEMENTATION_ implementation only....

#endif /* __AUDIOSYNTH__ */



