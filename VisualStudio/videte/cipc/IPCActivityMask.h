// IPCActivityMask.h: interface for the CIPCActivityMask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCACTIVITYMASK_H__A833F963_D41C_11D5_9A60_004005A19028__INCLUDED_)
#define AFX_IPCACTIVITYMASK_H__A833F963_D41C_11D5_9A60_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
class CIPC;
class CIPCExtraMemory;
//////////////////////////////////////////////////////////////////////
#define AM_UPPER_BOUNDARY	(WORD)(1000)
#define AM_LOWER_BOUNDARY	(WORD)0

// Ist dieser Header am Beginn der Maskendatein vorhanden, so ist dies eine
// Maskendatei neuen Types.
#define MAGIC_FILE_MASK_HEADER 0x11071964
#define MASK_VERSION	1

//////////////////////////////////////////////////////////////////////
typedef enum
{
	AM_INVALID		= 0,
	AM_PERMANENT	= 1,
	AM_ADAPTIVE		= 2
} MaskType;
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCActivityMask  
{
	// construction / destruction
public:
	CIPCActivityMask(MaskType eType);
	CIPCActivityMask();
	CIPCActivityMask(const CIPCActivityMask& src);
	CIPCActivityMask(CSize s, MaskType eType, WORD* pMask = NULL,BYTE bVersion = 0);
	virtual ~CIPCActivityMask();
	BOOL Create(MaskType eType, int nXDim, int nYDim);

	// attributes
public:
	inline BYTE     GetVersion() const;
	inline int      GetWidth() const;
	inline int      GetHeight() const;
	inline CSize    GetSize() const;
	inline MaskType	GetType() const;
		   WORD     GetAt(int nX, int nY) const;

	// operators
public:
	const CIPCActivityMask& operator = (const CIPCActivityMask& src);

	// operations
public:
	void SetVersion(BYTE bVersion);
	BOOL SetAt(int nX, int nY, WORD wValue);
	void SetType(MaskType eType);

	void Invert();
	void Clear();

	// implementation
public:
	CIPCExtraMemory*	BubbleFromMask(CIPC *pCipc) const;
	BOOL				MaskFromBubble(const CIPCExtraMemory* pExtraMem);
	BOOL 				LoadMaskFromFile(const CString& sFileName);
	BOOL 				SaveMaskToFile(const CString& sFileName);

	// data member
private:
	BYTE	    m_bVersion;
	CSize		m_Size;
	WORD*		m_pMask;
	MaskType	m_eType;
};
//////////////////////////////////////////////////////////////////////
inline BYTE CIPCActivityMask::GetVersion() const
{
	return m_bVersion;
}
//////////////////////////////////////////////////////////////////////
inline int CIPCActivityMask::GetWidth() const
{
	return m_Size.cx;
}
//////////////////////////////////////////////////////////////////////
inline int CIPCActivityMask::GetHeight() const
{
	return m_Size.cy;
}
//////////////////////////////////////////////////////////////////////
inline CSize CIPCActivityMask::GetSize() const
{
	return m_Size;
}
//////////////////////////////////////////////////////////////////////
inline MaskType	CIPCActivityMask::GetType() const
{
	return m_eType;
}
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_IPCACTIVITYMASK_H__A833F963_D41C_11D5_9A60_004005A19028__INCLUDED_)
