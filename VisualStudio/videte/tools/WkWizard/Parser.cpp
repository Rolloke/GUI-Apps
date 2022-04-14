// Parser.cpp

#include "stdafx.h"

#include "resource.h"

#include "Scanner.h"
#include "TranslateString.h"
#include "Parser.h"




/////////////////////////////////////////////////////////////////////////////
CParser::CParser(CScanner* pScanner, CTSArray* pTSArray, BOOL bCL)
{
	m_pScanner = pScanner;
	m_pTSArray = pTSArray;
	m_bCL = bCL;
//	m_sFile;
}
/////////////////////////////////////////////////////////////////////////////
CParser::CParser(CScanner* pScannerSource, CTSArray* pTSArrayReplace, 
				const CString& destinationFile,CString& sLang,BOOL bCL)
{
	m_pScanner = pScannerSource;
	m_pTSArray = pTSArrayReplace;
	m_bCL = bCL;
	m_sFile = destinationFile;
	m_sLang = sLang;
}
/////////////////////////////////////////////////////////////////////////////
CParser::~CParser()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CParser::Parse()
{
	// wenn m_sFile leer ist, dann extrahieren, sonst ersetzen
	CToken* pToken;
	CToken* pPrevToken;
	ECondition zustand = C_READ;
	ResourceStringType rst = RST_DEFAULT;

	CString StringId,Value,DialogId,MenuId;
	CString mes,ls;
	int popupcounter = 0;
	int iPopupBeginCounter = 0;

	CFile file;
	CFileException cfe;
	BOOL bReplace = m_sFile.IsEmpty()==FALSE;
	BOOL bReplaceToken = FALSE;
	BOOL bWrite = bReplace;
	CTranslateString* pReplaceTS = NULL;
	CTranslateString* pReadTS = NULL;

	CString sPreBuffer;
	CString sPostBuffer;

	// open destination file if replacing
	if (bReplace)
	{
		if (!file.Open(m_sFile,CFile::modeCreate|CFile::modeWrite,&cfe))
		{
			return FALSE;
		}
		// close file will be called from destructor of CFile
		// so we can return every time :-)
	}


	while (NULL!=(pToken = m_pScanner->NextToken()))
	{
		TRACE("%s,%s\n",pToken->GetSymbolName(),pToken->GetOriginal());
		switch (zustand)
		{
		// lesen
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
				StringId = pToken->GetOriginal();
				zustand = C_STRINGTABLE_IDENTIFIER;
				break;
			case R_END:
				zustand = C_READ;
				break;
			default:
				zustand = C_ERROR;
				break;
			}
			break;
		case C_STRINGTABLE_IDENTIFIER:
			switch (pToken->GetSymbol())
			{
			case S_STRING_LITERAL:
				Value = pToken->GetOriginal();
				pReadTS = new CTranslateString(m_pScanner->GetFileName(),StringId,Value);
				if (bReplace)
				{
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
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
// Dialog ---------------------------------------
		case C_DIALOG:
			pPrevToken = m_pScanner->GetTokens()->GetAt(m_pScanner->GetCurrentTokenIndex()-3);
			DialogId = pPrevToken->GetOriginal(); 
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
			}
			break;
		case C_DIALOG_CAPTION:
			switch (pToken->GetSymbol())
			{
			case S_STRING_LITERAL:
				Value = pToken->GetOriginal();
				pReadTS = new CTranslateString(RST_CAPTION,m_pScanner->GetFileName(),
												DialogId,"CAPTION",Value);
				if (bReplace)
				{
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
				}
				zustand = C_DIALOG_ID;
				break;
			default:
				zustand = C_ERROR;
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
				Value = pToken->GetOriginal();
				zustand = C_DIALOG_CONTROL_COMMA;
				if (bReplace)
				{
					bWrite = FALSE; 
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
				if (bReplace)
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
			if (pToken->GetSymbol()==S_IDENTIFIER)
			{
				if (pToken->GetOriginal()=="IDC_STATIC")
				{
					if (m_bCL)
					{
						cout << m_pScanner->GetPathName() << "(" << pToken->GetLine() << "):";
						cout << "IDC_STATIC im Dialog " << DialogId << " verwendet"<< endl << flush;
					}
					else
					{
						CString temp;
						ls.LoadString(IDS_ERROR_STATIC);
						temp.Format(ls,(const char*)DialogId);
						mes.Format("%s\n%s Zeile:%d Token:%s",
							(const char*)temp,(const char*)m_pScanner->GetFileName(),pToken->GetLine(),(const char*)pToken->GetOriginal());
						AfxMessageBox(mes);
					}
					return FALSE;
				}
				pReadTS = new CTranslateString(rst,m_pScanner->GetFileName(),DialogId,
												pToken->GetOriginal(),Value);
				if (bReplace)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWrite = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
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
			MenuId = pPrevToken->GetOriginal(); 
			zustand = C_MENU_ID;
			break;
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
			}
			break;
		case C_MENUITEM:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				Value = pToken->GetOriginal();
				zustand = C_MENUITEM_STR;
				if (bReplace)
				{
					sPreBuffer = pToken->GetWhiteAndComment();
					bWrite = FALSE;
				}
			}
			else if (pToken->GetSymbol()==R_SEPARATOR)
			{
				zustand = C_MENU_BEGIN;
			}
			else
			{
				TRACE("error in	C_MENUITEM\n");
				zustand = C_ERROR;
			}
			break;
		case C_MENUITEM_STR:
			if (pToken->GetSymbol()==S_COMMA)
			{
				zustand = C_MENUITEM_STR_COMMA;
				if (bReplace)
				{
					sPostBuffer = pToken->GetWhiteAndComment() + pToken->GetOriginal();
				}
			}
			else
			{
				TRACE("error in	C_MENUITEM_STR\n");
				zustand = C_ERROR;
			}
			break;
		case C_MENUITEM_STR_COMMA:
			if (pToken->GetSymbol()==S_IDENTIFIER)
			{
				pReadTS = new CTranslateString(RST_MENUITEM,m_pScanner->GetFileName(),
										MenuId,0,pToken->GetOriginal(),Value);
				if (bReplace)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWrite = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
				}
				zustand = C_MENU_BEGIN;
			}
			else
			{
				TRACE("error in	C_MENUITEM_STR_COMMA\n");
				zustand = C_ERROR;
			}
			break;
		case C_POPUP:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				popupcounter++;
				pReadTS = new CTranslateString(RST_POPUP,
					m_pScanner->GetFileName(),
					MenuId,popupcounter,"",pToken->GetOriginal());
				if (bReplace)
				{
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
				}
				zustand = C_POPUP_STR;
			}
			else
			{
				TRACE("error in	C_POPUP\n");
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
				TRACE("error in	C_POPUP_STR\n");
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
				TRACE("error at	C_POPUP_BEGIN\n");
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM:
			if (pToken->GetSymbol()==S_STRING_LITERAL)
			{
				Value = pToken->GetOriginal();
				zustand = C_POPUP_MENUITEM_STR;
				if (bReplace)
				{
					sPreBuffer = pToken->GetWhiteAndComment();
					bWrite = FALSE;
				}
			}
			else if (pToken->GetSymbol()==R_SEPARATOR)
			{
				zustand = C_POPUP_BEGIN;
			}
			else
			{
				TRACE("error at	C_POPUP_MENUITEM\n");
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM_STR:
			if (pToken->GetSymbol()==S_COMMA)
			{
				zustand = C_POPUP_MENUITEM_STR_COMMA;
				if (bReplace)
				{
					sPostBuffer = pToken->GetWhiteAndComment() + pToken->GetOriginal();
				}
			}
			else
			{
				TRACE("error at	C_POPUP_MENUITEM_STR\n");
				zustand = C_ERROR;
			}
			break;
		case C_POPUP_MENUITEM_STR_COMMA:
			if (pToken->GetSymbol()==S_IDENTIFIER)
			{
				pReadTS = new CTranslateString(RST_MENUITEM,m_pScanner->GetFileName(),
										MenuId,popupcounter,pToken->GetOriginal(),Value);
				if (bReplace)
				{
					sPostBuffer += pToken->GetWhiteAndComment() + pToken->GetOriginal();
					bWrite = TRUE;
					bReplaceToken = TRUE;
				}
				else
				{
					m_pTSArray->AddReplace(pReadTS);
				}
				zustand = C_POPUP_BEGIN;
			}
			else
			{
				TRACE("error in C_POPUP_MENUITEM_STR_COMMA\n");
				zustand = C_ERROR;
			}
			break;
		default:
			if (m_bCL)
			{
				cout << "Fehler, unbekannter parser zustand" << endl << flush;
			}
			else
			{
				AfxMessageBox(IDS_ERROR);
			}
			return FALSE;
			break;
		}
		if (zustand==C_ERROR)
		{
			if (m_bCL)
			{
				cout << m_pScanner->GetPathName() << "(" << pToken->GetLine() << "):";
				cout << "Fehler in Resource " << pToken->GetOriginal() << endl << flush;
				TRACE("Fehler in Resource %s, line %d\n",pToken->GetOriginal(),pToken->GetLine());
			}
			else
			{
				ls.LoadString(IDS_ERROR);
				mes.Format("%s\n%s Zeile:%d Token:%s",(const char*)ls,
					(const char*)m_pScanner->GetFileName(),pToken->GetLine(),(const char*)pToken->GetOriginal());
				AfxMessageBox(mes);
			}
			return FALSE;
		}
		if (bWrite)
		{
			if (bReplaceToken)
			{
				CString trans;
				// then replace the text with the translated one
				pReplaceTS = m_pTSArray->GetTSFromID(pReadTS->GetID());

				if (pReplaceTS)
				{
					trans = pReplaceTS->GetTranslate();
				}
				else
				{
					trans = pReadTS->GetText();
				}
				if ((zustand == C_DIALOG_BEGIN) || 
					(zustand == C_MENU_BEGIN) ||
					(zustand == C_POPUP_BEGIN))
				{
					file.Write((void*)(const char*)sPreBuffer,sPreBuffer.GetLength());
					sPreBuffer.Empty();
				}
				else
				{
					// first write the orig whitespace and comment
					file.Write((void*)(const char*)pToken->GetWhiteAndComment(),
								pToken->GetWhiteAndComment().GetLength());
				}
				file.Write((void*)(const char*)trans,
								trans.GetLength());
				if ((zustand == C_DIALOG_BEGIN) || 
					(zustand == C_MENU_BEGIN) ||
					(zustand == C_POPUP_BEGIN))
				{
					file.Write((void*)(const char*)sPostBuffer,sPostBuffer.GetLength());
					sPreBuffer.Empty();
				}
				delete pReadTS;
				pReadTS = NULL;
				bReplaceToken = FALSE;
			}
			else
			{
				if (bReplace)
				{
					file.Write((void*)(const char*)pToken->GetWhiteAndComment(),pToken->GetWhiteAndComment().GetLength());
					file.Write((void*)(const char*)pToken->GetOriginal(),pToken->GetOriginal().GetLength());
				}
			}
		} // write
	} //while
	return TRUE;
}
