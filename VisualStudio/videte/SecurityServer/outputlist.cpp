/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputlist.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/outputlist.cpp $
// CHECKIN:		$Date: 25.08.06 22:06 $
// VERSION:		$Revision: 93 $
// LAST CHANGE:	$Modtime: 25.08.06 21:54 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "sec3.h"
#include "CipcOutputServer.h"
#include "OutputList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CJpegStatistics::CJpegStatistics()
: CStatistics(20)
{
	m_iJPEGQuality = 75;
}
/////////////////////////////////////////////////////////////////////////////
CJpegStatistics::~CJpegStatistics()
{
}
/////////////////////////////////////////////////////////////////////////////
int CJpegStatistics::GetJPEGQuality() const
{
	return m_iJPEGQuality;
}
/////////////////////////////////////////////////////////////////////////////
void CJpegStatistics::SetJPEGQuality(int iJPEGQuality)
{
	m_iJPEGQuality = iJPEGQuality;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
COutput::COutput(CIPCOutputServer* pParent, WORD subID,SecOutputType type) 
{ 	
	m_pParent = pParent;
	m_id = CSecID(pParent->GetGroupID(), subID);
	m_sName.Format(_T("ohne Name(%x)"),m_id.GetID());
	m_outputType=type; 
	m_bRelayClosed = FALSE;
	m_bReference = TRUE;
	m_bTermination = TRUE;
	m_bIsFixed = TRUE;
	m_ePTZCmd = CCC_INVALID;
	m_iPTZInterface = 0;
	// new relais attributes
	m_dwRelayClosingTime = 0;
	m_clientRelayControl = RCT_INVALID;
	m_bClientMayKillRelayProcess = FALSE;
	m_bKeepRelayClosedAfterClientDisconnect = FALSE;
	m_pJPEGStatistics = NULL;

	m_dwX = 0;
	m_dwY = 0;

	// initialize the Savic Statistics
	if (   type == OUTPUT_CAMERA
		&& (pParent->GetCompressionType() == COMPRESSION_YUV_422)
		)
	{
		// ein 5er Array fuer die 5 verschiedenen Kompressionsstufen.
		m_pJPEGStatistics = new CJpegStatistics[10];
		m_pJPEGStatistics[0].SetJPEGQuality(80);
		m_pJPEGStatistics[1].SetJPEGQuality(75);
		m_pJPEGStatistics[2].SetJPEGQuality(70);
		m_pJPEGStatistics[3].SetJPEGQuality(60);
		m_pJPEGStatistics[4].SetJPEGQuality(50);

		m_pJPEGStatistics[5].SetJPEGQuality(80);
		m_pJPEGStatistics[6].SetJPEGQuality(75);
		m_pJPEGStatistics[7].SetJPEGQuality(70);
		m_pJPEGStatistics[8].SetJPEGQuality(60);
		m_pJPEGStatistics[9].SetJPEGQuality(50);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
COutput::~COutput()
{
	WK_DELETE_ARRAY(m_pJPEGStatistics);
}
///////////////////////////////////////////////////////////////////////////////////////
void COutput::SetCurrentArchiveID(CSecID id)
{
	m_idCurrentArchive = id;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutput::GetMDCoordinates(DWORD& dwX,DWORD& dwY)
{
	dwX = m_dwX;
	dwY = m_dwY;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutput::SetMDCoordinates(DWORD dwX,DWORD dwY)
{
	m_dwX = dwX;
	m_dwY = dwY;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutput::SetFixed(BOOL bFixed)
{
	// NOT YET any checks
	m_bIsFixed=bFixed;
}
void COutput::SetPTZ(CameraControlCmd ccc, int iPTZInterface)
{
	m_ePTZCmd = ccc;
	m_iPTZInterface = iPTZInterface;
}

void COutput::SetReference(BOOL bReference)
{
	m_bReference=bReference;
}
void COutput::SetTermination(BOOL bTerminated)
{
	m_bTermination=bTerminated;
}

void COutput::ReadRelayAttributesFromRegistryString(const CString &s)
{
// \RT relais closing time/Sekunden
// \RC relais control type/edge oder state
// \CK client may kill processes/BOOL 0,1
// \KC Keep closed KeepClosedAfterClientDisconnect

	m_dwRelayClosingTime = (DWORD) CWK_Profile::GetNrFromString(s, _T("\\RT"), 0);
		
	int iControl = CWK_Profile::GetNrFromString(s, _T("\\RC"), 3);	// 3 == unset
	if (iControl==0) {
		m_clientRelayControl = RCT_EDGE;
	} else if (iControl==1 || iControl==3) {
		m_clientRelayControl = RCT_STATE;
	} else {
		WK_TRACE_ERROR(_T("Invalid relais control type %d\n"),iControl);
	}
	int iTmp;
	iTmp = CWK_Profile::GetNrFromString(s, _T("\\CK"), 0);
	m_bClientMayKillRelayProcess = (iTmp!=0);
	
	iTmp = CWK_Profile::GetNrFromString(s, _T("\\KC"), 0);
	m_bKeepRelayClosedAfterClientDisconnect = (iTmp!=0);

}

/////////////////////////////////////////////////////////////////////////////
// COutputList
COutputList::COutputList()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}

/////////////////////////////////////////////////////////////////////////////
COutputList::~COutputList()
{
	SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::Load(CWK_Profile& wkp)
{
	int iMaxGroups = wkp.GetInt(_T("OutputGroups"), _T("MaxGroups"), 0);

	for (int i=0;i<iMaxGroups;i++)
	{
		CString sGroup;
		sGroup = wkp.GetString(_T("OutputGroups"),i+1,_T(""));
		if (sGroup.GetLength()) 
		{
			AddOutputGroup(sGroup);	
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::Save(CWK_Profile& wkp)
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Save(wkp);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputList::AddOutputGroup(LPCTSTR str)
{
	CWK_Profile wkp;
	int iNr = CWK_Profile::GetNrFromString(str, _T("\\NR"), 0L);
	if (iNr<=0) 
	{
		return FALSE;	// EXIT
	}

	CString sName= CWK_Profile::GetStringFromString(str, _T("\\CO"), _T(""));
	CString sShmName = CWK_Profile::GetStringFromString(str, _T("\\SM"), _T(""));	

	// get the unique id
	WORD storedID = (WORD)CWK_Profile::GetHexFromString(str, _T("\\ID"), -1);	
	ASSERT(storedID!=-1); 
	// NOY YET check for duplicate IDs

	CIPCOutputServer* pGroup = NewGroup(sName, iNr, sShmName, CSecID(storedID,0));


	if (pGroup)
	{
		pGroup->Load(wkp,iNr);
	}


	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void COutputList::StartThreads()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Start();
		Sleep(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// pName:		Der Name der neue Gruppe
// iNr:			Die Nummer der Ausgänge
// RETURN:		Ein Pointer auf die neue Gruppe wenn erfolgreich, sonst NULL
CIPCOutputServer* COutputList::NewGroup(const CString &sName, int iNr, 
										const CString &sShmName,
										CSecID newID)
{
	CIPCOutputServer* pGrp = NULL;
	if (iNr>=0) 
	{
		pGrp = new CIPCOutputServer(sName, iNr, sShmName);
		SafeAdd(pGrp);
		pGrp->SetGroupID(newID.GetGroupID());
	}
	return pGrp;
}
/////////////////////////////////////////////////////////////////////////////
CIPCOutputServer* COutputList::GetGroupAt(int i)
{ 
	CIPCOutputServer* pReturn = NULL;
	if (IsInArray(i, GetSize())) 
	{
		pReturn = GetAtFast(i);
	} 
	return pReturn;
}
/////////////////////////////////////////////////////////////////////////////
CIPCOutputServer* COutputList::GetGroupShmName(const CString& sShmName)
{
	CIPCOutputServer* pReturn = NULL;
	for (int i=0;i<GetSize();i++) 
	{
		if (sShmName == GetAtFast(i)->GetShmName()) 
		{
			pReturn = GetAtFast(i);
			break;
		}
	}
	return pReturn;
}
/////////////////////////////////////////////////////////////////////////////
CIPCOutputServer* COutputList::GetGroupByID(CSecID id)
{
	CIPCOutputServer* pReturn = NULL;
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID() == GetAtFast(i)->GetGroupID()) 
		{
			pReturn = GetAtFast(i);
			break;
		}
	}
	return pReturn;
}
/////////////////////////////////////////////////////////////////////////////
COutput* COutputList::GetOutputByID(CSecID id)
{
	COutput* pReturn = NULL;
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID() == GetAtFast(i)->GetGroupID()) 
		{
			pReturn = GetAtFast(i)->GetOutputAtWritable(id.GetSubID());
			break;
		}
	}
	return pReturn;
}
/////////////////////////////////////////////////////////
void COutputList::DeleteUnnecessaryReferenzImages()
{
	CIPCOutputServer* pOutputGroup;
	for (int i=0;i<GetSize();i++) 
	{
		pOutputGroup = GetGroupAt(i);
		for (int j=0;j<pOutputGroup->GetSize();j++)
		{
			const COutput* pOutput = pOutputGroup->GetOutputAt(j);
			if (pOutput && !pOutput->IsReference())
			{
				// keine Referenzkamera
				// also testen, ob da noch
				// Bilder da sind
				CString sReference,sIst;
				sReference.Format(_T("%s\\%08lx\\%08lx.jpg"),
					CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST,
					pOutput->GetID());
				if (DoesFileExist(sReference))
				{
					if (DeleteFile(sReference))
					{
						WK_TRACE(_T("reference image for %s deleted\n"),pOutput->GetName());
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot delete reference image for %s , %s\n"),
							pOutput->GetName(),GetLastErrorString());
					}
				}
				sIst.Format(_T("%s\\%08lx\\ist\\%08lx.jpg"),
					CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST,
					pOutput->GetID());
				if (DoesFileExist(sIst))
				{
					if (DeleteFile(sIst))
					{
						WK_TRACE(_T("ist image for %s deleted\n"),pOutput->GetName());
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot delete ist image for %s , %s\n"),
							pOutput->GetName(),GetLastErrorString());
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////
void COutputList::WriteNamesIni()
{
	CIPCOutputServer* pOutputGroup;
	CString sIni,sKey;

	sIni.Format(_T("%s\\%08lx\\names.ini"),
		CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);

	// ML 23.9.99 Falls das Verzeichnis nicht existiert, dieses unbedingt anlegen
	CString sDir;
	sDir.Format(_T("%s\\%08lx"),CNotificationMessage::GetWWWRoot(), SECID_LOCAL_HOST);
	WK_CreateDirectory(sDir);
	
	// delete old entries
	WritePrivateProfileString(_T("Camera"),NULL,NULL,sIni);

	for (int i=0;i<GetSize();i++) 
	{
		pOutputGroup = GetGroupAt(i);
		for (int j=0;j<pOutputGroup->GetSize();j++)
		{
			const COutput* pOutput = pOutputGroup->GetOutputAt(j);
			if (pOutput && pOutput->IsReference())
			{
				// Referenzkamera
				// also Name in names.ini schreiben
				sKey.Format(_T("%08lx"),pOutput->GetID().GetID());
				WritePrivateProfileString(_T("Camera"),sKey,pOutput->GetName(),sIni);
			}
		}
	}
	// flush it
	WritePrivateProfileString(NULL,NULL,NULL,sIni);
}
/////////////////////////////////////////////////////////////
void COutputList::OnLastClientDisconnected()
{
	for (int i=0;i<GetSize();i++) 
	{
		CIPCOutputServer* pOutputGroup = GetGroupAt(i);
		for (int j=0;j<pOutputGroup->GetSize();j++)
		{
			const COutput* pOutput = pOutputGroup->GetOutputAt(j);
			if (   pOutput 
				&& !pOutput->IsFixed())
			{
				CameraControlCmd ccc = pOutput->GetPTZCmd();
				if (ccc != CCC_INVALID)
				{
					CIPCInputServer* pInputGroup = theApp.GetInputGroups().GetGroupByShmName(SM_COMMUNIT_INPUT);
					if (   pInputGroup
						&& pInputGroup->GetSize()>0) 
					{
						WK_TRACE(_T("set camera %s to position %s by disconnect\n"),pOutput->GetName(),NameOfEnum(ccc));
						pInputGroup->DoRequestCameraControl(pInputGroup->GetInputAt(0)->GetID(),pOutput->GetID(), ccc, 0);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////
int COutput::GetJPEGQuality(Compression comp, Resolution res)
{
	int i = 0;
	int j = 0;

	if (m_pJPEGStatistics)
	{
		switch (res)
		{
		case RESOLUTION_2CIF:
			i = 0;
			break;
		case RESOLUTION_QCIF:
			i = 1;
			break;
		}
		switch (comp)
		{
		case COMPRESSION_5:
			j = 0;
			break;
		case COMPRESSION_7:
			j = 1;
			break;
		case COMPRESSION_12:
			j = 2;
			break;
		case COMPRESSION_17:
			j = 3;
			break;
		case COMPRESSION_22:
			j = 4;
			break;
		}
	}

	int iJPEGQuality = m_pJPEGStatistics[i*5+j].GetJPEGQuality();
	int iAverageSize = m_pJPEGStatistics[i*5+j].GetAverageValue();

	// 8,16,24,32,40 kB
	// halbe Groesse bei halber Aufloesung
	int iJpegSize = ((5-j)*8*1024)/(i+1);
	
	if (iAverageSize > ((iJpegSize*105)/100))
	{
		iJPEGQuality--;
		if (iJPEGQuality<25)
		{
			iJPEGQuality = 25;
		}
	}
	else if (iAverageSize < ((iJpegSize*95)/100))
	{
		iJPEGQuality++;
		if (iJPEGQuality>100)
		{
			iJPEGQuality = 100;
		}
	}

	m_pJPEGStatistics[i*5+j].SetJPEGQuality(iJPEGQuality);

	return iJPEGQuality;
}
/////////////////////////////////////////////////////////////
void COutput::AddCompressedJPEGSize(DWORD dwSize,Compression comp,Resolution res)
{
	int i = 0;
	int j = 0;

	if (m_pJPEGStatistics)
	{
		switch (res)
		{
		case RESOLUTION_2CIF:
			i = 0;
			break;
		case RESOLUTION_QCIF:
			i = 1;
			break;
		}
		switch (comp)
		{
		case COMPRESSION_5:
			j = 0;
			break;
		case COMPRESSION_7:
			j = 1;
			break;
		case COMPRESSION_12:
			j = 2;
			break;
		case COMPRESSION_17:
			j = 3;
			break;
		case COMPRESSION_22:
			j = 4;
			break;
		}
		m_pJPEGStatistics[i*5+j].AddValue(dwSize);
	}
}

