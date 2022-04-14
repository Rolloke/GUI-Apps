/////////////////////////////////////////////////////
inline CIPCDatabaseDVClient* CServer::GetDatabase()
{
	return m_pDatabase;
}
/////////////////////////////////////////////////////
inline CIPCOutputDVClient* CServer::GetOutput()
{
	return m_pOutput;
}
/////////////////////////////////////////////////////
inline CIPCInputDVClient* CServer::GetInput()
{
	return m_pInput;
}
/////////////////////////////////////////////////////
inline CIPCDataCarrierClient* CServer::GetDataCarrier()
{
	return m_pDataCarrier;
}
//////////////////////////////////////////////////////////////////////////
inline CIPCAudioDVClient* CServer::GetAudio()
{
	return m_pAudio;
}
/////////////////////////////////////////////////////
inline const CString& CServer::GetHostName()
{
	return m_sHostName;
}
/////////////////////////////////////////////////////
inline const CString& CServer::GetHostNumber()
{
	return m_sHostNumber;
}
/////////////////////////////////////////////////////
inline int CServer::GetMajorVersion() const
{
	return m_iMajorVersion;
}
/////////////////////////////////////////////////////
inline int CServer::GetMinorVersion() const
{
	return m_iMinorVersion;
}
/////////////////////////////////////////////////////
inline int CServer::GetSubVersion() const
{
	return m_iSubVersion;
}
/////////////////////////////////////////////////////
inline int CServer::GetBuildNr() const
{
	return m_iBuildNr;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanRectangleQuery() const
{
	// TRUE wenn Version  größer 1.4.1. oder größer 1.5.x. oder größer 2.x.x
	BOOL b = (    m_iMajorVersion == 1 
				&& ((m_iMinorVersion==4 && m_iSubVersion > 1) || m_iMinorVersion > 4)
				|| m_iMajorVersion>1);

	return b;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanSystemInfo() const
{
	// TRUE wenn Version  größer 2.1.2.589
	return  m_iBuildNr > 589;
/*	BOOL b = (    m_iMajorVersion == 2 
				&& ((m_iMinorVersion==1 && m_iSubVersion > 1) || m_iMinorVersion > 1)
				|| m_iMajorVersion>2);
	return b;
*/}

/////////////////////////////////////////////////////
inline BOOL CServer::CanMilliSeconds() const
{
	// TRUE wenn Version  größer 1.2.0.380
	return m_iBuildNr>380;
/*	BOOL b = (    m_iMajorVersion == 1 
				&& ((m_iMinorVersion==2 && m_iSubVersion >= 0) || m_iMinorVersion > 2)
				|| m_iMajorVersion>1);
	return b;
*/}
/////////////////////////////////////////////////////
inline BOOL CServer::CanAlarmList() const
{
	// TRUE wenn Version  größer 1.4.0 
	BOOL b = (    m_iMajorVersion == 1 
				&& ((m_iMinorVersion==4 && m_iSubVersion >= 0) || m_iMinorVersion > 4)
				|| m_iMajorVersion>1);
	return b;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanActivityMask() const
{
	// TRUE wenn Version  größer 1.5.1
	BOOL b = (    m_iMajorVersion == 1 
				&& ((m_iMinorVersion==5 && m_iSubVersion > 0) || m_iMinorVersion > 5)
				|| m_iMajorVersion>1);
	return b;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanAlarmOutput() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanAlarmCall() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanMultiCDExport() const
{
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
}
/////////////////////////////////////////////////////
inline BOOL CServer::HasExpanionCode() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////
inline BOOL	CServer::IsAlarm() const
{
	return m_bAlarm;
}
/////////////////////////////////////////////////////
inline WORD CServer::GetAlarmCameraNr() const
{
	return m_ConnectionRecord.GetCamID().GetSubID();
}


