// RMADatabase.h: interface for the CRMADatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMADATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_)
#define AFX_RMADATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRMADatabase  
{
public:
	CRMADatabase();
	virtual ~CRMADatabase();

public:
	Code4		m_cb;
	Data4		m_data4;
	Field4info	m_fieldinfo;

};

#endif // !defined(AFX_RMADATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_)
