// AudioUnit.h : main header file for the AUDIOUNIT application
//

#if !defined(AFX_AUDIOUNIT_H__50E8B626_FCF5_4405_8926_994339CE1819__INCLUDED_)
#define AFX_AUDIOUNIT_H__50E8B626_FCF5_4405_8926_994339CE1819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define DVRT_DEBUG_KEY        _T("Debug")
#define TEST_AUDIO_CONNECTION _T("TestAudioConnection")
#define CONNECT2AUDIOUNITS    _T("Connect2AudioUnits")
#define AUDIOOFFSETFORREPLAY  _T("AudioOffsetForReplay")
  #define DEFAULT_OFFSET_REPLAY 150
#define EXTENDEDMENU          _T("ExtendedMenu")
#define MERGE_SAMPLES         _T("MergeSamples")
/////////////////////////////////////////////////////////////////////////////
// CAudioUnitApp:
// See AudioUnit.cpp for the implementation of this class
//
class CDebug;
class CAudioUnitDlg;

class CAudioUnitApp : public CWinApp
{
public:
	CAudioUnitApp();
	virtual ~CAudioUnitApp();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	int					ReadAndInitIntKey(CWK_Profile&wkp, LPCTSTR sSection, LPCTSTR sKey, int nDefault = 0);
// Parameters
	CAudioUnitDlg*		GetAudioUnitDlg(){return m_pAudioUnitDlg;}
	WK_ApplicationId	GetApplicationId() {return m_AppID;}
	int					GetInstanceCount() {return m_nInstance;}
	BOOL				GetAudioDebug()    {return m_bAudioDebug;}
	BOOL				IsReadOnly()       {return (BOOL)m_dwReadOnly;}
	DWORD				GetMainTreadID()   {return m_dwThreadID;};
#if _MFC_VER >= 0x0710
	int					SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

	//{{AFX_MSG(CAudioUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:	// Trace Options
	BOOL m_bTraceStateFlags;
	BOOL m_bTraceOnRequestStartMediaEncoding;
	BOOL m_bTraceOnRequestStopMediaEncoding;
	BOOL m_bTraceOnIndicateMediaData;
	BOOL m_bTraceOnRequestMediaDecoding;
	BOOL m_bTraceOnRequestStopMediaDecoding;
	BOOL m_bTraceOnRequestSetValue;
	BOOL m_bTraceOnRequestValues;
	BOOL m_bTraceFilterMessages;
	BOOL m_bDoIndicateMediaData;
	BOOL m_bFreePushSourceBuffers;
	BOOL m_bAudioDebug;
#ifdef	_USE_AS_MEDIAPLAYER
	CString	m_sParam;
#endif // _USE_AS_MEDIAPLAYER
private:
	CAudioUnitDlg     *m_pAudioUnitDlg;
	WK_ApplicationId  m_AppID;
	int					m_nInstance;
	DWORD             m_dwReadOnly;
	DWORD             m_dwThreadID;
};	


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOUNIT_H__50E8B626_FCF5_4405_8926_994339CE1819__INCLUDED_)
