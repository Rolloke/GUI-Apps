// NotificationMessage.cpp: implementation of the CNotificationMessage class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "NotificationMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szNotificationMessage[] = _T("NotificationMessage");
static const TCHAR szNumberOfNotificationMessages[] = _T("NumberOfNotificationMessages");

CString g_sWWWRoot;

//////////////////////////////////////////////////////////////////////
CString CNotificationMessage::GetWWWRoot()
{
	if (g_sWWWRoot.IsEmpty())
	{
		CWK_Profile wkp;
		g_sWWWRoot = wkp.GetString(SECTION_COMMON, _T("WWWRoot"), _T("c:\\dvrtwww"));
	}
	return g_sWWWRoot;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNotificationMessage::CNotificationMessage()
{

}
/////////////////////////////////////////////////////////////////////////////
CNotificationMessage::CNotificationMessage(CSecID id)
{
	m_ID = id;
}
/////////////////////////////////////////////////////////////////////////////
CNotificationMessage::~CNotificationMessage()
{

}
/////////////////////////////////////////////////////////////////////////////
BOOL CNotificationMessage::Load(int i,CWK_Profile& wkp)
{
	CString sKey;
	CString sValue;

	sKey.Format(_T("%d"),i);
	sValue = wkp.GetString(szNotificationMessage,sKey, _T(""));

	if (sValue.IsEmpty())
	{
		return FALSE;
	}
	m_ID = ((DWORD)wkp.GetHexFromString(sValue,_T("\\ID"), SECID_NO_ID ));
	
	if (m_ID==SECID_NO_ID)
	{
		return FALSE;
	}
	m_sName = wkp.GetStringFromString(sValue, _T("\\CO"), _T(""));
	m_sMessage = wkp.GetStringFromString(sValue, _T("\\NM"), _T(""));
	m_sURL = wkp.GetStringFromString(sValue, _T("\\URL"), _T(""));
	m_stTimeStamp.FromString(wkp.GetStringFromString(sValue, _T("\\TS"), _T("")));

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CNotificationMessage::Save(int i,CWK_Profile& wkProfile)
{
	CString sKey;
	CString sValue;

	sKey.Format(_T("%d"),i);
	sValue.Format(_T("\\ID%08lx\\CO%s\\NM%s\\URL%s\\TS%s"),
		m_ID.GetID(),m_sName,m_sMessage,m_sURL,m_stTimeStamp.ToString());
	wkProfile.WriteString(szNotificationMessage,sKey,sValue);
}
/////////////////////////////////////////////////////////////////////////////
//  NYD
CNotificationMessageArray::CNotificationMessageArray()
{
}
/////////////////////////////////////////////////////////////////////////////
//  NYD
CNotificationMessageArray::~CNotificationMessageArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
void CNotificationMessageArray::Load(CWK_Profile& wkProfile)
{
	int i,c;
	CNotificationMessage* pNotificationMessage;

	c = GetSize();
	for (i=0;i<c;i++) 
	{
		if (GetAtFast(i)) 
		{ 
			delete GetAtFast(i); 
		} 
	} 
	RemoveAll();

	c = wkProfile.GetInt(szNotificationMessage,szNumberOfNotificationMessages,0);
	
	for (i=0;i<c;i++)
	{
		pNotificationMessage = new CNotificationMessage();
		if (pNotificationMessage->Load(i,wkProfile))
		{
			Add(pNotificationMessage);
		}
		else
		{
			delete pNotificationMessage;
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
void CNotificationMessageArray::Save(CWK_Profile& wkProfile)
{
	int i,c;
	CNotificationMessage* pNotificationMessage;

	c = GetSize();

	wkProfile.DeleteSection(szNotificationMessage);
	wkProfile.WriteInt(szNotificationMessage,szNumberOfNotificationMessages,c);

	for (i=0;i<c;i++)
	{
		pNotificationMessage = (CNotificationMessage*)GetAtFast(i);
		if (pNotificationMessage)
			pNotificationMessage->Save(i,wkProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
CNotificationMessage* CNotificationMessageArray::AddNotificationMessage()
{
	CNotificationMessage* pNotificationMessage;
	CSecID id(SECID_GROUP_NOTIFICATIONMESSAGE,0);
	
	do 
	{
		pNotificationMessage = GetNotificationMessage(id);
		if (pNotificationMessage) {
			id = id.GetID() + 1; 
		}
	} while (pNotificationMessage);
	
	pNotificationMessage = new CNotificationMessage(id);
	Add(pNotificationMessage);

	return pNotificationMessage;
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
void CNotificationMessageArray::DeleteNotificationMessage(CNotificationMessage* pNotificationMessage)
{
	int i,c;
	CNotificationMessage* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pNotificationMessage==pSearch)
		{
			// found
			RemoveAt(i);
			WK_DELETE(pNotificationMessage);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
CNotificationMessage* CNotificationMessageArray::GetNotificationMessage(const CSecID& id)
{
	int i,c;
	CNotificationMessage* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pSearch->GetID()==id)
		{
			// found
			return pSearch;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
/* not yet documented*/
void CNotificationMessageArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAtFast(i)) 
		{ 
			delete GetAtFast(i); 
		} 
	} 
	RemoveAll(); 
}
