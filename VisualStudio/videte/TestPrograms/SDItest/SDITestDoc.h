/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITestDoc.h $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/SDITestDoc.h $
// CHECKIN:		$Date: 21.12.05 16:50 $
// VERSION:		$Revision: 27 $
// LAST CHANGE:	$Modtime: 21.12.05 15:02 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
// SDITestDoc.h : interface of the CSDITestDoc class

#if !defined(AFX_SDITESTDOC_H__2D22C521_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTDOC_H__2D22C521_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDIRS232.h"
#include "SDISocket.h"

/////////////////////////////////////////////////////////////////////////////
class CSDITestView;

/////////////////////////////////////////////////////////////////////////////
class CSDITestDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CSDITestDoc)
	CSDITestDoc();
public:
	virtual ~CSDITestDoc();

// Attributes
public:
	BOOL	IsComOK();
	SDIControlType GetType();
	CString	GetDate(const CString& sFormat);
	CString	GetTime(const CString& sFormat);
	CString	GetWSID(int iLen, BOOL bTrimLeadingZeros=FALSE);
	CString GetTAN(int iLen, BOOL bIncrease, BOOL bTrimLeadingZeros=FALSE);
	CString GetBLZ(int iLen, BOOL bTrimLeadingZeros=FALSE);
	CString GetKonto(int iLen, BOOL bTrimLeadingZeros=FALSE);
	CString GetBetrag(int iLen, BOOL bTrimLeadingZeros=FALSE);
	CString GetCurrency();
	CString	GetDay(int iChar, BOOL bGerman);
	CString	GetTimeHHMMSS();
	CString GetFirstName();
	CString GetLastName();
	BYTE	GetXORCrcByte(CByteArray& byaData, int iStart=0, int iEnd=-1);
	int		GetRandomNumberBetweenAsInt(int iMin, int iMax);
	CString GetRandomNumberWithLen(int iLen, BOOL bTrimLeadingZeros=FALSE);
	CString	GetInternTAN(int iLen, BOOL bIncrease, BOOL bTrimLeadingZeros=FALSE);
	CString	GetRandomNumberBetweenAsString(int iMin, int iMax);
	CString GetRandomBetrag(int iLen, BOOL bTrimLeadingZeros=FALSE);
	BOOL	IsDateOK();
	BOOL	IsTimeOK();
	BOOL	IsBlzOK();
	BOOL	IsKontoOK();
	
	int     GetCOM();
	int		GetPort();
	CString GetAddress();
	UINT	GetIPType();

// Operations
public:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDITestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL
protected:
	virtual	BOOL	OpenCom();
	virtual BOOL	IsDataOK();
	virtual void	CreateData();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
public:
	virtual void ConfigCom();
	virtual void Send();
protected:
	void	LoadDebugConfiguration();
	void	InitFirstAndLastNameArray();
	void	GenerateBitError();
	CString TrimDataToLength(CString sData, int iLen, BOOL bTrimLeadingZeros=FALSE);
// Generated message map functions
protected:
	//{{AFX_MSG(CSDITestDoc)
	afx_msg void OnConfigCom();
	afx_msg void OnUpdateConfigCom(CCmdUI* pCmdUI);
	afx_msg void OnConfigIP();
	afx_msg void OnUpdateConfigIP(CCmdUI* pCmdUI);
	afx_msg void OnSend();
	afx_msg void OnUpdateSend(CCmdUI* pCmdUI);
	afx_msg void OnStopSend();
	afx_msg void OnUpdateStopSend(CCmdUI* pCmdUI);
	afx_msg void OnTraceRs232Ascii();
	afx_msg void OnUpdateTraceRs232Ascii(CCmdUI* pCmdUI);
	afx_msg void OnTraceRs232Hex();
	afx_msg void OnUpdateTraceRs232Hex(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
public:
	BOOL		m_bRepeat;
	UINT		m_uRepeatTime;
	int			m_iRepeatHow;
	BOOL		m_bLimited;
	UINT		m_uLimited;
	BOOL		m_bBitError;
	BOOL		m_bSendSingleBytes;
	UINT		m_uSendDelay;
	CString		m_sDate;
	CString		m_sTime;
	CString		m_sBLZ;
	CString		m_sKonto;
	CString		m_sBetrag;
	CString		m_sTAN;
	CString		m_sWSID;
	CString		m_sCurrency;
	BOOL		m_bStarted;

	BOOL			m_bTraceRS232DataHex;
	BOOL			m_bTraceRS232DataAscii;

protected:
	SDIControlType	m_eType;
	CSDIRS232*		m_pCom;
	CSDISocket		m_Socket;
	
	int				m_iCom;
	int				m_iIPPort;
	CString			m_sIPAddress;
	UINT			m_iIPType;

	BOOL			m_bTraceRS232Events;
	int				m_iTAN;
	UINT			m_uRandom;
	UINT			m_uCounter;
	CByteArray		m_byaData;
	UINT			m_uSerializeVersion;
	CString			m_sDirectory;
	CStringArray	m_sFirstNameArray;
	CStringArray	m_sLastNameArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTDOC_H__2D22C521_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
