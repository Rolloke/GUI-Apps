#include "stdafx.h"
#include "dbs.h"
#include "movethread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMoveThread::CMoveThread() : CWK_Thread(_T("MoveThread"))
{
	m_dwStatTime = 24*60*60*1000;
}
//////////////////////////////////////////////////////////////////////
CMoveThread::~CMoveThread()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CMoveThread::Run(LPVOID lpContext)
{
	if (m_dwArchiveIDs.GetSize())
	{
		CSecID idArchive = m_dwArchiveIDs.GetAt(0);
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(idArchive.GetSubID());
		if (pArchiv)
		{
			CArchiv* pPreAlarm = pArchiv->GetPreAlarm();
			if (   pPreAlarm
				&& pPreAlarm->IsPreAlarm())
			{
				pArchiv->MoveSequences(pPreAlarm);
			}
		}


		m_dwArchiveIDs.RemoveAt(0);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CMoveThread::AddArchiveID(CSecID id)
{
	m_dwArchiveIDs.Add(id.GetID());
}

