// MIME.h: interface for the CMIME class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIME_H__3D4DBA69_103A_11D4_A9EB_004005A26A3B__INCLUDED_)
#define AFX_MIME_H__3D4DBA69_103A_11D4_A9EB_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMIME  
{
	
public:

	CMIME();
	virtual ~CMIME();	

	CString GetEndBoundary();
	CString GetHeader();
	CString GetSubject();
	CString GetCharSet();

	void SetTransferEncoding(CString code);
	void SetSubtype(CString subtype);
	void SetSubject( CString subject);
	void SetTo(CString to);
	void SetFrom(CString from);
	void SetContentType(CString content);
	void SetContentName(CString sName);
	void SetCharSet(CString sCharSet);

	BYTE* CookMIMEMessage(BYTE*, int &Len);

	CBase64 m_Encoder;

private:	

	CString BuildHeader();

	CMemFile * m_pMailBuffer;
	CString m_sName;
	CString m_sMimeMessage;
	CString m_sHeader;
	CString m_sCONTENTENCODING;
	CString m_sBoundary;
	CString m_sCONTENTTYPE;
	CString m_sMESSAGEID;
	CString m_sMIME;
	CString m_sSUBJECT;
	CString m_sTO;
	CString m_sFROM;	
	CString m_sSUBTYPE;
	CString m_sCharSet;
};

#endif // !defined(AFX_MIME_H__3D4DBA69_103A_11D4_A9EB_004005A26A3B__INCLUDED_)
