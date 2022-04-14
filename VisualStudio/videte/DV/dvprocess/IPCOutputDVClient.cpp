// IPCOutputDVClient.cpp: implementation of the CIPCOutputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "IPCOutputDVClient.h"
#include <NotificationMessage.h>
#include "..\LangDll\LangDll.h" //for maintenance text
#include "..\LangDll\Resource.h" //for maintenance text
#include "wkclasses\WK_RuntimeErrors.h" //for notifiable errors like CPU temperature too high
#include <wkclasses\WK_ThreadPool.h>

#include "Camera.h"
#include "CameraGroup.h"

#define RTE_FILENAME  _T("rterrors.dat")
#define RTE_CONFIRMED _T("/RTE Confirmed/")	


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputDVClient::CIPCOutputDVClient(COutputClient* pClient, LPCTSTR shmName)
	: CIPCOutput(shmName, TRUE)
{
	m_pClient = pClient;
	m_iFastConfirms = 0;
	m_iSlowConfirms = 0;
	m_dwTime = GetTickCount();

	m_csConfirms.Lock();
	CSystemTime st;
	for (int i=0;i<MAX_CAM;i++)
	{
		m_iRequest[i] = 0;
		m_iFPS[i] = 0;
		m_wBlockNr[i] = 0;
		m_stConfirm[i] = st; //	time will be initialized to 0
	}
	m_csConfirms.Unlock();

	m_pTranscoder = NULL;
	m_Encoder.StartThread();
	m_GOPSender.SetParent(this);
	m_GOPSender.StartThread();

	m_hLanguageDll = NULL;

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCOutputDVClient::~CIPCOutputDVClient()
{
	m_GOPSender.StopThread();
	m_Encoder.StopThread();
	StopThread();
	DestroyTranscoder();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::ResetClientCounter()
{
	m_csConfirms.Lock();
	CSystemTime st;
	for (int i=0;i<MAX_CAM;i++)
	{
		m_wBlockNr[i] = 0;
		m_stConfirm[i] = st; //	time will be initialized to 0
	}
	m_csConfirms.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCOutputDVClient::IsTimedOut()
{
	if (GetIPCState() == CIPC_STATE_WRITE_CREATED)
	{
		if (GetTimeSpan(m_dwTime)>30*1000)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestInfoOutputs(WORD wGroupID)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 3,	NULL);
		return;
	}

	COutputList& outputList = theApp.GetOutputGroups();

	//call Notification sending functions (CheckNagScreen()) before 
	//DoConfirmInfoOutputs because the 
	//Notification window in the client has to be the first window in the window list
	CheckNagScreen();

	outputList.Lock();

	int iNumRecords = 0;
	int iNumGroups = outputList.GetSize();
	int g;
	
	for (g=0;g<iNumGroups;g++) 
	{
		iNumRecords += outputList.GetAtFast(g)->GetSize();
	}

	if (iNumRecords) 
	{
		CIPCOutputRecord *records = new CIPCOutputRecord[iNumRecords];
		int r=0; // record counter
		
		for (g=0,r=0;g<iNumGroups;g++) 
		{
			const COutputGroup *oneGroup = outputList.GetAtFast(g);
			DWORD dwOneBit=1;
			
			for (int i=0;i<oneGroup->GetSize();i++, dwOneBit<<1) 
			{
				const COutput *pOutput = oneGroup->GetAtFast(i);
				WORD wFlags = OREC_IS_ENABLED;

				if (pOutput->IsActive()) 
				{
					wFlags |= OREC_IS_OKAY;
				}
				if (oneGroup->HasCameras())
				{
					CCamera* pCamera = (CCamera*)pOutput;
					wFlags |= OREC_IS_CAMERA|OREC_CAMERA_COLOR| OREC_CAMERA_IS_FIXED;
					

					if (oneGroup->IsTasha())
					{
						// Tasha nur MPEG4
						wFlags |= OREC_CAMERA_DOES_MPEG;
					}
					else if (oneGroup->IsJacob())
					{
						if (m_pClient->IsLowBandwidth())
						{
							// ISDN or DSL
							if (m_pClient->CanMpeg4())
							{
								wFlags |= OREC_CAMERA_DOES_MPEG;
							}
							else if (m_pClient->UseH263())
							{
								// nothing is H.263
							}
							else
							{
								// unknown JPEG
								wFlags |= OREC_CAMERA_DOES_JPEG;
							}
						}
						else
						{
							// Jacob Local und Netz JPEG
							wFlags |= OREC_CAMERA_DOES_JPEG;
						}
					}
					else if (   oneGroup->IsSavic()
							 || oneGroup->IsQ())
					{
						if (m_pClient->IsLowBandwidth())
						{
							// ISDN or DSL
							if (m_pClient->CanMpeg4())
							{
								wFlags |= OREC_CAMERA_DOES_MPEG;
							}
							else if (m_pClient->UseH263())
							{
								// nothing is H.263
							}
							else
							{
								wFlags |= OREC_CAMERA_DOES_JPEG;
							}
						}
						else if (m_pClient->IsTCPIP())
						{
							if (m_pClient->CanMpeg4())
							{
								// new clients make  MPEG4
								wFlags |= OREC_CAMERA_DOES_MPEG;
							}
							else
							{
								// old ones do JPEG
								wFlags |= OREC_CAMERA_DOES_JPEG;
							}
						}
						else
						{
							if (oneGroup->IsSavic())
							{
								wFlags |= OREC_CAMERA_DOES_JPEG;
							}
							else
							{
								// RKE: local is YUV422 uncompressed data
								// it is handled by the CJPeg class
								wFlags |= OREC_CAMERA_YUV;
							}
						}
					}

					// PTZ is independent from Compression
					if (pCamera->IsPTZ())
					{
						wFlags &= ~OREC_CAMERA_IS_FIXED;
					}
				}
				else
				{
					wFlags |= OREC_IS_RELAY|OREC_IS_OKAY;
				}
				ASSERT(r<iNumRecords);
				records[r].Set(pOutput->GetName(),
							   pOutput->GetClientID(),
							   wFlags); 
				WK_TRACE(_T("confirm info outputs %s, %08lx, %04lx\n"),
					records[r].GetName(),records[r].GetID(),records[r].GetFlags());
				r++;
			}
		}
		DoConfirmInfoOutputs(wGroupID, iNumGroups, iNumRecords,
								records);
		WK_DELETE_ARRAY(records);
	} 
	else 
	{
		// no records at all
		CIPCOutputRecord dummyRecords[1];
		DoConfirmInfoOutputs(wGroupID, iNumGroups, 0, dummyRecords);
	}

	outputList.Unlock();
}
/////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::CheckNagScreen()
{
	CString sPathLanguageDll = GetPathCurrentLanguageDll();
	// load language dll
	ASSERT(m_hLanguageDll == NULL);
	m_hLanguageDll = AfxLoadLibrary(sPathLanguageDll);
	if (m_hLanguageDll)
	{
		WK_TRACE(_T("Language dll %s loaded\n"), sPathLanguageDll);
	}
	else
	{
		WK_TRACE_ERROR(_T("No language dll %s ; Error %u ; %s\n"),
			sPathLanguageDll, GetLastError(), GetLastErrorString());
	}
	COutputClient* pOC = GetClient();
	if(    pOC
		&& !pOC->IsAlarm())
	{
		//check for notifiable errors: HDD loss, CPU temperature too high, fan speed too slow
		CString sErrors = CheckNotifiableErrors();

		//check for maintenance
		CString sMaintenance = CheckMaintenance();

		UnloadLanguageDll();

		CString sMsg = sErrors + sMaintenance;
		//send notification message
		if(!sMsg.IsEmpty())
		{
			CSecID id(SECID_GROUP_NOTIFICATIONMESSAGE,0);
			DoRequestSetValue(id,NM_MESSAGE,sMsg,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputDVClient::CheckMaintenance()
{
	CString sMaintenanceText;

	// is maintenance necessary ??
	// inform the client
	CWK_Profile wkp;
	BOOL bMaintenanceOn = wkp.GetInt(_T("Maintenance"),_T("IntervallOnOff"),FALSE);
	if (bMaintenanceOn)
	{
		int nMonth = wkp.GetInt(_T("Maintenance"),_T("Intervall"),0);
		if (nMonth>0)
		{
			CString sStartDate = wkp.GetString(_T("Maintenance"),_T("IntervallStartDate"),_T(""));
			CSystemTime stMaintenance;
			if (stMaintenance.SetDBDate(sStartDate))
			{
				CSystemTime stNow;
				stNow.GetLocalTime();
				stMaintenance.IncrementTime(30*nMonth,0,0,0,0);
				if (stMaintenance < stNow)
				{
					// maintenance time interval is elapsed
					CSecID id(SECID_GROUP_NOTIFICATIONMESSAGE,0);
					sMaintenanceText = GetMaintenanceText();
				}
			}
		}
	}
	return sMaintenanceText;
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputDVClient::CheckNotifiableErrors()
{
	//check RTEs for HDD loss, CPU temperature too high, fan speed too slow

	CWK_RunTimeErrors RTEErrors;
	CString sPath;
	int iStartPos = 0;
	CMapDWordToCString dwMapRTE;

	GetModuleFileName(theApp.m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
#ifdef _DEBUG
	sPath = _T("C:\\DV\\");
#endif	
	int nI = sPath.ReverseFind(_T('\\'));
	if (nI != -1)
	{
		sPath = sPath.Left(nI);
	}

	RTEErrors.LoadFromFile(sPath+_T("\\")+RTE_FILENAME);
	iStartPos = max(RTEErrors.GetSize()-1, 0);

	CString sError, sErrorText, sReturnErrorText, sTimestamp;

	if (iStartPos > 0)
	{	
		CWK_RunTimeError* pRTE = NULL;
		int i = iStartPos;
		BOOL bGetErrorText = FALSE;
		CString* psDummy = NULL;

		for (i=iStartPos; i >= 0; i--)
		{
			pRTE = RTEErrors.GetAtFast(i);
			if(pRTE)
			{
				//every non confirmed error trace only once, the latest one
				WK_RunTimeError WKRTEError = pRTE->GetError();
				WORD wDevice = 0;
				BOOL bFoundNotifableError = TRUE;

				switch(WKRTEError)
				{
				case RTE_HARDDISK_FALURE:
					wDevice = LOWORD(pRTE->GetParam1()); // drive letter
					break;
				case RTE_SMART:
					wDevice = LOWORD(pRTE->GetParam1()); // drive number
					break;
				case RTE_TEMPERATURE:
					wDevice = HIWORD(pRTE->GetParam1()); // device (HDD, CPU, Power Supp, board, case, system)
					break;
				case RTE_FANCONTROL:
					wDevice = HIWORD(pRTE->GetParam1()); // device (HDD, CPU, Power Supp, board, case, system)
					break;
				default:
					bFoundNotifableError = FALSE;
					break;
				}

				DWORD dwRTE = MAKELONG((WORD)WKRTEError, wDevice);
				if(!bFoundNotifableError || dwMapRTE.Lookup(dwRTE, psDummy))
				{
					bGetErrorText = FALSE;
				}
				else
				{
					psDummy = NULL;
					bGetErrorText = TRUE;
					dwMapRTE.SetAt(dwRTE, psDummy);
				}

				if(bGetErrorText)
				{
					//build error message
					CString sMsg,sFmt;
					sFmt.LoadString(IDS_RTE);
					if (!sFmt.IsEmpty())
					{
						sMsg.Format(sFmt, i+1);
					}
					sTimestamp.Format(_T("%s %s"), sMsg, pRTE->GetFormattedTimeStamp());	
					sError = pRTE->GetErrorText();

					// is error msg confirmed?
					int nIndex = sError.Find(RTE_CONFIRMED, 0);
					if (nIndex == -1)
					{
						//create msg for non confirmed error
						sErrorText.Format(_T("%s - %s\r\n"), sTimestamp, sError);
						sReturnErrorText += sErrorText;
					}
					bGetErrorText = FALSE;
				}
			}
		}
	}
	if(!sReturnErrorText.IsEmpty())
	{
		CString sHeader;
		sHeader.LoadString(IDS_ERROR_HARDWARE_HEADER);
		sReturnErrorText = sHeader + sReturnErrorText + _T("\r\n\r\n");
	}
	return sReturnErrorText;
}

/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputDVClient::GetMaintenanceText()
{	
	CString sText;

	CWK_Profile wkp;
	CString sNewLine(_T("\r\n"));
	CString sBlank(_T("  "));

	//create headers
	//-------------------------------------
	CString sHeader1, sHeader2, sHeader3, sStartDate;

	sHeader1.LoadString(IDS_MAINT_INFO_TXT_HEADER1);
	if(sHeader1.IsEmpty()) //String is not yet implemented in LangDll
	{
		sText.Empty();
		WK_TRACE(_T("Maintenance not yet implemented in current language.\n"));
		return sText;
	}
	sHeader2.LoadString(IDS_MAINT_INFO_TXT_HEADER2);

	sStartDate = wkp.GetString(_T("Maintenance"),_T("IntervallStartDate"),_T(""));
	if(!sStartDate.IsEmpty())
	{
		CSystemTime st;
		st.SetDBDate(sStartDate);
		sHeader2 += sBlank + st.GetDate() + _T(".");
	}
	sHeader3.LoadString(IDS_MAINT_INFO_TXT_HEADER3);

	//create serial number
	//-------------------------------------
	CString sSerialComputer, sSerialStr, sSerial;
	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	sSerialComputer = prof.GetString(_T("System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"),_T("ComputerName"),_T(""));
	sSerialStr.LoadString(IDS_MAINT_INFO_TXT_SERIAL);
	sSerial.Format(_T("%s %s"), sSerialStr, sSerialComputer);

	//create software version
	//-------------------------------------
	CString sVersionIdip, sVersionStr, sVersion;
	sVersionStr.LoadString(IDS_MAINT_INFO_TXT_VERSION);
	sVersionIdip = _T("Version Test 233");//COemGuiApi::GetSoftwareVersion();
	sVersion.Format(_T("%s %s"), sVersionStr, sVersionIdip);

	//create installer header
	//-------------------------------------
	CString sInstStr;
	sInstStr.LoadString(IDS_MAINT_INFO_TXT_INSTALLER);

	//create installer data
	//-------------------------------------
	CString sCompany, sStreet, sPostcode, sCity, sEmail, sTelefon, sCountry;
	CString sCompanyStr, sStreetStr, sPostcodeStr, sCityStr, sEmailStr, sTelefonStr, sCountryStr;
	CString sCompanyReg, sStreetReg, sPostcodeReg, sCityReg, sEmailReg, sTelefonReg, sCountryReg;

	sCompanyReg = wkp.GetString(_T("Maintenance"),_T("Company"),_T(""));
	sStreetReg = wkp.GetString(_T("Maintenance"),_T("Street"),_T(""));
	sPostcodeReg = wkp.GetString(_T("Maintenance"),_T("Postcode"),_T(""));
	sCityReg = wkp.GetString(_T("Maintenance"),_T("City"),_T(""));
	sEmailReg = wkp.GetString(_T("Maintenance"),_T("Email"),_T(""));
	sTelefonReg = wkp.GetString(_T("Maintenance"),_T("Telefon"),_T(""));
	sCountryReg = wkp.GetString(_T("Maintenance"),_T("Country"),_T(""));

	sCompanyStr.LoadString(IDC_MAINT_TXT_INST_COMP);
	sStreetStr.LoadString(IDC_MAINT_TXT_INST_STR);
	sPostcodeStr.LoadString(IDS_MAINT_INFO_TXT_POSTCODE);
	sCityStr.LoadString(IDS_MAINT_INFO_TXT_CITY);
	sEmailStr.LoadString(IDC_MAINT_TXT_EMAIL);
	sTelefonStr.LoadString(IDC_MAINT_TXT_TELEFON);
	sCountryStr.LoadString(IDC_MAINT_TXT_COUNTRY);

	sCompany = sCompanyStr + sBlank + sCompanyReg;
	sStreet	 = sStreetStr + sBlank + sStreetReg;
	sPostcode = sPostcodeStr + sBlank + sPostcodeReg;
	sCity = sCityStr + sBlank + sCityReg;
	sEmail = sEmailStr + sBlank + sEmailReg;
	sTelefon = sTelefonStr + sBlank + sTelefonReg;
	sCountry = sCountryStr + sBlank + sCountryReg;

	//create the whole nag screen text
	//-------------------------------------
	sText = sHeader1 + sNewLine;
	sText += sHeader2 + sNewLine;
	sText += sHeader3 + sNewLine + sNewLine;

	sText += sSerial + sNewLine;
	sText += sVersion + sNewLine + sNewLine;

	sText += sInstStr + sNewLine;
	sText += _T("--------------------") + sNewLine;
	sText += sCompany + sNewLine;
	sText += sStreet + sNewLine;
	sText += sPostcode + sNewLine;
	sText += sCity + sNewLine;
	sText += sEmail + sNewLine;
	sText += sTelefon + sNewLine;
	sText += sCountry + sNewLine;

	return sText;
}
//////////////////////////////////////////////////////////////////////
CString CIPCOutputDVClient::GetPathCurrentLanguageDll()
{	
	CString sPath;

	//get current language from reg
#ifdef _UNICODE
	CString sLangPrefix(_T("DVU"));
#else
	CString sLangPrefix(_T("DV"));
#endif
	CString sLangDebug(_T("Debug"));
	CString sLangSuffix(_T(".dll"));
	CString sRegKey(_T("DV\\DVClient"));
	CString sDefaultLanguageEnu(_T("English"));
	CStringArray result;
	CString sSearch = sLangPrefix;
	CString sHomePath;
	CWK_Profile prof;

	CString sDefaultLanguage = prof.GetString(sRegKey, _T("Language"), sDefaultLanguageEnu);

#ifdef _DEBUG
	if (sDefaultLanguage.Find(sLangDebug) == -1)
	{
		sDefaultLanguage += _T("_");
		sDefaultLanguage += sLangDebug;
	}
#else
	int iIndex = sDefaultLanguage.Find(sLangDebug);
	if (iIndex != -1)
	{
		sDefaultLanguage = sDefaultLanguage.Left(iIndex-1);
	}
#endif


	// check language dlls in home path
	sSearch += _T("???");	// Do NOT translate, wildcard for chars
#ifdef _DEBUG
	sSearch += sLangDebug;
#endif
	sSearch += sLangSuffix;

	_TCHAR szPath[_MAX_PATH];
	GetModuleFileName(theApp.m_hInstance, szPath, _MAX_PATH);
	sHomePath = szPath;
	sHomePath = sHomePath.Left(sHomePath.ReverseFind(_T('\\')));

	int iFound = WK_SearchFiles(result, sHomePath, sSearch, FALSE);
	if (iFound)
	{
		// Limit to 16 languages
		iFound = min(iFound, 16);
		CString sDllPath, sFilename, sFilePath;

		for (int i = 0; i < iFound; i++)
		{	
			sFilePath = result.GetAt(i);
			WK_SplitPath(sFilePath, sDllPath, sFilename);
			if (sDllPath.IsEmpty())
			{
				sFilePath = sHomePath + _T("\\") + sFilename;
				//get languageDLL info
				HINSTANCE hInstance = LoadLibraryEx(sFilePath,NULL,0);
				CString sLanguage;
				if (hInstance)
				{
					FARPROC theProc = ::GetProcAddress(hInstance, "GetLanguageName");
					if (theProc)
					{
						sLanguage = ((GET_LANGUAGE_NAME_PTR)theProc)();
					}
					FreeLibrary(hInstance);
				}
				if(sLanguage == sDefaultLanguage)
				{
					sPath = sFilePath;
					break;
				}
			}
		}
	}
	else
	{
		//set english language to default
		sPath = sHomePath +_T("\\DVUEnu.dll");
	}
	return sPath;
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::LoadLanguageDll()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CIPCOutputDVClient::UnloadLanguageDll()
{
	BOOL bReturn = TRUE;
	if (m_hLanguageDll)
	{
		if (AfxFreeLibrary(m_hLanguageDll))
		{
			m_hLanguageDll = NULL;
			WK_TRACE(_T("Language dll unloaded\n"));
		}
		else
		{
			WK_TRACE_ERROR(_T("Language dll NOT unloaded\n"));
			bReturn = FALSE;
		}
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::ConfirmPictureSlow(CCamera* pCamera, const CIPCPictureRecord& pict, DWORD dwX, DWORD dwY)
{
	int x = pCamera->GetClientID().GetSubID();
	int iFps = m_iFPS[x];
	BOOL bConfirm = TRUE;
	
	m_csConfirms.Lock();
	if (m_iRequest[x]>0)
	{
		if (   pict.GetCompressionType() == COMPRESSION_MPEG4
			&& pict.GetPictureType() == SPT_DIFF_PICTURE)
		{
			if (iFps == 25)
			{
				if (pict.GetBlockNr()-m_wBlockNr[x] != 1)
				{
					TRACE(_T("block nr wrong client image %08lx %d,%d %d\n"),pCamera->GetClientID().GetID(),
						pict.GetBlockNr(),m_wBlockNr[x],m_iRequest[x]);
					bConfirm = FALSE;
				}
			}
			else
			{
				bConfirm = FALSE;
			}
		}
	}
	else
	{
		bConfirm = FALSE;
	}
	m_csConfirms.Unlock();
	if (bConfirm)
	{
		if (theApp.GetSettings().TraceClientConfirms())
		{
			WK_TRACE(_T("confirm slow %08lx %s,%d BlNr %d %s %s\n"),
				pCamera->GetClientID().GetID(),
				pict.GetTimeStamp().GetDateTime(),
				pict.GetTimeStamp().wMilliseconds,
				pict.GetBlockNr(),
				SPT2TCHAR(pict.GetPictureType()),
				NameOfEnum(pict.GetCompressionType()));
		}
		m_iSlowConfirms++;
		if (pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			if (iFps == 25)
			{
				// send the whole gop
				if (m_pClient->CanGOP())
				{
					SendPicture(pCamera,pict,dwX,dwY,FALSE);
				}
				else
				{
					// send the single images 
					m_GOPSender.AddPictureRecord(pCamera,pict,dwX,dwY);
					/*
					for (DWORD i=0;i<pict.GetNumberOfPictures();i++)
					{
						CIPCSavePictureType type = (i==0)?SPT_FULL_PICTURE:SPT_DIFF_PICTURE;
						CIPCPictureRecord pict1(this,
							pict.GetData(i),
							pict.GetDataLength(i),
							pict.GetCamID(),
							pict.GetResolution(),
							pict.GetCompression(),
							pict.GetCompressionType(),
							pict.GetTimeStamp(),
							pict.GetJobTime(),
							NULL,0,type);
						SendPicture(pCamera,pict1,dwX,dwY,FALSE);
					}*/
				}
			}
			else
			{
				// send only the first I-frame
				CIPCPictureRecord pict1(this,
										pict.GetData(0),
										pict.GetDataLength(0),
										pict.GetCamID(),
										pict.GetResolution(),
										pict.GetCompression(),
										pict.GetCompressionType(),
										pict.GetTimeStamp(),
										pict.GetJobTime());
				SendPicture(pCamera,pict1,dwX,dwY,FALSE);
			}
		}
		else
		{
			SendPicture(pCamera,pict,dwX,dwY,FALSE);
		}
	}
	else
	{
		// drop the p frame
		if (x==0)
		{
//			TRACE(_T("drop the frame %d\n"),x);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::SendPicture(CCamera* pCamera, 
									 const CIPCPictureRecord& pict, 
									 DWORD dwX, 
									 DWORD dwY,
									 BOOL bFromEncoderThread)
{
	int x = pCamera->GetClientID().GetSubID();
	CSecID idArchive = pCamera->GetCurrentArchiveID();

	if (   m_pClient->UseH263()
		&& (   pict.GetCompressionType() == COMPRESSION_JPEG
		    || pict.GetCompressionType() == COMPRESSION_YUV_422
			|| pict.GetCompressionType() == COMPRESSION_RGB_24)
			)
	{
		if (m_pTranscoder)
		{
			PictureSent(pict,x);
			m_pTranscoder->AddPictureRecordH263(pict);
		}
	}
	else if (   m_pClient->CanMpeg4()
			&& m_pClient->IsLowBandwidth()
			&& pict.GetResolution() == RESOLUTION_2CIF
			&& pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		PictureSent(pict,x);
		CIPCFields fields;

		m_Encoder.AddPictureRecord(pCamera,
			pict,
			fields,
			0,
			m_pClient->GetID(), COMPRESSION_MPEG4);
	}
	else
	{
		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			if (theApp.GetSettings().TraceClientConfirms())
			{
				WK_TRACE(_T("send jpeg to client %08lx %d\n"),pict.GetCamID(),m_iRequest[x]);
			}
			if (!bFromEncoderThread)
			{
				PictureSent(pict,x);
			}
			DoConfirmJpegEncoding(pict,pCamera->GetClientID().GetID(),idArchive);
		}
		else if (pict.GetCompressionType() == COMPRESSION_MPEG4)
		{
			if (theApp.GetSettings().TraceClientConfirms())
			{
				WK_TRACE(_T("send %d %d %02d:%02d:%02d,%03d %s-Frame %d Bytes %s\n"),
					x,
					pict.GetBlockNr(),
					pict.GetTimeStamp().GetHour(),
					pict.GetTimeStamp().GetMinute(),
					pict.GetTimeStamp().GetSecond(),
					pict.GetTimeStamp().GetMilliseconds(),
					SPT2TCHAR(pict.GetPictureType()),
					pict.GetDataLength(),
					NameOfEnum(pict.GetResolution()));
			}
			if (!bFromEncoderThread)
			{
				PictureSent(pict,x);
			}
			DoIndicateVideo(pict,dwX,dwY,pCamera->GetClientID().GetID(),idArchive);
		}
		else if (   pict.GetCompressionType() == COMPRESSION_RGB_24
			     || pict.GetCompressionType() == COMPRESSION_YUV_422)
		{
			if (   m_pClient->IsTCPIP()
				|| m_pClient->IsISDN())
			{
				CIPCFields fields;
/*
				TRACE(_T("add to jpeg/mpeg4 encoder %08lx %s,%03d %d\n"),
					pict.GetCamID(),
					pict.GetTimeStamp().GetTime(),
					pict.GetTimeStamp().GetMilliseconds(),
					m_iRequest[x]);*/
				PictureSent(pict,x);

				m_Encoder.AddPictureRecord(pCamera,
											pict,
											fields,
											0,
											m_pClient->GetID(),
											m_pClient->CanMpeg4() ? COMPRESSION_MPEG4 : COMPRESSION_JPEG);
			}
			else
			{
				if (!bFromEncoderThread)
				{
					PictureSent(pict,x);
				}
				DoConfirmJpegEncoding(pict,pCamera->GetClientID().GetID(),idArchive);
			}
		}
		else if (pict.GetCompressionType() == COMPRESSION_H263)
		{
			CIPCPictureRecord temp(this,
									pict.GetData(),
									pict.GetDataLength(),
									pict.GetCamID(),
									pict.GetResolution(),
									pict.GetCompression(),
									pict.GetCompressionType(),
									pict.GetTimeStamp(),
									pict.GetJobTime(),
									NULL,
									pict.GetBitrate(),
									pict.GetPictureType(),
									pict.GetBlockNr());
			if (!bFromEncoderThread)
			{
				PictureSent(temp,x);
			}
			DoIndicateH263Data(temp,temp.GetCamID().GetID(),idArchive);
		}
		else
		{
			WK_TRACE_ERROR(_T("unknown compression type for confirming live image to client\n"));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::PictureSent(const CIPCPictureRecord& pict, int x)
{
	m_csConfirms.Lock();
	if (m_iRequest[x]>0)
	{
		m_iRequest[x]--;
	}

	//	TRACE(_T("DECREASE %d, %d, %d, %s\n"),x,m_iRequest[x], pict.GetNumberOfPictures(), GetTickCountEx());

	m_stConfirm[x] = pict.GetTimeStamp();
	m_wBlockNr[x] = pict.GetBlockNr();
	m_csConfirms.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestStartH263Encoding(CSecID camID, 
													Resolution res, 
													Compression comp,
													DWORD dwBitrate,
													DWORD dwRecordTime)
{
//	TRACE(_T("OnRequestStartH263Encoding %08lx res=%s\n"),camID.GetID(),NameOfEnum(res));
	if (res == RESOLUTION_2CIF)
	{
		CreateTranscoder(25,m_pClient->GetBitrate());
		OnRequestNewJpegEncoding(camID,res,comp,dwBitrate,dwRecordTime);
		CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(camID);
		if (pCamera)
		{
			pCamera->DeleteH263ClientQCIF(m_pClient->GetID());
		}
	}
	else if (res == RESOLUTION_QCIF)
	{
		CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(camID);
		if (pCamera)
		{
			pCamera->AddH263ClientQCIF(m_pClient->GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestStopH263Encoding(CSecID camID)
{
	CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(camID);
	if (pCamera)
	{
		pCamera->DeleteH263ClientQCIF(m_pClient->GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestStartVideo(CSecID id,			// camera nr.
											 Resolution res,		// resolution
											 Compression comp,		// image size or bitrate
												CompressionType ct,	// JPEG, YUV, MPEG4 ...
												int iFPS,
												int iIFrameInterval, 
												DWORD dwUserData		// unique server id
												)
{
	CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(id);

	if (pCamera)
	{
		if (m_pClient->IsLowBandwidth())
		{
			if (pCamera->GetGroup()->IsTasha())
			{
				// tasha
				pCamera->AddMPEG4Client(m_pClient,res);
			}
			else
			{
				// savic/jacob/q
				TRACE(_T("MPEG4 SaVic/Jacob/Q request %s\n"),NameOfEnum(res));

				if (res == RESOLUTION_2CIF)
				{
					res = RESOLUTION_CIF;
				}
				if (res == RESOLUTION_CIF)
				{
					m_Encoder.AddLowBandwidthClient(m_pClient->GetID(),id);
					OnRequestNewJpegEncoding(id,res,comp,iFPS,dwUserData);
					pCamera->DeleteMPEG4Client(m_pClient->GetID());
				}
				else if (res == RESOLUTION_QCIF)
				{
					m_Encoder.RemoveLowBandwidthClient(m_pClient->GetID(),id);
					pCamera->AddMPEG4Client(m_pClient,res);
				}
			}
		}
		else
		{
			m_iFPS[id.GetSubID()] = iFPS;
			OnRequestNewJpegEncoding(id,res,comp,iFPS,dwUserData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestStopVideo(CSecID camID,
											DWORD dwUserData)
{
	m_csConfirms.Lock();
	m_iRequest[camID.GetSubID()] = 0;
	m_csConfirms.Unlock();

	if (m_pClient->IsLowBandwidth())
	{
		CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(camID);
		if (pCamera)
		{
			pCamera->DeleteMPEG4Client(m_pClient->GetID());
		}
		m_Encoder.RemoveLowBandwidthClient(m_pClient->GetID(),camID);
	}
	else if (m_pClient->IsTCPIP())
	{
		m_Encoder.RemoveLowBandwidthClient(m_pClient->GetID(),camID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestNewJpegEncoding(CSecID camID, 
												  Resolution res, 
												  Compression comp,
												  int iNumPictures,
												  DWORD	dwUserID)
{
	int x;
	x = camID.GetSubID();

	m_csConfirms.Lock();
	m_iRequest[x]++;
//	TRACE(_T("INCREASE %d, %d, %d, %s\n"),x,m_iRequest[x], iNumPictures, GetTickCountEx());
	m_csConfirms.Unlock();

	if (!theApp.IsResetting())
	{
		// der Normalfall
		if (theApp.GetSettings().TraceClientRequests())
		{
			WK_TRACE(_T("Request Client %08lx\n"),camID.GetID());
		}
		CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByClientID(camID);
		if (pCamera)
		{
			CIPCPictureRecord *pPict = NULL;
			pCamera->Lock();
			if (pCamera->GetLivePicture())
			{
				pPict = new CIPCPictureRecord(*pCamera->GetLivePicture());
			}
			pCamera->Unlock();

			if (pPict)
			{
				// set there was ever a request to true
				BOOL bFast = FALSE;
				m_csConfirms.Lock();

				if (   pPict->GetCompressionType() == COMPRESSION_JPEG
					|| (pPict->GetCompressionType() == COMPRESSION_YUV_422 && m_pClient->IsLocal())
					|| (pPict->GetCompressionType() == COMPRESSION_RGB_24 && m_pClient->IsLocal())
					)
				{
					bFast = m_stConfirm[x] < pPict->GetTimeStamp();
				}
				else if (pPict->GetCompressionType() == COMPRESSION_MPEG4)
				{
					bFast = FALSE;
					// bFast = (m_wBlockNr[x] == 0	|| m_stConfirm[x] < pPict->GetTimeStamp());
				}
				m_csConfirms.Unlock();

				if (bFast)
				{
					if (theApp.GetSettings().TraceClientConfirms())
					{
						WK_TRACE(_T("confirm fast %08lx %s,%d BlNr %d %s %s\n"),
									camID.GetID(),
									pPict->GetTimeStamp().GetDateTime(),
									pPict->GetTimeStamp().wMilliseconds,
									pPict->GetBlockNr(),
									SPT2TCHAR(pPict->GetPictureType()),
									NameOfEnum(pPict->GetCompressionType()));
					}
					m_iFastConfirms++;
					SendPicture(pCamera,*pPict,0,0,FALSE);
				}
			}
			WK_DELETE(pPict);
		}
		else
		{
			WK_TRACE_ERROR(_T("no Ouput found for JPEG request %08lx\n"),camID.GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSync(DWORD dwTickSend, DWORD dwType,DWORD dwUserID)
{
	DoConfirmSync(GetTickCount(), dwTickSend, dwType, dwUserID);
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestDisconnect()
{
	DelayedDelete();
	theApp.GetOutputClients().ClientDisconnected(m_pClient->GetID());
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
	// TODO remove all from H.263 Codecs
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSetRelay(CSecID id, BOOL bClosed)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 3,	NULL);
		return;
	}
	COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(id);

	if (pOutput)
	{
		COutputGroup* pOutputGroup = pOutput->GetGroup();

		if (pOutputGroup)
		{
            pOutputGroup->DoRequestSetRelay(pOutput->GetUnitID(),bClosed);
		}
		else
		{
			WK_TRACE(_T("no group for set relais %08lx\n"),id.GetID());
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("no output for set relais %08lx\n"),id.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestGetValue(CSecID id,const CString &sKey,DWORD dwServerData)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 3,	NULL);
		return;
	}

	if (theApp.GetSettings().TraceClientRequests())
	{
		WK_TRACE(_T("request get value %08lx %s\n"),id.GetID(),sKey);
	}

	if (   (id == SECID_NO_ID)
		|| (0 == sKey.Find(_T("Timer")))
	   )
	{
		theApp.OnRequestGetValue(id,sKey,dwServerData,this);
	}
	else
	{
		COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(id);

		if (pOutput)
		{
			if (sKey == CSD_MODE)
			{
				if (pOutput->IsMD())
				{
					DoConfirmGetValue(id,sKey,CSD_MD,0);
				}
				else if (pOutput->IsUVV())
				{
					DoConfirmGetValue(id,sKey,CSD_UVV,0);
				}

			}
			else if (sKey == CSD_NAME)
			{
				DoConfirmGetValue(id,sKey,pOutput->GetName(),0);
			}
			else if (sKey == CSD_IS_TIMER_ACTIVE)
			{
				DoConfirmGetValue(id,sKey,pOutput->IsTimerActive() ? CSD_ON : CSD_OFF,0);
			}
			else if (sKey == CSD_DWELL)
			{
				if (!pOutput->IsRelay())
				{
					CCamera* pCamera = (CCamera*)pOutput;
					CString sValue;
					sValue.Format(_T("%d"),pCamera->GetDwellTime());
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}
			else if (sKey == CSD_FPS)
			{
				if (!pOutput->IsRelay())
				{
					if (id.GetSubID() == SECID_NO_SUBID)
					{
						COutputGroup* pOutputGroup = pOutput->GetGroup();
						if (pOutputGroup)
						{
							pOutputGroup->DoRequestGetValue(id, sKey, m_pClient->GetID());
						}
						else
						{
							WK_TRACE_ERROR(_T("no group for get value %08lx, %s\n"),id.GetID(),sKey);
						}
					}
					else
					{
						CCamera* pCamera = (CCamera*)pOutput;
						CString sValue;
						sValue.Format(_T("%d"),pCamera->GetFPS());
						DoConfirmGetValue(id,sKey,sValue,dwServerData);
					}
				}
			}
			else if (sKey == CSD_ALARMCALL_EVENT)
			{
				if (!pOutput->IsRelay())
				{
					CCamera* pCamera = (CCamera*)pOutput;
					CString sValue;

					switch (pCamera->GetAlarmCallHandling())
					{
					case 0:
						sValue = CSD_ALARMCALL_EVENT_OFF;
						break;
					case ALARMCALL_ACTIVITY:
						sValue = CSD_ALARMCALL_EVENT_A;
						break;
					case ALARMCALL_EXTERN:
						sValue = CSD_ALARMCALL_EVENT_1;
						break;
					case ALARMCALL_ACTIVITY|ALARMCALL_EXTERN:
						sValue = CSD_ALARMCALL_EVENT_A1;
						break;
					case ALARMCALL_SUSPECT:
						sValue = CSD_ALARMCALL_EVENT_2;
						break;
					case ALARMCALL_SUSPECT|ALARMCALL_ACTIVITY:
						sValue = CSD_ALARMCALL_EVENT_A2;
						break;
					case ALARMCALL_SUSPECT|ALARMCALL_EXTERN:
						sValue = CSD_ALARMCALL_EVENT_12;
						break;
					case ALARMCALL_ACTIVITY|ALARMCALL_SUSPECT|ALARMCALL_EXTERN:
						sValue = CSD_ALARMCALL_EVENT_A12;
						break;
					}
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}

			//on which card (Jacob/Savic/Tasha) is the current camera
			else if (sKey == CSD_CARD_TYPE)
			{
				CString sValue;
				COutputGroup* pOutputGroup = pOutput->GetGroup();
				if(pOutputGroup)
				{
					if (pOutputGroup->IsJacob())
					{
						sValue = CSD_CARD_TYPE_JACOB;
					}
					else if (pOutputGroup->IsSavic())
					{
						sValue = CSD_CARD_TYPE_SAVIC;
					}
					else if (pOutputGroup->IsTasha())
					{
						sValue = CSD_CARD_TYPE_TASHA;
					}
					else if (pOutputGroup->IsQ())
					{
						sValue = CSD_CARD_TYPE_Q;
					}
				}

				DoConfirmGetValue(id,sKey,sValue,dwServerData);
			}
			else if (sKey == CSD_COMPRESSION)
			{
				COutputGroup* pOutputGroup = pOutput->GetGroup();
				if(pOutputGroup->HasCameras())
				{
					CString sValue;
					CCamera* pCamera = (CCamera*)pOutput;
					Compression CamCompression = pCamera->GetCompression();
					sValue = NameOfEnum(CamCompression);
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}
			else if (sKey == CSD_CAM_PTZ_TYPE)
			{
				COutputGroup* pOutputGroup = pOutput->GetGroup();
				if(pOutputGroup->HasCameras())
				{
					CString sValue;
					CCamera* pCamera = (CCamera*)pOutput;
					sValue = NameOfEnum(pCamera->GetType());
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}
			else if (sKey == CSD_CAM_PTZ_ID)
			{
				COutputGroup* pOutputGroup = pOutput->GetGroup();
				if(pOutputGroup->HasCameras())
				{
					CString sValue;
					CCamera* pCamera = (CCamera*)pOutput;
					sValue.Format(_T("%u"), pCamera->GetPTZID());
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}
			else if (sKey == CSD_CAM_PTZ_COMPORT)
			{
				COutputGroup* pOutputGroup = pOutput->GetGroup();
				if(pOutputGroup->HasCameras())
				{
					CString sValue;
					CCamera* pCamera = (CCamera*)pOutput;
					sValue.Format(_T("%i"), pCamera->GetComPort());
					DoConfirmGetValue(id,sKey,sValue,dwServerData);
				}
			}
			else if (sKey == CSD_VIDEO_FORMAT)
			{
				CString sValue;
				sValue.Format(_T("%i"), theApp.GetSettings().GetMPEG4());
				DoConfirmGetValue(id,sKey,sValue,dwServerData);
			}
			else
			{
				COutputGroup* pOutputGroup = pOutput->GetGroup();

				if (pOutputGroup)
				{
					pOutputGroup->DoRequestGetValue(pOutput->GetUnitID(),sKey,m_pClient->GetID());
				}
				else
				{
					WK_TRACE_ERROR(_T("no group for get value %08lx, %s\n"),id.GetID(),sKey);
				}
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("no output for get value %08lx, %s\n"),id.GetID(),sKey);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSetValue(CSecID id,const CString &sKey,const CString &sValue, DWORD dwServerData)
{
	TRACE(_T("OnRequestSetValue %s,%s\n"),sKey,sValue);
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 3,	NULL);
		return;
	}

	if (   id == SECID_NO_ID
		&& (0 == sKey.CompareNoCase(CSD_DV_CONFIG))
		)
	{
		m_pClient->SetSupervisor(sValue == CSD_ON);
	}
	else if (   (id == SECID_NO_ID)
		|| (0 == sKey.Find(_T("Timer")))
	   )
	{
		theApp.OnRequestSetValue(id,sKey,sValue,dwServerData,this);
	}
	else
	{
		COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(id);

		if (pOutput)
		{
			COutputGroup* pOutputGroup = pOutput->GetGroup();

			if (pOutputGroup)
			{
				int iCard = pOutputGroup->GetCardNr();

				if (sKey == CSD_V_OUT)
				{
					if (pOutputGroup->HasCameras())
					{
						CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
						// always map to correct PORTx
						// ignore sValue from Client
						//					TRACE(_T("OnRequestSetValue %08lx %s %s\n"),id.GetID(),sKey,sValue);

						if (theApp.GetVOutModePort(iCard,0) == VOUT_CLIENT)
						{
							if (theApp.GetSettings().TraceVOUT())
							{
								WK_TRACE(_T("Manual %s CAM %08lx\n"),CSD_PORT0,pOutput->GetUnitID().GetID());
							}
							pOutputGroup->DoRequestSetValue(pOutput->GetUnitID(),
								CSD_V_OUT,
								CSD_PORT0,
								m_pClient->GetID());
						}
						if (theApp.GetVOutModePort(iCard,1) == VOUT_CLIENT)
						{
							if (theApp.GetSettings().TraceVOUT())
							{
								WK_TRACE(_T("Manual %s CAM %08lx\n"),CSD_PORT1,pOutput->GetUnitID().GetID());
							}
							pOutputGroup->DoRequestSetValue(pOutput->GetUnitID(),
								CSD_V_OUT,
								CSD_PORT1,
								m_pClient->GetID());
						}
						// that's the active cam, remember it
						m_idActiceVOUTCam = id;
						pCameraGroup->SetClientActiveCam(id);
					}
				}
				else if (sKey == CSD_MODE)
				{
					if (sValue == CSD_MD)
					{
						pOutput->SetMode(COutput::OM_MD);
						DoConfirmSetValue(id,sKey,sValue,0);
					}
					else if (sValue == CSD_UVV)
					{
						pOutput->SetMode(COutput::OM_UVV);
						DoConfirmSetValue(id,sKey,sValue,0);
					}
					theApp.OnOutputChanged(FALSE);
				}
				else if (sKey == CSD_NAME)
				{
					theApp.SetOutputName(m_pClient->GetID(),pOutput,sValue);
					DoConfirmSetValue(id,sKey,pOutput->GetName(),0);
				}
				else if (sKey == CSD_DWELL)
				{
					if (!pOutput->IsRelay())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						DWORD dwDwell = 0;
						if (1==_stscanf(sValue,_T("%d"),&dwDwell))
						{
							pCamera->SetDWellTime(dwDwell);
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
					}
					theApp.OnOutputChanged(FALSE);
				}
				else if (sKey == CSD_FPS)
				{
					if (!pOutput->IsRelay())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						DWORD dwFPS = 0;
						if (1==_stscanf(sValue,_T("%d"),&dwFPS))
						{
							pCamera->SetFPS(dwFPS);
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
					}
					theApp.OnOutputChanged(FALSE);
				}
				else if (sKey == CSD_ALARMCALL_EVENT)
				{
					if (!pOutput->IsRelay())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						BOOL bConfirm = FALSE;

						if (sValue == CSD_ALARMCALL_EVENT_OFF)
						{
							pCamera->SetAlarmCallHandling(0x00);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_A)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_ACTIVITY);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_1)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_EXTERN);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_A1)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_ACTIVITY|ALARMCALL_EXTERN);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_2)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_SUSPECT);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_A2)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_ACTIVITY|ALARMCALL_SUSPECT);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_12)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_SUSPECT|ALARMCALL_EXTERN);
							bConfirm = TRUE;
						}
						else if (sValue == CSD_ALARMCALL_EVENT_A12)
						{
							pCamera->SetAlarmCallHandling(ALARMCALL_ACTIVITY|ALARMCALL_SUSPECT|ALARMCALL_EXTERN);
							bConfirm = TRUE;
						}

						if (bConfirm)
						{
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
						theApp.OnOutputChanged(FALSE);
					}
				}
				else if (sKey == CSD_COMPRESSION)
				{
					COutputGroup* pOutputGroup = pOutput->GetGroup();
					if(pOutputGroup->HasCameras())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						Compression compression = (Compression)(_ttoi(sValue));
						pCamera->SetCompression(compression);
						DoConfirmSetValue(id,sKey,sValue,dwServerData);
						theApp.OnOutputChanged(FALSE);
					}
				}
				else if (sKey == CSD_CAM_PTZ_TYPE)
				{
					//TODO TKR PTZ Type setzen
					COutputGroup* pOutputGroup = pOutput->GetGroup();
					if(pOutputGroup->HasCameras())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						CameraControlType type = NameToCameraControlType(sValue);
						pCamera->SetType(type);
						theApp.m_bErrorOccurred = FALSE;
						theApp.OnOutputChanged(TRUE);
						if (!theApp.m_bErrorOccurred)
						{
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
					}
				}
				else if (sKey == CSD_CAM_PTZ_ID)
				{
					//TODO TKR PTZ ID Type setzen
					COutputGroup* pOutputGroup = pOutput->GetGroup();
					if(pOutputGroup->HasCameras())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						DWORD dwPTZID = _ttoi(sValue);
						pCamera->SetPTZID(dwPTZID);
						theApp.m_bErrorOccurred = FALSE;
						theApp.OnOutputChanged(TRUE);
						if (!theApp.m_bErrorOccurred)
						{
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
					}
				}
				else if (sKey == CSD_CAM_PTZ_COMPORT)
				{
					//TODO TKR PTZ ComPort Type liefern
					COutputGroup* pOutputGroup = pOutput->GetGroup();
					if(pOutputGroup->HasCameras())
					{
						CCamera* pCamera = (CCamera*)pOutput;
						int iComPort = _ttoi(sValue);
						pCamera->SetComPort(iComPort);
						theApp.m_bErrorOccurred = FALSE;
						theApp.OnOutputChanged(TRUE);
						if (!theApp.m_bErrorOccurred)
						{
							DoConfirmSetValue(id,sKey,sValue,dwServerData);
						}
					}
				}
				else if (sKey == CSD_VIDEO_FORMAT)
				{
					BOOL bMPEG4 = _ttoi(sValue);
					theApp.GetSettings().SetMPEG4(bMPEG4);
					DoConfirmSetValue(id,sKey,sValue,dwServerData);
				}
				else
				{
					pOutputGroup->DoRequestSetValue(pOutput->GetUnitID(),
						sKey,
						sValue,
						m_pClient->GetID());
					if (sKey == CSD_TERM)
					{
						if (pOutputGroup->HasCameras())
						{
							CCamera* pCamera = (CCamera*)pOutput;
							pCamera->SetTerminateVideoSignal(sValue == CSD_ON);
							theApp.OnOutputChanged(FALSE);
						}
					}
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("no group for set value %08lx %s %s\n"),id.GetID(),sKey,sValue);
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("no output for set value %08lx %s %s\n"),id.GetID(),sKey,sValue);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	if (m_pClient->IsSupervisor())
	{
		COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(camID);
		
		if (pOutput)
		{
			COutputGroup* pOutputGroup = pOutput->GetGroup();
			
			if (pOutputGroup)
			{
				pOutputGroup->DoRequestGetMask(pOutput->GetUnitID(),
					type,
					m_pClient->GetID());
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestGetMask %08lx no camera\n"),camID.GetID());
		}
	}
	else
	{
		DoIndicateError(GetLastCmd(),camID,CIPC_ERROR_ACCESS_DENIED,type);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	if (m_pClient->IsSupervisor())
	{
		COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(camID);
		
		if (pOutput)
		{
			COutputGroup* pOutputGroup = pOutput->GetGroup();
			
			if (pOutputGroup)
			{
				pOutputGroup->DoRequestSetMask(pOutput->GetUnitID(),
					m_pClient->GetID(),
					mask);
			}
		}
	}
	else
	{
		DoIndicateError(GetLastCmd(),camID,CIPC_ERROR_ACCESS_DENIED,mask.GetType());
	}
}
/////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::CreateTranscoder(int iNumPictures,DWORD dwCalculatedBitrate)
{
	m_csTranscoder.Lock();
	if (   (   m_pClient->UseH263() 
			|| (m_pClient->CanMpeg4() && m_pClient->IsLowBandwidth()))
		&& m_pTranscoder == NULL)
	{
		m_pTranscoder = new CTranscoderThread(this);
		m_pTranscoder->OnModified(iNumPictures,dwCalculatedBitrate);
		m_pTranscoder->StartThread();
	}
	m_csTranscoder.Unlock();
}
/////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::DestroyTranscoder()
{
	CTranscoderThread* pTranscoder = NULL;
	m_csTranscoder.Lock();
	pTranscoder = m_pTranscoder;
	m_pTranscoder = NULL;
	m_csTranscoder.Unlock();

	if (pTranscoder)
	{
		pTranscoder->StopThread();
		WK_DELETE(pTranscoder);
	}
}
