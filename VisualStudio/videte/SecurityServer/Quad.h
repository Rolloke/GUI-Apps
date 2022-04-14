#pragma once

class CWK_RS232;

class CQuad : public CWK_RS232
{
	// construction destruction
public:
	CQuad(int iCom, QuadControlType qct, CComParameters* pComParameters = NULL);
	virtual ~CQuad();

	// attributes
public:
	inline BOOL IsLocked() const;
	inline BOOL IsVCRMode() const;
	inline BOOL IsLiveMode() const;
	inline BOOL IsSequencing() const;
	inline BOOL IsQuad() const;
	inline BOOL IsFullScreen() const;
	inline BOOL IsCameraOn(int iCamera) const;
		   int  GetVideoOut(CSecID idGroup, int iPort);

public:
	void Load(CWK_Profile& wkp);
	BOOL Create();

	void SwitchCamSingle(int iCam);
	void SwitchQuad();

	// virtual functions
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

private:
	int		         m_iCom;
	QuadControlType  m_eType;
	BYTE			 m_bStatus;
	CComParameters*	 m_pComParameters;
	CSecID			 m_GroupIDs[4];
	int				 m_iVOUT[4];

public:
	static BOOL m_bTraceQuad;
};
///////////////////////////////////////////////////////////////////////////////
inline BOOL CQuad::IsLocked() const
{
	return (m_bStatus & 0x80) == 0;
}
inline BOOL CQuad::IsVCRMode() const
{
	return (m_bStatus & 0x40) == 0;
}
inline BOOL CQuad::IsLiveMode() const
{
	return (m_bStatus & 0x40) > 0;
}
inline BOOL CQuad::IsSequencing() const
{
	return (m_bStatus & 0x20) == 0;
}
inline BOOL CQuad::IsQuad() const
{
	return (m_bStatus & 0x10) == 0 && !IsSequencing();
}
inline BOOL CQuad::IsFullScreen() const
{
	return (m_bStatus & 0x10) > 0 && !IsSequencing();
}
inline BOOL CQuad::IsCameraOn(int iCamera) const
{
	BYTE bCam = (BYTE)(1 << iCamera);
	return (m_bStatus & bCam) > 0;
}
