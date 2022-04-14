#pragma once


// CDocPM100XML document
#include "SDITestDoc.h"

class CDocPM100XML : public CSDITestDoc
{
	DECLARE_DYNCREATE(CDocPM100XML)

public:
	CDocPM100XML();
	virtual ~CDocPM100XML();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL IsDataOK();
	virtual void CreateData();

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwTeleNum;
	DWORD m_dwEvtCounter;
	DWORD m_dwBFR;
	DWORD m_dwTCC;
	DWORD m_dwAlarm;
	CString m_sMessage;
	DWORD m_dwTicSerNum;
};
