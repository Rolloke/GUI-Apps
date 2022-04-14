// BackupEraseMedium.h: interface for the CBackupEraseMedium class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPERASEMEDIUM_H__8239DD56_D7A6_4315_B3F4_E375E89F8D37__INCLUDED_)
#define AFX_BACKUPERASEMEDIUM_H__8239DD56_D7A6_4315_B3F4_E375E89F8D37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackupEraseMedium : public CWK_Thread  
{
public:
	CBackupEraseMedium(CString sDriveToErase);
	virtual ~CBackupEraseMedium();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
	BOOL IsAlreadyErased();

	//members
private:

	CString m_sDriveToErase;
	BOOL m_bAlreadyErased;
};

#endif // !defined(AFX_BACKUPERASEMEDIUM_H__8239DD56_D7A6_4315_B3F4_E375E89F8D37__INCLUDED_)
