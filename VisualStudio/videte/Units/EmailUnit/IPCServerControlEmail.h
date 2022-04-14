// IPCServerControlEmail.h: interface for the CIPCServerControlEmail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLEMAIL_H__4A9B9A03_0F86_11D4_A9E8_004005A26A3B__INCLUDED_)
#define AFX_IPCSERVERCONTROLEMAIL_H__4A9B9A03_0F86_11D4_A9E8_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEmailUnitDlg;
class CIPCServerControlEmail : public CIPCServerControl  
{
	
public:

	CIPCServerControlEmail(CEmailUnitDlg* pDlg);
	virtual ~CIPCServerControlEmail();

	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);
  	virtual void OnRequestServerReset(const CConnectionRecord &c);

private:
	CEmailUnitDlg* m_pDlg;
};

#endif // !defined(AFX_IPCSERVERCONTROLEMAIL_H__4A9B9A03_0F86_11D4_A9E8_004005A26A3B__INCLUDED_)
