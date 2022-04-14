/* GlobalReplace: EX_OUTPUT_CAMERA_COLOR --> EX_OUTPUT_CAMERA_FBAS */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: output.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/output.h $
// CHECKIN:		$Date: 28.08.06 16:51 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 28.08.06 15:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __OUTPUT_H_
#define __OUTPUT_H_


#define PTZ_INTERFACE_HTTP			33
#define PTZ_INTERFACE_AXIS_RS232	34
#define PTZ_INTERFACE_AXIS_RS485	35
#define PTZ_INTERFACE_UDP_RS485		36

class COutputGroup;
///////////////////////////////////////////////////////////////////////////
enum ExOutputType 
{
	EX_OUTPUT_OFF = -1,
	EX_OUTPUT_RELAY = 1,
	EX_OUTPUT_CAMERA_FBAS = 4,
	EX_OUTPUT_CAMERA_SVHS = 5,
};
/////////////////////////////////////////////////////////////////////
class COutput
{          
	DECLARE_DYNAMIC(COutput)
	// construction / destruction
public:
	COutput(WORD wGroupID, WORD wSubID);
	~COutput();
	const COutput& operator=(const COutput& outp);

	// access:
public:
	// common
	inline const CString&	GetName() const;
	inline const CSecID GetID() const;
	
	// camera
	inline BOOL					IsCamera() const;
	inline BOOL					GetSN() const;
	inline BOOL					IsSTM() const;
	inline int					GetPTZInterface() const;
	inline ExOutputType			GetExOutputType() const;
	inline CameraControlType	GetCameraControlType() const;
	inline DWORD				GetSnId() const;
	inline BOOL					IsReference() const;
	inline BOOL					IsTerminated() const;

	// IP / DS
	inline const CString&		GetType() const;
	inline const CString&		GetDeviceName() const;
	inline const CString&		GetUrl() const;
	inline		 UINT			GetPort() const;
	inline const CString&		GetRotation() const;
	inline const CString&		GetCompression() const;
	inline       int			GetOutputNr() const;

	inline CameraControlCmd		GetCameraControlCmd() const;
	
	// relais
	inline BOOL		IsRelay() const;
	inline DWORD	GetHoldClose() const;
	inline BOOL		GetEdge() const;
	inline BOOL		GetClient() const;
	inline BOOL		GetKeep() const;
	inline BOOL		DeleteActivations() const;

	// operations
public:
	// common
	inline void  SetName(const CString &sName);

	// camera
	inline void  SetReference(BOOL bReference);
	inline void  SetTermination(BOOL bTerminated);
	inline void  SetSN(BOOL bSN);
	inline void  SetSTM(BOOL bSTM);
	inline void  SetCameraControlType(CameraControlType type);
	inline void  SetPTZInterface(int i);
	inline void  SetExOutputType(ExOutputType type);
	inline void  SetSnId(DWORD dwSnId);
	inline void  SetCameraControlCmd(CameraControlCmd cmd);

	// relais
	inline void  SetHoldClose(DWORD dwValue);
	inline void  SetEdge(BOOL bEdge);
	inline void  SetClient(BOOL bClient);
	inline void  SetKeep(BOOL bKeep);
	inline void  SetDeleteActivation();

	// IP / DS
	inline void		SetType(const CString&);
	inline void		SetDeviceName(const CString&);
	inline void		SetUrl(const CString&);
	inline void		SetPort(UINT);
	inline void		SetRotation(const CString&);
	inline void		SetCompression(const CString&);
	inline void		SetOutputNr(int);
	// serialization
	void GenerateHTMLFileCamera(COutputGroup* pOutputGroup);

public:
	BOOL	Load(CWK_Profile& wkp);
	BOOL	Save(CWK_Profile& wkp);

private:
	ExOutputType	TypeName2TypeNr(const CString &s);
	LPCTSTR    TypeNr2TypeName(ExOutputType iTypeNr);

private:
	CString			m_sName;			// name of the output
	CSecID			m_id;				// the unique SecID

	ExOutputType	m_ExOutputType;		// Supervisor-Typ des Ausgangs EX_OUTPUT_RELAY, EX_OUTPUT_CAMERA_SW etc

	BOOL				m_bReference;
	BOOL				m_bTerminated;
	BOOL				m_bSN;				// SN kopf
	CameraControlType	m_SNType;
	int					m_iPTZInterface;
	DWORD				m_dwSNID;			// HIWORD: opt. system id, LOWORD; camera id
	CameraControlCmd	m_eSNCmd;

	// Störungsmelder
	BOOL			m_bSTM;

	// Relais
	DWORD			m_HoldClose;		// Relay Hold time in s
	BOOL			m_bEdge;
	BOOL			m_bClient;
	BOOL			m_bKeep;
	BOOL			m_bDeleteActivations;

