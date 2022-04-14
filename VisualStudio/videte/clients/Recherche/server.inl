//////////////////////////////////////////////////////////////////////
inline WORD CServer::GetID() const
{
	return m_wID;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CServer::GetHostID() const
{
	return m_idHost;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CServer::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CServer::GetKind() const
{
	return m_sKind;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CServer::IsLocal() const
{
	return m_bLocal;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CServer::IsDV() const
{
	return m_bDV;
}
//////////////////////////////////////////////////////////////////////
inline void CServer::SetDV()
{
	m_bDV = TRUE;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CServer::GetError() const
{
	return m_sConnectionError;
}
//////////////////////////////////////////////////////////////////////
inline CIPCDatabaseRecherche* CServer::GetDatabase() const
{
	return m_pDatabase;
}
//////////////////////////////////////////////////////////////////////
inline CIPCDataCarrierRecherche* CServer::GetDataCarrier() const
{
	return m_pDataCarrier;
}
//////////////////////////////////////////////////////////////////////
inline CRechercheDoc* CServer::GetDocument() const
{
	return m_pDoc;
}
//////////////////////////////////////////////////////////////////////
inline CDataBaseInfoDlg* CServer::GetInfoDialog() const
{
	return m_pDataBaseInfoDlg;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCFields& CServer::GetFields() const
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline CQueryArchives& CServer::GetQueryArchives()
{
	return m_QueryArchives;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CServer::IsSearching() const
{
	return m_bSearching;
}
//////////////////////////////////////////////////////////////////////
inline CIPCDrives& CServer::GetDrives()
{
	return m_Drives;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanRectangleQuery() const
{
	BOOL b = FALSE;

	if (IsDV())
	{
		b = (    m_iMajorVersion == 1 
			&& ((m_iMinorVersion==4 && m_iSubVersion > 1) || m_iMinorVersion > 4)
			|| m_iMajorVersion>1);
	}
	else
	{
		b = (    m_iMajorVersion == 4 
			&& (m_iMinorVersion > 2)
			|| m_iMajorVersion>4);
	}
	return b;
}
/////////////////////////////////////////////////////
inline BOOL CServer::CanSplittingBackup() const
{
	BOOL b = FALSE;

	if (IsDV())
	{
		// Remote, not yet implemented
	}
	else
	{
#ifdef _DEBUG
		b = (   ((m_iMajorVersion == 4) && (m_iMinorVersion > 3))
			 || ((m_iMajorVersion == 4) && (m_iMinorVersion == 3) && (m_iBuildNr >= 488))
			 || m_iMajorVersion > 4
			 );
#else
		b = (   ((m_iMajorVersion == 4) && (m_iMinorVersion > 3))
//			 || ((m_iMajorVersion == 4) && (m_iMinorVersion == 3) && (m_iBuildNr >= 493))
			 || m_iMajorVersion > 4
			 );
#endif
	}
	return b;
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
inline BOOL CServer::IsInBackupMode() const
{
	return m_bBackupMode;
}
/////////////////////////////////////////////////////
inline void CServer::SetDataCarrierReady(BOOL bReady)
{
	m_bDataCarrierReady = bReady;
}
