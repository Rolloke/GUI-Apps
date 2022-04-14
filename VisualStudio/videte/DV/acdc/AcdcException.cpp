// AcdcException.cpp: implementation of the CAcdcException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "AcdcException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcdcException::CAcdcException(WK_ApplicationId AppId, PVOID pContext) 
: CUnhandledException(AppId, pContext)
{
	m_bHandleCurrentException = FALSE;
}

CAcdcException::~CAcdcException()
{

}

///////////////////////////////////////////////////////////////////////////////////////
LONG CAcdcException::OnExceptionHandler(LPEXCEPTION_POINTERS pExc)
{
	CAcdcApp		*pApp			= NULL;
	CAcdcDlg		*pAcdcDlg		= NULL;
	
	WK_TRACE(_T("There is an exception in the exceptionhandling\n"));
	// Wird gerade eine Exception bearbeitet?
	if (!m_bHandleCurrentException)
	{
		m_bHandleCurrentException = TRUE;

		CString sMsg = GetExceptionErrorString(pExc);

		WK_TRACE(_T(">>>>>>>>>> OnExceptionHandler <<<<<<<<<<<<<<\n"));

		CWK_RunTimeError RTerr(WAI_AC_DC, REL_ERROR, RTE_OS_EXCEPTION, sMsg, 0, 0);
		RTerr.Send();

		pApp =  (CAcdcApp*)GetContext();
		if (pApp)
			pAcdcDlg = pApp->GetAcdcDlg();

		// Dialog schließen und damit ACDC aufräumen
		if (pAcdcDlg)
			pAcdcDlg->OnException();
	}
	else
	{
		WK_TRACE_ERROR(_T("There is an exception in the exceptionhandling...terminating\n"));
	}

	m_bHandleCurrentException = FALSE;


	return EXCEPTION_EXECUTE_HANDLER;
}