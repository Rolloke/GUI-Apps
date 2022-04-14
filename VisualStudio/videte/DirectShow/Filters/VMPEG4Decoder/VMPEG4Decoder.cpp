// VMPEG4Decoder.cpp: implementation of CVMPEG4Decoder class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow
#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include <stdio.h>

#include "iVMPEG4Decoder.h"
#include "VMPEG4Decoder.h"
#include "VMPEG4DecoderProp.h"


#define TRANSFORM_NAME L"Videte MPEG4 Decoder Filter"

// setup data - allows the self-registration to work.
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{ &MEDIATYPE_NULL        // clsMajorType
, &MEDIASUBTYPE_NULL };  // clsMinorType

const AMOVIESETUP_PIN psudPins[] =
{ { L"Input"            // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
, { L"Output"           // strName
  , FALSE               // bRendered
  , TRUE                // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
};

const AMOVIESETUP_FILTER sudVMPEG4Decoder =
{ &CLSID_VMPEG4Decoder              // clsID
, TRANSFORM_NAME                    // strName
, MERIT_DO_NOT_USE                  // dwMerit
, 2                                 // nPins
, psudPins };                       // lpPin

// Needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
    {   { TRANSFORM_NAME
        , &CLSID_VMPEG4Decoder
        , CVMPEG4Decoder::CreateInstance
        , NULL
        , &sudVMPEG4Decoder },
        { TRANSFORM_NAME L" Properties"
        , &CLSID_VMPEG4DecoderProp
        , CVMPEG4DecoderProp::CreateInstance }
    };
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

///////////////////////////////////////////////////////////////////////
// CreateInstance: for COM to create a CVMPEG4Decoder object
///////////////////////////////////////////////////////////////////////
CUnknown * WINAPI CVMPEG4Decoder::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CVMPEG4Decoder *pNewObject = new CVMPEG4Decoder(NAME("VMPEG4Decoder"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

///////////////////////////////////////////////////////////////////////
// CVMPEG4Decoder: Constructor
///////////////////////////////////////////////////////////////////////
CVMPEG4Decoder::CVMPEG4Decoder(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
    : CTransformFilter (tszName, punk, CLSID_VMPEG4Decoder),
      CPersistStream(punk, phr)
{
    readConfig();                  // read default value from registry
    SetParams(&m_VMPEG4DecoderParams); // Set Filter parameters
}

///////////////////////////////////////////////////////////////////////
// ~CVMPEG4Decoder: Destructor
///////////////////////////////////////////////////////////////////////
CVMPEG4Decoder::~CVMPEG4Decoder()
{
}

///////////////////////////////////////////////////////////////////////
// saveConfig: write the parameters into the registry
///////////////////////////////////////////////////////////////////////
void CVMPEG4Decoder::saveConfig()
{
    char sz[STR_MAX_LENGTH];
    // write int into registry
    sprintf(sz, "%d", m_VMPEG4DecoderParams.param1);
    WriteProfileStringA("VMPEG4Decoder", "Parameters1", sz);
    // Write double value parameters
    sprintf(sz, "%f", m_VMPEG4DecoderParams.param2);
    WriteProfileStringA("VMPEG4Decoder", "Parameters2", sz);
}

///////////////////////////////////////////////////////////////////////
// readConfig: read the parameters from the registry
///////////////////////////////////////////////////////////////////////
void CVMPEG4Decoder::readConfig()
{
    // read integer from registry
    m_VMPEG4DecoderParams.param1 = 
        GetProfileInt("VMPEG4Decoder", "Parameters1", 1);
    // Read double value parameters
    char sz[STR_MAX_LENGTH];
    GetProfileStringA("VMPEG4Decoder", "Parameters2", "0.5", sz, STR_MAX_LENGTH);
    m_VMPEG4DecoderParams.param2 = atof(sz);
}

///////////////////////////////////////////////////////////////////////
// CheckInputType: Check if the input type can be done
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::CheckInputType(const CMediaType *mtIn)
{
    if (canPerformTransform(mtIn))
        return S_OK;
    else
        return VFW_E_TYPE_NOT_ACCEPTED;
}

///////////////////////////////////////////////////////////////////////
// Checktransform: Check a transform can be done between these formats
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    if (canPerformTransform(mtIn)) {
        if (*mtIn == *mtOut) {
            return NOERROR;
        }
    }
    return E_FAIL;
}

///////////////////////////////////////////////////////////////////////
// DecideBufferSize: Tell the output pin's allocator what size buffers 
// we require. Can only do this when the input is connected
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    ASSERT(pAlloc);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

    pProperties->cBuffers = 1;

    // Get input pin's allocator size and use that
    ALLOCATOR_PROPERTIES InProps;
    IMemAllocator * pInAlloc = NULL;
    hr = m_pInput->GetAllocator(&pInAlloc);
    if (SUCCEEDED (hr))
    {
        hr = pInAlloc->GetProperties (&InProps);
        if (SUCCEEDED (hr))
        {
            pProperties->cbBuffer = InProps.cbBuffer;
        }
        pInAlloc->Release();
    }

    if (FAILED(hr))
        return hr;

    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT( Actual.cBuffers == 1 );

    if ( pProperties->cBuffers > Actual.cBuffers ||
         pProperties->cbBuffer > Actual.cbBuffer) {
                return E_FAIL;
    }
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetMediaType: I support one type, namely the type of the input pin
// This type is only available if my input is connected
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }
    // This should never happen
    if (iPosition < 0) {
        return E_INVALIDARG;
    }
    // Do we have more items to offer
    if (iPosition > 0) {
        return VFW_S_NO_MORE_ITEMS;
    }

    *pMediaType = m_pInput->CurrentMediaType();
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// Transform (for CTransformFitler)
//
// Copy the input sample into the output sample - then transform the 
// output sample 'in place'. If we have all keyframes, then we should
// not do a copy. If we have cinepak or indeo and are decompressing 
// frame N it needs frame decompressed frame N-1 available to calculate
// it, unless we are at a keyframe. So with keyframed codecs, you can't 
// get away with applying the transform to change the frames in place, 
// because you'll mess up the next frames decompression. The runtime 
// MPEG decoder does not have keyframes in the same way so it can be 
// done in place. We know if a sample is key frame as we transform 
// because the sync point property will be set on the sample
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
    // Copy the properties across
    HRESULT hr = copyMediaSample(pIn, pOut);
    if (FAILED(hr)) {
        return hr;
    }
    // Transform the output media sample in-place
    return Transform(pOut);
}

///////////////////////////////////////////////////////////////////////
// copyMediaSample: Make destination an identical copy of source
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::copyMediaSample(IMediaSample *pSource, IMediaSample *pDest) const
{
    // Copy the sample data
    BYTE *pSourceBuffer, *pDestBuffer;
    long lSourceSize = pSource->GetActualDataLength();
    long lDestSize  = pDest->GetSize();

    ASSERT(lDestSize >= lSourceSize);

    pSource->GetPointer(&pSourceBuffer);
    pDest->GetPointer(&pDestBuffer);

    CopyMemory( (PVOID) pDestBuffer,(PVOID) pSourceBuffer,lSourceSize);

    // Copy the sample times
    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pSource->GetTime(&TimeStart, &TimeEnd)) {
        pDest->SetTime(&TimeStart, &TimeEnd);
    }

    LONGLONG MediaStart, MediaEnd;
    if (pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
        pDest->SetMediaTime(&MediaStart,&MediaEnd);
    }

    // Copy the Sync point property
    HRESULT hr = pSource->IsSyncPoint();
    if (hr == S_OK) {
        pDest->SetSyncPoint(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetSyncPoint(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the media type
    AM_MEDIA_TYPE *pMediaType;
    pSource->GetMediaType(&pMediaType);
    pDest->SetMediaType(pMediaType);
    DeleteMediaType(pMediaType);

    // Copy the preroll property
    hr = pSource->IsPreroll();
    if (hr == S_OK) {
        pDest->SetPreroll(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetPreroll(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property
    hr = pSource->IsDiscontinuity();
    if (hr == S_OK) {
    pDest->SetDiscontinuity(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetDiscontinuity(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the actual data length
    long lDataLength = pSource->GetActualDataLength();
    pDest->SetActualDataLength(lDataLength);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// canPerformTransform: We support RGB24 and RGB32 input
///////////////////////////////////////////////////////////////////////
BOOL CVMPEG4Decoder::canPerformTransform(const CMediaType *pMediaType) const
{
    // we accept the following image type: (RGB24, ARGB32 or RGB32)
    if (IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video)) {
        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24)) {
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pMediaType->Format();
            return (pvi->bmiHeader.biBitCount == 24);
        }
        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_ARGB32)) {
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pMediaType->Format();
            return (pvi->bmiHeader.biBitCount == 32);
        }
        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32)) {
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pMediaType->Format();
            return (pvi->bmiHeader.biBitCount == 32);
        }
    }
    return FALSE;
} 

///////////////////////////////////////////////////////////////////////
// Transform: Transforms each image frame in-place
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::Transform(IMediaSample *pMediaSample)
{
    CAutoLock cAutolock(&m_VMPEG4DecoderLock);

    // get current media type
    AM_MEDIA_TYPE* pmt = &(m_pInput->CurrentMediaType());
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pmt->pbFormat;

    BYTE *pData;                // Pointer to the actual image buffer
    long lDataLen;              // Holds length of any given sample
    int iPixel;                 // Used to loop through the image pixels
    pMediaSample->GetPointer(&pData);
    lDataLen = pMediaSample->GetSize();

    // Get the image properties from the BITMAPINFOHEADER
    int iPixelSize = pvi->bmiHeader.biBitCount / 8;
    int cxImage    = pvi->bmiHeader.biWidth;
    int cyImage    = pvi->bmiHeader.biHeight;
    int cbImage    = cyImage * cxImage * iPixelSize;
    int numPixels  = cxImage * cyImage;

    // Transform it to its XOR image according the parameters
    if (m_VMPEG4DecoderParams.param1)
    {
        BYTE *prgb = (BYTE*) pData;
        for (iPixel=0; iPixel < numPixels; iPixel++, prgb+=iPixelSize) {
            *(prgb)     = *(prgb)     ^ 0xff;  // B channel
            *(prgb + 1) = *(prgb + 1) ^ 0xff;  // G channel
            *(prgb + 2) = *(prgb + 2) ^ 0xff;  // R channel
        }
    }
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface:
//   Reveals IVMPEG4Decoder, ISpecifyPropertyPages, IPersistStream
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CVMPEG4Decoder::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_IVMPEG4Decoder) {
        return GetInterface((IVMPEG4Decoder *) this, ppv);
    } else if (riid == IID_IPersistStream) {
        return GetInterface((IPersistStream *) this, ppv);
    } else if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    } else {
        return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}

///////////////////////////////////////////////////////////////////////
// GetParams: Get the filter parameters to a given destination
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CVMPEG4Decoder::GetParams(VMPEG4DecoderParams *irp)
{
    CAutoLock cAutolock(&m_VMPEG4DecoderLock);
    CheckPointer(irp,E_POINTER);

    *irp = m_VMPEG4DecoderParams;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// SetParams: Set the filter parameters
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CVMPEG4Decoder::SetParams(VMPEG4DecoderParams *irp)
{
    CAutoLock cAutolock(&m_VMPEG4DecoderLock);
    m_VMPEG4DecoderParams = *irp;
    SetDirty(TRUE);

    // Save the config whenever the use changes it
    saveConfig();

    return NOERROR;
} 

#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

///////////////////////////////////////////////////////////////////////
// GetClassID: This is the only method of IPersist
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CVMPEG4Decoder::GetClassID(CLSID *pClsid)
{
    return CBaseFilter::GetClassID(pClsid);
}

///////////////////////////////////////////////////////////////////////
// WriteToStream: called when save the filter to stream
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::WriteToStream(IStream *pStream)
{
    HRESULT hr;
    WRITEOUT(m_VMPEG4DecoderParams.param1);
    WRITEOUT(m_VMPEG4DecoderParams.param2);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// ReadFromStream: called when read from stream
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4Decoder::ReadFromStream(IStream *pStream)
{
    HRESULT hr;
    READIN(m_VMPEG4DecoderParams.param1);
    READIN(m_VMPEG4DecoderParams.param2);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetPages: Returns the clsid's of the property pages we support
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CVMPEG4Decoder::GetPages(CAUUID *pPages)
{
    pPages->cElems = 1;
    pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
    if (pPages->pElems == NULL) {
        return E_OUTOFMEMORY;
    }
    *(pPages->pElems) = CLSID_VMPEG4DecoderProp;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// DllRegisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );
}

///////////////////////////////////////////////////////////////////////
// DllUnregisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );
}
