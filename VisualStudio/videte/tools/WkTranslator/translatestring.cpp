/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include <malloc.h>

#include "TranslateString.h"

#ifndef _CONSOLE
#include "WkTranslator.h"
extern CWkTranslatorApp theApp;
#endif

#ifdef _UNICODE
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CStdioFileU, CStdioFile)
/////////////////////////////////////////////////////////////////////////////
CStdioFileU::CStdioFileU()
{
	m_dwUnicodeFile = 0;
	m_fposition = 0;
}
/////////////////////////////////////////////////////////////////////////////
CStdioFileU::CStdioFileU(FILE* pOpenStream) : CStdioFile(pOpenStream)
{
	m_dwUnicodeFile = 0;
	m_fposition = 0;
}
/////////////////////////////////////////////////////////////////////////////
CStdioFileU::CStdioFileU(LPCTSTR lpszFileName, UINT nOpenFlags) : CStdioFile(lpszFileName, nOpenFlags)
{
	m_dwUnicodeFile = 0;
	m_fposition = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CStdioFileU::ReadString(CString& rString)
{
	if (m_dwUnicodeFile == TEXTIO_ANSI)
	{
		return CStdioFile::ReadString(rString);
	}
	else
	{
		ASSERT_VALID(this);
		
		rString.Empty();
		const int nMaxSize = 128;
		LPTSTR lpsz = rString.GetBuffer(nMaxSize);
		LPTSTR lpszResult = lpsz;
		int  i, nPartLen = 0, nLen = 0;
		BOOL bFirst = FALSE;

		for (;;)
		{
			nLen = CFile::Read(lpsz, nMaxSize*2);

			if (IsFileUndefined() || IsFileUnicode() || IsFileUnicodeBE())
			{
//				nLen = fread(lpsz, 2, nMaxSize, m_pStream);
				nLen /= 2;
			}
			else // MBCS
			{
//				nLen = fread(lpsz, 1, nMaxSize*2, m_pStream);
			}

			if (nLen == 0) return FALSE;
			
			if (m_dwUnicodeFile==0)
			{
				bFirst = TRUE;
				if (*((WORD*)lpsz) == (WORD)TEXTIO_UNICODE)
				{
					m_dwUnicodeFile = TEXTIO_UNICODE;
					for (i=0; i<nLen-1; i++)
					{
						lpsz[i] = lpsz[i+1];
					}
				}
				else if (*((WORD*)lpsz) == (WORD)TEXTIO_UNICODE_BIGENDIAN)
				{
					m_dwUnicodeFile = TEXTIO_UNICODE_BIGENDIAN;
					for (i=0; i<nLen-1; i++)
					{
						lpsz[i] = lpsz[i+1];
					}
				}
				else if (*((DWORD*)lpsz) == TEXTIO_UNICODE_UTF8)
				{
					m_dwUnicodeFile = TEXTIO_UNICODE_UTF8;
					return FALSE;
				}
				else
				{
					m_dwUnicodeFile  = TEXTIO_ANSI;
					fsetpos(m_pStream, &m_fposition);
					return CStdioFile::ReadString(rString);
				}
			}

			if (IsFileUnicode() || IsFileUnicodeBE())
			{
				for (i=0; i<nLen; i++)
				{
					if (IsFileUnicodeBE())
					{
						WORD c = (BYTE)lpsz[i];
						lpsz[i] >>= 8;
						c >>= 8;
						lpsz[i] |= c;
					}
					if (   (lpsz[i] == L'\r')
						|| (lpsz[i] == L'\n')
						|| (lpsz[i] == 0x0a0d)
						)
					{
						nLen = i;
						if (lpsz[i+1] == '\n') m_fposition += 2;
						if (bFirst)            m_fposition += 2;
						if (lpsz[i] == 0x0a0d) m_fposition ++;
						rString.GetBufferSetLength(nPartLen+i);
						m_fposition += (++i)*2;
						fsetpos(m_pStream, &m_fposition);
						nLen = -1;
						break;
					}
				} // for
				
				if (i==nLen)
				{
					rString.GetBufferSetLength(nPartLen+i);
					m_fposition += i*2;
					if (bFirst) m_fposition += 2;
				}
			}
			else if (IsFileMCBS())
			{
				char *lpcsz = (char*)lpsz;
				for (i=0; i<nLen; i++)
				{
					if (  (lpcsz[i] == '\r')
						|| (lpcsz[i] == '\n'))
					{
						CString sLine;
						if (i == 0) break;
						lpcsz[i] = 0;
						m_fposition++;											// \r oder \n
						m_fposition += i;									   // Zeichen in der Zeile
						if (((i+1) < nLen)									// ist noch ein weiteres Zeichen in dem Buffer ?
							 && (lpcsz[i+1] == '\n'))						// ist es ein \n
						{
							m_fposition++;										// weiterspringen
						}

						nLen = nPartLen*2+i;
						LPTSTR pstr = sLine.GetBufferSetLength(nLen);
//						nLen = MultiByteToWideChar(LOWORD(m_dwUnicodeFile), 0, (const char*)LPCTSTR(rString), nLen, pstr, nLen);
						nLen = MultiByteToWideChar(LOWORD(m_dwUnicodeFile), MB_PRECOMPOSED, lpcsz, nLen, pstr, nLen);
						sLine.ReleaseBuffer(nLen);
						rString.ReleaseBuffer(1);
						
						fsetpos(m_pStream, &m_fposition);

						rString = sLine;
						lpsz    = rString.GetBuffer(0);
						nLen    = -1;
						break;
					}
				} // for
				if (i==0)
				{
					m_fposition++;
					fsetpos(m_pStream, &m_fposition);
					continue;
				}
				else if (i==nLen)
				{
					m_fposition += i;
					if (bFirst) m_fposition += 2;
					nLen = nMaxSize+nPartLen;
					rString.ReleaseBuffer(nLen);
				}
			}

			// handle error/eof case
			if (lpszResult == NULL && !feof(m_pStream))
			{
				clearerr(m_pStream);
				AfxThrowFileException(CFileException::generic, _doserrno,
					m_strFileName);
			}

			// if string is read completely or EOF
			if (IsFileMCBS())
			{
				if (nLen == -1) break;
			}
			else
			{
				rString.ReleaseBuffer();
				if (nLen == -1) break;
				nLen = rString.GetLength();
			}
			lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;
			nPartLen += nMaxSize;
		} // for (;;)

		// remove '\n' from end of string if present
		lpsz = rString.GetBuffer(0);
		nLen = rString.GetLength();
		if (nLen != 0 && lpsz[nLen-1] == '\n')
			rString.GetBufferSetLength(nLen-1);

		return lpszResult != NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStdioFileU::WriteString(LPCTSTR lpsz)
{
	if (IsFileAnsi())
	{
		CStdioFile::WriteString(lpsz);
	}
	else if (IsFileUnicode())
	{
		if (m_fposition==0)
		{
			CFile::Write(&m_dwUnicodeFile, sizeof(WORD));
			m_fposition = 1;
		}
		bool bReplace = false;
		int nLen = wcslen(lpsz);

		if (lpsz[nLen-1] == '\n')
		{
			nLen--;
			bReplace = true;
		}
		CFile::Write(lpsz, nLen* sizeof(_TCHAR));

		if (bReplace)
		{
			DWORD dwCRLF = 0x000a000d;
			CFile::Write(&dwCRLF, sizeof(DWORD));
		}
	}
	else if (IsFileMCBS())
	{
		bool bReplace = false;
		int nLen  = wcslen(lpsz);
		if (lpsz[nLen-1] == '\n')
		{
			nLen--;
			bReplace = true;
		}
		char*pszC = (char*) _alloca(nLen*2+4);
		nLen = WideCharToMultiByte(LOWORD(m_dwUnicodeFile), 0, lpsz, nLen, pszC, nLen*2, NULL, NULL);
		pszC[nLen]   = 0;
		CFile::Write(pszC, nLen);
		if (bReplace)
		{
			CFile::Write("\r\n", 2);
		}
	}
}
//class CStdioFile
/////////////////////////////////////////////////////////////////////////////
#endif // _UNIDODE
/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
CString Enum2Name(enumRSType rst)
{
	CString sRet;
	switch (rst)
	{
	case RST_DEFAULT:
		sRet = _T("DEFAULT");
		break;
	case RST_MENUITEM:
		sRet = _T("MENUITEM");
		break;
	case RST_POPUP:
		sRet = _T("POPUP");
		break;
	case RST_CAPTION:
		sRet = _T("CAPTION");
		break;
	case RST_LTEXT:
		sRet = _T("LTEXT");
		break;
	case RST_CTEXT:
		sRet = _T("CTEXT");
		break;
	case RST_RTEXT:
		sRet = _T("RTEXT");
		break;
	case RST_DEFPUSHBUTTON:
		sRet = _T("DEFPUSHBUTTON");
		break;
	case RST_PUSHBUTTON:
		sRet = _T("PUSHBUTTON");
		break;
	case RST_GROUPBOX:
		sRet = _T("GROUPBOX");
		break;
	case RST_CONTROL:
		sRet = _T("CONTROL");
		break;
	case RST_PUSHBOX:
		sRet = _T("PUSHBOX");
		break;
	case RST_SCROLLBAR:
		sRet = _T("SCROLLBAR");
		break;
	case RST_STATE3:
		sRet = _T("STATE3");
		break;
	case RST_STRING:
		sRet = _T("STRING");
		break;
	default:
		sRet = _T("ERROR");
		break;
	}
	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
enumRSType Name2Enum(const CString& sName)
{
	if (sName==_T("DEFAULT"))
		return RST_DEFAULT;
	if (sName==_T("MENUITEM"))
		return RST_MENUITEM;
	if (sName==_T("POPUP"))
		return RST_POPUP;
	if (sName==_T("CAPTION"))
		return RST_CAPTION;
	if (sName==_T("LTEXT"))
		return RST_LTEXT;
	if (sName==_T("CTEXT"))
		return RST_CTEXT;
	if (sName==_T("RTEXT"))
		return RST_RTEXT;
	if (sName==_T("DEFPUSHBUTTON"))
		return RST_DEFPUSHBUTTON;
	if (sName==_T("PUSHBUTTON"))
		return RST_PUSHBUTTON;
	if (sName==_T("GROUPBOX"))
		return RST_GROUPBOX;
	if (sName==_T("CONTROL"))
		return RST_CONTROL;
	if (sName==_T("PUSHBOX"))
		return RST_PUSHBOX;
	if (sName==_T("SCROLLBAR"))
		return RST_SCROLLBAR;
	if (sName==_T("STATE3"))
		return RST_STATE3;
	if (sName==_T("STRING"))
		return RST_STRING;

	return RST_DEFAULT;
}
/////////////////////////////////////////////////////////////////////////////
CString GetLastErrorString(LONG uOsError)
{
	CString sRet;
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		uOsError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	sRet.Format(_T("%d,%s"),uOsError,lpMsgBuf);

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
_TCHAR CTranslateString::m_cDelimiter = '@';
BOOL	 CTranslateString::gm_bReplace  = TRUE;
HWND   CTranslateString::gm_hProgress = NULL;
/////////////////////////////////////////////////////////////////////////////
CTranslateString::CTranslateString()
{
//	CString	m_sResourceFile;
//	CString m_sText;
//	CString m_sTranslate;
	m_uMaxLength = 0;
//	CString m_sComment
//	CString m_sResourceID;
	m_rst = RST_DEFAULT;

//	CString m_sDialogID;
//	CString m_sMenuID;
//	CString m_sPopup;

//	CTime	m_tScan;
//	CTime	m_tTranslate;
//	CTime	m_tInsert;
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString::CTranslateString(const CString& sResourceFile,
								   const CString& sID,
								   const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_sText = sText;
	if (CheckTODO())
	{
		m_sTranslate = szTODO + sText;
	}
	else
	{
		m_sTranslate = sText;
	}
	m_uMaxLength = 0;
//	CString m_sComment

	m_sResourceID = sID;
	m_rst = RST_STRING;

//	m_tScan = CTime::GetCurrentTime();
//	m_tTranslate = m_tScan;
//	m_tInsert = m_tScan;

	m_sFullID = CalcFullID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
// constructor for dialog control
CTranslateString::CTranslateString(enumRSType rst,
								   const CString& sResourceFile,
								   const CString& sDialogID,
								   const CString& sControlID,
								   const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_sText = sText;
	if (CheckTODO())
	{
		m_sTranslate = szTODO + sText;
	}
	else
	{
		m_sTranslate = sText;
	}
	m_uMaxLength = 0;
//	CString m_sComment

	m_sResourceID = sControlID;
	m_rst = rst;

	m_sDialogID = sDialogID;

//	m_tScan = CTime::GetCurrentTime();
//	m_tTranslate = m_tScan;
//	m_tInsert = m_tScan;

	m_sFullID = CalcFullID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
// constructor for menu
CTranslateString::CTranslateString(enumRSType	rst,
									 const CString& sResourceFile,
									 const CString& sMenuID,
									 int   iPopup,
									 const CString& sItemID,
									 const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_sText = sText;
	if (CheckTODO())
	{
		m_sTranslate = szTODO + sText;
	}
	else
	{
		m_sTranslate = sText;
	}
	m_uMaxLength = 0;
//	CString m_sComment

	m_sResourceID = sItemID;
	m_rst = rst;

	m_sMenuID = sMenuID;
	m_sPopup.Format(_T("POPUP%d"),iPopup);

//	m_tScan = CTime::GetCurrentTime();
//	m_tTranslate = CTime(1980,1,1,1,1,1);
//	m_tInsert = m_tTranslate;

	m_sFullID = CalcFullID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString::~CTranslateString()
{
}
/////////////////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckDataAll(CString& sOriginal, CString& sTranslation, UINT& uMaxLength) 
{
	BOOL bReturn = TRUE;
	CString sStartChars;
	if (!CheckFormatSpecifier(sOriginal, sTranslation))
	{
		bReturn = FALSE;
	}
	else if (!CheckSpecialStartChars(sOriginal, sTranslation, sStartChars))
	{
		bReturn = FALSE;
	}
	else if (!CheckMaxLength(sTranslation, uMaxLength))
	{
		bReturn = FALSE;
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CTranslateString
 Function   : CheckFormatSpecifier
 Description: Checks translated text for matching format specifiers

 Parameters:   
  sOriginal   : (E): Original text  (CString&)
  sTranslation: (E): Translated text  (CString&)

 Result type: Translation valid or not (BOOL)
 Author: GF
 created: June, 05 2002
 <TITLE CheckFormatSpecifier>
 <AUTOLINK ON>
 <IMPLEMENTATION ON>
 <KEYWORDS format specifiers>
*********************************************************************************************/
BOOL CTranslateString::CheckFormatSpecifier(CString& sOriginal, CString& sTranslation) 
{
	BOOL bReturn   = TRUE;
	BOOL bPermutable = FALSE, bProbablyPermutable = FALSE;
	int iStart     = sOriginal.Find('%');
	int iEnd       = iStart;
	int iTemp      = 0;
	int iStartLast = 0;
	CString sFirstChar;
	CString sSpecifier;
	CString sFmtSpecs		= _T("123456789aAbBcCdDEfgGHiIjmMnopsSuUwWxXyYzZ");
	CString sFmtPerms		= _T("123456789aAbBHIjmMpUwWyYzZ");
	CString sFmtProbPerms	= _T("dSxX");

	while (   (iStart != -1)
		   && (iStart < (sOriginal.GetLength()-1)) // Last character is always allowed
		   )
	{
		sFirstChar = sOriginal.GetAt(iStart+1);
		if (sFirstChar != '%')
		{
			sSpecifier = sOriginal.Mid(iStart);
			iTemp = sSpecifier.FindOneOf(sFmtSpecs);
			if (iTemp != -1)
			{
				iEnd = iStart + iTemp;
				sSpecifier = sOriginal.Mid(iStart, iTemp+1);
				if (!bPermutable)
				{
					iTemp = sSpecifier.FindOneOf(sFmtPerms);
					if (iTemp != -1)				// permutable
					{
						bPermutable = TRUE;
					}
					else
					{
						iTemp = sSpecifier.FindOneOf(sFmtProbPerms);
						if (iTemp != -1)			// probably permutable
						{
							bProbablyPermutable = TRUE;
						}
					}
				}
				iTemp = sTranslation.Find(sSpecifier, iStartLast);
				if(iTemp != -1)
				{
					iStartLast = iTemp+1;
				}
				else
				{
					// something differs...
					// maybe it is really just a percent sign...
					/// ...followed by blank or punctation mark
					if (sFirstChar.FindOneOf(_T(" .!?,;:]")) != -1)
					{
						// assumed as ok...
						// missing errors:
						//    BLANK as flag
						//    PRECISION without prior width
						iEnd = iStart + 1;
					}
					else
					{
						if (bPermutable)			// permutable format sepcifiers
						{							// %1 - %9 or date time fs
							iTemp = sTranslation.Find(sSpecifier);	// search from beginning
							if (iTemp == -1)		// not found
							{
								bReturn = FALSE;	// FALSE is now default
								iTemp = sSpecifier.FindOneOf(_T("awyz"));
								if (iTemp != -1)	// fs may be upper case
								{
									sSpecifier.MakeUpper();
									iTemp = sTranslation.Find(sSpecifier);
									if (iTemp != -1)
									{
										bReturn = TRUE;
									}
								}

								iTemp = bReturn ? -1 : sSpecifier.FindOneOf(_T("AWYZ"));
								if (iTemp != -1)// fs may be lower case
								{
									sSpecifier.MakeLower();
									iTemp = sTranslation.Find(sSpecifier);
									if (iTemp != -1)
									{
										bReturn = TRUE;
									}
								}
								CString sMfs = _T("bBm");
								iTemp = bReturn ? -1 : sSpecifier.FindOneOf(sMfs);
								if (iTemp != -1)// fs may be a month fs
								{
									int i, iT2 = sMfs.Find(sSpecifier[iTemp]);
									for (i=0; i<3; i++)
									{
										if (i != iT2)
										{
											sSpecifier.SetAt(1, sMfs[i]);
											iTemp = sTranslation.Find(sSpecifier);
											if (iTemp != -1)
											{
												bReturn = TRUE;
												break;
											}
										}
									}
								}
								if (!bReturn)
								{
									break;
								}
							}
						}
						else if (bProbablyPermutable)
						{
							bProbablyPermutable++;
							iTemp = sTranslation.Find(sSpecifier);	// search from beginning
							if (iTemp == -1)		// not found
							{
								bReturn = FALSE;	// FALSE is now default
								break;
							}
						}
						else
						{
							// error
							bReturn = FALSE;
							break;
						}
					}
				}
			}
			else
			{
				iEnd = iStart + 1;
			}
		}
		else
		{
			iEnd = iStart + 1;
		}
		iStart = sOriginal.Find('%', iEnd+1);
	}
	if (bProbablyPermutable > 1 && !bPermutable)
	{
		bReturn = FALSE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckMaxLength(CString& sTranslation, UINT uMaxLength) 
{
	BOOL bReturn = TRUE;
	if (uMaxLength > 0)
	{
		// Ignore single '&' (Accelerator) in text,
		// but consider double '&&' as one character
		CString sText = sTranslation;
		sText.Replace(_T("&&"), _T("|"));
		// simple count remaining '&'
		int iCount = sText.Replace('&', '&');

		UINT uLength = (UINT)(sText.GetLength() - iCount);
		if (uMaxLength < uLength)
		{
			bReturn = FALSE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckSpecialStartChars(CString& sOriginal, CString& sTranslation, CString& sStartChars) 
{
	BOOL bReturn = TRUE;
	if (   sOriginal.GetLength()
		&& sTranslation.GetLength()
		&& (CheckTODO(sTranslation) == TRUE) // do not check, if TODO
		)
	{

		CStringArray sArray;
		sArray.Add(_T(">"));
		int iLen;
		for (int i=0 ; i<sArray.GetSize() ; i++)
		{
			sStartChars = sArray.GetAt(i);
			iLen = sStartChars.GetLength();
			if (   sOriginal.Left(iLen) == sStartChars
				&& sTranslation.Left(iLen) != sStartChars
				)
			{
				// error
				bReturn = FALSE;
				break;
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckSpecialCharsToAvoid(CString& sOriginal, CString& sTranslation, CString& sChars) 
{
	BOOL bReturn = TRUE;
	if (   sOriginal.GetLength()
		&& sTranslation.GetLength()
		)
	{
		CStringArray sArray;
		sArray.Add('"');
		int iIndex;
		for (int i=0 ; i<sArray.GetSize() ; i++)
		{
			sChars = sArray.GetAt(i);
			iIndex = sTranslation.Find(sChars);
			if (iIndex != -1)
			{
				// error
				bReturn = FALSE;
				break;
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckTODO() 
{
	return CheckTODO(m_sTranslate) ;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckTODO(CString& sTranslation)
{
	BOOL bReturn = TRUE;
	if (sTranslation.Left(_tcsclen(szTODO)) == szTODO)
	{
		bReturn = FALSE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::Update(CTranslateString* pTS)
{
	if (m_sText != pTS->m_sText)
	{
		m_sText = pTS->m_sText;
		CountWords();
		if (CheckTODO())
		{
			m_sTranslate = szTODO + m_sTranslate;
		}
//		m_tScan = pTS->m_tScan;
	}
	if (m_uMaxLength != pTS->m_uMaxLength)
	{
		m_uMaxLength = pTS->m_uMaxLength;
	}
	if (m_sComment != pTS->m_sComment)
	{
		m_sComment = pTS->m_sComment;
	}
}
/////////////////////////////////////////////////////////////////////////////
const CString CTranslateString::CalcFullID()
{
	switch (m_rst)
	{
	case RST_STRING:
		return m_sResourceID;
		break;
	case RST_CAPTION:
		return m_sDialogID + _T(".CAPTION");
		break;
	case RST_LTEXT:	
	case RST_CTEXT:
	case RST_RTEXT:
	case RST_DEFPUSHBUTTON:
	case RST_PUSHBUTTON:
	case RST_GROUPBOX:
	case RST_CONTROL:
	case RST_PUSHBOX:
	case RST_SCROLLBAR:
	case RST_STATE3:
		return m_sDialogID + '.' + m_sResourceID;
		break;
	case RST_POPUP:
		return m_sMenuID + '.' + m_sPopup;
		break;
	case RST_MENUITEM:
		return m_sMenuID + '.' + m_sPopup + '.' + m_sResourceID;
		break;
	default:
		break;
	}

	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
const CString CTranslateString::ToString()
{
	CString sLine;
	CString sTemp;

	// resource file
	sLine = m_sResourceFile;
	sLine += m_cDelimiter;
	// resource type
	sLine += Enum2Name(m_rst);
	sLine += m_cDelimiter;
	// resource id
	sLine += GetFullID();
	sLine += m_cDelimiter;
	// scan time
// Times redundant
//	sLine += m_tScan.Format(_T("%d,%m,%Y,%H,%M,%S"));
	sLine += m_cDelimiter;
	// translate time
//	sLine += m_tTranslate.Format(_T("%d,%m,%Y,%H,%M,%S"));
	sLine += m_cDelimiter;
	// reinsert time
//	sLine += m_tInsert.Format(_T("%d,%m,%Y,%H,%M,%S"));
	sLine += m_cDelimiter;
	// test to translate
	sLine += m_sText;
	sLine += m_cDelimiter;
	// translated text
	sLine += m_sTranslate;
	sLine += m_cDelimiter;
	// maximum length
	sTemp.Format(_T("%u"), m_uMaxLength);
	sLine += sTemp;
	sLine += m_cDelimiter;
	// comment to maximum length
	sLine += m_sComment;

#ifdef _DEBUG
	static BOOL bTrace = FALSE;
	if (bTrace)
	{
		// CAVEAT Trace buffer is max 512 TCHARs!
		if (sLine.GetLength() > 500)
		{
			TRACE(_T("%s[snip]\n"), sLine.Left(500));
		}
		else
		{
			TRACE(_T("%s\n"), sLine);
		}
	}
#endif

/*
	sLine.Format(_T("%s%c%s%c%s%c%c%c%c%s%c%s%c%s%c%s"),
		m_sResourceFile , m_cDelimiter, 
		Enum2Name(m_rst), m_cDelimiter,
		GetFullID()     , m_cDelimiter,
		                  m_cDelimiter,
		                  m_cDelimiter,
		                  m_cDelimiter,
		m_sText , m_cDelimiter, 
		m_sTranslate , m_cDelimiter, 
		sTemp , m_cDelimiter, 
		m_sComment);

	// CAVEAT Trace buffer is max 512 TCHARs!
	if (sLine.GetLength() > 500)
	{
		TRACE(_T("%s[snip]\n"), sLine.Left(500));
	}
	else
	{
		TRACE(_T("%s\n"), sLine);
	}
*/
	return sLine;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::SetTranslate(const CString& sTranslate)
{
	m_sTranslate = sTranslate;
//	m_tTranslate = CTime::GetCurrentTime();
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::SetMaxLength(UINT uMaxLength)
{
	m_uMaxLength = uMaxLength;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::SetComment(const CString& sComment)
{
	m_sComment = sComment;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::FromString(const CString& str)
{
	CString s = str;
	CString id,temp;
	int p;
//	int day,month,year,hour,minute,second;

	// resource file
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_sResourceFile = s.Left(p);
	s = s.Mid(p+1);

	// resource type
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_rst = Name2Enum(s.Left(p));
	s = s.Mid(p+1);

	// resource id
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	id = s.Left(p);
	if (!IdFrom(id))
	{
		return FALSE;
	}
	s = s.Mid(p+1);
	// scan time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
//	temp = s.Left(p);
//	sscanf(temp,_T("%d,%d,%d,%d,%d,%d"),&day,&month,&year,&hour,&minute,&second);
//	m_tScan = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// translate time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
//	temp = s.Left(p);
//	sscanf(temp,_T("%d,%d,%d,%d,%d,%d"),&day,&month,&year,&hour,&minute,&second);
//	m_tTranslate = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// reinsert time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
//	temp = s.Left(p);
//	sscanf(temp,_T("%d,%d,%d,%d,%d,%d"),&day,&month,&year,&hour,&minute,&second);
//	m_tInsert = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// text to translate
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_sText = s.Left(p);
	s = s.Mid(p+1);
	// translated text
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		// old format
		m_sTranslate = s;
		m_uMaxLength = 0;
		m_sComment = _T("");
	}
	else
	{
		// New format with maximum length and comment
		m_sTranslate = s.Left(p);
		s = s.Mid(p+1);
		// maximum length
		p = s.Find(m_cDelimiter);
		if (p==-1)
		{
			return FALSE;
		}
		m_uMaxLength = _ttoi(s.Left(p));
		s = s.Mid(p+1);
		m_sComment = s;
	}

	m_sFullID = CalcFullID();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::IdFrom(const CString& id)
{
	int p;
	CString sid=id;

	switch (m_rst)
	{
	case RST_STRING:
		m_sResourceID = sid;
		break;
	case RST_CAPTION:
		p = sid.Find(_T(".CAPTION"));
		if (p==-1)
		{
			return FALSE;
		}
		m_sDialogID = sid.Left(p);
		break;
	case RST_LTEXT:	
	case RST_CTEXT:
	case RST_RTEXT:
	case RST_DEFPUSHBUTTON:
	case RST_PUSHBUTTON:
	case RST_GROUPBOX:
	case RST_CONTROL:
	case RST_PUSHBOX:
	case RST_SCROLLBAR:
	case RST_STATE3:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sDialogID = sid.Left(p);
		m_sResourceID = sid.Mid(p+1);
		break;
	case RST_POPUP:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sMenuID = sid.Left(p);
		m_sPopup = sid.Mid(p+1);
		break;
	case RST_MENUITEM:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sMenuID = sid.Left(p);
		sid = sid.Mid(p+1);
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sPopup = sid.Left(p);
		m_sResourceID = sid.Mid(p+1);
		break;
	default:
		break;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::AutomaticTranslate(const CMapStringToString& mapAutomatic)
{
	BOOL bReturn = FALSE;
	if ( (m_sText.GetLength()<2) ||
		 (m_sText.SpanIncluding(_T("0123456789"))==m_sText) ||
		 (m_sText.SpanIncluding(_T("<>,.: +-*/\\!\"\'§$%&()=?[]{}#_;"))==m_sText) ||
		 (m_sText.SpanIncluding(_T(" "))==m_sText) ||
		 (m_sText == _T("List1")) ||
		 (m_sText == _T("Progress1")) ||
		 (m_sText == _T("Slider1")) ||
		 (m_sText == _T("DateTimePicker1"))
	   )
	{
		if (m_sTranslate != m_sText)
		{
			m_sTranslate = m_sText;
//			m_tTranslate = CTime::GetCurrentTime();
			bReturn = TRUE;
		}
	}
	else
	{
		BOOL bTodo  = (0==m_sTranslate.Find(szTODO)) ? TRUE:FALSE;
		BOOL bEmpty = m_sTranslate.IsEmpty();
		if (!gm_bReplace || bEmpty || bTodo)
		{
			CString sValue;
			BOOL bFound = FALSE;
			bFound = mapAutomatic.Lookup(m_sText,sValue);
			// Identic text found ?
			if (!bFound)
			{
				// Try to find similar text
				// e.g. without menu accelerator, '&', '...', '.' and others?
				CString sText = m_sText;
				int iIndex = -1;
				sText.TrimLeft(' ');
				sText.TrimRight(' ');
				bFound = mapAutomatic.Lookup(sText, sValue);

				if (!bFound)
				{
					iIndex = sText.Find(_T("\\t"));
					if (iIndex != -1)
					{
						sText = sText.Left(iIndex);
						bFound = mapAutomatic.Lookup(sText, sValue);
					}
				}
				if (!bFound)
				{
					if (sText.Right(3) == _T("..."))
					{
						sText = sText.Left(sText.GetLength()-3);
						bFound = mapAutomatic.Lookup(sText, sValue);
					}
				}
				if (!bFound)
				{
					if (sText.Right(1) == _T("."))
					{
						sText = sText.Left(sText.GetLength()-1);
						bFound = mapAutomatic.Lookup(sText, sValue);
					}
				}
				if (!bFound)
				{
					iIndex = sText.Find('&');
					if (iIndex != -1)
					{
						sText = sText.Left(iIndex)+sText.Mid(iIndex+1);
						bFound = mapAutomatic.Lookup(sText, sValue);
						if (!bFound)
						{
							// any other regular expression possible?
						}
					}
				}
				if (bFound)
				{
					// Place a TODO! in front
					if (!gm_bReplace)
					{
//						if (bTodo) sValue = szTODO + sValue;
					}
					else
					{
						bTodo = bEmpty = FALSE;
					}
				}
			}
			else if (gm_bReplace)
			{
				bTodo = bEmpty = FALSE;
			}
			if (bFound)
			{
				if (bTodo)
				{
					m_sSuggestion = sValue;
				}
				else if (bEmpty)
				{
					m_sTranslate = szTODO + sValue;
				}
				else if (m_sTranslate != sValue)
				{
					m_sTranslate = sValue;
					// CAVEAT: Text and translation can be very long...
					if (m_sText.GetLength() + m_sTranslate.GetLength() > 490)
					{
						TRACE(_T("AUTO <%s...> -> <%s...>\n"), m_sText.Left(240),m_sTranslate.Left(240));
					}
					else
					{
						TRACE(_T("AUTO <%s> -> <%s>\n"),m_sText,m_sTranslate);
					}
//					m_tTranslate = CTime::GetCurrentTime();
//					bReturn = TRUE;
				}
				else if (!gm_bReplace)
				{
					m_sSuggestion = sValue;
				}
				bReturn = TRUE;
			}
		}
	}
//	Sleep(0);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::CountWords()
{
	m_iNumOfWords = 0;
	int i;
	BOOL bDeli;

	bDeli = FALSE;

	for (i=0;i<m_sText.GetLength();i++)
	{
		if (isalpha(m_sText[i])!=0)
		{
			if (!bDeli)
			{
				m_iNumOfWords++;
			}
			bDeli = TRUE;
		}
		else
		{
			bDeli = FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::CheckData()
{
	return CheckDataAll(m_sText, m_sTranslate, m_uMaxLength);
}
/////////////////////////////////////////////////////////////////////////////
const CString CTranslateString::GetTranslate(BOOL bIgnoreTODO /*= FALSE*/) const
{
	CString sReturn;
	sReturn = m_sTranslate;
	if (   bIgnoreTODO
		&& (sReturn.Left(_tcsclen(szTODO)) == szTODO)
		)
	{
		sReturn = sReturn.Mid(_tcsclen(szTODO));
	}
	return sReturn;
}
const CString CTranslateString::GetSuggestion()
{
	return m_sSuggestion;
}

/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
CTSArray::CTSArray()
{
	m_iNumOfWords = 0;
}
/////////////////////////////////////////////////////////////////////////////
CTSArray::~CTSArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CTSArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAt(i)) 
		{ 
			delete (CTranslateString*) GetAt(i); 
		} 
	} 
	RemoveAll(); 
	m_iNumOfWords = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::ExportAsGlossary(const CString& sPathName, BOOL bGerman)
{
#ifdef _UNICODE
	CStdioFileU file;
#ifndef _CONSOLE
	if (theApp.m_wCodePage == CP_UNICODE)
	{
		file.SetFileUnicode();
	}
	else
	{
		file.SetFileMCBS(theApp.m_wCodePage);
	}
#endif
#else
//	CFile file;
	CStdioFile file;
#endif
	CFileException cfe;

	TRACE(_T("exporting glossary %s\n"), LPCTSTR(sPathName));
	if (file.Open(sPathName,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive/*|CFile::modeNoTruncate*/ ,&cfe))
	{
//		file.SeekToEnd();

		CString sLine, sFirstEntry, sSecondEntry, sLength, sComment;
		CTranslateString* pTS = NULL;
		for (int i=0;i<GetSize();i++)
		{
			pTS = GetAt(i);
			// do not save TODOs as Glossary
			if (pTS->CheckTODO())
			{
				if (bGerman)
				{
					sFirstEntry		= pTS->GetTranslate();
					sSecondEntry	= pTS->GetText();
				}
				else
				{
					sSecondEntry	= pTS->GetTranslate();
					sFirstEntry		= pTS->GetText();
				}
				if (-1!=sFirstEntry.Find(','))
				{
					sFirstEntry = '\"' + sFirstEntry + '\"';
				}
				if (-1!=sSecondEntry.Find(','))
				{
					sSecondEntry = '\"' + sSecondEntry + '\"';
				}
				sLength.Format(_T("%u"), pTS->GetMaxLength());
				sComment = pTS->GetComment();
				sLine =   sFirstEntry
						+ _T(",,")
						+ sSecondEntry
						+ _T(",,,,,,,,,")
						+ sLength
						+ _T(",")
						+ sComment
						+ _T(",\n");
				// Achtung abspeichern in Unicode im Clossar ?
//				file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
				file.WriteString(LPCTSTR(sLine));
			}
		} // end for
		file.Flush();
		file.Close();
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDP_SAVE_FILE_FAILED, sPathName);
		sMsg += _T("\n") + GetLastErrorString(cfe.m_cause);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::ExportAsTextOnly(const CString& sPathName)
{
#ifdef _UNICODE
	CStdioFileU file;
#ifndef _CONSOLE
	if (theApp.m_wCodePage == CP_UNICODE)
	{
		file.SetFileUnicode();
	}
	else
	{
		file.SetFileMCBS(theApp.m_wCodePage);
	}
#endif
#else
	CStdioFile file;
#endif
	CFileException cfe;

	TRACE(_T("exporting as text only %s\n"),LPCTSTR(sPathName));
	if (file.Open(sPathName,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive ,&cfe))
	{
		file.SeekToEnd();

		CString sLine;
		for (int i=0;i<GetSize();i++)
		{
			// ** Dateizugriff
			sLine.Format(_T("%s%s\n"), szOriginal, GetAt(i)->GetText());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine.Format(_T("%s%s\n"), szTranslation, GetAt(i)->GetTranslate());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine =      _T("|\n");
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
		}
//		file.Flush();
		file.Close();
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDP_SAVE_FILE_FAILED, sPathName);
		sMsg += _T("\n") + GetLastErrorString(cfe.m_cause);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::ExportAsOriginalLengthComment(const CString& sPathName)
{
#ifdef _UNICODE
	CStdioFileU file;
#ifndef _CONSOLE
	if (theApp.m_wCodePage == CP_UNICODE)
	{
		file.SetFileUnicode();
	}
	else
	{
		file.SetFileMCBS(theApp.m_wCodePage);
	}
#endif
#else
	CStdioFile file;
#endif
	CFileException cfe;

	TRACE(_T("exporting as translation, length and comment only %s\n"),LPCTSTR(sPathName));
	if (file.Open(sPathName,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive ,&cfe))
	{
		file.SeekToEnd();

		CString sLine;
		for (int i=0;i<GetSize();i++)
		{
			// ** Dateizugriff
			sLine.Format(_T("%s%s\n"), szOriginal, GetAt(i)->GetText());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine.Format(_T("%s%s\n"), szTranslation, GetAt(i)->GetTranslate());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine.Format(_T("Max. length : %u chars\n"), GetAt(i)->GetMaxLength());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine.Format(_T("Comment     : %s\n"), GetAt(i)->GetComment());
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
			sLine =      _T("|\n");
//			file.Write(LPCTSTR(sLine), sLine.GetLength()*sizeof(_TCHAR));
			file.WriteString(sLine);
		}
//		file.Flush();
		file.Close();
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDP_SAVE_FILE_FAILED, sPathName);
		sMsg += _T("\n") + GetLastErrorString(cfe.m_cause);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::ImportFromTxtFile(const CString& sPathName)
{
#ifdef _UNICODE
	CStdioFileU file;
#else
	CStdioFile file;
#endif
	CFileException cfe;

	TRACE(_T("importing from text file %s\n"), LPCTSTR(sPathName));
	if (file.Open(sPathName,CFile::modeRead|CFile::shareDenyWrite ,&cfe))
	{
		CString sLine;
		CTranslateString* pTS;

		DeleteAll();

		while (file.ReadString(sLine))
		{

			pTS = new CTranslateString();
			if (pTS->FromString(sLine))
			{
				Add(pTS);
			}
			else
			{
				// CAVEAT Trace buffer is max 512 TCHARs!
				if (sLine.GetLength() > 480)
				{
					TRACE(_T("FromString Failed %s[snip]\n"), sLine.Left(480));
				}
				else
				{
					TRACE(_T("FromString Failed %s\n"), sLine);
				}
				delete pTS;
			}
		}
		file.Close();
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDP_FILE_OPEN_FAILED, sPathName);
		sMsg += _T("\n") + GetLastErrorString(cfe.m_cause);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::Save(const CString& sFilename)
{
	CFile file;
	CFileException cfe;
	CString sLine;
	int i, nLen; 
	BYTE *pstr;

	TRACE(_T("saving file %s\n"), LPCTSTR(sFilename));
	if (file.Open(sFilename,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		TRACE(_T("saving %d lines\n"), GetSize());
		for (i=0;i<GetSize();i++)
		{
			sLine = GetAt(i)->ToString() + _T("\n");
#ifdef _DEBUG
			// CAVEAT Trace buffer is max 512 TCHARs!
			static BOOL bTrace = FALSE;
			if (bTrace)
			{
				if (sLine.GetLength() > 500)
				{
					TRACE(_T("%s[snip]\n"), LPCTSTR(sLine.Left(500)));
				}
				else
				{
					TRACE(_T("%s\n"), LPCTSTR(sLine));
				}
			}
#endif
			nLen = sLine.GetLength();
			pstr  = (BYTE*)sLine.GetBuffer(nLen);
#ifdef _UNICODE
			nLen *= sizeof(_TCHAR);
			if (i==0)
			{
				WORD wStart = TEXTIO_UNICODE;
				file.Write(&wStart, sizeof(WORD));
			}
#endif
			file.Write(pstr, nLen);
		}
		file.Close();
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDP_SAVE_FILE_FAILED, sFilename);
		sMsg += _T("\n") + GetLastErrorString(cfe.m_cause);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CTSArray
 Function   : Load
 Description: Loads the CTSArray from a LNG file

 Parameters:   
  sFilename: (E): File name  (const CString&)

 Result type:  1: ok, 0: File not found, -1: File not ANSI (int)
 created: July, 31 2003
 <TITLE Load>
*********************************************************************************************/
int CTSArray::Load(const CString& sFilename)
{
#ifdef _UNICODE
	CStdioFileU file;
#else
	CStdioFile file;
#define TEXTIO_UNICODE_BIGENDIAN 0x0000fffe
#define TEXTIO_UNICODE           0x0000feff
#define TEXTIO_UNICODE_UTF8      0x00efbbbf
#endif
	CFileException cfe;
	CString sLine;
	CTranslateString* pTS;
	int p;

	DeleteAll();

	p = sFilename.ReverseFind('\\');
	m_sName = sFilename.Mid(p+1);
	TRACE(_T("loading file %s\n"), LPCTSTR(sFilename));

	if (!file.Open(sFilename,CFile::modeRead,&cfe))
	{
		return 0;
	}

	int nLine=0;
	while (file.ReadString(sLine))
	{
#ifndef _UNICODE
		if (nLine == 0)
		{
			BOOL bTest = TRUE;
			DWORD *pdwTag = (DWORD*)LPCTSTR(sLine);
			switch ((WORD)*pdwTag)
			{
			case (WORD)TEXTIO_UNICODE:
			case (WORD)TEXTIO_UNICODE_BIGENDIAN:
				bTest = FALSE;
				break;
			case 0xbbbf: //(WORD)TEXTIO_UNICODE_UTF8:
				if (sLine.GetAt(2) == 0xef)
				{
					bTest = FALSE;
				}
				break;
			}
			if (bTest == FALSE)
			{
				return -1;
			}
		}
#endif
		pTS = new CTranslateString();
		if (pTS->FromString(sLine))
		{
			Add(pTS);
		}
		else
		{
			// CAVEAT Trace buffer is max 512 TCHARs!
			if (sLine.GetLength() > 480)
			{
				TRACE(_T("FromString(%d) Failed %s[snip]\n"), nLine, LPCTSTR(sLine.Left(480)));
			}
			else
			{
				TRACE(_T("FromString(%d) Failed %s\n"), nLine, LPCTSTR(sLine));
			}
			delete pTS;
		}
		nLine++;
	}
	file.Close();
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CTSArray::AddReplace(CTranslateString* pTSNew)
{
	m_iNumOfWords += pTSNew->GetNumOfWords();

	BOOL bFound = FALSE;

	CTranslateString* pTSOld = NULL;
	for (int i=0 ; !bFound && i<GetSize() ; i++)
	{
		pTSOld = GetAt(i);
		if (pTSNew->GetFullID() == pTSOld->GetFullID())
		{
			bFound = TRUE;
		}
	}
	if (bFound)
	{
		pTSOld->Update(pTSNew);
		delete pTSNew;
	}
	else
	{
		Add(pTSNew);
	}
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString* CTSArray::GetTSFromFullID(const CString& id)
{
	int i;

	for (i=0;i<GetSize();i++)
	{
		if (id== GetAt(i)->GetFullID())
		{
			return GetAt(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString* CTSArray::GetTSFromResourceID(const CString& id)
{
	int i;

	for (i=0;i<GetSize();i++)
	{
		if (id== GetAt(i)->GetResourceID())
		{
			return GetAt(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::AutomaticTranslate(const CMapStringToString& mapAutomatic)
{
	BOOL bReturn = 0;

	for (int i=0;i<GetSize();i++)
	{
		if (GetAt(i)->AutomaticTranslate(mapAutomatic))
		{
			bReturn++;
			if (CTranslateString::gm_hProgress)
			{
				CString str;
				str.Format(_T("%d"), bReturn);
				::SetWindowText(CTranslateString::gm_hProgress, str);
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::CheckData()
{
	BOOL bReturn = TRUE;

	for (int i=0 ; bReturn && i<GetSize() ; i++)
	{
//		TRACE(_T("%d\n"), i);
//		Sleep(0);
		bReturn = GetAt(i)->CheckData();
	}
	return bReturn;
}

