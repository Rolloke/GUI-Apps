// Producer.h: interface for the CProducer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRODUCER_H__5EA022E9_DC2F_418B_BDD0_D8E60DE6EE88__INCLUDED_)
#define AFX_PRODUCER_H__5EA022E9_DC2F_418B_BDD0_D8E60DE6EE88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProducer  
{
public:
	CProducer();
	virtual ~CProducer();

	BurnProducer	FindInstalledBurnSoftware();
	BOOL			DoesFileExist(LPCTSTR szFileOrDirectory);
	BOOL			GetNeroVersion(CString &sInstalledVersion, CString sAtLeastVersion  = _T("5.5.8.2"));

	BOOL IsNeroInstalled();
	BOOL IsDirectCDInstalled();
	BOOL IsWinXP();

	
//functions
private:

	CString GetFileVersion(const CString& sFileName);

//members
private:

	CString m_sNeroVersion;
};

#endif // !defined(AFX_PRODUCER_H__5EA022E9_DC2F_418B_BDD0_D8E60DE6EE88__INCLUDED_)
