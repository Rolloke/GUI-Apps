// CUpdateSystem.h: interface for the CUpdateSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUPDATESYSTEM_H__9C7539A3_A9A7_11D4_8FA0_004005A11E32__INCLUDED_)
#define AFX_CUPDATESYSTEM_H__9C7539A3_A9A7_11D4_8FA0_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	UPDATE_WRONG_OEM		1
#define UPDATE_NO_UPDATE		2
#define UPDATE_WRONG_DISK		3
#define UPDATE_WRONG_CRC		4
#define UPDATE_WRONG_BUILD		5
#define UPDATE_DISK_READ_FAILED	6
#define UPDATE_COPY_FAILED		7
#define UPDATE_DECODE_FAILED	8
#define UPDATE_LAST_DISK		9
#define UPDATE_NEXT_DISK		10
#define UPDATE_NO_UPDATE_DISK	11

class CUpdateSystem  
{
	// construction / destruction
public:
	CUpdateSystem();
	virtual ~CUpdateSystem();

	// attributes
public:
	inline int  GetDiskNumber() const;
	inline BOOL IsFDDUpdate() const;
	inline BOOL IsCDUpdate() const;
	inline BOOL IsRemoteUpdate() const;

	// operations
public:
	BOOL ContinueUpdate();
	BOOL StartUpdate(OEM eOEM);

	BOOL  LoadFile(const CString& sSource);
	DWORD GetCRC();
	BOOL  Decode();
	BOOL  SaveFile(const CString& sTarget);

private:
	BYTE*			m_pBuffer;
	DWORD			m_dwLen;
	int				m_nUpdateDiskCounter;
	OEM				m_eOem;
	DWORD			m_dwTimeStamp;
	DWORD			m_dwBuild;
	BOOL			m_bCDUpdate;
	BOOL			m_bFDDUpdate;
	BOOL			m_bRemoteUpdate;
};
inline int CUpdateSystem::GetDiskNumber() const
{
	return m_nUpdateDiskCounter;
}
inline BOOL CUpdateSystem::IsCDUpdate() const
{
	return m_bCDUpdate;
}
inline BOOL CUpdateSystem::IsFDDUpdate() const
{
	return m_bFDDUpdate;
}
inline BOOL CUpdateSystem::IsRemoteUpdate() const
{
	return m_bRemoteUpdate;
}

#endif // !defined(AFX_CUPDATESYSTEM_H__9C7539A3_A9A7_11D4_8FA0_004005A11E32__INCLUDED_)
