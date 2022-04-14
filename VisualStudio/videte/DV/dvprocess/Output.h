// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__43C9E4A4_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_OUTPUT_H__43C9E4A4_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COutputGroup;
class CInput;

class COutput  
{
public:
	typedef enum 
	{
		OM_INVALID = 0,
		OM_MD      = 1,
		OM_UVV     = 2,
		OM_RELAY   = 3,
	} OutputMode;

	// construction / destruction
public:
	COutput(COutputGroup* pGroup, CSecID id);
	virtual ~COutput();

	// attributes
public:
	inline OutputMode     GetMode() const;
	inline BOOL		IsRelay() const;
	inline BOOL		IsMD() const;
	inline BOOL		IsUVV() const;

	inline BOOL		IsActive() const;
		   BOOL		IsTimerActive() const;
	inline WORD		GetSubID() const;
	inline WORD		GetCard() const;
	inline CSecID	GetUnitID() const;
	inline CSecID	GetClientID() const;
	inline CString 	GetName() const;
	inline COutputGroup* GetGroup();

	virtual void	FromString(const CString& s);
	virtual CString ToString();
	virtual CString Format();

	// operations
public:
    virtual void SetActive(BOOL bActive);
	virtual void SetTimerActive(BOOL bTimerActive);
	virtual void SetMode(OutputMode mode);
		    void SetClientID(CSecID id);

	void SetName(const CString& sName);

	void Lock();
	void Unlock();

	virtual void Disable();
	virtual void Enable();

	// Implementation
protected:

	// data member
protected:
	BOOL		m_bActive;
	BOOL		m_bTimerActive;
	OutputMode		m_Mode;
	// data member
private:
	CString				m_sName;
	COutputGroup*		m_pGroup;
	CSecID				m_UnitID;
	CSecID				m_ClientID;
	CCriticalSection	m_cs;
};
//////////////////////////////////////////////////////////////////////
inline COutputGroup* COutput::GetGroup()
{
	return m_pGroup;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	COutput::IsActive() const
{
	return m_bActive;
}
//////////////////////////////////////////////////////////////////////
inline COutput::OutputMode COutput::GetMode() const
{
	return m_Mode;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	COutput::IsRelay() const
{
	return m_Mode == OM_RELAY;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	COutput::IsMD() const
{
	return m_Mode == OM_MD;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	COutput::IsUVV() const
{
	return m_Mode == OM_UVV;
}
//////////////////////////////////////////////////////////////////////
inline CSecID COutput::GetUnitID() const
{
	return m_UnitID;
}
//////////////////////////////////////////////////////////////////////
inline CSecID COutput::GetClientID() const
{
	return m_ClientID;
}
//////////////////////////////////////////////////////////////////////
inline WORD	COutput::GetSubID() const
{
	return m_UnitID.GetSubID();
}
//////////////////////////////////////////////////////////////////////
inline WORD	COutput::GetCard() const
{
	return (WORD)(m_UnitID.GetGroupID() & 0xFF);
}
//////////////////////////////////////////////////////////////////////
inline CString COutput::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
typedef COutput* COutputPtr;
WK_PTR_ARRAY_CS(COutputArray,COutputPtr,COutputArrayCS);
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_OUTPUT_H__43C9E4A4_8B84_11D3_99EB_004005A19028__INCLUDED_)
