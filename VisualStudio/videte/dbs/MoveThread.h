// MoveThread.h: interface for the CMoveThread class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CMoveThread : public CWK_Thread
{
public:
	CMoveThread(void);
	virtual ~CMoveThread(void);
	
	// attributes
public:

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
		    void AddArchiveID(CSecID id);

private:
	CDWordArray m_dwArchiveIDs;
};
