/* GlobalReplace: EX_OUTPUT_CAMERA_COLOR --> EX_OUTPUT_CAMERA_FBAS */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: output.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/output.cpp $
// CHECKIN:		$Date: 28.08.06 16:51 $
// VERSION:		$Revision: 31 $
// LAST CHANGE:	$Modtime: 28.08.06 15:58 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "Systemverwaltung.h"
#include "output.h"
#include "outputgroup.h"

#include "MainFrm.h"
#include "SVDoc.h"
#include "SVPage.h"
#include "globals.h"

/////////////////////////////////////////////////////////////////////////////
// COutput
/////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(COutput, CObject)

COutput::COutput(WORD groupID, WORD subID) 
{ 	
	m_id = CSecID(groupID, subID);
	m_ExOutputType = EX_OUTPUT_OFF; 

	m_bReference = FALSE;
	m_bTerminated = TRUE;
	m_bSN = FALSE;
	m_SNType = CCT_UNKNOWN;
	m_iPTZInterface = 0;
	m_dwSNID = 0;
	m_eSNCmd = CCC_INVALID;

	m_HoldClose = 1;
	m_bSTM = FALSE;
	m_bEdge = FALSE;
	m_bClient = FALSE;
	m_bKeep = FALSE;
	m_bDeleteActivations = FALSE;

	m_uPort = 0;
	m_nOutputNr = 0;
	m_sRotation = _T("0");
}
/////////////////////////////////////////////////////////////////////
COutput::~COutput() 
{ 	
}
/////////////////////////////////////////////////////////////////////
const COutput& COutput::operator =(const COutput &src)
{
	m_id = src.m_id;
	m_ExOutputType = src.m_ExOutputType; 
	m_sName =	src.m_sName;

	m_bReference = src.m_bReference;
	m_bTerminated = src.m_bTerminated;
	m_bSN = src.m_bSN;
	m_SNType = src.m_SNType;
	m_iPTZInterface = src.m_iPTZInterface;
	m_dwSNID = src.m_dwSNID;
	m_eSNCmd = src.m_eSNCmd;

	m_HoldClose = src.m_HoldClose;
	m_bEdge = src.m_bEdge;
	m_bClient = src.m_bClient;
	m_bKeep = src.m_bKeep;

	return *this;
}
//////////////////////////////////////////////////////////////////////////////
BOOL COutput::Load(CWK_Profile& wkp)
{
	CString sSection;
	CString sOutputDefine;

	sSection.Format(_T("%s\\Group%04x"),SEC_NAME_OUTPUTGROUPS,m_id.GetGroupID());
	sOutputDefine = wkp.GetString(sSection,m_id.GetSubID()+1,_T(""));

	if (sOutputDefine.IsEmpty())
	{
		return FALSE;
	}

	m_sType.Empty();
	// name
	m_sName = wkp.GetStringFromString(sOutputDefine, INI_COMMENT, _T(""));
	m_ExOutputType = (ExOutputType)wkp.GetNrFromString(sOutputDefine, INI_OUTPUT_TYPE, EX_OUTPUT_OFF);

	switch (m_ExOutputType)
	{
		case EX_OUTPUT_CAMERA_FBAS: case EX_OUTPUT_CAMERA_SVHS:
			m_bSN = wkp.GetNrFromString(sOutputDefine, INI_SCHWENK_NEIGER, 0);
			m_bReference = wkp.GetNrFromString(sOutputDefine, INI_REFERENCE, m_bReference);
			m_bTerminated = wkp.GetNrFromString(sOutputDefine, INI_TERMINATION, m_bTerminated);
			if (m_bSN)
			{
				m_eSNCmd = (CameraControlCmd)wkp.GetNrFromString(sOutputDefine,_T("SNCmd"),CCC_INVALID);
				int iCom = wkp.GetNrFromString(sOutputDefine,_T("\\I"),0);
				if (iCom>32)
				{
					m_iPTZInterface = iCom;
				}
			}
			m_sType     = wkp.GetStringFromString(sOutputDefine, INI_CAMERA_TYPE, NULL);
			m_nOutputNr = wkp.GetNrFromString(sOutputDefine, INI_CAMERA_NR, 0);
			break;
		case EX_OUTPUT_RELAY:
			m_HoldClose = wkp.GetNrFromString(sOutputDefine, _T("\\RT"), 0);
			m_bEdge = wkp.GetNrFromString(sOutputDefine, _T("\\RC"), 1);
			m_bClient = wkp.GetNrFromString(sOutputDefine, _T("\\CK"), 0);
			m_bKeep = wkp.GetNrFromString(sOutputDefine, INI_KEEP_CLOSED, 0);
			m_bSN = FALSE;
			if (   m_bEdge
				|| m_bKeep)
			{
				m_bClient = TRUE;
			}
			m_sType     = wkp.GetStringFromString(sOutputDefine, INI_RELAIS_TYPE, NULL);
			m_nOutputNr = wkp.GetNrFromString(sOutputDefine, INI_RELAIS_NR, 0);
			break;
		case EX_OUTPUT_OFF:	// off is saved as camera
			m_sType     = wkp.GetStringFromString(sOutputDefine, INI_CAMERA_TYPE, NULL);
			m_nOutputNr = wkp.GetNrFromString(sOutputDefine, INI_CAMERA_NR, 0);
			break;
		default:
			m_bSN = FALSE;
			break;
	}

	// get IP and ds parameter
	if (!m_sType.IsEmpty())
	{
		m_sDeviceName = wkp.GetStringFromString(sOutputDefine, INI_NAME, NULL);
		m_sUrl        = wkp.GetStringFromString(sOutputDefine, INI_URL, NULL);
		m_uPort       = wkp.GetNrFromString(sOutputDefine, INI_PORT, 0);
		m_sRotation   = wkp.GetStringFromString(sOutputDefine, INI_TURN, _T("0"));
		m_sCompression= wkp.GetStringFromString(sOutputDefine, INI_COMPRESSION, _T("MJPG"));
	}

	if (m_bSN)
	{
		// sn load under CommUnit
		// searching for my camid
		CString sSection,sKey,sValue,sTemp;
		int j;
		int k,e;

		DWORD dwComMask;
		DWORD dwBit = 1;
		
		dwComMask = wkp.GetInt(SECTION_COMMON,COMMON_COMM_PORT_INFO,0);

		sValue.Format(_T("%08x"),m_id.GetID());
		sTemp = wkp.GetString(_T("CommUnit\\RelayPTZ"),_T("CID"),_T(""));
		if (sValue==sTemp)
		{
			// it's a RelayPTZ
			m_SNType = CCT_RELAY_PTZ;
			m_iPTZInterface = 0;
			m_dwSNID = 0;
		}

		for (dwBit = 1,j=1; dwBit; dwBit<<=1,j++)
		{
			if (dwComMask & dwBit)
			{
				BOOL bFound = FALSE;
				sSection.Format(_T("CommUnit\\COM%d"),j);
				e = wkp.GetInt(sSection,_T("Num"),0);
				for (k=0;k<e;k++)
				{
					sKey.Format(_T("SID%d"),k);
					sTemp = wkp.GetString(sSection,sKey,_T(""));
					if (sTemp==sValue)
					{
						// bingo found
						m_SNType = (CameraControlType)wkp.GetInt(sSection,_T("Typ"),0);
						m_iPTZInterface = j;
						sKey.Format(_T("CID%d"),k);
						m_dwSNID = wkp.GetInt(sSection,sKey,0);
						bFound = TRUE;
						break;
					}
				}
				if (bFound)
				{
					break;
				}
			}
		}
	} // sn

	m_bDeleteActivations = FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL COutput::Save(CWK_Profile& wkp)
{
	CString sSection;
	CString sOutputDefine;

	sSection.Format(_T("%s\\Group%04x"),SEC_NAME_OUTPUTGROUPS,m_id.GetGroupID());

	switch (m_ExOutputType)
	{
		case EX_OUTPUT_OFF:
			sOutputDefine.Format(_T("%s%s\\TY%s\\CT%d"),
								INI_COMMENT, m_sName, 
								TypeNr2TypeName(m_ExOutputType),m_ExOutputType);
			break;
		case EX_OUTPUT_RELAY:
			sOutputDefine.Format(_T("%s%s\\TY%s\\CT%d\\SN%d\\RT%d\\RC%d\\CK%d\\KC%d"),
								INI_COMMENT, m_sName, 
								TypeNr2TypeName(m_ExOutputType),m_ExOutputType,
								m_bSN,
								m_HoldClose,m_bEdge,m_bClient,m_bKeep);
			break;
		case EX_OUTPUT_CAMERA_FBAS:
		case EX_OUTPUT_CAMERA_SVHS:
			sOutputDefine.Format(_T("%s%s\\TY%s\\SN%d\\CT%u\\REF%d\\TERM%d"),
								INI_COMMENT, m_sName, 
								TypeNr2TypeName(m_ExOutputType), 
								m_bSN,
								m_ExOutputType,
								m_bReference,
								m_bTerminated);
			if (m_bSN)
			{
				CString s;
				s.Format(_T("\\SNCmd%d\\I%d"),(int)m_eSNCmd,GetPTZInterface());
				sOutputDefine += s;
			}
			break;
	}

	// save additional parameters for ip and ds cameras
	if (!m_sType.IsEmpty())
	{
		CString sIPdata;
		switch (m_ExOutputType)
		{
			case EX_OUTPUT_CAMERA_FBAS:
			case EX_OUTPUT_CAMERA_SVHS:
			case EX_OUTPUT_OFF:	// off saves also all values as camera type
				sIPdata.Format(_T("%s%s%s%s%s%s%s%d%s%s%s%d%s%s"),
					INI_CAMERA_TYPE, m_sType,
					INI_NAME, m_sDeviceName,
					INI_URL, m_sUrl,
					INI_PORT, m_uPort,
					INI_TURN, m_sRotation,
					INI_CAMERA_NR, m_nOutputNr,
					INI_COMPRESSION, m_sCompression);
				break;
			case EX_OUTPUT_RELAY:
				sIPdata.Format(_T("%s%s%s%s%s%s%s%d%s%d"),
					INI_RELAIS_TYPE, m_sType,
					INI_NAME, m_sDeviceName,
					INI_URL, m_sUrl,
					INI_PORT, m_uPort,
					INI_RELAIS_NR, m_nOutputNr);
				break;
		}

		sOutputDefine += sIPdata;
	}

	wkp.WriteStringIndex(sSection,m_id.GetSubID()+1,sOutputDefine); 

	// sn save under CommUnit
	if (m_bSN)
	{
		CString sSection,sValue,sKey,sTemp;
		if (m_SNType == CCT_RELAY_PTZ)
		{
			sValue.Format(_T("%08x"),m_id.GetID());
			wkp.WriteString(_T("CommUnit\\RelayPTZ"),_T("CID"),sValue);
		}
		else
		{
			int iNum,i;
			sSection.Format(_T("CommUnit\\COM%d"),m_iPTZInterface);
			iNum = wkp.GetInt(sSection,_T("Num"),0);
			sValue.Format(_T("%08x"),m_id.GetID());

			for (i=0;i<iNum;i++)
			{
				sKey.Format(_T("SID%d"),i);
				sTemp = wkp.GetString(sSection,sKey,_T(""));
				if (sTemp == sValue)
				{
					iNum = i;
					break;
				}
			}
			wkp.WriteInt(sSection,_T("Typ"),(int)m_SNType);
			sKey.Format(_T("SID%d"),iNum);
			wkp.WriteString(sSection,sKey,sValue);
			sKey.Format(_T("CID%d"),iNum);
			wkp.WriteInt(sSection,sKey,m_dwSNID);
			iNum++;
			wkp.WriteInt(sSection,_T("Num"),iNum);
		}
	}
	if (m_bDeleteActivations)
	{
		CWnd *pWnd = theApp.GetMainWnd();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		{
			CDocument*pDoc = ((CMainFrame*)pWnd)->GetActiveDocument();
			if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CSVDoc)))
			{
				CSVPage* pPage = ((CSVDoc*)pDoc)->GetView()->GetSVPage();
				if (pPage)
				{
					pPage->CheckActivationsWithOutput(GetID(), 0, SVP_CO_CHECK_SILENT);
				}
			}
		}
		m_bDeleteActivations = FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
