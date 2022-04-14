// NeroThread.cpp: implementation of the CNeroThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "NeroThread.h"
#include "PARAMETERS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeroThread::CNeroThread()
: CWK_Thread(_T("BurnThread"),NULL)
{
	m_pProducer = new CProducer();
	m_pNeroBurn = new CNeroBurn(m_pProducer);
	m_MainFunktion = MF_START;

}

CNeroThread::~CNeroThread()
{
	WK_DELETE(m_pNeroBurn);
	WK_DELETE(m_pProducer);
}


/*********************************************************************************************
 Class		   : CNeroThread
 Function      : Run 
 Description   : Run function of the Nero thread. As a function of the current MainFunction, 
				 which is delivered from CIPC, this function controls the NeroAPI function
				 like erasing, burning, ect.

 Parameters: 
  lpContext (I): has to be here but is unused (LPVOID)

 Result type:  returns TRUE if this function should be run again  (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE Run >
 <GROUP CNeroThread>
 <TOPICORDER 0>
 <KEYWORDS CNeroThread, Run>
*********************************************************************************************/
BOOL CNeroThread::Run(LPVOID lpContext)
{
	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		switch(m_MainFunktion)
		{
			case MF_START:
				//first get all available CD-RW drives
				m_pNeroBurn->BurnThread(MF_CD_DRIVES);
				TRACE(_T("Start running\n"));
				m_MainFunktion = MF_COMPLETE;
				break;

			case MF_CD_INFO:
				//first get all available CD-RW drives
				m_pNeroBurn->BurnThread(MF_CD_DRIVES);

				//start specified function
				m_pNeroBurn->BurnThread(m_MainFunktion);
				m_MainFunktion = MF_COMPLETE;
				break;

			case MF_EJECT:
			case MF_CD_WRITE_ISO:
			case MF_CD_WRITE_VCD:
			case MF_CD_WRITE_SVCD:
				//start specified function
				m_pNeroBurn->BurnThread(m_MainFunktion);
				m_MainFunktion = MF_COMPLETE;
				break;

			case MF_COMPLETE:
				break;
			default:
				break;
		}
	}

	// run again
	return TRUE;
}

/*********************************************************************************************
 Class		   : CNeroThread
 Function      : SetMainFunktion 
 Description   : Set the MainFunction which is delivered from CIPC.

 Parameters: 
  mf (I): main function indicates, what`s to do next (erase, burn, ect.) (enum MainFunction)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetMainFunktion >
 <GROUP CNeroThread>
 <TOPICORDER 0>
 <KEYWORDS CNeroThread, SetMainFunktion>
*********************************************************************************************/
void CNeroThread::SetMainFunktion(MainFunction mf)
{
	m_MainFunktion = mf;
}

/*********************************************************************************************
 Class		   : CNeroThread
 Function      : StartThread 
 Description   : This function starts the Nero thread.

 Parameters: --

 Result type:  TRUE, if thread was started (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE StartThread >
 <GROUP CNeroThread>
 <TOPICORDER 0>
 <KEYWORDS CNeroThread, StartThread>
*********************************************************************************************/
BOOL CNeroThread::StartThread()
{
	return CWK_Thread::StartThread();
}

/*********************************************************************************************
 Class		   : CNeroThread
 Function      : StopThread 
 Description   : This function stopps the Nero thread.

 Parameters: --

 Result type:  TRUE, if thread was stopped (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE StopThread >
 <GROUP CNeroThread>
 <TOPICORDER 0>
 <KEYWORDS CNeroThread, StopThread>
*********************************************************************************************/
BOOL CNeroThread::StopThread()
{
	TRACE(_T("StopThread angekommen\n"));

	return CWK_Thread::StopThread();
}

/*********************************************************************************************
 Class		   : CNeroThread
 Function      : GetNeroBurn 
 Description   : This function returns a pointer to the object, which is running within
				 this NeroThread.

 Parameters: --

 Result type:  Pointer to the object which runs within this Nero thread (CNeroBurn*)

 Author: TKR
 created: August, 02 2002
 <TITLE GetNeroBurn >
 <GROUP CNeroThread>
 <TOPICORDER 0>
 <KEYWORDS CNeroThread, GetNeroBurn>
********************************************************************************************/
CNeroBurn* CNeroThread::GetNeroBurn()
{
	return m_pNeroBurn;
}

