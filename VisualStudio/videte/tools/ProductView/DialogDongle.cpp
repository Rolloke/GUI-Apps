/* GlobalReplace: m_bRunVCUnit --> m_bRunVCSUnit */
/* GlobalReplace: GAAUnit --> GAUnit */
/* GlobalReplace: m_bRunGAUnit --> m_bRunGAUnit */
/* GlobalReplace: Industrial --> IndustrialEquipment */
/* GlobalReplace: EntryControl --> AccessControl */
/* GlobalReplace: CashSystem --> CashSystems */

#include "stdafx.h"


#include "DialogDongle.h"
#include "ProductViewdlg.h"

#include "stdlib.h"

//@doc
//@topic General Overview|
// NYD

//@topic Update Functions|
// NYD
// UpdateConstraints UpdateFunctionalityConstraints
// UpdateLabelsForOEM UpdateVisibilityForOEM
// @xref <l Insert new flag>

// @topic Insert new flag|
// NYD
//
CDialogDongle::CDialogDongle()
{
	// super class does the job
}

CDialogDongle::~CDialogDongle()
{
}


// OOPS calls copy contructor void CDialogDongle::TransferSettings(const CProductViewDlg &dlg)
void CDialogDongle::TransferSettings(const CProductViewDlg *dlg)
{
	m_bMajorVersion = 4;
	m_bMinorVersion = 0;

	m_productCode = dlg->GetCurrentProduct();
	m_wOEMCode=dlg->GetCurrentOEMCode();
	// NOT YET range check

	// base
	m_bRunLauncher = TRUE;
	m_bRunSupervisor = TRUE;
	m_bRunServer = 	dlg->m_bRunServer;
	m_bRunExplorer = TRUE;
	m_bRunDatabaseClient = TRUE;
	m_bRunDatabaseServer = TRUE;
	m_bRunCDRWriter = dlg->m_bRunCDRWriter;

	// codecs
	m_bRunQUnit = dlg->m_bRunQUnit;
	m_bRunIPCameraUnit = dlg->m_bRunIPCameraUnit;

	// link
	m_bRunISDNUnit = 	dlg->m_bRunISDNUnit;
	m_bRunISDNUnit2 = 	dlg->m_bRunISDNUnit2;
	m_bRunSocketUnit = dlg->m_bRunSocketUnit;

	// boxes
	m_bRunVCSUnit = 	dlg->m_bRunVCSUnit;
	m_bRunPTUnit = dlg->m_bRunPTUnit;
	m_bRunPTUnit2 = dlg->m_bRunPTUnit2;
	m_bRunTOBSUnit = dlg->m_bRunTOBSUnit;

	// misc units
	m_bRunAKUUnit = dlg->m_bRunAKUUnit ;
	m_bRunCommUnit = dlg->m_bRunCommUnit;
	m_bRunGAUnit = dlg->m_bRunGAUnit;
	
	m_bRunSDIUnit = dlg->m_bRunSDIUnit;
	m_bRunSimUnit = dlg->m_bRunSimUnit;

	// misc
	m_bRunServicePack = TRUE;
	m_bRunSecAnalyzer = dlg->m_bRunSecAnalyzer;
	m_bRunSDIConfig = dlg->m_bRunSDIConfig;

	// special
	m_bAllowInternalAccess = dlg->m_bAllowInternalAccess;

	// functionalities
	m_bAllowHardDecodeCoCo = TRUE;
	m_bAllowSoftDecodeCoCo = TRUE;
	m_bAllowSoftDecodePT = dlg->m_bAllowSoftDecodePT;
	m_bAllowLocalProcesses = dlg->m_bAllowLocalProcesses;

	// and the unsed fields, for future use
	// for future use
	m_bRunJaCobUnit1=dlg->m_bRunJaCobUnit1;
	m_bRunJaCobUnit2=dlg->m_bRunJaCobUnit2;
	m_bRunJaCobUnit3=dlg->m_bRunJaCobUnit3;
	m_bRunJaCobUnit4=dlg->m_bRunJaCobUnit4;
	
	m_bRunGemosUnit=dlg->m_bRunGemosUnit;
	m_bRunICPCONUnit=dlg->m_bRunICPCONUnit;
	m_bRunUSBCameraUnit=dlg->m_bRunUSBCameraUnit;


	m_bRunHTTP = dlg->m_bRunHTTP;
	m_bRunICPCONClient=FALSE;
	m_bRunClient4=FALSE;
	m_bRunClient5=FALSE;
	m_bRunClient6=FALSE;
	m_bRunClient7=FALSE;
	m_bRunClient8=FALSE;
	m_bRunClient9=FALSE;
	m_bRunClient10=FALSE;

	m_bRunSaVicUnit1 = dlg->m_bRunSaVicUnit1;
	m_bRunSaVicUnit2 = dlg->m_bRunSaVicUnit2;
	m_bRunSaVicUnit3 = dlg->m_bRunSaVicUnit3;
	m_bRunSaVicUnit4 = dlg->m_bRunSaVicUnit4;
	
	m_bRunTashaUnit1 = dlg->m_bRunTashaUnit1;
	m_bRunTashaUnit2 = dlg->m_bRunTashaUnit2;
	m_bRunTashaUnit3 = dlg->m_bRunTashaUnit3;
	m_bRunTashaUnit4 = dlg->m_bRunTashaUnit4;

	m_bRunAudioUnit  = dlg->m_bRunAudioUnit;
	m_bRunQUnit = dlg->m_bRunQUnit;
	m_bRunIPCameraUnit = dlg->m_bRunIPCameraUnit;
	m_bRunProgram8=FALSE;
	m_bRunProgram9=FALSE;
	m_bRunProgram10=FALSE;

	m_bRunISDNUnit2=dlg->m_bRunISDNUnit2;
	m_bRunPTUnit2=dlg->m_bRunPTUnit2;
	m_bRunSMSUnit=dlg->m_bRunSMSUnit;
	m_bRunTOBSUnit=dlg->m_bRunTOBSUnit;
	m_bRunEMailUnit=dlg->m_bRunEMailUnit;
	m_bRunFAXUnit=dlg->m_bRunFAXUnit;
	m_bRunLinkUnit7=FALSE;
	m_bRunLinkUnit8=FALSE;
	m_bRunLinkUnit9=FALSE;
	m_bRunLinkUnit10=FALSE;

	m_bAllowSDICashSystems= dlg->m_bAllowSDICashSystems;
	m_bAllowSDIAccessControl= dlg->m_bAllowSDIAccessControl;
	m_bAllowSDIParkmanagment= dlg->m_bAllowSDIParkmanagment;
	m_bAllowISDN2BChannels=TRUE;
	m_bAllowMultiMonitor = dlg->m_bAllowMultiMonitor;
	m_bAllowHTMLmaps = dlg->m_bAllowHTMLmaps;
	m_bAllowTimers = dlg->m_bAllowTimers;
	m_bAllowFunctionality8=TRUE;
	m_bAllowFunctionality9=TRUE;
	m_bAllowFunctionality10=TRUE;

	m_bAllowRouting = dlg->m_bAllowRouting;

	m_bAllowSDIIndustrialEquipment= dlg->m_bAllowSDIIndustrialEquipment;
	m_bAllowSoftDecodePT = dlg->m_bAllowSoftDecodePT;
	m_bAllowMotionDetection = dlg->m_bAllowMotionDetection;

	m_bAllowAlarmOffSpans = FALSE; // ALWAYS FALSE!
	m_bIsDemo=FALSE;
	m_bRestrictedTo8CamerasPerBoard=dlg->m_bRestrictedTo8CamerasPerBoard;
	
	m_iNrOfHosts = dlg->m_iNoOfHosts;

	if (m_bRunQUnit)
	{
		m_iNrOfCameras = dlg->m_iNrOfCameras;
	}
	else
	{
		m_iNrOfCameras = 0;
	}
	if (m_bRunIPCameraUnit)
	{
		m_iNrOfIPCameras = dlg->m_iNrOfIPCameras;
	}
	else
	{
		m_iNrOfIPCameras = 0;
	}
}


