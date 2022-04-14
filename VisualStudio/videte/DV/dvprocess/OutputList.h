// OutputList.h: interface for the COutputList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTLIST_H__43C9E4A6_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_OUTPUTLIST_H__43C9E4A6_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OutputGroup.h"

class COutputList : public COutputGroupArrayCS
{
	// construction / destruction
public:
	COutputList();
	virtual ~COutputList();

	// attributes
public:
	int  GetNrOfActiveCameras();
	int  GetNrOfCameras();
	void ResetFramesPerDay();
	void ResetFramesPerWeek();
	
	COutput*  GetOutputByClientID(CSecID id);
	COutput*  GetOutputByUnitID(CSecID id);
	CCamera*  GetCameraByArchiveNr(WORD wArchiveNr);
	CCameraGroup* GetCameraGroup(int iCard);

	BOOL      IsScanning();

	// operations
public:
	void CalcClientIDs();
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp);
	void StartCIPCThreads();
	void WaitForConnect();
	void WaitForCameraScan();
	void RescanCameras();
	void StartRequests();
	void StopRequests();
	void SaveReferenceImage();
	void RemoveClient(DWORD dwClientID);
	void ResetAudioReferences();
	void OnResolutionChanged(Resolution resolution);
	void ClearActiveClientCam();
};

#endif // !defined(AFX_OUTPUTLIST_H__43C9E4A6_8B84_11D3_99EB_004005A19028__INCLUDED_)