ExOutputType COutput::TypeName2TypeNr(const CString &s)
{
	if      (s == _T("camera")) return EX_OUTPUT_CAMERA_FBAS;
	else if (s == _T("relay"))  return EX_OUTPUT_RELAY;
	else return EX_OUTPUT_OFF;
}
///////////////////////////////////////////////////////////////////////////////////////
LPCTSTR COutput::TypeNr2TypeName(ExOutputType iTypeNr)
{
	switch (iTypeNr)
	{
		case EX_OUTPUT_CAMERA_FBAS: return _T("camera");
		case EX_OUTPUT_CAMERA_SVHS: return _T("camera");
		case EX_OUTPUT_RELAY:		return _T("relay");
		case EX_OUTPUT_OFF:			return _T("off");
		default:					return _T("invalid output");
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void COutput::GenerateHTMLFileCamera(COutputGroup* pOutputGroup)
{
	CString sFileName;
	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sCam,sPath;
	sHost.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sCam.Format(_T("%08lx"),m_id.GetID());
	sFileName = sDir + _T('\\') + sHost + _T('\\') + sCam + _T(".htm");

	CFile file;
	CFileException cfe;

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_CAMERA, m_sName);
		sText.Write(file);

		sText.CleanUp();
		sText = _T("<body bgcolor=\"#C0C0C0\" topmargin=\"2\" leftmargin=\"2\">") HTML_LINEFEED;
		sText.Write(file);

		sL.LoadString(IDS_REFERENCE_IMAGE);
		sText.Format(_T("<h3>%s %s</h3><br>%s"), m_sName, sL, HTML_LINEFEED);
		sText.Write(file);

		sL.Format(_T("%08lx.jpg"), m_id.GetID());
		BOOL bPictureExists = DoesFileExist(sDir + _T('\\') + sHost + _T('\\') + sCam + _T('\\') + sL);

		if (   bPictureExists
			&& m_ExOutputType == EX_OUTPUT_CAMERA_FBAS || m_ExOutputType == EX_OUTPUT_CAMERA_SVHS)
		{
			sText.Format(_T("<img src=\"%s\" width=\"90%%\" height=\"90%%\"><br>%s"),
				sL, HTML_LINEFEED);
			sText.Write(file);
		}
		else
		{
			sL.LoadString(IDS_NOTIMPLEMENTED);
			sText.Format(_T("<i>%s</i><br>%s"), sL, HTML_LINEFEED);
			sText.Write(file);
		}

		sText.Format(_T("</h3><a href=\"%04lx.htm\">%s</a><br>%s"),
			m_id.GetGroupID(), pOutputGroup->GetName(), HTML_LINEFEED);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}
