/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RawDataArray.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/RawDataArray.h $
// CHECKIN:		$Date: 20.01.06 12:11 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 20.01.06 10:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CRawDataArray
#define _CRawDataArray

class CRawDataArray
{
public:
	inline CRawDataArray();
	inline ~CRawDataArray();
	//
	inline BYTE GetAt(int ix) const;
	inline BYTE operator[](int ix) const;
	inline int GetSize() const;
	inline const BYTE *GetData() const;
	//
	inline void Append(const BYTE *pData,int iDataLen);
	inline void RemoveHead(int iLen);
	inline void Clear();
	//
	inline int GetFFFFIndex() const;
	//
	BOOL m_bDoTrace;
private:
	inline void Init();
	inline void Alloc(int iDataLen);
	//
	int  m_iLen;
	int m_iMaxLen;
	int m_iFFFFIndex; // updated in Append and RemoveHead
	BYTE *m_pData;

};

inline void CRawDataArray::Init()
{
	m_iLen = 0;
	m_iMaxLen = 0;
	m_pData = NULL;
	m_iFFFFIndex = -1;
	m_bDoTrace = FALSE;
}
inline CRawDataArray::CRawDataArray()
{
	Init();
}
inline void CRawDataArray::Alloc(int iDataLen)
{
	if (iDataLen>m_iMaxLen) {
		BYTE *pOldData = m_pData;	// save old data
		m_pData = new BYTE[iDataLen];
		if (pOldData) {
			for (int i=0;i<m_iLen;i++) {
				m_pData[i] = pOldData[i];
			}
			WK_DELETE_ARRAY(pOldData);	// drop after saved
		} else {
			// no data in there, no need to copy
		}
		if (m_bDoTrace && m_iMaxLen) {
			WK_TRACE(_T("raw data grown to %d\n"),iDataLen);
		}
		m_iMaxLen = iDataLen;
	} else {
		// already have enough space
	}
}

inline CRawDataArray::~CRawDataArray()
{
	WK_DELETE_ARRAY(m_pData);
	m_iLen = 0;
	m_iMaxLen = 0;
}
//
inline BYTE CRawDataArray::GetAt(int ix) const
{
	if (ix<m_iLen) {
		return m_pData[ix];
	} else {
		WK_TRACE_ERROR(_T("Index error %d not within 0..%d\n"),ix,m_iLen);
		return 0;
	}
}
inline BYTE CRawDataArray::operator[](int ix) const
{
	if (ix<m_iLen) {
		return m_pData[ix];
	} else {
		WK_TRACE_ERROR(_T("Index error %d not within 0..%d\n"),ix,m_iLen);
		return 0;
	}
}


inline int CRawDataArray::GetSize() const
{
	return m_iLen;
}
inline const BYTE *CRawDataArray::GetData() const
{
	return m_pData;
}

inline void CRawDataArray::Append(const BYTE *pData,int iDataLen)
{
	Alloc(m_iLen+iDataLen);
	// on the fly update of m_iFFFFIndex
	// if it's not already != -1
	int ix = m_iLen;	// write index is same as m_iLen+i
	for (int i=0;i<iDataLen;i++) {
		m_pData[ix] = pData[i];
		// is it an FF and index not set yet
		if (m_pData[ix]==0xff) {
			if (m_iFFFFIndex == -1) {
				// look left so even FF|FF is detected
				int ixLeft = ix-1;
				if ( ixLeft>=0) {
					if (m_pData[ixLeft] == 0xff) {
						m_iFFFFIndex = ixLeft;
					}
				}
			} else {
				// already have one
			}
		}
		ix++;
	}
	m_iLen += iDataLen;
}
inline void CRawDataArray::RemoveHead(int iLen)
{
	if (iLen<=m_iLen) {
		// OOPS CopyMemory or MoveMemory
		for (int i=0;i<m_iLen-iLen;i++) {
			m_pData[i] = m_pData[i+iLen];
		}
		// CopyMemory(m_pData,m_pData+iLen,m_iLen-iLen);
		m_iLen -= iLen;
		// now take care of m_iFFFFIndex
		if (m_iFFFFIndex != -1) {
			// in the remove block or behind it ?
			if (m_iFFFFIndex <= iLen) {
				// it was in the removed block so there can be new one afterwards
				m_iFFFFIndex = -1;	// reset
				// scan the remaining data
				for (int f=0;m_iFFFFIndex == -1 && f<m_iLen-1;f++) {
					if (m_pData[f]==0xff && m_pData[f+1]==0xff) {
						m_iFFFFIndex = f;
					}
				}
			} else {
				// it was after the removed block, move the index left too
				m_iFFFFIndex -= iLen;
				if (m_iFFFFIndex < 0) {
					WK_TRACE_ERROR(_T("FFFFIndex error %d\n"),m_iFFFFIndex);
					m_iFFFFIndex = -1;	// OOPS
				}
			}
		} else {
			// there was none and after remove there won't be one
		}
	} else {
		WK_TRACE_ERROR(_T("RemoveHead %d > len %d\n"),iLen,m_iLen);
		Clear();
	}
}

inline int CRawDataArray::GetFFFFIndex() const
{
	return m_iFFFFIndex;
}

inline void CRawDataArray::Clear()
{
	WK_DELETE_ARRAY(m_pData);
	m_iLen=0;
	m_iMaxLen=0;
	m_iFFFFIndex = -1;
}

#endif