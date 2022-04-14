#pragma once

#include "SDITestDoc.h"
// CDocPM100IPAlarm document

class CDocPM100IPAlarm : public CSDITestDoc
{
	DECLARE_DYNCREATE(CDocPM100IPAlarm)

public:
	CDocPM100IPAlarm();
	virtual ~CDocPM100IPAlarm();
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
	DWORD m_dwBFR;
	DWORD m_dwTCC;
	DWORD m_dwAlarm;
	CString m_sMessage;

};
