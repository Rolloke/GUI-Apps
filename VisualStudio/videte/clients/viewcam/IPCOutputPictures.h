// IPCOutputPictures.h: interface for the CIPCOutputPictures class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCOUTPUTPICTURES_H__2F17D9F8_F16C_11D2_8949_004005A1D890__INCLUDED_)
#define AFX_IPCOUTPUTPICTURES_H__2F17D9F8_F16C_11D2_8949_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CIPCOutputPictures : public CIPCOutputVision  
{
public:
	CIPCOutputPictures(const char *shmName, CServer* pServer, CSecID id);
	virtual ~CIPCOutputPictures();

};

#endif // !defined(AFX_IPCOUTPUTPICTURES_H__2F17D9F8_F16C_11D2_8949_004005A1D890__INCLUDED_)
