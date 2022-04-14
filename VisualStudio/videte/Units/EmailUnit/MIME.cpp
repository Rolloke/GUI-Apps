// MIME.cpp: implementation of the CMIME class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIME.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIME::CMIME()
{
	
	m_sMESSAGEID = _T("<No_ID>");
	m_sMIME=_T("1.0");
	m_sTO=_T("");
	m_sBoundary=_T("gc0p4Jq0M2Yt08jU534c0p");	//Zufällige Zeichenfolge,
											//erlaubte Zeichen, siehe RFC1521
	m_sSUBTYPE=_T("rfc822");					//Standard Sub_Type
	m_sCONTENTENCODING=_T("7bit");				//Standard Kodierung
	m_sHeader=_T("");
	m_pMailBuffer=new CMemFile(1);

}

CMIME::~CMIME()
{
	WK_DELETE(m_pMailBuffer);
}

BYTE* CMIME::CookMIMEMessage(BYTE *mail, int &iLen)
{
	CByteArray ba;
	CWK_String sHelpString;
	BYTE* pHelpmem_ptr;

	//Mime Sub_Header erstellen
	if(m_sName==_T(""))
	{
		sHelpString.Format(_T("\r\n--%s\r\nContent-Type: %s/%s%s\r\nContent-transfer-encoding: %s\r\n\r\n"),m_sBoundary,m_sCONTENTTYPE,m_sSUBTYPE,m_sCharSet,m_sCONTENTENCODING);
	}
	else //Es ist ein Name für das Bild, Anhängsel... vorhanden
	{
		CString sName;
		int i, nLen = m_sName.GetLength();
		for (i=0; i<nLen; i++)
		{
			if (((WORD)m_sName[i]) < 255)
			{
				sName += m_sName[i];
			}
		}
		sHelpString.Format(_T("\r\n--%s\r\nContent-Type: %s/%s;name=\"%s\"\r\nContent-transfer-encoding: %s\r\n\r\n"),m_sBoundary,m_sCONTENTTYPE,m_sSUBTYPE,sName,m_sCONTENTENCODING);
	}

	LPCSTR szHelpString = sHelpString;
	m_pMailBuffer->Write(szHelpString, strlen(szHelpString));	//Kodierung des Message Body's
	m_sCONTENTENCODING.MakeLower();		//Base64
	if(m_sCONTENTENCODING == _T("base64"))
	{
		if (m_Encoder.Encode((const char *)mail, iLen, ba) == 0)
		{
			WK_TRACE_ERROR(_T("[CEmailUnitDlg::CookMIMEMessage] Encode Base64 FAILED\n"));	
		}
		else
		{
			iLen = ba.GetSize();
			mail = ba.GetData();
		}
	}
	else if(m_sCONTENTENCODING == _T("binary"))
	{
					
	}

	m_pMailBuffer->Write(mail,iLen);
	iLen = iLen + sHelpString.GetLength();

	//an das Ende ein CR LF hängen
	sHelpString.CleanUp();
	sHelpString = _T("\r\n");
	szHelpString = sHelpString;
	m_pMailBuffer->Write(szHelpString, strlen(szHelpString));
	iLen = iLen + sHelpString.GetLength();

	pHelpmem_ptr = m_pMailBuffer->Detach();

	return pHelpmem_ptr;
}

void CMIME::SetContentType(CString content)
{
	m_sCONTENTTYPE=content;
}

void CMIME::SetFrom(CString from)
{
	m_sFROM = from;
}

void CMIME::SetTo(CString to)
{
	if(m_sTO!=_T(""))
	{
		m_sTO=m_sTO + _T(",");
		m_sTO=m_sTO + to;
	}
	else
	{
		m_sTO=to;
	}
}

void CMIME::SetSubject(CString subject)
{
	m_sSUBJECT = subject;
}

void CMIME::SetSubtype(CString subtype)
{
	m_sSUBTYPE = subtype;
}

void CMIME::SetTransferEncoding(CString code)
{
	m_sCONTENTENCODING = code;	
}

CString CMIME::BuildHeader()
{
	CString HilfsString;
	CString sSubject;

	int i, nLen = m_sSUBJECT.GetLength();
	for (i=0; i<nLen; i++)
	{
		if (((WORD)m_sSUBJECT[i]) < 255)
		{
			sSubject += m_sSUBJECT[i];
		}
	}

	m_sHeader.Format(_T("From: %s\r\nTo: %s\r\nSubject: %s\r\nMIME-Version: %s\r\nMessage-ID: %s\r\nContent-type: multipart/mixed"), //
						m_sFROM,m_sTO,sSubject,m_sMIME,m_sMESSAGEID);

	if (!m_sCharSet.IsEmpty())
	{
		m_sHeader += m_sCharSet;
	}
	m_sHeader += _T("; ");

	HilfsString.Format(_T("boundary=%s\r\n"),m_sBoundary);
	m_sHeader=m_sHeader+HilfsString;

	if (!m_sCharSet.IsEmpty())
	{
		m_sHeader += _T("Content-transfer-encoding: 8bit\r\n");
	}
	m_sHeader += _T("\r\n");
	return m_sHeader;
}

CString CMIME::GetHeader()
{
	CString Header=BuildHeader();
	return Header;
}

CString CMIME::GetSubject()
{
	return m_sSUBJECT;
}

CString CMIME::GetCharSet()
{
	return m_sCharSet;
}

CString CMIME::GetEndBoundary()
{
	return _T("\r\n--")+m_sBoundary+_T("--\r\n");
}

void CMIME::SetContentName(CString sName)
{
	//Der Name des Bildes, Anhangs...
	m_sName=sName;
}
void CMIME::SetCharSet(CString sCharSet)
{
	if (sCharSet.IsEmpty())
	{
		m_sCharSet.Empty();
	}
	else
	{
		m_sCharSet.Format(_T("; charset=\"%s\""), sCharSet); 
	}
}