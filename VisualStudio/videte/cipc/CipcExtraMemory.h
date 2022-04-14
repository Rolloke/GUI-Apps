// FILE: $Workfile: CipcExtraMemory.h $ from $Archive: /Project/CIPC/CipcExtraMemory.h $
// VERSION: $Date: 10.05.05 12:32 $ $Revision: 31 $
// LAST:	$Modtime: 10.05.05 12:05 $ by $Author: Rolf.kary-ehlers $
//$Nokeywords:$

#ifndef _CIPCExtraMemory_H
#define _CIPCExtraMemory_H

// utility class to get reference counting for chunks of extra memory
// handed by CIPCExtraMemory

class CIPC;				// forward declaration
class CIPCChannel;		// forward declaration
class CIPCExtraMemory;	// forward declaration
class CIPCFields;		// forward declaration

class CIPCExtraMemoryRecord 
{
	friend class CIPCExtraMemory;

	// construction
private:	
	CIPCExtraMemoryRecord() 
	{
		m_dwExtraMemoryLen=0;
		m_hExtraMemory=NULL;
		m_pExtraMemory=NULL;
		m_iRefCount=0;
		m_bIsMapped = FALSE;
	}

private:
	inline void Lock()
	{
		m_cs.Lock();
	}
	inline void Unlock()
	{
		m_cs.Unlock();
	}
	
	// data member
private:
	volatile int m_iRefCount;
	DWORD	m_dwExtraMemoryLen;
	HANDLE	m_hExtraMemory;
	void*	m_pExtraMemory;
	BOOL	m_bIsMapped;
	CCriticalSection m_cs;
};

class AFX_EXT_CLASS CIPCExtraMemory 
{
	// construction / destruction
public:
	CIPCExtraMemory();
	// internal, copy from readChannel
	CIPCExtraMemory(const CIPCChannel *pReadChannel );
	// copy constructor
	CIPCExtraMemory(const CIPCExtraMemory &src);
	virtual ~CIPCExtraMemory();

	// attributes
public:
	const   void* GetAddress() const;
			void* GetAddressForWrite() const;
	inline	DWORD GetLength() const;
	inline	HANDLE GetHandle() const;
	inline   const CIPC*  GetCIPC() const;
	
			// special access:
			CString		  GetString() const;
			CStringArray* GetStrings() const;
			static	int GetCreatedBubbles();
	
	// operations
public:
	// real creation
	BOOL Create(const CIPC *pOneCipc, DWORD dwLength, const void *pData=NULL);
#ifdef _UNICODE
	BOOL Create(const CIPC *pOneCipc, const CString &sString);
#else
	BOOL Create(const CIPC *pOneCipc, const CString &sString);
	BOOL Create(const CIPC *pOneCipc, LPCTSTR szBuffer);
#endif	
	BOOL Create(const CIPC *pOneCipc, const CStringArray &someStrings);

	// bubble help code
public:
	static void BYTE2Memory(BYTE*& pByte, BYTE b);
	static void WORD2Memory(BYTE*& pByte, WORD w);
	static void DWORD2Memory(BYTE*& pByte, DWORD dw);
	static void ULARGE_INTEGER2Memory(BYTE*& pByte, ULARGE_INTEGER ul);
#ifdef _UNICODE
	static WORD CString2Memory(BYTE*& pByte, const CString &s, WORD);
#else
	static void CString2Memory(BYTE*& pByte, const CString &s);
#endif

	static BYTE  Memory2BYTE(const BYTE*& pByte);
	static WORD  Memory2WORD(const BYTE*& pByte);
	static DWORD Memory2DWORD(const BYTE*& pByte);
	static ULARGE_INTEGER Memory2ULARGE_INTEGER(const BYTE*& pByte);
#ifdef _UNICODE
	static CString Memory2CString(const BYTE*& pByte, WORD);
#else
	static CString Memory2CString(const BYTE*& pByte);
#endif
	// implementation
private:
	BOOL Allocate(const CIPC *pOneCipc, DWORD dwLength);
	BOOL SetMemoryFrom(const void *pData, DWORD dwDataLen);
	void DoMapping() const;
	void CheckMax();
	
	// data member
private:
	const CIPC*	m_pCipc;
	CIPCExtraMemoryRecord *m_pRecord;
	BOOL m_bReadOnly;
	//
	volatile static LONG m_iNumCreated;
	static int m_iMaxCreated;
	volatile static DWORD m_dwGlobalCounter;	// for unique ids
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIPCExtraMemory::GetLength() const 
{ 
	return m_pRecord->m_dwExtraMemoryLen; 
}
/////////////////////////////////////////////////////////////////////////////
inline HANDLE CIPCExtraMemory::GetHandle() const 
{ 
	return m_pRecord->m_hExtraMemory; 
}
inline const CIPC* CIPCExtraMemory::GetCIPC() const 
{ 
	return m_pCipc;
}

#endif
