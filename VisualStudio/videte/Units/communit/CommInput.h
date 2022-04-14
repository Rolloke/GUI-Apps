// GAInput.h
// Author : Uwe Freiwald

// Definition of CGAInput
// 

#ifndef _CCommInput_H_
#define _CCommInput_H_

class CCommInput : public CIPCInput
{
	// construction / destruction
public:
	CCommInput(LPCTSTR shmName);
	~CCommInput();
	
	// member variables access
public:
	WORD GetGroupID() const {return m_wGroupID;};

	// operations
public:
	void IndicateData(LPVOID pBuffer,DWORD dwLen);

	LPVOID  GetWriteBuffer() const;
	DWORD   GetWriteBufferLength() const;
	void    DeleteWriteBuffer();
	// overridables
public:	
	// requests
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);

	virtual void OnRequestDisconnect();

	virtual void OnWriteCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									);
	virtual void OnRequestCameraControl(
								CSecID commID, CSecID camID,
								CameraControlCmd cmd,
								DWORD dwValue
								);
	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					);
	// private data
private:
	WORD   m_wGroupID;
	DWORD  m_dwBlockNr;
	LPVOID m_pWriteBuffer;
	DWORD  m_dwBufferLen;

};
////////////////////////////////////////////////////////////
inline LPVOID CCommInput::GetWriteBuffer() const
{
	return m_pWriteBuffer;
}
////////////////////////////////////////////////////////////
inline DWORD CCommInput::GetWriteBufferLength() const
{
	return m_dwBufferLen;
}
////////////////////////////////////////////////////////////
#endif	// _CCommInput_H_