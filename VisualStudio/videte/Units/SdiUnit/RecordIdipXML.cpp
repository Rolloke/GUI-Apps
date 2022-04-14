#include "stdafx.h"
#include "recordidipxml.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////////////////
CRecordIdipXML::CRecordIdipXML(SDIControlType type,
							   int iCom,
							   int iPort,
							   LPCTSTR szIPAddress,
							   BOOL bTraceAscii,
							   BOOL bTraceHex)
 : CSDIControlRecord(type, iCom, iPort,szIPAddress, bTraceAscii, bTraceHex,FALSE)
{
	m_bUnicode = FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////
CRecordIdipXML::~CRecordIdipXML()
{
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordIdipXML::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bRet = FALSE;
	if (CreateRecord())
	{
		if (GetCOMNumber() > 0)
		{
			if (OpenCom(CBR_9600, 8, NOPARITY, ONESTOPBIT)) 
			{
				bRet = TRUE;
			}
		}
		else if (m_iPort)
		{
			if (m_sIPAddress.IsEmpty())
			{
				bRet = CAsyncSocket::Create(m_iPort,SOCK_DGRAM,FD_READ);
			}
			else
			{
				bRet = CAsyncSocket::Create(m_iPort,SOCK_DGRAM,FD_READ,m_sIPAddress);
			}
		}
	}
	if (!bRet)
	{
		WK_TRACE_ERROR(_T("%s Create Open %s FAILED\n"), CSDIControl::NameOfEnum(m_Type), GetCOMNumber() > 0 ? _T("COM") : _T("Socket"));
	}
	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CRecordIdipXML::ThreadIsStarted()
{
	CoInitialize(0);
}
///////////////////////////////////////////////////////////////////////////////////////////
void CRecordIdipXML::ThreadIsStopped()
{
	CoUninitialize();
}
///////////////////////////////////////////////////////////////////////////////////////////
void CRecordIdipXML::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	// check for unicode if both buffers are empty
	DWORD dwDataLen = dwLen;
	BOOL bCompleteData = GetCOMNumber() == 0 ? TRUE : FALSE;
	BOOL  bDataOK = bCompleteData;
	// COM data arrives in several bits
	// UDP data arrives complete
	// TODO! RKE: TCP is not implemented now and arrives bit wise

	if (m_sXML.IsEmpty())
	{
		if (dwLen>1)
		{
			WORD wUnicodeSpecifier = ((WORD*)pData)[0];
			TRACE(_T("first 2 Bytes XML %04hx\n"),wUnicodeSpecifier);
			m_bUnicode = wUnicodeSpecifier == 0xFEFF;
			if (m_bUnicode)
			{
				pData+=2;
				dwDataLen -=2;
			}
		}
	}

	CWK_String sData;
	if (m_bUnicode)
	{
		sData.InitFromMemory(TRUE, pData, dwDataLen);
	}
	else
	{
		sData.InitFromMemory(FALSE, pData, dwDataLen);
	}
	TRACE(_T("%s\n"), LPCTSTR(sData));

	// remove printable characters
	sData.Replace(_T("\r"), _T(""));	// carriage return
	sData.Replace(_T("\f"), _T(""));	// formfeed
	sData.Replace(_T("\t"), _T(""));	// tabulator

	m_sXML += sData;
	if (!bDataOK)
	{
		int nFind = sData.Find(_T("<?"));
		if (nFind != -1)
		{
			m_sXML = sData.Mid(nFind);
		}
		if (m_sXML.Find(_T("?>")) != -1)	// xml headline has arrived
		{
			bDataOK = TRUE;
		}
		Sleep(100);	// wait a little bit
	}

	if (bDataOK)
	{
		CWK_XmlDocument doc;
		CString sError;
		BOOL bRet = doc.LoadXML(m_sXML);
		if (bRet)
		{
			CString		 sID, sName, sControl, sData, sState;
			CStringArray ar;
			IDispatch *pNode= doc.AsNode();
			while (pNode)
			{
				sID.Empty();
				sName.Empty();
				sControl.Empty();
				sState.Empty();
				ParseNode(pNode, _T("ID"), sID);
				ParseNode(pNode, _T("Name"), sName);

				if (!sName.IsEmpty())
				{
					m_DataFieldArrayActual.AddNoDuplicates(CIPCField(CIPCField::m_sfInNm, sName, 'C'));
				}

				ParseNode(pNode, _T("Control"), sControl);
				ParseNode(pNode, _T("State"), sState);
				IDispatch *pNextData = pNode;
				while (pNextData)
				{
					sData.Empty();
					ar.RemoveAll();
					pNextData = ParseNode(pNextData, _T("Data"), sData);
					if (!sData.IsEmpty())
					{
						SplitString(sData, ar, _T(","));
						if (ar.GetCount() >= 4)
						{
							m_DataFieldArrayActual.AddNoDuplicates(CIPCField(ar[1], ar[3], CIPCField::TranslateToType(ar[1])));
						}
						else
						{
							sError.Format(IDS_SDI_XML_WRONG_PARAMETERS, sID, sName, sControl, sState, sData);
						}
					}
				}
				if (!sID.IsEmpty() && !sControl.IsEmpty() && !sState.IsEmpty())
				{
					UINT nID = _ttoi(sID), nState = _ttoi(sState);
					int iKind = -1; // edge
					if (sControl.CompareNoCase(_T("level")) == 0)
					{
						iKind = nState;
					}
					else if (sControl.CompareNoCase(_T("edge")) == 0 && nState == 1)
					{
						iKind = 2;
					}
					else
					{
						sError.Format(IDS_SDI_XML_WRONG_PARAMETERS, sID, sName, sControl, sState, _T(""));
					}
					IndicateAlarm((WORD)nID, iKind);
					ClearAllData();
				}
				else
				{
					sError.Format(IDS_SDI_XML_WRONG_PARAMETERS, sID, sName, sControl, sState, _T(""));
				}

				pNode = ParseNode(pNode, _T("Alarm"), sData);
			}
			m_sXML.CleanUp();
			m_sXML.Empty();
		}
		else if (bCompleteData)
		{
			sError.LoadString(IDS_SDI_XML_LOAD_ERROR);
		}
		if (!sError.IsEmpty())
		{
			CWK_RunTimeError rte(WAI_SDIUNIT, REL_WARNING, RTE_PROTOCOL, sError);
			rte.Send();
			m_sXML.Empty();
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordIdipXML::CheckForNewAlarmData()
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
IDispatch* CRecordIdipXML::ParseNode(IDispatch*pNode, const CString &sSearch, CString &sResult)
{
	int i, n;
	IDispatch* pReturn = NULL;
	CWK_XmlNode node(pNode);

	if (   node.NodeType() == _T("element") 
		&& sSearch.CompareNoCase(node.Name()) == 0)
	{
		sResult = node.GetText();
		pReturn = pNode;
	}

	if (pReturn)
	{
		n = node.NumAttributes();
		if (!sResult.IsEmpty() && n > 0)
		{
			sResult += _T(":");
		}
		for (i=0; i<n; i++)
		{
			sResult += node.GetAttribName(i) + _T(",") + node.GetAttribVal(i);
			if (i<n-1)
			{
				sResult += _T(",");
			}
		}
		pReturn = node.GetNextSibling();
		return pReturn;
	}

	n = node.NumNodes();
	for (i=0; i<n; i++)
	{
		pReturn = ParseNode(node.GetNode(i), sSearch, sResult);
		if (!sResult.IsEmpty())
		{
			break;
		}
	}

	return pReturn;
}
