// OutputGroup.h: interface for the COutputGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTGROUP_H__43C9E4A5_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_OUTPUTGROUP_H__43C9E4A5_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Output.h"

class COutputGroup : public COutputArrayCS , public CIPCOutput
{
	// construction / destruction
public:
	COutputGroup(LPCTSTR shmName, int wGroup);
	virtual ~COutputGroup();

	// attributes
public:
	inline	WORD GetID() const;
	inline  BYTE GetCardNr() const;
	virtual BOOL HasCameras() const;
	virtual int  GetNrOfActiveCameras();
			COutput* GetOutputByClientID(CSecID id);

	// operations
public:
			BOOL IsJacob() const;
			BOOL IsSavic() const;
			BOOL IsTasha() const;
			BOOL IsQ() const;
			void Load(CWK_Profile& wkp);
			void Save(CWK_Profile& wkp);
	virtual void StartRequests();
	virtual void StopRequests();
	virtual void SaveReferenceImage();
	CString GetAppname() const;

	// overrides
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual	void OnRequestDisconnect();
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(WORD wGroupID, int errorCode,
								BOOL bCanSWCompress,
								BOOL bCanBWDecompress,
								BOOL bCanColorCompress,
								BOOL bCanColorDecompress,
								BOOL bCanOverlay
						);	// errorCode 0==no error, else some error code
	virtual void OnConfirmReset(WORD wGroupID);

	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnConfirmSetRelay(CSecID relayID, BOOL bClosed);
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);
	// data member
private:
	BOOL m_bIsJaCob;
	BOOL m_bIsSaVic;
	BOOL m_bIsTasha;
	BOOL m_bIsQ;
	WORD m_wGroup;
	int	 m_iHardware;	// Hardwareflag, 0: ok, !=0 Fehler im Modul
	CString m_sAppname;
};
//////////////////////////////////////////////////////////////////////
inline WORD COutputGroup::GetID() const
{
	return m_wGroup;
}
//////////////////////////////////////////////////////////////////////
inline BYTE COutputGroup::GetCardNr() const
{
	return LOBYTE(m_wGroup);
}
//////////////////////////////////////////////////////////////////////
typedef COutputGroup* COutputGroupPtr;
WK_PTR_ARRAY_CS(COutputGroupArray,COutputGroupPtr,COutputGroupArrayCS);

#endif // !defined(AFX_OUTPUTGROUP_H__43C9E4A5_8B84_11D3_99EB_004005A19028__INCLUDED_)
