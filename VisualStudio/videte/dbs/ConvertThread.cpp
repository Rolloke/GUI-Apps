// ConvertThread.cpp: implementation of the CConvertThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "ConvertThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConvertThread::CConvertThread() : CWK_Thread(_T("ConvertThread"),NULL)
{

}
//////////////////////////////////////////////////////////////////////
CConvertThread::~CConvertThread()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CConvertThread::Run(LPVOID lpContext)
{
	int c;
	CAutoCritSec acs1(&theApp.m_Conversions.m_cs);
	c = theApp.m_Conversions.GetSize();
	acs1.Unlock();

	if (c==0)
	{
		return FALSE;
	}

	CAutoCritSec acs2(&theApp.m_Conversions.m_cs);
	CConversion* pConversion = theApp.m_Conversions.GetAt(0);
	acs2.Unlock();

	if (pConversion)
	{
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(pConversion->GetArchivNr());
		if (pArchiv)
		{
			if (!pArchiv->ConvertSequence(*pConversion))
			{
				WK_TRACE_ERROR(_T("sequence conversion failed\n"));
			}
			if (!pConversion->DeleteFiles())
			{
				WK_TRACE_ERROR(_T("sequence deleting failed\n"));
			}
		}
	}
	Sleep(10);

	CAutoCritSec acs3(&theApp.m_Conversions.m_cs);
	WK_DELETE(pConversion);
	theApp.m_Conversions.RemoveAt(0);
	acs3.Unlock();

	// run again
	return TRUE;
}

