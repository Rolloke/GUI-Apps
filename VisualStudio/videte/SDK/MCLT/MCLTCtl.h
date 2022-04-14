#if !defined(AFX_MCLTCTL_H__D6BFB2BA_C940_4F08_A5BE_986ED98EE39C__INCLUDED_)
#define AFX_MCLTCTL_H__D6BFB2BA_C940_4F08_A5BE_986ED98EE39C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPCInputOCX.h"
#include "IPCOutputOCX.h"
#include "LivePicture.h"

class CIPCOutputOCX;

// MCLTCtl.h : Declaration of the CMCLTCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl : See MCLTCtl.cpp for implementation.


class CMCLTCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMCLTCtrl)

// Constructor
public:
	CMCLTCtrl();
	DWORD	GetControlFlags();
	BOOL	CheckPassword(CString sHash);
	BOOL	ConnectToInput();
	BOOL	ConnectToOutput();
	CConnectionRecord	SetParamsConnectionRecord();
	void CloseConnection();
	void Fill_Detectors(const CIPCInputRecordArray& detectors);

	void IndicateRecords(WORD wGroupID, 
						 int iNumGroups, 
						 int numRecords, 
						 const CIPCOutputRecord records[],
						 CIPCOutputOCX* pOutputSample);

	void PictureData(const CIPCPictureRecord &pict,
		CSecID idArchive, 
		DWORD dwX, 
		DWORD dwY,
		int iFrameRate);

	void					KillGOPTimer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCLTCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CMCLTCtrl();

	DECLARE_OLECREATE_EX(CMCLTCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CMCLTCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMCLTCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CMCLTCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CMCLTCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT			DoRequestPicture(WPARAM, LPARAM);
	afx_msg void			OnTimer(UINT nIDEvent);
	afx_msg LRESULT			OnDecodeVideo(WPARAM, LPARAM);

// Dispatch maps
	//{{AFX_DISPATCH(CMCLTCtrl)
	afx_msg BOOL GetSelected();
	afx_msg short GetShape();
	afx_msg void SetShape(short nNewValue);
	afx_msg void SetConnect(CString sNew);
	afx_msg void SetCamera(short nCamNr);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CMCLTCtrl)
	void FireSelect(BOOL IsSelected)
		{FireEvent(eventidSelect,EVENT_PARAM(VTS_BOOL), IsSelected);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CMCLTCtrl)
	dispidSelected = 1L,
	dispidShape = 2L,
	eventidSelect = 1L,
	//}}AFX_DISP_ID
	};
private:

	//implementations for use of mpeg4 images
	void					SetGOPTimer(UINT uElapse);
	void					GetNextPictureOfGOP();
	static void CALLBACK	GOPTimer(UINT wTimerID, 
		UINT msg, 
		DWORD dwUser, 
		DWORD dw1, 
		DWORD dw2);

private:
	//members

	BOOL	m_bSelected;
	short	m_nShape;
	CString m_sFoundLanguage;
	CString m_sConnected;
	CString m_sTextMsg;
	CIPCInputOCX* m_pCIPCInputOCX;
	CIPCOutputOCX* m_pCIPCOutputOCX;
	CMCLTCtrl* m_pMCLTCtrl;	

	CString m_sIPAdress;
	CString m_sPassword;
	CString m_sPin;
	DWORD m_dwCurrentCamera;

	CIPCControlInterface* m_pCIPCControlInterface;

	CIPCOutputRecordArray	m_paCameras;	//pointerarray of the output records
	//of type 'camera'
	CIPCOutputRecordArray	m_paRelays;		//pointerarray of the output records
	//of type 'relay'

	CCriticalSection	m_csPictureRecord;
	int					m_iPictureRequests;	//counter for number of live
	CIPCPictureRecord*	m_pPictureRecord;	//pointer to the live picture
	CIPCPictureRecords	m_GOPs;				//container for MPEG4 picture
	int					m_nCurrentPicture;	//number of current decoded Mpeg4 picture
	//groups. Up to 5 picture in one GOP.
	CMPEG4Decoder*		m_pMPEG4Decoder;	//pointer to the MPEG4 decoder
	CJpeg*				m_pJpeg;			//pointer to the Jpeg decoder
	volatile UINT		m_uGOPTimer;		//identifier for the multi media
	//GOP timer event
	BOOL				m_bFrameWasDropped;	//FALSE if decoding of a MPEG4 picture fails
	BOOL				m_bUseMMTimer;		//TRUE if multi media timer can be used
	static UINT			gm_wTimerRes;		//timer resolution in milli seconds of
	//the multi media timer
	BOOL				m_bGOPTimerThreadPriorIsSet;	//TRUE if the thread priority
	//of the multi media thread
	CSecID				m_secIDCurrentCamera;//system wide internal SecID 
	CompressionType		m_CompressionType;	//the compression type of the live cameras,
	//MPEG 4 or Jpeg.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCLTCTL_H__D6BFB2BA_C940_4F08_A5BE_986ED98EE39C__INCLUDED)
