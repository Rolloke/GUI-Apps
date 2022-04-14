// inlined functions:
//
inline SecOutputType COutput::GetOutputType() const
{
	return (m_outputType);
}

inline const CString &COutput::GetName() const
{
	return (m_sName);
}

inline CSecID COutput::GetID() const
{
	return (m_id);
}
inline BOOL COutput::IsFixed() const
{
	return (m_bIsFixed);
}
inline BOOL COutput::IsReference() const
{
	return (m_bReference);
}
inline BOOL COutput::IsTerminated() const
{
	return m_bTermination;
}


inline RelayControlType  COutput::GetRelayControlType() const
{
	return m_clientRelayControl;
}
inline DWORD COutput::GetRelayClosingTime() const
{
	return m_dwRelayClosingTime;
}
inline BOOL COutput::ClientMayKillRelayProcess() const
{
	return m_bClientMayKillRelayProcess;
}
inline BOOL COutput::KeepClosedAfterDisconnect() const
{
	return m_bKeepRelayClosedAfterClientDisconnect;
}
inline CameraControlCmd COutput::GetPTZCmd() const
{
	return m_ePTZCmd;
}
inline int COutput::GetPTZInterface() const
{
	return m_iPTZInterface;
}
inline CSecID COutput::GetCurrentArchiveID() const
{
	return m_idCurrentArchive;
}
inline CIPCOutputServer* COutput::GetGroup()
{
	return m_pParent;
}
