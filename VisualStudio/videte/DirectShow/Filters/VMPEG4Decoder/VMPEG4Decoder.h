// VMPEG4Decoder.h: interface for the CVMPEG4Decoder class.
// 
//////////////////////////////////////////////////////////////////////
//  CVMPEG4Decoder: (A DirectShow (DirectX 8.0) filter)
//
//  Purpose: This filter is a basic Transform Filter to process video
//        
//  Usage: 
//  1.  Register the filter. 
//      regsvr32 VMPEG4Decoder.ax
//
//  2.  Insert the filter into the graph to process video data (live 
//      capture or video files)
//      a.  Use the GraphEdt.exe (in DirectX SDK) to build a graph 
//      b.  To use this filter in App, include iVMPEG4Decoder.h for
//          definition of interfaces and build the graph in the App.
///////////////////////////////////////////////////////////////////////
#pragma once

class CVMPEG4Decoder
    : public CTransformFilter,     // Derive from Transform Filter
      public IVMPEG4Decoder,       // Interface of CVMPEG4Decoder
      public ISpecifyPropertyPages,// Having property page
      public CPersistStream        // Persist the object
{
public:

    DECLARE_IUNKNOWN;

// Constructor && Deconstructor
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
    CVMPEG4Decoder(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
    ~CVMPEG4Decoder();

// Overrriden from CTransformFilter base class
    // Check if the transform type can be accomplished
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                 ALLOCATOR_PROPERTIES *pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    // This function is called to process each new frame in the video.
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
    HRESULT Transform(IMediaSample *pMediaSample);
    
    // Reveals the interface the filter support
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
    // ISpecifyPropertyPages interface
    STDMETHODIMP GetPages(CAUUID *pPages);

// IVMPEG4Decoder interface to custom the filter
    // Get the current parameters of the filter
    STDMETHODIMP GetParams(VMPEG4DecoderParams *irp);
    // Set the parameters of the filter
    STDMETHODIMP SetParams(VMPEG4DecoderParams *irp);

// CPersistStream stuff -- Save the config persistently
// These functions are used to serialize object, (like save or load
// graph file (*.grf) in GraphEdt.exe. It will be automaticly called 
// when you save filter to file.
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int     SizeMax() { return 2 * STR_MAX_LENGTH; }
    // This is the only method of IPersist
    STDMETHODIMP GetClassID(CLSID *pClsid);

protected:
    // Check if we can accepte the input media type
    BOOL    canPerformTransform(const CMediaType *pMediaType) const;
    // Copy from the input pin to output pin
    HRESULT copyMediaSample(IMediaSample *pSource, IMediaSample *pDest) const;
    // Save the configuration into registry
    void    saveConfig();
    // Read Config from registry
    void    readConfig(); 

    // Parameters controling the filter behavior
    VMPEG4DecoderParams m_VMPEG4DecoderParams;
    // Critical Section (used for multi-thread share)
    CCritSec m_VMPEG4DecoderLock; 
};
