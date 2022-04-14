#pragma once


// CDocSchneiderIntercom document
#include "SDITestDoc.h"

class CDocSchneiderIntercom : public CSDITestDoc
{
	DECLARE_DYNCREATE(CDocSchneiderIntercom)

public:
	CDocSchneiderIntercom();
	virtual ~CDocSchneiderIntercom();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL IsDataOK();
	virtual void CreateData();
	virtual	BOOL OpenCom();

	DECLARE_MESSAGE_MAP()

public:
	CString m_sSubscriberNr;
	int m_iAlarmType;
};
