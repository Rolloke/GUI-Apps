// IPCOutputViewcam.h: interface for the CIPCOutputViewcam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCOUTPUTVIEWCAM_H__A2307A83_F0CA_11D2_8948_004005A1D890__INCLUDED_)
#define AFX_IPCOUTPUTVIEWCAM_H__A2307A83_F0CA_11D2_8948_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CViewcamDlg;
class CIPCOutputViewcam : public CIPCOutputClient  
{
	// construction
public:
	CIPCOutputViewcam(const char* shmName);

	virtual ~CIPCOutputViewcam();


	// overrides
public:
	int m_MainViewcam;

   	void OnAddRecord(const CIPCOutputRecord &pRec);
	void OnUpdateRecord(const CIPCOutputRecord &pRec);
	void OnDeleteRecord(const CIPCOutputRecord &pRec);

	void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
										DWORD dwUserData);
private:

	
	CViewcamDlg * m_pViewcamDlg;
};

#endif // !defined(AFX_IPCOUTPUTVIEWCAM_H__A2307A83_F0CA_11D2_8948_004005A1D890__INCLUDED_)