	// IP / DS
	CString			m_sType;
	CString			m_sDeviceName;
	CString			m_sUrl;
	UINT			m_uPort;
	CString			m_sRotation;
	CString			m_sCompression;
	int				m_nOutputNr;
};
/////////////////////////////////////////////////////////////////////
inline ExOutputType COutput::GetExOutputType() const
{
	return m_ExOutputType;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetExOutputType(ExOutputType type)
{
	m_ExOutputType = type;
}
/////////////////////////////////////////////////////////////////////
inline const CString &COutput::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
inline void COutput::SetName(const CString &sName)
{
	m_sName = sName;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetCameraControlType(CameraControlType type)
{
	m_SNType = type;
}
/////////////////////////////////////////////////////////////////////
inline CameraControlType COutput::GetCameraControlType() const
{
	return m_SNType;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetPTZInterface(int iPTZInterface)
{
	m_iPTZInterface = iPTZInterface;
}
/////////////////////////////////////////////////////////////////////
inline int COutput::GetPTZInterface() const
{
	return m_iPTZInterface;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::IsCamera() const
{
	return (   m_ExOutputType==EX_OUTPUT_CAMERA_FBAS
			|| m_ExOutputType==EX_OUTPUT_CAMERA_SVHS);
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::GetSN() const
{
	return m_bSN;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::IsSTM() const
{
	return m_bSTM;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::IsReference() const
{
	return m_bReference;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::IsTerminated() const
{
	return m_bTerminated;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetSTM(BOOL bSTM)
{
	m_bSTM = bSTM;
	if (m_bSTM)
		SetExOutputType(EX_OUTPUT_OFF);
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetSN(BOOL bSN)
{
	m_bSN = bSN;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetReference(BOOL bReference)
{
	m_bReference = bReference;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetTermination(BOOL bTerminated)
{
	m_bTerminated = bTerminated;
}
/////////////////////////////////////////////////////////////////////
inline const CSecID COutput::GetID() const
{
	return m_id;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::IsRelay() const
{
	return ( m_ExOutputType==EX_OUTPUT_RELAY );
}
/////////////////////////////////////////////////////////////////////
inline DWORD COutput::GetHoldClose() const
{
	return m_HoldClose;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::GetEdge() const
{
	return m_bEdge;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::GetClient() const
{
	return m_bClient;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::GetKeep() const
{
	return m_bKeep;
}
/////////////////////////////////////////////////////////////////////
inline BOOL COutput::DeleteActivations() const
{
	return m_bDeleteActivations;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetHoldClose(DWORD dwValue)
{
	m_HoldClose = dwValue;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetEdge(BOOL bEdge)
{
	m_bEdge = bEdge;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetClient(BOOL bClient)
{
	m_bClient = bClient;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetKeep(BOOL bKeep)
{
	m_bKeep = bKeep;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetDeleteActivation()
{
	m_bDeleteActivations = TRUE;
}
/////////////////////////////////////////////////////////////////////
inline DWORD COutput::GetSnId() const
{
	return m_dwSNID;
}
/////////////////////////////////////////////////////////////////////
inline void  COutput::SetSnId(DWORD dwSnId)
{
	m_dwSNID = dwSnId;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetCameraControlCmd(CameraControlCmd cmd)
{
	m_eSNCmd = cmd;
}
/////////////////////////////////////////////////////////////////////
inline CameraControlCmd COutput::GetCameraControlCmd() const
{
	return m_eSNCmd;
}
/////////////////////////////////////////////////////////////////////
inline const CString& COutput::GetType() const
{
	return m_sType;
}
/////////////////////////////////////////////////////////////////////
inline const CString& COutput::GetDeviceName() const
{
	return m_sDeviceName;
}
/////////////////////////////////////////////////////////////////////
inline const CString& COutput::GetUrl() const
{
	return m_sUrl;
}
/////////////////////////////////////////////////////////////////////
inline UINT	COutput::GetPort() const
{
	return m_uPort;
}
/////////////////////////////////////////////////////////////////////
inline const CString& COutput::GetRotation() const
{
	return m_sRotation;
}
/////////////////////////////////////////////////////////////////////
inline const CString& COutput::GetCompression() const
{
	return m_sCompression;
}
/////////////////////////////////////////////////////////////////////
inline int COutput::GetOutputNr() const
{
	return m_nOutputNr;
}
/////////////////////////////////////////////////////////////////////
inline void COutput::SetType(const CString&sType)
{
	m_sType = sType;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetDeviceName(const CString&sName)
{
	m_sDeviceName = sName;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetUrl(const CString&sUrl)
{
	m_sUrl = sUrl;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetPort(UINT uPort)
{
	m_uPort = uPort;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetRotation(const CString&sRotation)
{
	m_sRotation = sRotation;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetCompression(const CString&sCompression)
{
	m_sCompression = sCompression;
}
/////////////////////////////////////////////////////////////////////
inline void	COutput::SetOutputNr(int nNr)
{
	m_nOutputNr = nNr;
}
/////////////////////////////////////////////////////////////////////
typedef COutput* COutputPtr;
WK_PTR_ARRAY(COutputArray,COutputPtr);
#endif
