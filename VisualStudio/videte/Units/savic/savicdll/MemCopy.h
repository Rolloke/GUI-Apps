// MemCopy.h: interface for the CMemCopy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMCOPY_H__75506668_1A11_4946_87D2_054AB6E92CC2__INCLUDED_)
#define AFX_MEMCOPY_H__75506668_1A11_4946_87D2_054AB6E92CC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Siehe: http://www.sgi.com/developers/technology/irix/resources/asc_cpu.html

class CMemCopy  
{
public:
	CMemCopy();
	virtual ~CMemCopy();

	void FastCopy(void* pDest, void* pSrc, DWORD dwLen);
	// AMD 1200MHz, 100MHz SDRAM -> 183MByte/Sekunde

protected:
	BOOL HasMMX();
	BOOL SupportCheckForSFence();

	void FastCopy1(void* pDest, void* pSrc, DWORD dwLen);
	// AMD 1200MHz, 100MHz SDRAM -> 472MByte/Sekunde

private:
	BOOL	m_bSFenc2;
	BOOL	m_bMMX;
};

#endif // !defined(AFX_MEMCOPY_H__75506668_1A11_4946_87D2_054AB6E92CC2__INCLUDED_)
