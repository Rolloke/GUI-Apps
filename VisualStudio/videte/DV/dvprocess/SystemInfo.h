// SystemInfo.h: interface for the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMINFO_H__21675EEE_ED87_4488_95FF_869341490DFB__INCLUDED_)
#define AFX_SYSTEMINFO_H__21675EEE_ED87_4488_95FF_869341490DFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////////////////////////////////////
typedef enum
{				
	OemDTS		= 0,
	OemAlarmCom	= 1,
	OemDResearch= 2,
	OemSantec	= 3,
	OemPOne		= 4
} OEM;

#define DIRECTCD _T("DirectCD")
#define PACKETCD _T("PacketCD")
#define NERO "Nero"

class CSystemInfo  

{
public:
	CSystemInfo();
	virtual ~CSystemInfo();
	
	CString GetSystemInformations();

protected:

private:
	//functions

public:
	static CString	GetSystemSerialNumber();
	static CString	GetOSVersion();
	static CString	GetOEMVersion();
	static void		GetAvailableOptions(BOOL &bNet, BOOL &bISDN, BOOL &bBackup, BOOL &bBackupDVD);
		   CString	GetBurnSoftware(CString &sVersionNr);
		   BOOL		IsNeroInstalled();
		   BOOL		GetNeroVersion(CString &sVersion);
	static CString	GetDTSSoftwareVersion();
	static CString	GetInfoHarddisks();
	static int		GetAvailableQCams();

	static int CheckBackupProgram();

private:
	//members
	CString m_sNeroVersion;
};

#endif // !defined(AFX_SYSTEMINFO_H__21675EEE_ED87_4488_95FF_869341490DFB__INCLUDED_)
