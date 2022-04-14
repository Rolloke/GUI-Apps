#ifndef _CIPCChannel_H
#define _CIPCChannel_H


#include "Cipc.h"

typedef struct {
	DWORD	dwMagicNumber;
	//
	DWORD	dwCmd;		// >0 Data vorhanden, Kommando noch nicht abgearbeitet
	DWORD	dwParam1;
	DWORD	dwParam2;
	DWORD	dwParam3;
	DWORD	dwParam4;
	// extra memory data
	DWORD	dwExtraMemUsed;
	HANDLE	hExtraMemory;
	// process/thread data
	DWORD	dwProcessID;	// set in the WriteChannel by the constructor
	HANDLE  hThread;	// set in the WriteChannel by the constructor
	DWORD	dwVersion;
} CIPC_SHM_STRUCT;

// hidden class, only used in this cfile
class CIPCChannel 
{
	// construction / destruction
public:
	CIPCChannel(CIPC *pParent);
	virtual ~CIPCChannel();

	// attributes
public:
	inline CIPC* GetParent() const;

	// functions:
public:
	BOOL CreateFileMapping(LPCTSTR szMutex,
							LPCTSTR szShmName, 
							BOOL bReadAndWrite,
							BOOL asMaster);
	BOOL CreateEvents(const CString &shmName, 
					  BOOL bReadAndWrite,
					  BOOL asMaster);
	//
	void AccessMutex();
	void ReleaseMutex();

	void SetCmd(const CIPCCmdRecord& cmd);
	CIPCCmdRecord* GetCmd();

protected:
	//
	void CreateExtraMemory(const CIPCExtraMemory &src);
	void ReleaseExtraMemory();
	
	// data:
private:
	CIPC*				m_pParent;
	// cmd shared memory
	HANDLE				m_hMutex;
	HANDLE				m_hMap;		// result from CreateFileMapping or OpenFileMapping
	CIPC_SHM_STRUCT*	m_pSharedMemoryStruct;	// result from MapViewOfFile
	
	// for read channel only
	HANDLE				m_hWriteProcess;
	// events:
	HANDLE				m_hEventCmdSend;	// signaled in WriteCmd
	HANDLE				m_hEventCmdDone;	// signaled in ReadCmd
	HANDLE				m_hEventDisconnect;	// signaled in destructor
	//
	// extra memory
	CIPCExtraMemory*	m_pChannelExtraMemory;

	friend class CIPC;
	friend class CIPCExtraMemory;
};
///////////////////////////////////////////////////
inline CIPC* CIPCChannel::GetParent() const
{
	return m_pParent;
}


#endif
