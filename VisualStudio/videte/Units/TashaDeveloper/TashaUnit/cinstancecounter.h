// InstanceCounter.h: interface for the CInstanceCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCECOUNTER_H__4BB55053_8B49_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_INSTANCECOUNTER_H__4BB55053_8B49_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInstanceCounter  
{
public:
	CInstanceCounter();
	virtual ~CInstanceCounter();
	
	int GetInstance(){return m_nInstance+1;};
	WK_ApplicationId GetAppID(){return m_AppID;};

private:
	int					m_nInstance; // Bis zu 32 Instanzen sind möglich
	WK_ApplicationId	m_AppID;
};

#endif // !defined(AFX_INSTANCECOUNTER_H__4BB55053_8B49_11D2_8CA3_004005A11E32__INCLUDED_)
