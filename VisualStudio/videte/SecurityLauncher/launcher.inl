// launcher.inl

inline CString CSecurityLauncherApp::GetWorkingDirectory() 
{
	return m_sWorkingDirectory;
}
inline CString CSecurityLauncherApp::GetWindowsDirectory() 
{
	return m_sWindowsDirectory;
}
inline CString CSecurityLauncherApp::GetSystemDirectory() 
{
	return m_sSystemDirectory;
}
inline CString CSecurityLauncherApp::GetTempDirectory() 
{
	return m_sTempDirectory;
}
inline CString CSecurityLauncherApp::GetName() 
{
	return m_sName;
}
inline CString CSecurityLauncherApp::GetVersion() 
{
	return m_sVersion;
}
inline CString CSecurityLauncherApp::GetOsVersion() 
{
	return m_sOsVersion;
}
inline CString CSecurityLauncherApp::GetOsVersionShort()
{
	return m_sOsVersionShort;
}
inline int CSecurityLauncherApp::GetAlarmPercent()
{
	return m_iAlarmPercent;
}
inline int CSecurityLauncherApp::GetISDNBChannel()
{
	return m_iISDNBChannel;
}
inline CWatchDog& CSecurityLauncherApp::GetWatchDog()
{
	return m_WatchDog;
}
inline BOOL	CSecurityLauncherApp::UseUpdateHandler() const
{
	return m_bUseUpdateHandler;
}
inline BOOL	CSecurityLauncherApp::ShallReboot() const
{
	return m_bShallReboot;
}
