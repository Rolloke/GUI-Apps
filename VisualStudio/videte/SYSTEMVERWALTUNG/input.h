/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: input.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/input.h $
// CHECKIN:		$Date: 16.08.06 14:21 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 15.08.06 12:15 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __INPUT_H_
#define __INPUT_H_

#include "SecID.h"
#include "InputToOutput.h"

////////////////////////////////////////////////////////////////////////////
typedef CInputToOutput* CInputToOutputPtr;
WK_PTR_ARRAY(CInputToOutputArray,CInputToOutputPtr);
////////////////////////////////////////////////////////////////////////////
// Definitionen für Input Struktur           
//
// Status-word(bitmask) der Struktur
#define I_STATE_DEFAULT		0x0000		//Default Edge Positiv
#define I_CAN_EDGE			0x0002		//Bit 1
#define I_STATE_EDGE		0x0004		//Bit 2
#define I_CAN_PUSHBUTTON	0x0010		//Bit 4
#define I_STATE_PUSHBUTTON	0x0020		//Bit 5
#define I_STATE_TEMP_DEACT	0x0040		//Bit 6


class CInputGroup;
//////////////////////////////////////////////////////////////////////
class CInput : public CObject
{
	DECLARE_DYNAMIC(CInput)
	// construction / destruction
public:
	CInput(WORD wGroupID, WORD wSubID);
	~CInput();
	const CInput& operator=(const CInput& inp);
	
	// access:
public:
	inline BOOL	GetTempDeactivate() const;
	inline BOOL	GetEdge() const;
	inline BOOL	CanEdge() const;
	inline BOOL	GetPushButton() const;
	inline BOOL	CanPushButton() const;
	inline const CSecID		GetID() const;
	inline const CSecID		GetIDActivate() const;
	inline const CString&	GetName() const;

	inline const CString&	GetType() const;
	inline const CString&	GetUrl() const;
	inline UINT				GetPort() const;
	inline int				GetInputNr() const;

	inline DWORD GetExtra(int i) const;

	// settings:
public:
	inline void	SetIDActivate(CSecID id);
	inline void SetName(const CString &sName);
	inline void	SetEdge(BOOL bEdge);
	inline void	SetCanEdge(BOOL bCanEdge);
	inline void	SetPushButton(BOOL bPushButton);
	inline void	SetCanPushButton(BOOL bCanPushButton);
	inline void	SetTempDeactivate(BOOL bTempDeactivate);
	
	inline void	SetDeleteInputToOutputs(CSecID id);

	inline void	SetType(const CString& sType);
	inline void	SetUrl(const CString& sUrl);
	inline void	SetPort(UINT uPort);
	inline void	SetInputNr(int nNr);
	// serialization
public:
	BOOL Load(CWK_Profile& wkp);
	BOOL Save(CWK_Profile& wkp);
	void GenerateHTMLFile(CInputGroup* pInputGroup);

	// InputToOutput
public:
	CInputToOutput*	AddInputToOutput();
	BOOL			DeleteActivation(CSecID id);
	void			DeleteInputToOutput(CInputToOutput* pInputToOutput);
	void			DeleteAllInputToOutputs();
	inline int		GetNumberOfInputToOutputs();
	CInputToOutput*	GetInputToOutput(int n) const;
	void SetExtra(int i, DWORD dwExtra);

private:
	CInputToOutputArray m_InputToOutputs;
	CSecID	m_id;			// InputID
	CSecID	m_idActivate;		// SECID_ACTIVE_XXX or TimerID
	CString	m_sName;
	WORD	m_wState;			// Gesamtstatus des Eingangs, flanke, freigabe usw.
	BOOL    m_bDeleteInputToOutputs;

	// IP / DS
	CString			m_sType;
	CString			m_sUrl;
	UINT			m_uPort;
	int				m_nInputNr;

