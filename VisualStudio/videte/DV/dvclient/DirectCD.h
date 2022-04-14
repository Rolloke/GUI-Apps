// DirectCD.h: interface for the CDirectCD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCD_H__CA571931_7DA4_11D4_AE57_004005A1D890__INCLUDED_)
#define AFX_DIRECTCD_H__CA571931_7DA4_11D4_AE57_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectCD  
{

// Construction
public:
	CDirectCD();
	virtual ~CDirectCD();


public:

	enum DCDErrorStatus
	{
		DCD_NO_ERROR						= 1,
		DCD_ERROR_START_WATCHCD				= 2,
		DCD_ERROR_WAIT_FOR_EMPTY_CDR		= 3,
		DCD_ERROR_FORMAT					= 4,
		DCD_ERROR_CANT_FIND_WATCHCD			= 5,
		DCD_ERROR_WAIT_FOR_FINALIZE_SUCCESS = 6,
	};

//Implementation
public:
	void ChangeDCDErrorStatus(DCDErrorStatus newStatus);
	BOOL ExecuteWatchCD(CString sDrive);

	CString GetErrorString();


//Implementation
protected:



// Implementation
private:
	CString NameOfEnumDCDErrorStatus(DCDErrorStatus Status);

// data member
private:
	DCDErrorStatus m_DCDErrorStatus;

};

#endif // !defined(AFX_DIRECTCD_H__CA571931_7DA4_11D4_AE57_004005A1D890__INCLUDED_)
