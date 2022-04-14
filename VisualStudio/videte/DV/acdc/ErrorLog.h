// ErrorLog.h: interface for the ErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORLOG_H__FA8B8388_E443_498E_99B3_EA4481075BA4__INCLUDED_)
#define AFX_ERRORLOG_H__FA8B8388_E443_498E_99B3_EA4481075BA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CErrorLog  
{
protected:
	FILE * m_fh;

public:
	CErrorLog ();
	~CErrorLog ();

	bool Open (LPCSTR psFilename);
	int printf (const char * format, ...);

};

#endif // !defined(AFX_ERRORLOG_H__FA8B8388_E443_498E_99B3_EA4481075BA4__INCLUDED_)
