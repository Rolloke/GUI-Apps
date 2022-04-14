// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__43C9E4A1_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_INPUT_H__43C9E4A1_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COutput;
class CInputGroup;
class CMedia;

class CInput  
{
public:
	typedef enum 
	{
		IM_INVALID = -1,
		IM_EXTERN  = 0,
		IM_MD      = 1,
		IM_SUSPECT = 2,
		IM_PTZ     = 3,
		IM_AUDIO   = 4,
	} InputMode;

	// construction / destruction
public:
	CInput(CInputGroup* pGroup, CSecID id, InputMode mode);
	virtual ~CInput();

	// attributes
public:
	inline CSecID	GetUnitID() const;
	inline CSecID	GetClientID() const;
	inline BOOL     IsOn() const;
	inline BOOL		IsActive() const;
	inline BOOL		IsMD() const;
	inline BOOL		IsExtern() const;
	inline BOOL		IsSuspect() const;
	inline BOOL		IsPTZ() const;
	inline BOOL		IsAudio() const;
		   CString 	GetName() const;
	inline CIPCFields& GetFields();
	inline CInputGroup* GetGroup();
	inline CCamera* GetCamera() const;
	inline const CString& GetKey() const;

	inline DWORD GetX() const;
	inline DWORD GetY() const;

	// operations
public:
		   void		SetClientNr(BYTE bNr);
		   void		FromString(const CString& s);
		   CString  ToString(BOOL bShutdown);
		   CString  Format();

	inline void	SetMode(InputMode mode);
	inline void SetCamera(CCamera* pCamera);
	inline void SetMedia(CMedia* pMedia);

		   void SetName(const CString& sName);
		   void SetActive(BOOL bActive);
		   void SetActive(BOOL bActive, DWORD dwX, DWORD dwY);
		   void SetActive(BOOL bActive, const CIPCFields& fields);
		   void ClearActive();
		   void CheckActive();
		   int	IsTrackPict() const;
		   void InformClientsNotActive();

		   int	GetOutstandingPicts() const;
		   int  GetAndDecreaseOutstandingPicts();
		   
		   void SetOutstandingPicts();
		   void	ResetOutstandingPicts();

protected:
	CString	NameOfEnum(InputMode mode);
	void    SaveAlarm();
	
	void	SetOutstandingPicts(int iOutstandingPicts);
	void	SetUVVAlarmStart();

	// data member
private:
	CSecID	m_UnitID;
	CSecID	m_ClientID;
	CInputGroup* m_pGroup;
	
	CCamera* m_pCamera;
	CMedia*  m_pMedia;

	CString		m_sName;
	CString		m_sKey;
	CIPCFields	m_Fields;
	DWORD		m_dwX;
	DWORD		m_dwY;
	BOOL		m_bActive;
	InputMode   m_Mode;
	BOOL		m_bOn;
	
	int     m_iOutstandingPicts;
	DWORD	m_dwUVVAlarmStart;
	DWORD   m_dwUVVKeyFlushed;
	BOOL	m_bInformClients;
	BYTE	m_byAlarmCallHandling;
};
//////////////////////////////////////////////////////////////////////
inline CInputGroup* CInput::GetGroup()
{
	return m_pGroup;
}
//////////////////////////////////////////////////////////////////////
inline CCamera* CInput::GetCamera() const
{
	return m_pCamera;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsActive() const
{
	return m_bActive;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsOn() const
{
	return m_bOn;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CInput::GetUnitID() const
{
	return m_UnitID;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CInput::GetClientID() const
{
	return m_ClientID;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsMD() const
{
	return m_Mode == IM_MD;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsExtern() const
{
	return m_Mode == IM_EXTERN;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsSuspect() const
{
	return m_Mode == IM_SUSPECT;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsPTZ() const
{
	return m_Mode == IM_PTZ;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::IsAudio() const
{
	return m_Mode == IM_AUDIO;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetMode(InputMode mode)
{
    m_Mode = mode;
	if (   m_Mode == IM_PTZ
		|| m_Mode == IM_AUDIO)
	{
		m_ClientID = m_UnitID;
	}
}
//////////////////////////////////////////////////////////////////////
inline CIPCFields& CInput::GetFields()
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline void CInput::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
}
/////////////////////////////////////////////////////////////////////
inline void CInput::SetMedia(CMedia* pMedia)
{
	m_pMedia = pMedia;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CInput::GetKey() const
{
	return m_sKey;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CInput::GetX() const
{
	return m_dwX;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CInput::GetY() const
{
	return m_dwY;
}
//////////////////////////////////////////////////////////////////////
typedef CInput* CInputPtr;
WK_PTR_ARRAY_CS(CInputArray,CInputPtr,CInputArrayCS);
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_INPUT_H__43C9E4A1_8B84_11D3_99EB_004005A19028__INCLUDED_)
