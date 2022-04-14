/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: AudioChannelPage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/AudioChannelPage.h $
// CHECKIN:		$Date: 10.01.06 15:51 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 10.01.06 12:48 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef AUDIO_CHANNEL_PAGE_H
#define AUDIO_CHANNEL_PAGE_H

#include "input.h"
#include "SecID.h"
#include "SVPage.h"

#include "AudioChannel.h"
#include "AudioChannelDialog.h"

class CIPCControlAudioUnit;
/////////////////////////////////////////////////////////////////////////////
// CAudioChannelPage dialog

class CAudioChannelPage : public CSVPage
{
	DECLARE_DYNAMIC(CAudioChannelPage)

// Construction
public:
	CAudioChannelPage(CSVView* pParent, bool bInputs);
	~CAudioChannelPage();

// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAudioChannelPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL StretchElements();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual int  GetNoOfSubDlgs();
	virtual CWK_Dialog* GetSubDlg(int);

// Implementation
protected:
	void  CheckDefault(int nUnit, int nChannel, int nStereoChannel);
	void	Resize();
	void	Reset(BOOL bDirection);
	void  CreateChannelDialogs();

	// Generated message map functions
	//{{AFX_MSG(CAudioChannelPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	BOOL CheckDeactivateChannel();
	//{{AFX_DATA(CAudioChannelPage)
	enum { IDD = IDD_AUDIO_CHANNELS };
	CStatic	m_staticInputNr;
	CScrollBar	m_ScrollBar;
	int		m_iScroll;
	//}}AFX_DATA

	CAudioChannelDialogArray m_ChannelDlgs;
	CAudioChannelArray       m_Channels;

	friend class CAudioChannelDialog;

private:
	int  m_nChannels;
	int  m_nUnits;
	bool m_bInputs;
	bool m_bRecursive;
};

#endif // AUDIO_CHANNEL_PAGE_H
