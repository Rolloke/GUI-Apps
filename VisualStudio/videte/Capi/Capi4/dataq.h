/* GlobalReplace: QUEUEBLOCK --> CQueueBlock */
#ifndef _INC_DATAQ
#define _INC_DATAQ

////////////////////////////////////////////////////////////////////////////
// dataq.h

#include "wk.h"
#include "hdr.h"
                            
class CQueueBlock
{
public:
	inline CQueueBlock(WORD wBufSize);
	inline ~CQueueBlock();
//
	inline BOOL FillFromHeader(LPDEFHDR lpHdr, LPBYTE lpData);
// data:
	BOOL	bGetData;		// TRUE wenn die Daten dieses Buffers schon won GetData gehohlt worden sind.
	WORD	wLen;			// Die Länge der Daten im Buffer.
    LPBYTE 	pData;			// Pointer auf Windowsdata.
	WORD	m_wMaxLen;
};

typedef CQueueBlock *CQueueBlockPtr;

WK_PTR_ARRAY(CQueueBlockArray,CQueueBlockPtr);

                      
class CDataQ
{
private:
	WORD 			R;
	WORD 			W; 
	CQueueBlockArray m_queueBlockArray;	
	WORD 			m_wQLen;
	WORD 			m_wBufSize;
	BOOL			m_bCreateOk;
	int				m_iValidBuffers;
	BYTE			m_byLastNumber;	
	//
	BOOL			AllocMem(WORD i);
	void 			FreeMem(WORD i); 
public:	//------------------------ Allgemeine Operations -------------------------
	CDataQ();
	BOOL	Create(WORD wLen, WORD wBufSize);
	// Erzeugt DataQ von Länge wLen und Buffergroesse wBufSize.
	// wLen		: Anzahl der Buffer in der Queue
	// wBufSize	: Groesse eines Buffers
	//				MAXIMAL 0xfffd, weil intern 2 Byte addiert werden ( wegen T70 ?)
	// Mit iType kann die Q im Dosspeicher oder in normalen Speicher angelegt werden.
	
	int  	GetValidBuffers()			{ return m_iValidBuffers; }
	// return: Anzahl der Valid Buffers in der Q

	void	Reset();
                                    
	~CDataQ();

                                    
public:	//------------------------ Write Operations -------------------------
	BOOL	PutData(LPDEFHDR lpHdr, LPBYTE lpData=NULL);
	// Ein Header (lpHdr) von der Grösse 16 Byte wird erst in den buffer kopiert. Danach die 
	// Daten von der Länge die in Hdr angegebei ist.  Falls kein Buffer frei, lpHdr ungültig, 
	// lpData ungültig oder wLen>Bufferlen-Hdrlen wird FALSE zurückgegeben.
	// Mit der Variable byNr in dem Header (Blocknummer), kann ein Nummer für diesen Buffer angegeben werden.
	// In der Fkt. IsData wird nach dieser byNr gesucht.
	
	LPBYTE	GetPtrOnPutBuf(WORD wStartPos);
	// Gibt ein Pointer auf den nächsten Put-Buffer.
	                                   
	                                   
public:	//------------------------ Read Operations -------------------------
	inline LPBYTE	GetData(WORD& wLen);	
	// return: Pointer auf Buffer. wLen=Datenlänge.  Pointer=NULL wenn kein Buffer valid.

	LPBYTE 	GetHeader()	const				
	{ 
		return m_queueBlockArray[R]->pData; 
	}
	// return: Pointer auf den aktuellen Header
	
	void	MarkBuffer()				
	{ 
		m_queueBlockArray[R]->bGetData = TRUE; 
	}
	// Markiert den aktuellen Readbuffer, d. h. die Daten sind in Verarbeitung.
	
	BOOL 	FreeBuffer(BYTE byNumber);
	// Gibt ein Buffer frei wenn byNumber mit Blocknummer (byNr) in einem Buffer übereinstimmt.
	// RETURN TRUE wenn buffer gefunden.
};  

inline 	LPBYTE	CDataQ::GetPtrOnPutBuf(WORD wStartPos)	
{	
	LPBYTE	p = NULL;
	if (m_queueBlockArray[W]->wLen==0){
		p = &(m_queueBlockArray[W]->pData[wStartPos]);
	} else {
		// OOPS no error msg
	}
	return p;
}

inline LPBYTE CDataQ::GetData(WORD& wLen)
{                                
	if (m_bCreateOk)
	{                            
		WORD i = R;
		while (m_queueBlockArray[R]->wLen==0 
				|| m_queueBlockArray[R]->bGetData)		// Sind keine Daten vorhanden ?
		{												// d.h. wenn Datenlänge = 0 oder
			R++;										// sind die Daten schon gehohlt worden?
			R %= m_wQLen;
			if (R==i) {
				wLen=0;
				return NULL;							// Keine Daten vorhanden.   
			}
		}                                   
		wLen = m_queueBlockArray[R]->wLen;
		return(m_queueBlockArray[R]->pData);
	}

	wLen=0;
	return NULL;
} 

inline CQueueBlock::CQueueBlock(WORD wBufSize)
{
	bGetData	=	FALSE;		// TRUE wenn die Daten dieses Buffers schon won GetData gehohlt worden sind.
	wLen		=	0;			// Die Länge der Daten im Buffer.
	m_wMaxLen = wBufSize;
	pData = new BYTE[wBufSize];
}

inline CQueueBlock::~CQueueBlock()
{
	WK_DELETE_ARRAY(pData);
}

inline BOOL CQueueBlock::FillFromHeader(LPDEFHDR lpHdr, LPBYTE lpData)
{
	if (wLen==0 && lpHdr->wDataLen<=(m_wMaxLen-(HDRSIZE)))
	{
		int nLen = HDRSIZE + lpHdr->wDataLen;
		wLen =  (WORD)nLen;
		
		memmove(pData, lpHdr, HDRSIZE);				
		
		if (lpData!=NULL && lpHdr->wDataLen!=0){
			memmove(&pData[HDRSIZE],
					lpData, 
					lpHdr->wDataLen
					);				
		}
		bGetData = FALSE;                                                
		return TRUE;
	} else {
#if 0
		// no message, it seems to be a valid state
		if (wLen) {
			WK_TRACE_ERROR(_T("PutData failed already have data %d\n"),wLen);
		} else {
			WK_TRACE_ERROR(_T("PutData failed len %d > %d\n"),
				lpHdr->wDataLen,
				m_wMaxLen-(HDRSIZE+lpHdr->byHdrLen)
				);
		}
#endif
		return FALSE;
	}
}
#endif

