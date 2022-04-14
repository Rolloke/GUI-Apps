// PushSource.cpp: implementation of CPushSource class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow
#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include <stdio.h>

#include "DirectBufferAllocator.h"
#include "PushSource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDirectBufferAllocater::~CDirectBufferAllocater( )
{
	ReallyFree();
	m_pBuffer = NULL;
}

// Alloc
HRESULT CDirectBufferAllocater::Alloc( )
{
   CAutoLock lck(this);										// look at the base class code to see where this came from!

   HRESULT hr = CBaseAllocator::Alloc();				// Check he has called SetProperties
   if (FAILED(hr))
	{
      return hr;
   }

   if (hr == S_FALSE)										// If the requirements haven't changed then don't reallocate
	{
      return NOERROR;
   }
   ASSERT(hr == S_OK);										// we use this fact in the loop below

	if (m_pBuffer)												// Free the old resources
	{
		ReallyFree();
	}

   LONG lAlignedSize = m_lSize + m_lPrefix;			// Compute the aligned size
   if (m_lAlignment > 1)
	{
      LONG lRemainder = lAlignedSize % m_lAlignment;
      if (lRemainder != 0)
		{
         lAlignedSize += (m_lAlignment - lRemainder);
      }
   }

   /* Create the contiguous memory block for the samples
      making sure it's properly aligned (64K should be enough!)
   */
   ASSERT(lAlignedSize % m_lAlignment == 0);

	if (m_pFilter->m_nBufferType == BUFFER_FIXED)	// Buffer is fixed ?
	{																// don't create the buffer - use what was passed to us
		m_pBuffer = (BYTE*)m_pFilter->m_pSampleProperties->pBuffer;
		if (m_pBuffer == NULL)								// set the external buffer
		{
			 return E_OUTOFMEMORY;
		}

		LPBYTE pNext = m_pBuffer;
		CMediaSample *pSample;

		ASSERT(m_lAllocated == 0);

		// Create the new samples with the external buffer
		for (; m_lAllocated < m_lCount; m_lAllocated++, pNext += lAlignedSize)
		{
			pSample = new CMediaSample(
								NAME("external buffer media sample"),
								this,
								&hr,
								pNext + m_lPrefix,      // GetPointer() value
								m_lSize);               // not including prefix

			ASSERT(SUCCEEDED(hr));
			if (pSample == NULL)
			{
				return E_OUTOFMEMORY;
			}

			m_lFree.Add(pSample);
		}
	}
	else if ((m_pFilter->m_nBufferType == BUFFER_ATTACH) || // Buffer is attached ?
		      (m_pFilter->m_nBufferType == BUFFER_COPY  ))   // Buffer is copied   ?
	{																		  // don't create the buffer 
		CMediaSample *pSample;

		// Create the new samples with empty buffer
		// the external buffers are applied later
		for (; m_lAllocated < m_lCount; m_lAllocated++)
		{
			pSample = new CMediaSample(
								NAME("external buffers media sample"),
								this,
								&hr,
								NULL, 
								m_lSize);

			ASSERT(SUCCEEDED(hr));
			if (pSample == NULL)
			{
				return E_OUTOFMEMORY;
			}

			m_lFree.Add(pSample);
		}
	}

   m_bChanged = FALSE;
   return NOERROR;
}
// End Alloc

// ReallyFree
void CDirectBufferAllocater::ReallyFree()
{
	// look at the base class code to see where this came from!
	ASSERT(m_lAllocated == m_lFree.GetCount());

	/* Free up all the CMediaSamples */
	CMediaSample *pSample;
	for (;;)											// remove all samples
	{
		pSample = m_lFree.RemoveHead();
		if (pSample != NULL)
		{
			BYTE *pBuffer = NULL;				// get data pointer
			HRESULT hr = pSample->GetPointer(&pBuffer);
			if (SUCCEEDED(hr))
			{											// check for prefix
				if (m_pFilter->m_allocprops.cbPrefix)
				{										// correct the offset
					pBuffer -= m_pFilter->m_allocprops.cbPrefix;
				}										// release the buffer
				m_pFilter->ReleaseMediaBuffer(pBuffer);
			}
		   delete pSample;
		}
		else
		{
			break;
		}
   }

   m_lAllocated = 0;
}
// End ReallyFree

// Free
void CDirectBufferAllocater::Free(void)
{
	CMemAllocator::Free();
}
// End Free

STDMETHODIMP CDirectBufferAllocater::SetProperties(ALLOCATOR_PROPERTIES* pRequest,ALLOCATOR_PROPERTIES* pActual)
{
	return CMemAllocator::SetProperties(pRequest, pActual);
}

STDMETHODIMP CDirectBufferAllocater::GetBuffer(IMediaSample **ppBuffer, REFERENCE_TIME *pT1, REFERENCE_TIME *pT2, DWORD dwFlags)
{
	return CMemAllocator::GetBuffer(ppBuffer, pT1, pT2, dwFlags);
}

STDMETHODIMP CDirectBufferAllocater::ReleaseBuffer(IMediaSample *pBuffer)
{
	HRESULT hr = E_NOTIMPL;
	
	if (m_pFilter->m_nBufferType)						// if there are external buffers
	{
		BYTE *pData = NULL;
		hr = pBuffer->GetPointer((BYTE**)&pData);	// get the data pointer to identify the properties
		if (SUCCEEDED(hr))
		{
			if (m_pFilter->m_allocprops.cbPrefix)	// correct the offset
			{
				pData -= m_pFilter->m_allocprops.cbPrefix;
			}
			m_pFilter->ReleaseMediaBuffer(pData);	// release the external data buffer
		}
	}
	hr = CMemAllocator::ReleaseBuffer(pBuffer);	// release the sample buffer
	return hr;
}
