// DirectBufferAllocater.h: interface for the CDirectBufferAllocater class.
// 
//////////////////////////////////////////////////////////////////////
//  CDirectBufferAllocater: (A DirectShow (DirectX 8.0) Allocator)
//
///////////////////////////////////////////////////////////////////////
#pragma once

class CPushSource;

class CDirectBufferAllocater : public CMemAllocator
{

protected:

    // our pin who created us
    //
public:

	CDirectBufferAllocater(CPushSource* pParent, HRESULT *phr ) : CMemAllocator( TEXT("PushSourceAllocator"), NULL, phr ) 
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
	 CPushSource *m_pFilter;
};

