// Parser.cpp

#include "stdafx.h"

#include "resource.h"

#include "Scanner.h"
#include "TranslateString.h"
#include "Parser.h"




/////////////////////////////////////////////////////////////////////////////
CParser::CParser(CScanner* pScanner, CTSArray* pTSArray, CTSArray* pTSArrayEnu, BOOL bCL)
{	// constructor for extracting the resource strings
	m_pScanner		= pScanner;
	m_pTSArray		= pTSArray;
	m_pTSArrayEnu	= pTSArrayEnu;
	m_bCommandLine	= bCL;
//	m_sDestRCFile;
}
/////////////////////////////////////////////////////////////////////////////
CParser::CParser(CScanner* pScannerSource, CTSArray* pTSArrayReplace, 
				const CString& destinationFile, BOOL bCL)
{	// constructor for inserting the resource strings
	m_pScanner		= pScannerSource;
	m_pTSArray		= pTSArrayReplace;
	m_pTSArrayEnu	= NULL;
	m_bCommandLine	= bCL;
	m_sDestRCFile	= destinationFile;
}
/////////////////////////////////////////////////////////////////////////////
CParser::~CParser()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CParser::Parse(BOOL bIgnoreTODO /*= FALSE*/)
{
	// wenn m_sDestRCFile leer ist, dann extrahieren, sonst ersetzen
	CToken* pToken;
	CToken* pPrevToken;
	enumCondition zustand = C_READ;
	enumRSType rst = RST_DEFAULT;

	CString sStringID, sValue, sDialogID, sMenuID;
	int popupcounter = 0;
	int iPopupBeginCounter = 0;

	CFile file;
	CFileException cfe;
	BOOL bWriteRC = m_sDestRCFile.IsEmpty()==FALSE;
	BOOL bReplaceToken = FALSE;
	BOOL bWriteRCLine = bWriteRC;
	BOOL bAddReplaceToTSArray = FALSE;

	CTranslateString* pTSRead = NULL;
	CTranslateString* pTSEnu = NULL;

	CString sPreBuffer;
	CString sPostBuffer;


/*
	//TODO RKE/GF: CONTROLS mit diesen ClassNames müssen nicht übersetzt werden !
	TCHAR *pszClassNames[] =
	{
		_T("msctls_trackbar32"),
		_T("msctls_updown32"),
		_T("msctls_progress32"),
		_T("msctls_hotkey32"),
		_T("SysListView32"),
		_T("SysTreeView32"),
		_T("SysTabControl32"),
		_T("SysAnimate32"),
		_T("SysDateTimePick32"),
		_T("SysMonthCal32"),
		_T("SysIPAddress32"),
		NULL
	};

	case S_STRING_LITERAL:
	{
		CString str = pToken->GetOriginal();
		int i=0;
		while (pszClassNames[i])
		{
			if (str == pszClassNames[i])
			{
				pTSRead->SetComment(str);
				bAddReplaceToTSArray = TRUE;
				break; 
			}
			i++;
		}
	}break;
*/
	// open destination file if replacing
	if (bWriteRC)
	{
		if (file.Open(m_sDestRCFile,CFile::modeCreate|CFile::modeWrite,&cfe))
		{
#ifdef _UNICODE
			WORD dwPre = TEXTIO_UNICODE;
			file.Write(&dwPre, sizeof(WORD));
#endif			
		}
		else
		{
			return FALSE;
		}

		// close file will be called from destructor of CFile
		// so we can return every time :-)
	}


	while ((pToken = m_pScanner->NextToken()) != NULL)
	{
		pTSRead = NULL;
		// Original may be VERY long, TRACE will assert at expanded length >512
		TRACE(_T("%s,%s\n"), pToken->GetSymbolName(), pToken->GetOriginal().Left(450));
		switch (zustand)
		{
		// Lesen ---------------------------------------------
		case C_READ:
			switch (pToken->GetSymbol())
			{
				case R_STRINGTABLE:
					zustand = C_STRINGTABLE;
					break;
				case R_DIALOG:
				case R_DIALOGEX:
					zustand = C_DIALOG;
					break;
				case R_MENU:
					zustand = C_MENU;
					break;
				default:
					zustand = C_READ;
					break;
			}
			break;

		// StringTable ---------------------------------------
		case C_STRINGTABLE:
			if (pToken->GetSymbol()==R_BEGIN)
			{
				zustand = C_STRINGTABLE_BEGIN;
			}
			break;
		case C_STRINGTABLE_BEGIN:
			switch (pToken->GetSymbol())
			{
				case S_IDENTIFIER:
					sStringID = pToken->GetOriginal();
					zustand = C_STRINGTABLE_IDENTIFIER;
					break;
				case R_END:
					zustand = C_READ;
					break;
				case S_LITERAL:
					if (m_pScanner->IsTolerant())
					{
						sStringID = pToken->GetOriginal();
						zustand = C_STRINGTABLE_IDENTIFIER;
					}break;
				default:
					zustand = C_ERROR;
					break;
			}
			break;
		case C_STRINGTABLE_IDENTIFIER:
			switch (pToken->GetSymbol())
			{
				case S_STRING_LITERAL:
					sValue = pToken->GetOriginal();
					pTSRead = new CTranslateString(m_pScanner->GetFileName(), sStringID, sValue);
					if (bWriteRC)
					{
						bReplaceToken = TRUE;
					}
					else
					{
						bAddReplaceToTSArray = TRUE;
					}
					zustand = C_STRINGTABLE_BEGIN;
					break;
				case R_END:
					zustand = C_READ;
					break;
				default:
					zustand = C_ERROR;
					break;
			}
			break;
		// Dialog --------------------------------------------
		case C_DIALOG:
			pPrevToken = m_pScanner->GetTokens()->GetAt(m_pScanner->GetCurrentTokenIndex()-3);
			sDialogID = pPrevToken->GetOriginal(); 
			zustand = C_DIALOG_ID;
			break;
		case C_DIALOG_ID:
			switch (pToken->GetSymbol())
			{
				case R_CAPTION:
					zustand = C_DIALOG_CAPTION;
					break;
				case R_BEGIN:
					zustand = C_DIALOG_BEGIN;
					break;
				case R_END:
					zustand = C_READ;
					break;
				default:
					zustand = C_DIALOG_ID;
					break;
			}
			break;

		case C_DIALOG_CAPTION:
			switch (pToken->GetSymbol())
			{
				case S_STRING_LITERAL:
					sValue = pToken->GetOriginal();
					pTSRead = new CTranslateString(RST_CAPTION,
													m_pScanner->GetFileName(),
													sDialogID,
													_T("CAPTION"),
													sValue);
					if (bWriteRC)
					{
						bReplaceToken = TRUE;
					}
					else
					{
						bAddReplaceToTSArray = TRUE;
					}
					zustand = C_DIALOG_ID;
					break;
				default:
					zustand = C_ERROR;
					break;
			}
			break;
		case C_DIALOG_BEGIN:
			switch (pToken->GetSymbol())
			{
				case R_LTEXT:
				case R_CTEXT:
				case R_RTEXT:
				case R_DEFPUSHBUTTON:
				case R_GROUPBOX:
				case R_PUSHBOX:
				case R_STATE3:
				case R_CONTROL:
				case R_PUSHBUTTON:
					zustand = C_DIALOG_CONTROL;
					rst = RST_LTEXT;
					break;
				case R_END:
					zustand = C_READ;
					break;
			}
			break;
		case C_DIALOG_CONTROL:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				sValue = pToken->GetOriginal();
				zustand = C_DIALOG_CONTROL_COMMA;
				if (bWriteRC)
				{
					bWriteRCLine = FALSE; 
					sPreBuffer = pToken->GetWhiteAndComment();
				}
			}
			else
			{
				zustand = C_DIALOG_BEGIN;
			}
			break;
		case C_DIALOG_CONTROL_COMMA:
			if (pToken->GetSymbol()==S_COMMA)
			{
				zustand = C_DIALOG_CONTROL_COMMA_IDENTIFIER;
				if (bWriteRC)
				{
					sPostBuffer = pToken->GetWhiteAndComment() + pToken->GetOriginal();
				}
			}
			else
			{
				zustand = C_ERROR;
			}
			break;
		case C_DIALOG_CONTROL_COMMA_IDENTIFIER:
			if (	pToken->GetSymbol()==S_IDENTIFIER
				|| (m_pScanner->IsTolerant()&& pToken->GetSymbol()==S_LITERAL))
			{
				if (pToken->GetOriginal()==_T("IDC_STATIC"))
				{
					if (m_bCommandLine)
					{
						_ftprintf(stdout, _T("%s (%i):"), m_pScanner->GetPathName(), pToken->GetLine());
						_ftprintf(stdout, _T("IDC_STATIC im Dialog %s\n"), sDialogID);
					}
					else if (!m_pScanner->IsTolerant())
					{
						CString sMsg, sError, sTemp;
						sError.LoadString(IDS_ERROR_STATIC);
						sTemp.Format(sError,LPCTSTR(sDialogID));
						sMsg.Format(_T("%s\n%s Zeile:%d Token:%s"),
									LPCTSTR(sTemp), LPCTSTR(m_pScanner->GetFileName()),
									pToken->GetLine(), LPCTSTR(pToken->GetOriginal()));
						AfxMessageBox(sMsg);
					}
					return FALSE;
				}
				pTSRead = new CTranslateString(rst,
												m_pScanner->GetFileName(),
												sDialogID,
												pToken->GetOriginal(),
												sValue);
				if (bWriteRC)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWriteRCLine = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					bAddReplaceToTSArray = TRUE;
				}
				zustand = C_DIALOG_BEGIN;
			}
			else
			{
				zustand = C_ERROR;
			}
			break;
// Menu --------------------------------------------
		case C_MENU:
			pPrevToken = m_pScanner->GetTokens()->GetAt(m_pScanner->GetCurrentTokenIndex()-3);
			sMenuID = pPrevToken->GetOriginal(); 
			zustand = C_MENU_ID;
			// FALLTHROUGH !!
			// es kann gleich der nächste Zustand genutzt werden
		case C_MENU_ID:
			if (pToken->GetSymbol()==R_BEGIN)
			{
				iPopupBeginCounter = 0;
				zustand = C_MENU_BEGIN;
			}
			else
			{
				// waiting for BEGIN
			}
			break;
		case C_MENU_BEGIN:
			switch (pToken->GetSymbol())
			{
				case R_POPUP:
					zustand = C_POPUP;
					break;
				case R_END:
					zustand = C_READ;
					break;
				case R_MENUITEM:
					zustand = C_MENUITEM;
					break;
				default:
					zustand = C_ERROR;
					break;
			}
			break;
		case C_MENUITEM:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				sValue = pToken->GetOriginal();
				zustand = C_MENUITEM_STR;
				if (bWriteRC)
				{
					sPreBuffer = pToken->GetWhiteAndComment();
					bWriteRCLine = FALSE;
				}
			}
			else if (pToken->GetSymbol()==R_SEPARATOR)
			{
				zustand = C_MENU_BEGIN;
			}
			else
			{
				TRACE(_T("error in	C_MENUITEM\n"));
				zustand = C_ERROR;
			}
			break;
		case C_MENUITEM_STR:
			if (pToken->GetSymbol()==S_COMMA)
			{
				zustand = C_MENUITEM_STR_COMMA;
				if (bWriteRC)
				{
					sPostBuffer = pToken->GetWhiteAndComment() + pToken->GetOriginal();
				}
			}
			else
			{
				TRACE(_T("error in	C_MENUITEM_STR\n"));
				zustand = C_ERROR;
			}
			break;
		case C_MENUITEM_STR_COMMA:
			if (pToken->GetSymbol()==S_IDENTIFIER)
			{
				pTSRead = new CTranslateString(RST_MENUITEM,
												m_pScanner->GetFileName(),
												sMenuID,
												0,
												pToken->GetOriginal(),
												sValue);
				if (bWriteRC)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWriteRCLine = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					bAddReplaceToTSArray = TRUE;
				}
				zustand = C_MENU_BEGIN;
			}
			else
			{
				TRACE(_T("error in	C_MENUITEM_STR_COMMA\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				popupcounter++;
				pTSRead = new CTranslateString(RST_POPUP,
												m_pScanner->GetFileName(),
												sMenuID,
												popupcounter,
												_T(""),
												pToken->GetOriginal());
				if (bWriteRC)
				{
					bReplaceToken = TRUE;
				}
				else
				{
					bAddReplaceToTSArray = TRUE;
				}
				zustand = C_POPUP_STR;
			}
			else
			{
				TRACE(_T("error in	C_POPUP\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_STR:
			if (pToken->GetSymbol()==R_BEGIN)
			{
				iPopupBeginCounter++;
				zustand = C_POPUP_BEGIN;
			}
			else
			{
				TRACE(_T("error in	C_POPUP_STR\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_BEGIN:
			if (pToken->GetSymbol()==R_POPUP)
			{
				zustand = C_POPUP;
			}
			else if (pToken->GetSymbol()==R_MENUITEM)
			{
				zustand = C_POPUP_MENUITEM;
			}
			else if (pToken->GetSymbol()==R_END)
			{
				iPopupBeginCounter--;
				if (iPopupBeginCounter == 0) {
					zustand = C_MENU_BEGIN;
				}
				else
				{
					zustand = C_POPUP_BEGIN;
				}
			}
			else
			{
				TRACE(_T("error at	C_POPUP_BEGIN\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				sValue = pToken->GetOriginal();
				zustand = C_POPUP_MENUITEM_STR;
				if (bWriteRC)
				{
					sPreBuffer = pToken->GetWhiteAndComment();
					bWriteRCLine = FALSE;
				}
			}
			else if (pToken->GetSymbol()==R_SEPARATOR)
			{
				zustand = C_POPUP_BEGIN;
			}
			else
			{
				TRACE(_T("error at	C_POPUP_MENUITEM\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM_STR:
			if (pToken->GetSymbol()==S_COMMA)
			{
				zustand = C_POPUP_MENUITEM_STR_COMMA;
				if (bWriteRC)
				{
					sPostBuffer = pToken->GetWhiteAndComment() + pToken->GetOriginal();
				}
			}
			else
			{
				TRACE(_T("error at	C_POPUP_MENUITEM_STR\n"));
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM_STR_COMMA:
			if (pToken->GetSymbol()==S_IDENTIFIER)
			{
				pTSRead = new CTranslateString(RST_MENUITEM,
												m_pScanner->GetFileName(),
												sMenuID,
												popupcounter,
												pToken->GetOriginal(),
												sValue);
				if (bWriteRC)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWriteRCLine = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					bAddReplaceToTSArray = TRUE;
				}
				zustand = C_POPUP_BEGIN; 
			}
			else
			{
				TRACE(_T("error in C_POPUP_MENUITEM_STR_COMMA\n"));
				zustand = C_ERROR;
			}
			break;
		default:
			if (m_bCommandLine)
			{
				_ftprintf(stdout, _T("Fehler, unbekannter parser zustand\n"));
			}
			else
			{
				AfxMessageBox(IDS_ERROR);
			}
			return FALSE;
			break;
		} // end of switch

		if (zustand==C_ERROR)
		{
			if (m_bCommandLine)
			{
				// Original may be VERY long, TRACE will assert at expanded length >512
				_ftprintf(stdout, _T("%s(%d):"), m_pScanner->GetPathName(), pToken->GetLine());
				_ftprintf(stdout, _T("Fehler in Resource %s\n"), pToken->GetOriginal().Left(450));
				TRACE(_T("Fehler in Resource %s, line %d\n"), pToken->GetOriginal().Left(450), pToken->GetLine());
			}
			else
			{
				CString sMsg, sError;
				sError.LoadString(IDS_ERROR);
				sMsg.Format(_T("%s\n%s Zeile:%d Token:%s"),
							LPCTSTR(sError), LPCTSTR(m_pScanner->GetFileName()),
							pToken->GetLine(), LPCTSTR(pToken->GetOriginal().Left(400)));
				AfxMessageBox(sMsg);
			}
			return FALSE;
		}

		if (bWriteRC)
		{
			if (bWriteRCLine)
			{
				if (bReplaceToken)
				{
					// then replace the text with the translated one
					CTranslateString* pReplaceTS = m_pTSArray->GetTSFromFullID(pTSRead->GetFullID());
					CString sTranslation;

					if (pReplaceTS)
					{
						sTranslation = pReplaceTS->GetTranslate(bIgnoreTODO);
					}
					else
					{
						sTranslation = pTSRead->GetText();
					}
					if ((zustand == C_DIALOG_BEGIN) || 
						(zustand == C_MENU_BEGIN) ||
						(zustand == C_POPUP_BEGIN))
					{
						file.Write((void*)LPCTSTR(sPreBuffer), sPreBuffer.GetLength()*sizeof(_TCHAR));
						sPreBuffer.Empty();
					}
					else
					{
						// first write the orig whitespace and comment
						file.Write((void*)LPCTSTR(pToken->GetWhiteAndComment()), pToken->GetWhiteAndComment().GetLength()*sizeof(_TCHAR));
					}
					file.Write((void*)LPCTSTR(sTranslation), sTranslation.GetLength()*sizeof(_TCHAR));

					if ((zustand == C_DIALOG_BEGIN) || 
						(zustand == C_MENU_BEGIN) ||
						(zustand == C_POPUP_BEGIN))
					{
						file.Write((void*)LPCTSTR(sPostBuffer),sPostBuffer.GetLength()*sizeof(_TCHAR));
						sPostBuffer.Empty();
					}

					delete pTSRead;
					pTSRead = NULL;
					bReplaceToken = FALSE;
				}
				else
				{
					file.Write((void*)LPCTSTR(pToken->GetWhiteAndComment()), pToken->GetWhiteAndComment().GetLength()*sizeof(_TCHAR));
					file.Write((void*)LPCTSTR(pToken->GetOriginal())       , pToken->GetOriginal().GetLength()*sizeof(_TCHAR));
				}
			} // bWriteRCLine
		} // bWriteRC
		else if (bAddReplaceToTSArray)
		{
			if (m_pTSArrayEnu)
			{
				pTSEnu = m_pTSArrayEnu->GetTSFromFullID(pTSRead->GetFullID());
				if (pTSEnu)
				{
					pTSRead->SetMaxLength(pTSEnu->GetMaxLength());
					pTSRead->SetComment(pTSEnu->GetComment());
				}
				m_pTSArray->AddReplace(pTSRead);
			}
			else
			{
				m_pTSArray->AddReplace(pTSRead);
			}
			bAddReplaceToTSArray = FALSE;
		}
		else if (pTSRead)
		{
			delete pTSRead;
			pTSRead = NULL;
		}
	} //while
	return TRUE;
}
