/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ConnectionRecord.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/ConnectionRecord.cpp $
// CHECKIN:		$Date: 11.03.05 14:34 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 11.03.05 14:33 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
#include "stdcipc.h"

#include "ConnectionRecord.h"
#include "CipcExtraMemory.h"
#include "notificationmessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*{CConnectRecord Overview|Overview,CConnectionRecord}
 {members|CConnectionRecord},
{CIPCServerControl} 
*/

/*
  {CIPCServerControl}
*/

/*********************************************************************************************
 Class      : CConnectionRecord
 Function   : CConnectionRecord
 Description: Standard Constructor

 Parameters: None 

 Result type:  ()
 created: September, 29 2003
 <TITLE CConnectionRecord>
*********************************************************************************************/
CConnectionRecord::CConnectionRecord()
{
	m_dwTimeout=30000;
	m_dwOptions=0;
}

/*********************************************************************************************
 Class      : CConnectionRecord
 Function   : CConnectionRecord
 Description: Contructor wit arguments

 Parameters:   
  remoteHost : (E): Destination Host  (const CString&)
  user       : (E): User trying to connect  (const CUser&)
  sPermission: (E): Permissions  (const CString&)
  sPassword  : (E): Password for the connection  (const CString&)
  dwTimeout  : (E): Timeout for the connection   (DWORD)

 Result type:  ()
 created: September, 29 2003
 <TITLE CConnectionRecord>
*********************************************************************************************/
CConnectionRecord::CConnectionRecord(
					const CString &remoteHost,
					const CUser &user,
					const CString &sPermission,
					const CString &sPassword,
					DWORD dwTimeout
					  )
{
	m_dwTimeout=30000;
	m_dwOptions=0;

	m_sDestinationHost = remoteHost;
	m_sUser = user.GetName();
	m_sPermission = sPermission;
	m_sPassword = sPassword;
	m_dwTimeout = dwTimeout;
}
/*********************************************************************************************
 Class      : CConnectionRecord
 Function   : CConnectionRecord
 Description: Copy contructor

 Parameters:   
  Source: (E): Source Record  (const CConnectionRecord&)

 Result type:  ()
 created: September, 29 2003
 <TITLE CConnectionRecord>
*********************************************************************************************/
CConnectionRecord::CConnectionRecord(const CConnectionRecord &Source)
{
	m_sDestinationHost=Source.m_sDestinationHost;
	m_sUser=Source.m_sUser;
	m_sPermission=Source.m_sPermission;
	m_sPassword=Source.m_sPassword;
	m_sSourceHost=Source.m_sSourceHost;
	//
	m_sInputShm=Source.m_sInputShm;
	m_sOutputShm=Source.m_sOutputShm;
	//
	m_dwOptions=Source.m_dwOptions;
	m_dwTimeout=Source.m_dwTimeout;
	m_camID=Source.m_camID;

	// copy fields
	for (int f=0;f<Source.m_fields.GetSize();f++) {
		m_fields.Add( Source.m_fields[f]);
	}
}
/*********************************************************************************************
 Class      : &CConnectionRecord
 Function   : operator =
 Description: Assignement operator

 Parameters:   
  Source: (E): Source record  (const CConnectionRecord&)

 Result type:  (const CConnectionRecord)
 created: September, 29 2003
 <TITLE operator =>
*********************************************************************************************/
const CConnectionRecord &CConnectionRecord::operator =(const CConnectionRecord &Source)
{
	if (this!=&Source) 
	{
		m_sDestinationHost=Source.m_sDestinationHost;
		m_sUser=Source.m_sUser;
		m_sPermission=Source.m_sPermission;
		m_sPassword=Source.m_sPassword;
		m_sSourceHost=Source.m_sSourceHost;
		//
		m_sInputShm=Source.m_sInputShm;
		m_sOutputShm=Source.m_sOutputShm;
		//
		m_dwOptions=Source.m_dwOptions;
		m_dwTimeout=Source.m_dwTimeout;
		m_camID=Source.m_camID;

		// copy fields
		for (int f=0;f<Source.m_fields.GetSize();f++) {
			m_fields.Add( Source.m_fields[f]);
		}
	}

	return *this;
}
/*********************************************************************************************
 Class      : CConnectionRecord
 Function   : CConnectionRecord
 Description: Constructor with argument

 Parameters:   
  cmd: (E): Initialising Cmd record  (const CIPCCmdRecord&)

 Result type:  ()
 created: September, 29 2003
 <TITLE CConnectionRecord>
*********************************************************************************************/
CConnectionRecord::CConnectionRecord(const CIPCCmdRecord &cmd)
{
	m_dwTimeout=cmd.m_dwParam1;
	m_dwOptions=cmd.m_dwParam2;
	m_camID=cmd.m_dwParam3;

	if (cmd.m_pExtraMemory)
	{
		CStringArray *pTheStrings = cmd.m_pExtraMemory->GetStrings();
		int i,c;
		c = pTheStrings->GetSize();
#ifdef _UNICODE
		CIPC *pCipc = (CIPC*)cmd.m_pExtraMemory->GetCIPC();
		if (pCipc)
		{
			UINT uCodePage = (UINT) -1;
			WORD wOldCodePage = pCipc->GetCodePage();
			int nFind;
			for (i=7;i<c;i++)
			{
				nFind = (*pTheStrings)[i].Find(_T("@="));
				if (nFind != -1) 
				{
					if ((*pTheStrings)[i].Find(NM_CODEPAGE) != -1)
					{
						uCodePage = _ttoi((*pTheStrings)[i].Mid(nFind + 2));
						if (((WORD)uCodePage) != wOldCodePage)
						{
							WK_TRACE(_T("Decoding with different CodePage %d, %d\n"), wOldCodePage, uCodePage);
							pCipc->SetCodePage((WORD)uCodePage);
							WK_DELETE(pTheStrings);
							pTheStrings = cmd.m_pExtraMemory->GetStrings();
							pCipc->SetCodePage((WORD)wOldCodePage);
						}
						break;
					}
				}
			}
		}
#endif

		if (c>=5) 
		{
			m_sDestinationHost = (*pTheStrings)[0];
			m_sUser = (*pTheStrings)[1];
			m_sPermission = (*pTheStrings)[2];
			m_sPassword = (*pTheStrings)[3];
			m_sSourceHost = (*pTheStrings)[4];
		} 
		else 
		{
			WK_TRACE(_T("CConnectionRecord only %d strings, expected at least 5\n"),
				pTheStrings->GetSize());
		}

		if (c>5) 
		{
			m_sInputShm = (*pTheStrings)[5];
		}
		if (c>6) 
		{
			m_sOutputShm = (*pTheStrings)[6];
		}
		if (c>7)
		{
			CString sAdd;
			for (i=7;i<c;i++)
			{
				m_fields.Add((*pTheStrings)[i]);
			}
		}

		WK_DELETE(pTheStrings);
	}
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetDestinationHost() const
{
	return m_sDestinationHost;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetUser() const
{
	return m_sUser;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetPermission() const
{
	return m_sPermission;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetPassword() const
{
	return m_sPassword;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetSourceHost() const
{
	return m_sSourceHost;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetInputShm() const
{
	return m_sInputShm;
}

/*Function: NOT YET DOCUMENTED */
const CString &CConnectionRecord::GetOutputShm() const
{
	return m_sOutputShm;
}
	//

/*Function: NOT YET DOCUMENTED */
DWORD CConnectionRecord::GetOptions() const
{
	return m_dwOptions;
}

/*Function: NOT YET DOCUMENTED */
DWORD CConnectionRecord::GetTimeout() const
{
	return m_dwTimeout;
}

/*Function: NOT YET DOCUMENTED */
CSecID CConnectionRecord::GetCamID() const
{
	return m_camID;
}


/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetDestinationHost(const CString &sDestination)
{
	m_sDestinationHost = sDestination;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetUser(const CString &sUser)
{
	m_sUser = sUser;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetPermission(const CString &sPermission)
{
	m_sPermission = sPermission;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetPassword(const CString &sPassword)
{
	m_sPassword = sPassword;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetSourceHost(const CString &sSource)
{
	m_sSourceHost = sSource;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetInputShm(const CString &sSource)
{
	m_sInputShm= sSource;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetOutputShm(const CString &sSource)
{
	m_sOutputShm= sSource;
}


/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetOptions(DWORD dwOptions)
{
	m_dwOptions = dwOptions;
}
/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetTimeout(DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
}

/*Function: NOT YET DOCUMENTED */
void CConnectionRecord::SetCamID(CSecID id)
{
	m_camID = id;
}

BOOL CConnectionRecord::GetFieldValue(const CString &sFieldName,
									  CString &sResult) const
{
	BOOL bFound = FALSE;
	sResult.Empty();
	CString sFieldToSearch(sFieldName);
	sFieldToSearch += _T("@=");
	for (int f=0;bFound==FALSE && f<m_fields.GetSize();f++) {
		CString sOneField = m_fields[f];
		if (sOneField.Find(sFieldToSearch)==0) {
			bFound = TRUE;
			sResult = sOneField.Right(sOneField.GetLength()-sFieldToSearch.GetLength());
		}
	}
	return bFound;
}

void CConnectionRecord::SetFieldValue(const CString &sFieldName,
									  const CString &sFieldValue)
{
	BOOL bFound = FALSE;

	CString sFieldToSearch(sFieldName);
	sFieldToSearch += _T("@=");

	// first kill an existing old value
	for (int f=0;bFound==FALSE && f<m_fields.GetSize();f++) {
		CString sOneField = m_fields[f];
		if (sOneField.Find(sFieldToSearch)==0) {
			bFound = TRUE;
			m_fields.RemoveAt(f);	// drop the old value
			// no need to adjust loop, the loop aborts if bFound is TRUE
		}
	}

	CString sNewField(sFieldName);
	sNewField += _T("@=");
	sNewField += sFieldValue;
	m_fields.Add(sNewField);
}

void CConnectionRecord::GetAllFields(CStringArray &fields) const
{
	fields.RemoveAll();
	
	for (int f=0;f<m_fields.GetSize();f++) {
		fields.Add(m_fields[f]);
	}

}
/*********************************************************************************************
 Class      : *CConnectionRecord
 Function   : CreateCmdRecord
 Description: Creates a CmdRecord from this connectionrecord

 Parameters:   
  pCipc: (E): CIPC object to send the Cmd record  (const CIPC*)
  dwCmd: (E): Command for the CmdRecord  (DWORD)

 Result type:  (CIPCCmdRecord)
 created: September, 29 2003
 <TITLE CreateCmdRecord>
*********************************************************************************************/
CIPCCmdRecord *CConnectionRecord::CreateCmdRecord(const CIPC *pCipc, DWORD dwCmd) const
{
	CStringArray theStrings;
	theStrings.Add(GetDestinationHost());
	theStrings.Add(GetUser());
	theStrings.Add(GetPermission());
	theStrings.Add(GetPassword());
	theStrings.Add(GetSourceHost());
	
/* old style pre 4.0 Alpha3
	if (m_sInputShm.GetLength()) theStrings.Add(m_sInputShm);
	if (m_sOutputShm.GetLength()) theStrings.Add(m_sOutputShm);
*/
	// new style
	// always add InputShm OutputShm to have 7 strings
	theStrings.Add(m_sInputShm);
	theStrings.Add(m_sOutputShm);
#ifdef _UNICODE
	UINT uCodePage = (UINT) -1;
	WORD wOldCodePage = pCipc->GetCodePage();
	int nFind;
	for (int f=0;f<m_fields.GetSize();f++) 
	{
		CString sOneField = m_fields[f];
		nFind = sOneField.Find(_T("@="));
		if (nFind != -1) 
		{
			if (sOneField.Find(NM_CODEPAGE) != -1)
			{
				uCodePage = _ttoi(sOneField.Mid(nFind + 2));
			}
			theStrings.Add(sOneField);
		} 
		else 
		{
			WK_TRACE_ERROR(_T("ConnectionRecord: field misses @= in '%s'\n"),sOneField);
		}
	}
	if (uCodePage != (UINT)-1 && ((WORD)uCodePage) != wOldCodePage)
	{
		WK_TRACE(_T("Encoding with different CodePage %d, %d\n"), wOldCodePage, uCodePage);
		((CIPC*)pCipc)->SetCodePage((WORD)uCodePage);
	}
#endif
	// fields go after that
	CIPCCmdRecord* pCmd = NULL;
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,theStrings))
	{
		pCmd = new CIPCCmdRecord(pBubble,
								dwCmd,
								GetTimeout(),	// dw1
								GetOptions(),	// dw2
								m_camID.GetID()	// dw3
								);
	}
	else
	{
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in CreateCmdRecord\n"));
	}
	WK_DELETE(pBubble);
#ifdef _UNICODE
	if (uCodePage != (UINT)-1 && ((WORD)uCodePage) != wOldCodePage)
	{
		((CIPC*)pCipc)->SetCodePage(wOldCodePage);
	}
#endif

	return pCmd;
}