	CDWordArray  m_dwExtras;


public:
	CString		m_sEditor;
	CString		m_sInformation;
	CTime		m_tInstructionStamp;
	CTime		m_tInstructionStampNew;
};
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetCanEdge(BOOL bCanEdge)
{
	if (bCanEdge)
	{
		m_wState |= I_CAN_EDGE;
	}
	else
	{
		m_wState &= ~I_CAN_EDGE;
	}
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::CanEdge() const
{
	return (m_wState & I_CAN_EDGE) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CInput::GetEdge() const
{
	return (m_wState & I_STATE_EDGE)!=0;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetEdge(BOOL bEdge)
{
	if (bEdge)
	{
		m_wState |= I_STATE_EDGE;
	}
	else
	{
		m_wState &= ~I_STATE_EDGE;
	}
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetCanPushButton(BOOL bCanPushButton)
{
	if (bCanPushButton)
	{
		m_wState |= I_CAN_PUSHBUTTON;
	}
	else
	{
		m_wState &= ~I_CAN_PUSHBUTTON;
	}
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CInput::CanPushButton() const
{
	return (m_wState & I_CAN_PUSHBUTTON) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CInput::GetPushButton() const
{
	return (m_wState & I_STATE_PUSHBUTTON)!=0;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetPushButton(BOOL bPushButton)
{
	if (bPushButton)
	{
		m_wState |= I_STATE_PUSHBUTTON;
	}
	else
	{
		m_wState &= ~I_STATE_PUSHBUTTON;
	}
}
//////////////////////////////////////////////////////////////////////
const CSecID CInput::GetID() const
{
	return (m_id);
}
//////////////////////////////////////////////////////////////////////
const CSecID CInput::GetIDActivate() const
{
	return (m_idActivate);
}
//////////////////////////////////////////////////////////////////////
const CString &CInput::GetName() const
{
	return (m_sName);
}
//////////////////////////////////////////////////////////////////////
inline void CInput::SetIDActivate(CSecID id)
{
	m_idActivate = id;
}
//////////////////////////////////////////////////////////////////////
inline void CInput::SetName(const CString &sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
inline int CInput::GetNumberOfInputToOutputs()
{
	return m_InputToOutputs.GetSize();
}
//////////////////////////////////////////////////////////////////////
inline BOOL CInput::GetTempDeactivate() const
{
	return (m_wState & I_STATE_TEMP_DEACT)!=0;
}
//////////////////////////////////////////////////////////////////////
inline void CInput::SetTempDeactivate(BOOL bTempDeactivate)
{
	if (bTempDeactivate)
	{
		m_wState |= I_STATE_TEMP_DEACT;
	}
	else
	{
		m_wState &= ~I_STATE_TEMP_DEACT;
	}
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetType(const CString& sType)
{
	m_sType = sType;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetUrl(const CString& sUrl)
{
	m_sUrl = sUrl;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetPort(UINT uPort)
{
	m_uPort = uPort;
}
//////////////////////////////////////////////////////////////////////
inline void	CInput::SetInputNr(int nNr)
{
	m_nInputNr = nNr;
}
//////////////////////////////////////////////////////////////////////
inline void CInput::SetDeleteInputToOutputs(CSecID id)
{
	if (id == GetID())
	{
		m_bDeleteInputToOutputs = TRUE;
	}
	else
	{
		for (int i=0;i<m_InputToOutputs.GetSize();i++)
		{
			CInputToOutput* pInputToOutput = m_InputToOutputs.GetAtFast(i);

			if (pInputToOutput->GetOutputOrMediaID() == id)
			{
				pInputToOutput->m_bMarkForDelete = TRUE;
			}
			else if (pInputToOutput->GetArchiveID() == id)
			{
				pInputToOutput->m_bMarkForDelete = TRUE;
			}
			else if (pInputToOutput->GetTimerID() == id)
			{
				pInputToOutput->m_bMarkForDelete = TRUE;
			}
			else if (pInputToOutput->GetProcessID() == id)
			{
				pInputToOutput->m_bMarkForDelete = TRUE;
			}
			else if (pInputToOutput->GetNotificationID() == id)
			{
				pInputToOutput->m_bMarkForDelete = TRUE;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
inline const CString& CInput::GetType() const
{
	return m_sType;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CInput::GetUrl() const
{
	return m_sUrl;
}
//////////////////////////////////////////////////////////////////////
inline UINT CInput::GetPort() const
{
	return m_uPort;
}
//////////////////////////////////////////////////////////////////////
inline int CInput::GetInputNr() const
{
	return m_nInputNr;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CInput::GetExtra(int i) const
{
	if (i>=0 && i< m_dwExtras.GetSize())
		return m_dwExtras[i];
	else
		return 0;
}
//////////////////////////////////////////////////////////////////////
typedef CInput* CInputPtr;
WK_PTR_ARRAY(CInputArray,CInputPtr);
#endif
