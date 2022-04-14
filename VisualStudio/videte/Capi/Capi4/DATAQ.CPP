/* GlobalReplace: QUEUEBLOCK --> CQueueBlock */
///////////////////////////////////////////////////////////////////////////
// dataq.cpp             
#include <afxwin.h>         // MFC core and standard components

#include <string.h>
#include <dos.h>

#include "wk.h"
#include "wkclasses\wk_trace.h"
#include "hdr.h"
#include "dataq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




CDataQ::CDataQ()
{
	R = 0;
	W = 0;
	// m_queueBlockArray
	m_wQLen = 0;
	m_wBufSize = 0;
	m_bCreateOk = FALSE;      
	m_iValidBuffers = 0;
	m_byLastNumber = 0;
} 

CDataQ::~CDataQ()
{                                        
	m_bCreateOk = FALSE;      
	m_queueBlockArray.DeleteAll();
}

BOOL CDataQ::Create(WORD wLen, WORD wBufSize)
{
	WORD i;
	m_bCreateOk = TRUE;
	m_wQLen = wLen;                
	m_wBufSize = wBufSize;
	// fill pointer array
	for (i=0;i<m_wQLen;i++)
	{
		m_queueBlockArray.Add(new CQueueBlock(m_wBufSize));
	}

	return m_bCreateOk;
}

BOOL CDataQ::PutData(LPDEFHDR lpHdr, LPBYTE lpData)
{
	if (lpHdr==NULL) {
		WK_TRACE_ERROR(_T("NULL in PutData\n"));
		return FALSE;	// <<< EXIT >>>
	}
#if 1
	// check on wLen==0 in FillFromHeader
	if (m_queueBlockArray[W]->FillFromHeader(lpHdr,lpData)) {
		m_queueBlockArray[W]->bGetData = FALSE;                                                
		W++;
		W %= m_wQLen;    
		m_iValidBuffers++;
		return TRUE;
	} else {
		// WK_TRACE(_T("PutData failed W=%d\n"),W);
		return FALSE;
	}
#else
	if (m_queueBlockArray[W]->wLen==0 
		&& lpHdr->wDataLen<=(m_wBufSize-(HDRSIZE+lpHdr->byHdrLen)))
	{
		int nLen = HDRSIZE + lpHdr->byHdrLen + lpHdr->wDataLen;
		m_queueBlockArray[W]->wLen =  (WORD)nLen;
		
		memmove(m_queueBlockArray[W]->pData, lpHdr, HDRSIZE+lpHdr->byHdrLen);				
		
		if (lpData!=NULL && lpHdr->wDataLen!=0){
			memmove(&m_queueBlockArray[W]->pData[HDRSIZE+lpHdr->byHdrLen],
					lpData, 
					lpHdr->wDataLen
					);				
		}
		m_queueBlockArray[W]->bGetData = FALSE;                                                
		W++;
		W %= m_wQLen;    
		m_iValidBuffers++;
		return TRUE;
	} else {
		// WK_TRACE(_T("PutData failed W=%d\n"),W);
		return FALSE;
	}
#endif
}


BOOL CDataQ::FreeBuffer(BYTE byNumber)
{   
//	for(int j=0;j<2;j++)            	// Diese Schleife ist für denn fall das die Nummer
//	{		                           	// nicht gefunden wird.
	for (WORD i=0;i<m_wQLen;i++)
	{ 
		LPDEFHDR lpHdr = (LPDEFHDR)m_queueBlockArray[i]->pData;
		if (m_queueBlockArray[i]->bGetData && lpHdr->byNr==byNumber)
		{
			m_queueBlockArray[i]->bGetData = FALSE;
			m_queueBlockArray[i]->wLen = 0;   
			m_iValidBuffers--;
			if (m_iValidBuffers<0) {
				m_iValidBuffers=0;
				WK_TRACE_ERROR(_T("ValidBuffer UnderFlow\n"));
			}

			m_byLastNumber = byNumber;
			return TRUE;	// <<< EXIT >>>
		}	
	}
	return FALSE;
}

void CDataQ::Reset()
{           
	R = 0;
	W = 0;
	for (WORD i=0;i<m_wQLen;i++)
	{
		if (m_bCreateOk)
		{
			m_queueBlockArray[i]->bGetData = FALSE;
			m_queueBlockArray[i]->wLen = 0;
		}
	}                   
	m_iValidBuffers = 0;
}
	
