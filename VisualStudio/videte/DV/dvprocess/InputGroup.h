// InputGroup.h: interface for the CInputGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTGROUP_H__43C9E4A2_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_INPUTGROUP_H__43C9E4A2_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Input.h"

class CInputList;

class CInputGroup : public CInputArrayCS , public CIPCInput
{
	// construction / destruction
public:
	CInputGroup(CInputList* pInputList, 
				LPCTSTR shmName, 
				int wGroup, 
				int iSize, 
				int iNrExternal,
				BOOL bMotion);
	virtual ~CInputGroup();

	// attributes
public:
	inline	WORD	GetID() const;
	inline  const CString& GetSection() const;
			BOOL	IsInputActive(CSecID id);
			int		GetOutstandingPicts();
			CInput* GetInputByUnitID(CSecID id);

	// operations
public:
	BOOL IsJacob() const;
	BOOL IsSavic() const;
	BOOL IsTasha() const;
	BOOL IsQ() const;
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp, BOOL bShutdown);
	void ClearActive();
	void CheckActive();

	// overrides
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);
	virtual void OnRequestDisconnect();
	virtual void OnConfirmReset(WORD wGroupID);
	virtual void OnConfirmEdge(WORD wGroupID,DWORD edgeBitmask);	// 1 positive, 0 negative
	virtual void OnConfirmFree(WORD wGroupID,DWORD openBitmask);	// 1 open, 0 closed
	virtual void OnConfirmAlarmState(WORD wGroupID,DWORD inputMask);
		// 1 high, 0 low; 1 changed, 0 unchanged
	virtual	void OnIndicateAlarmState(WORD wGroupID,
									  DWORD inputMask,
									  DWORD changeMask,
									  LPCTSTR szInfoString);
	virtual void OnIndicateAlarmFieldsState(WORD wGroupID,
											DWORD inputMask, 	// 1 high, 0 low
											DWORD changeMask,	// 1 changed, 0 unchanged
											int iNumFields,
											const CIPCField fields[]);
	virtual void OnIndicateAlarmFieldsUpdate(CSecID id,
											int iNumFields,
											const CIPCField fields[]);
	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);

	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	// implementation
protected:
	void IndicateAlarmToClients(DWORD inputMask,DWORD changeMask);

	// data member
private:
	CInputList* m_pInputList;
	WORD	m_wGroupID;
	int		m_iHardware;	// Hardwareflag, 0: ok, !=0 Fehler im Modul
	DWORD	m_dwState;		// Der aktuelle Status der max 32 Gruppenmitglieder.
	DWORD	m_dwFree;		// Der aktuelle Freigabe der 32 Gruppenmitglider
	BOOL    m_bMD;
	CString m_sSection;
	BOOL m_bIsJaCob;
	BOOL m_bIsSaVic;
	BOOL m_bIsTasha;
	BOOL m_bIsQ;
};
//////////////////////////////////////////////////////////////////////
inline WORD CInputGroup::GetID() const
{
	return m_wGroupID;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CInputGroup::GetSection() const
{
	return m_sSection;
}
typedef CInputGroup* CInputGroupPtr;
WK_PTR_ARRAY_CS(CInputGroupArray,CInputGroupPtr,CInputGroupArrayCS);

#endif // !defined(AFX_INPUTGROUP_H__43C9E4A2_8B84_11D3_99EB_004005A19028__INCLUDED_)
