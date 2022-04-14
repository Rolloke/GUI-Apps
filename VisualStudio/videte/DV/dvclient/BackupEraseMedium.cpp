// BackupEraseMedium.cpp: implementation of the CBackupEraseMedium class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BackupEraseMedium.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackupEraseMedium::CBackupEraseMedium(CString sDriveToErase)
: CWK_Thread("BackupEraseMediumThread",NULL)
{
	m_sDriveToErase = sDriveToErase;
	m_bAlreadyErased = FALSE;
}

CBackupEraseMedium::~CBackupEraseMedium()
{

}


//////////////////////////////////////////////////////////////////////
BOOL CBackupEraseMedium::Run(LPVOID lpContext)
{
	DeleteDirRecursiv(m_sDriveToErase);
	m_bAlreadyErased = TRUE;
	return FALSE; // no longer run
}

//////////////////////////////////////////////////////////////////////
BOOL CBackupEraseMedium::IsAlreadyErased()
{
	return m_bAlreadyErased;
}
