// DirectCDError.h: interface for the CDirectCDError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCDERROR_H__9B790FA3_7EFE_11D4_AE60_004005A1D890__INCLUDED_)
#define AFX_DIRECTCDERROR_H__9B790FA3_7EFE_11D4_AE60_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectCDError  
{
	//Konstructor
public:

	CDirectCDError();
	virtual ~CDirectCDError();
	inline DirectCDErrorStatus GetErrorStatus();

	//Implementation
public:
	BOOL IsError();
	void SetError(BOOL bError);
	void ChangeErrorStatus(DirectCDErrorStatus newError);

private:
	DirectCDErrorStatus	m_ErrorStatus;
	BOOL		m_bErrorOccured;
};

//////////////////////////////////////////////////////////////////////
inline DirectCDErrorStatus CDirectCDError::GetErrorStatus()
{
	return m_ErrorStatus;
}

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_DIRECTCDERROR_H__9B790FA3_7EFE_11D4_AE60_004005A1D890__INCLUDED_)
