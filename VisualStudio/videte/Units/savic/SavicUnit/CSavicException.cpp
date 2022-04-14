/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CSavicException.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CSavicException.cpp $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 12:20 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSavicException.h"
#include "SavicUnitApp.h"
#include "SavicUnitDlg.h"

///////////////////////////////////////////////////////////////////////////////////////
CSaVicException::CSaVicException(WK_ApplicationId AppId, PVOID pContext) : CUnhandledException(AppId, pContext)
{
	m_bHandleCurrentException = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
LONG CSaVicException::OnExceptionHandler(LPEXCEPTION_POINTERS pExc)
{
	CSaVicApp		*pApp			= NULL;
	CSaVicUnitDlg	*pSaVicUnitDlg	= NULL;
	
	// Wird gerade eine Exception bearbeitet?
	if (!m_bHandleCurrentException)
	{
		m_bHandleCurrentException = TRUE;

		CString sMsg = GetExceptionErrorString(pExc);

		WK_TRACE(_T(">>>>>>>>>> OnExceptionHandler <<<<<<<<<<<<<<\n"));

		CWK_RunTimeError RTerr(REL_ERROR, RTE_OS_EXCEPTION, sMsg, 0, 0);
		RTerr.Send();

		pApp =  (CSaVicApp*)GetContext();
		if (pApp)
			pSaVicUnitDlg = pApp->GetSaVicUnitDlg();
				 			 
		// Dialog schließen und damit die SaVicUnit aufräumen
		if (pSaVicUnitDlg)
			pSaVicUnitDlg->OnDestroy();

		// Den Rest aufräumen.
		if (pApp)
			pApp->CleanUp();
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
