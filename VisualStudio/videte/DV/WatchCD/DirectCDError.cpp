// DirectCDError.cpp: implementation of the CDirectCDError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WatchCD.h"
#include "DirectCDError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCDError::CDirectCDError()
{
	m_bErrorOccured = FALSE;
	m_ErrorStatus = ERROR_NO_ERROR;

}

CDirectCDError::~CDirectCDError()
{

}

//////////////////////////////////////////////////////////////////////
void CDirectCDError::ChangeErrorStatus(DirectCDErrorStatus newError)
{
	DirectCDErrorStatus LastErrorStatus = m_ErrorStatus;
	m_ErrorStatus = newError;

	WK_TRACE(_T("CDirectCDError() DCDErrorStatus: from last %s to new %s\n")
			,NameOfEnumDirectCDError(LastErrorStatus)
			,NameOfEnumDirectCDError(m_ErrorStatus));
}

//////////////////////////////////////////////////////////////////////
void CDirectCDError::SetError(BOOL bError)
{
	m_bErrorOccured = TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDError::IsError()
{
	return m_bErrorOccured;
}

