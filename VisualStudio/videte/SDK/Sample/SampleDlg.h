/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: SampleDlg.h
|*
|* PURPOSE: Client dialog interface.
******************************************************************************/


#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "IPCDatabaseSample.h"
#include "IPCOutputSample.h"
#include "resource.h"
#include "PlayPicture.h"


class CSampleDlg : public CDialog
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CSampleDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CSampleDlg();

// Dialog Data
	enum { IDD = IDD_SAMPLE_DIALOG };

//////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////
public:

	void					OnDraw(CDC* pDC);
	void					FillListCtrl_Cameras(const CIPCOutputRecordArray& pa_cameras);
	void					FillListCtrl_Relays(const CIPCOutputRecordArray& pa_relays);
	void					FillListCtrl_Detectors(const CIPCInputRecordArray& detectors);
	void					FillListCtrl_Archives();
	void					FillListCtrl_Sequences(WORD wArchiveNr, 
												   CIPCSequenceRecords& sequences);
	void					SetDBTextMsg(CString sTextMessage);
	void					SetPictureInfo(CString sTextMessage);
	PlayStatus				ActivatePlayButtons(PlayStatus ps, 
												BOOL bVisible = TRUE);
	void					SetCurrentRecord(WORD wArchiveNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr);
	inline	BOOL			IsDTS() const;
//////////////////////////////////////////////////////////////////////
protected:

	virtual void			DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

//////////////////////////////////////////////////////////////////////
private:
	CString					GetIPAdress();				
	CConnectionRecord		SetParamsConnectionRecord();
	void					InitControls();			
	void					DeleteListCtrlSequences(WORD wArchiveNr);
	void					CloseConnection();
	BOOL					ConnectToDatabase();
	BOOL					ConnectToOutput();
	BOOL					ConnectToInput();

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
protected:

	HICON m_hIcon;

//////////////////////////////////////////////////////////////////////
private:
	CIPCControlInterface*	m_pCIPCControlInterface;//pointer to the control interface
													//which controls the connection to
													//the server via LAN or ISDN
	CString					m_sPassword;			//the password for the permission level
													//(e.g. "supervisor") on the 
													//idip server (if exists)
	CString					m_sPin;					//pin number for connections to DTS systems
	DWORD					m_dwIPAdress;			//network IP address as DWORD 
													//(e.g. "3232235599" for "192.168.0.79") of 
													//the server to connect to
	CString					m_sIPAdress;			//network IP address as string 
	CIPCDatabaseSample*		m_pCIPCDatabaseSample;	//pointer to DatabasSample which
													//controls the database connection
	CIPCOutputSample*		m_pCIPCOutputSample;	//pointer to OutputSample which
													//controls the output connection
	CIPCInputSample*		m_pCIPCInputSample;		//pointer to InputSample which
													//controls the input connection
	CSecID					m_secIDCurrentCamera;	//system wide internal SecID 
	PlayStatus				m_CurrentPlayStatus;	//current playstatus of database playback
	BOOL					m_bIsDTS;				//TRUE if Sample is connected to a DTS systems
	CEdit					m_ctrlPin;				//Pin number for connections to DTS systems		
	CString					m_sTextMsg;				//text message of connection status and errors
	CString					m_sDBText;				//text message with archive, sequence and 
													//record number of the currently shown database picture
	CString					m_sPictureInfo;			//text message with the whole time stamp 
													//of the currently shown database picture

	//members for playing database pictures
	WORD					m_wCurrentArchive;	//archive number of the currently shown database picture
	WORD					m_wCurrentSequence;	//sequence number of the currently shown database picture
	DWORD					m_dwCurrentRecord;	//record number of the currently shown database picture

	//controls of the main dialog
	CListCtrl				m_ListCameras;				//list box of all live cameras
	CListCtrl				m_ListRelays;				//list box of all relays
	CListCtrl				m_ListDetectors;			//list box of all alarm detectors
	CListCtrl				m_ListArchives;				//list box of all database archives
	CListCtrl				m_ListSequences;			//list box of all sequences of currently selected
														//archive
	CPlayPicture			m_ctrlPlayPicture;			//windows for showing the database picture
	CLivePicture			m_ctrlLivePicture;			//windows for showing the live picture
	CButton					m_ctrlButtonStepBackward;	//control of play button "step b."
	CButton					m_ctrlButtonStepForward;	//control of play button "step f."
	CButton					m_ctrlButtonPlayForward;	//control of play button "play f."
	CButton					m_ctrlButtonPlayBackward;	//control of play button "play b."
	CButton					m_ctrlButtonFastForward;	//control of play button "fast f."
	CButton					m_ctrlButtonFastBackward;	//control of play button "play b."

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
private:

	// Generated message map functions
	virtual BOOL			OnInitDialog();
	afx_msg void			OnPaint();
	afx_msg HCURSOR			OnQueryDragIcon();
	afx_msg void			OnBnClickedOk();
	afx_msg void			OnLvnItemchangedListArchives(NMHDR *pNMHDR, 
														 LRESULT *pResult);
	afx_msg void			OnLvnItemchangedListSequences(NMHDR *pNMHDR,	
														  LRESULT *pResult);
	afx_msg void			OnLvnItemchangedListCameras(NMHDR *pNMHDR, 
														LRESULT *pResult);
	afx_msg void			OnBnClickedButtonConnect();
	afx_msg void			OnBnClickedButtonDisconnect();
	afx_msg void			OnBnClickedButtonStepForward();
	afx_msg void			OnBnClickedButtonStepBackward();
	afx_msg void			OnBnClickedButtonPlayForward();
	afx_msg void			OnBnClickedButtonPlayBackward();
	afx_msg void			OnBnClickedButtonFastForward();
	afx_msg void			OnBnClickedButtonFastBackward();
	afx_msg void			OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ctrlPictureInfo;
	CStatic m_ctrlDBText;
};
//////////////////////////////////////////////////////////////////////
inline BOOL CSampleDlg::IsDTS() const
{
	return m_bIsDTS;
}