BOOL CDialogDongle::WriteDCF(const CString &sFilename) const
{
	BOOL bOkay=TRUE;

	CStdioFile f;
	bOkay = f.Open(sFilename,CFile::modeCreate| CFile::modeWrite);
	if (bOkay) {
		CString sValue;
		BOOL bValue;


		CTime ct=CTime::GetCurrentTime();
		sValue.Format(_T(";created %d.%d, %d:%d\n"),
			ct.GetDay(), ct.GetMonth(),
			ct.GetHour(), ct.GetMinute()
			);
		f.WriteString(sValue);

		// non-BOOL values first
		sValue.Format(_T("VersionNumber=%s\n"),GetVersionString());
		f.WriteString(sValue);

		sValue.Format(_T("Major=%d\n"), m_bMajorVersion );
		f.WriteString(sValue);

		sValue.Format(_T("Minor=%d\n"), m_bMinorVersion );
		f.WriteString(sValue);

		sValue.Format(_T("ProductCode=%d\n"),GetProductCode());
		f.WriteString(sValue);

		sValue.Format(_T("OEMCode=%d\n"),(WORD)GetOemCode());
		f.WriteString(sValue);

// call the member function via x()
// convert to string #x
#define TRANSFER(x) \
	bValue = x(); \
	sValue.Format(_T("%s=%d\n"), _T(#x), (int)bValue); \
	f.WriteString(sValue); \
	TRACE(_T("%s"),sValue);
// CAVEAT all Transfers as ONE , so all will get the SAME
#include "AllDongleTransfers.h"

	sValue=_T("MagicNumber=234567\n");
	f.WriteString(sValue);
	sValue.Format(_T("NrOfHosts=%d\n"), m_iNrOfHosts);
	f.WriteString(sValue);
	sValue.Format(_T("NrOfCameras=%d\n"), m_iNrOfCameras);
	f.WriteString(sValue);
	sValue.Format(_T("NrOfIPCameras=%d\n"), m_iNrOfIPCameras);
	f.WriteString(sValue);

#undef TRANSFER

		f.Close();
	}

	return bOkay;
}
BOOL CDialogDongle::WriteEEP(const CString &sFilename) const
{
	BOOL bOkay=TRUE;
	CFile f;


	bOkay = f.Open(sFilename,CFile::modeCreate| CFile::modeWrite);
	if (bOkay) {
		BYTE eepData[128];
		
		int bitIndex=0;
		int byteIndex=0;

		// reset to 0, implies FALSE for non existing (future) values
		for (byteIndex=0;byteIndex<128;byteIndex++) {
			eepData[byteIndex]=0;
		}


		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// CAVEAT this has to stay in sync with the ReadHardwareDongle in the lib
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		byteIndex=0;
		eepData[byteIndex++] = 0xAF;
		eepData[byteIndex++] = 0xFE;
		eepData[byteIndex++] = 1;	// Dongle Format Version

		eepData[byteIndex++] = (BYTE) (m_bMajorVersion >> 8);
		eepData[byteIndex++] = (BYTE) (m_bMajorVersion & 255);

		eepData[byteIndex++] = (BYTE) (m_bMinorVersion >> 8);
		eepData[byteIndex++] = (BYTE) (m_bMinorVersion & 255);

		eepData[byteIndex++] = (BYTE) (((WORD)GetOemCode()) >> 8);
		eepData[byteIndex++] = (BYTE) (((WORD)GetOemCode()) & 255);

		eepData[byteIndex++] = (BYTE) (GetProductCode() >> 8);
		eepData[byteIndex++] = (BYTE) (GetProductCode() & 255);

// call the member function via x()
// convert to string #x
#define TRANSFER(x) \
		if (x()) { \
			eepData[byteIndex] |= (1 << bitIndex); \
		} \
		bitIndex++; \
		if (bitIndex>7) { \
			bitIndex=0; \
			byteIndex++; \
			if (byteIndex>96) { \
				WK_TRACE_ERROR(_T("EEP data index error %d"),byteIndex);\
				byteIndex=0; \
			} \
		}

// CAVEAT all Transfers as ONE, so all will get the SAME
#include "AllDongleTransfers.h"
		
#undef TRANSFER

		TRACE(_T("DEBUG: byteIndex %d bitIndex %d\n"),byteIndex,bitIndex);

		// NOT YET exception
		f.Write(eepData,128);

		f.Close();
		WK_TRACE(_T("final byteIndex %d, bitIndex %d\n"),byteIndex,bitIndex);
	}



	return bOkay;
}


BOOL CDialogDongle::WriteToFile(const CString &sFilename) const
{
	BOOL bOkay=FALSE;
	CString sBaseName;
	CString sExtension;

	BOOL bSplitOkay = WK_SplitExtension(sFilename,sBaseName,sExtension);

	if (bSplitOkay==FALSE) {
		WK_TRACE_ERROR(_T("Could not split '%s'\n"), sFilename);
		return FALSE;	// <<< EXIT >>>
	}

	CString sOutName;
	sOutName = sBaseName;

	sOutName += _T(".dcf");
	bOkay=WriteDCF(sOutName);

	sOutName = sBaseName;
	sOutName += _T(".eep");

	bOkay &= WriteEEP(sOutName);

	TCHAR szTmp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szTmp);
	bOkay &= WriteHCC(szTmp, sBaseName);

	return bOkay;
}

static TCHAR *szTheDongleSection=_T("Dongle");

BOOL CDialogDongle::WriteToRegistry() const
{
	CWK_Profile wkp;

	return Save(wkp);
}

BOOL CDialogDongle::WriteHCC(const CString &sDir, const CString &sBaseName) const
{
	BOOL bOkay=TRUE;
	CFile f;

	CString sEepName = sDir;
	sEepName += _T("\\");
	sEepName += sBaseName;
	sEepName += _T(".eep");

	CString sOutName =sBaseName;
	sOutName += _T(".hcc");

	bOkay = f.Open(sOutName,CFile::modeCreate| CFile::modeWrite);
	if (bOkay) {
		// CAVEAT data is directly from an existing .hcc file
		f.Write(sEepName,sEepName.GetLength());
		BYTE hccData[] = {
			0x0a,0x02,					// 2
			0,0,0,						// 3
			2, 0,0,0,0,0,0,0,			// 1+7
			0x21, 0x0b,					// 2
			0,0,0,0,0,0,0,0,0,0,0,0,0,0	// 14
		};
		f.Write(hccData,2+3+1+7+2+14);
		f.Close();
	}
	return bOkay;
}
