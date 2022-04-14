/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CTashaEception.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CTashaEception.cpp $
// CHECKIN:		$Date: 29.03.04 9:37 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 29.03.04 8:41 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CTashaException.h"
#include "TashaUnit.h"
#include "TashaUnitDlg.h"

///////////////////////////////////////////////////////////////////////////////////////
CTashaException::CTashaException(WK_ApplicationId AppId, PVOID pContext) : CUnhandledException(AppId, pContext)
{
	m_bHandleCurrentException = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
LONG CTashaException::OnExceptionHandler(LPEXCEPTION_POINTERS pExc)
{
	CTashaUnitApp	*pApp			= NULL;
	CTashaUnitDlg	*pTashaUnitDlg	= NULL;
	
	// Wird gerade eine Exception bearbeitet?
	if (!m_bHandleCurrentException)
	{
		m_bHandleCurrentException = TRUE;

		CString sMsg = GetExceptionErrorString(pExc);

		WK_TRACE(_T(">>>>>>>>>> OnExceptionHandler <<<<<<<<<<<<<<\n"));

		CWK_RunTimeError RTerr(REL_ERROR, RTE_OS_EXCEPTION, sMsg, 0, 0);
		RTerr.Send();

		pApp =  (CTashaUnitApp*)GetContext();
		if (pApp)
		{
			pTashaUnitDlg = (CTashaUnitDlg*)pApp->m_pMainWnd;
				 			 
			// Dialog schließen und damit die TashaUnit aufräumen
			if (pTashaUnitDlg)
				pTashaUnitDlg->OnDestroy();

			// Den Rest aufräumen.
			pApp->CleanUp();
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("There is an exception in the exceptionhandling...terminating\n"));
		
		// Den Rest aufräumen.
		if (pApp)
			pApp->CleanUp();
	}

	m_bHandleCurrentException = FALSE;

	return EXCEPTION_EXECUTE_HANDLER;
}
