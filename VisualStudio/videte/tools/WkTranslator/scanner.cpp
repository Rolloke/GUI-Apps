// Scanner.cpp
#include "stdafx.h"
#include "Scanner.h"
/////////////////////////////////////////////////////////////////////////////
CString NameOfEnum(ESymbol es)
{
	CString ret;
	switch (es)
	{
		case S_EOF: ret = _T("S_EOF");break;
		case S_error: ret = _T("S_error");break;
		case S_ABSTRACT: ret = _T("S_ABSTRACT");break;
		case S_BOOLEAN: ret = _T("S_BOOLEAN");break;
		case S_BREAK: ret = _T("S_BREAK");break;
		case S_BYTE: ret = _T("S_BYTE");break;
		case S_CASE: ret = _T("S_CASE");break;
		case S_CATCH: ret = _T("S_CATCH");break;
		case S_CHAR: ret = _T("S_CHAR");break;
		case S_CLASS: ret = _T("S_CLASS");break;
		case S_CONTINUE: ret = _T("S_CONTINUE");break;
		case S_DEFAULT: ret = _T("S_DEFAULT");break;
		case S_DO: ret = _T("S_DO");break;
		case S_DOUBLE: ret = _T("S_DOUBLE");break;
		case S_ELSE: ret = _T("S_ELSE");break;
		case S_EXTENDS: ret = _T("S_EXTENDS");break;
		case S_FINAL: ret = _T("S_FINAL");break;
		case S_FINALLY: ret = _T("S_FINALLY");break;
 		case S_FLOAT: ret = _T("S_FLOAT");break;
 		case S_FOR: ret = _T("S_FOR");break;
 		case S_IF: ret = _T("S_IF");break;
 		case S_IMPLEMENTS: ret = _T("S_IMPLEMENTS");break;
 		case S_IMPORT: ret = _T("S_IMPORT");break;
 		case S_INSTANCEOF: ret = _T("S_INSTANCEOF");break;
 		case S_INT: ret = _T("S_INT");break;
 		case S_INTERFACE: ret = _T("S_INTERFACE");break;
 		case S_NATIVE: ret = _T("S_NATIVE");break;
 		case S_LONG: ret = _T("S_LONG");break;
 		case S_NEW: ret = _T("S_NEW");break;
 		case S_NULL: ret = _T("S_NULL");break;
 		case S_PACKAGE: ret = _T("S_PACKAGE");break;
 		case S_PRIVATE: ret = _T("S_PRIVATE");break;
 		case S_PROTECTED: ret = _T("S_PROTECTED");break;
 		case S_PUBLIC: ret = _T("S_PUBLIC");break;
 		case S_RETURN: ret = _T("S_RETURN");break;
 		case S_SHORT: ret = _T("S_SHORT");break;
 		case S_STATIC: ret = _T("S_STATIC");break;
 		case S_SUPER: ret = _T("S_SUPER");break;
 		case S_SWITCH: ret = _T("S_SWITCH");break;
 		case S_SYNCHRONIZED: ret = _T("S_SYNCHRONIZED");break;
 		case S_THIS: ret = _T("S_THIS");break;
 		case S_THROW: ret = _T("S_THROW");break;
 		case S_THROWS: ret = _T("S_THROWS");break;
 		case S_TRY: ret = _T("S_TRY");break;
 		case S_VOID: ret = _T("S_VOID");break;
 		case S_WHILE: ret = _T("S_WHILE");break;
 		case S_PLUS: ret = _T("S_PLUS");break;
 		case S_MINUS: ret = _T("S_MINUS");break;
 		case S_STAR: ret = _T("S_STAR");break;
 		case S_OP_DIV: ret = _T("S_OP_DIV");break;
 		case S_OP_MOD: ret = _T("S_OP_MOD");break;
 		case S_OP_AND: ret = _T("S_OP_AND");break;
 		case S_OP_OR: ret = _T("S_OP_OR");break;
 		case S_OP_XOR: ret = _T("S_OP_XOR");break;
 		case S_OP_INC: ret = _T("S_OP_INC");break;
 		case S_OP_DEC: ret = _T("S_OP_DEC");break;
 		case S_OP_SHL: ret = _T("S_OP_SHL");break;
 		case S_OP_SHR: ret = _T("S_OP_SHR");break;
 		case S_OP_SHRR: ret = _T("S_OP_SHRR");break;
 		case S_OP_GE: ret = _T("S_OP_GE");break;
 		case S_OP_LE: ret = _T("S_OP_LE");break;
 		case S_OP_EQ: ret = _T("S_OP_EQ");break;
 		case S_OP_NE: ret = _T("S_OP_NE");break;
 		case S_OP_G: ret = _T("S_OP_G");break;
 		case S_OP_L: ret = _T("S_OP_L");break;
 		case S_OP_LAND: ret = _T("S_OP_LAND");break;
 		case S_OP_LOR: ret = _T("S_OP_LOR");break;
 		case S_OP_DIM: ret = _T("S_OP_DIM");break;
 		case S_ASS_MUL: ret = _T("S_ASS_MUL");break;
 		case S_ASS_DIV: ret = _T("S_ASS_DIV");break;
 		case S_ASS_MOD: ret = _T("S_ASS_MOD");break;
 		case S_ASS_ADD: ret = _T("S_ASS_ADD");break;
 		case S_ASS_SUB: ret = _T("S_ASS_SUB");break;
 		case S_ASS_SHL: ret = _T("S_ASS_SHL");break;
 		case S_ASS_SHR: ret = _T("S_ASS_SHR");break;
 		case S_ASS_SHRR: ret = _T("S_ASS_SHRR");break;
 		case S_ASS_AND: ret = _T("S_ASS_AND");break;
 		case S_ASS_XOR: ret = _T("S_ASS_XOR");break;
 		case S_ASS_OR: ret = _T("S_ASS_OR");break;
 		case S_IDENTIFIER: ret = _T("S_IDENTIFIER");break;
 		case S_LITERAL: ret = _T("S_LITERAL");break;
 		case S_STRING_LITERAL: ret = _T("S_STRING_LITERAL");break;
 		case S_CHAR_LITERAL: ret = _T("S_CHAR_LITERAL");break;
 		case S_COMMA: ret = _T("S_COMMA");break;
 		case S_SEMICOLON: ret = _T("S_SEMICOLON");break;
 		case S_POINT: ret = _T("S_POINT");break;
 		case S_EQUALS_SIGN: ret = _T("S_EQUALS_SIGN");break;
 		case S_COLON: ret = _T("S_COLON");break;
 		case S_QUESTION_MARK: ret = _T("S_QUESTION_MARK");break;
 		case S_TILDE: ret = _T("S_TILDE");break;
 		case S_EXCLAMATION: ret = _T("S_EXCLAMATION");break;
 		case S_OPEN1: ret = _T("S_OPEN1");break;
 		case S_CLOSE1: ret = _T("S_CLOSE1");break;
 		case S_OPEN2: ret = _T("S_OPEN2");break;
 		case S_CLOSE2: ret = _T("S_CLOSE2");break;
 		case S_OPEN3: ret = _T("S_OPEN3");break;
 		case S_CLOSE3: ret = _T("S_CLOSE3");break;
 		case S_CONST: ret = _T("S_CONST");break;
 		case S_GOTO: ret = _T("S_GOTO");break;
 		case S_OPERATOR: ret = _T("S_OPERATOR");break;
 		case S_VOLATILE: ret = _T("S_VOLATILE");break;

 		case R_ICON: ret = _T("R_ICON");break;
 		case R_DISCARDABLE: ret = _T("R_DISCARDABLE");break;
 		case R_BITMAP: ret = _T("R_BITMAP");break;
 		case R_MOVEABLE: ret = _T("R_MOVEABLE");break;
 		case R_PURE: ret = _T("R_PURE");break;
 		case R_TOOLBAR: ret = _T("R_TOOLBAR");break;
 		case R_SEPARATOR: ret = _T("R_SEPARATOR");break;
 		case R_BUTTON: ret = _T("R_BUTTON");break;
 		case R_PRELOAD: ret = _T("R_PRELOAD");break;
 		case R_BEGIN: ret = _T("R_BEGIN");break;
 		case R_END: ret = _T("R_END");break;
 		case R_POPUP: ret = _T("R_POPUP");break;
 		case R_MENUITEM: ret = _T("R_MENUITEM");break;
 		case R_ACCELERATORS: ret = _T("R_ACCELERATORS");break;
 		case R_VIRTKEY: ret = _T("R_VIRTKEY");break;
 		case R_ALT: ret = _T("R_ALT");break;
 		case R_SHIFT: ret = _T("R_SHIFT");break;
 		case R_CONTROL: ret = _T("R_CONTROL");break;
 		case R_DIALOG: ret = _T("R_DIALOG");break;
 		case R_DIALOGEX: ret = _T("R_DIALOGEX");break;
 		case R_STYLE: ret = _T("R_STYLE");break;
 		case R_FONT: ret = _T("R_FONT");break;
 		case R_LTEXT: ret = _T("R_LTEXT");break;
 		case R_CTEXT: ret = _T("R_CTEXT");break;
 		case R_RTEXT: ret = _T("R_RTEXT");break;
 		case R_DEFPUSHBUTTON: ret = _T("R_DEFPUSHBUTTON");break;
 		case R_PUSHBUTTON: ret = _T("R_PUSHBUTTON");break;
 		case R_VERSIONINFO: ret = _T("R_VERSIONINFO");break;
 		case R_BLOCK: ret = _T("R_BLOCK");break;
 		case R_VALUE: ret = _T("R_VALUE");break;
 		case R_STRINGTABLE: ret = _T("R_STRINGTABLE");break;
 		case R_CAPTION: ret = _T("R_CAPTION");break;
 		case R_GROUPBOX: ret = _T("R_GROUPBOX");break;
 		case R_PUSHBOX: ret = _T("R_PUSHBOX");break;
 		case R_SCROLLBAR: ret = _T("R_SCROLLBAR");break;
 		case R_STATE3: ret = _T("R_STATE3");break;
 		case R_MENU: ret = _T("R_MENU");break;

 		case P_INCLUDE: ret = _T("P_INCLUDE");break;
 		case P_DEFINE: ret = _T("P_DEFINE");break;
 		case P_IFDEF: ret = _T("P_IFDEF");break;
 		case P_ELSE: ret = _T("P_ELSE");break;
 		case P_ENDIF: ret = _T("P_ENDIF");break;
 		case P_UNDEF: ret = _T("P_UNDEF");break;
 		case P_IFNDEF: ret = _T("P_IFNDEF");break;
 		case P_PRAGMA: ret = _T("P_PRAGMA");break;
 		case P_ELSIF: ret = _T("P_ELSIF");break;
 		case P_IF: ret = _T("P_IF");break;
 		case P_DEFINED: ret = _T("P_DEFINED");break;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
CToken::CToken(ESymbol sym, const CString& str, int l)
{
	m_Symbol = sym;
	m_Original = str;
	m_Line = l;
#ifdef _UNICODE
#ifdef _DEBUG
	m_pszOriginal = NULL;
	int nLen = m_Original.GetLength();
	if (nLen)
	{
		m_pszOriginal = new char[nLen+1];
		nLen = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, m_Original, nLen,
									m_pszOriginal, nLen, NULL, NULL);
		m_pszOriginal[nLen] = 0;
	}
#endif	
#endif	
}
/////////////////////////////////////////////////////////////////////////////
CToken::CToken(ESymbol sym, const CString& str, const CString& white, int l)
{
	m_Symbol = sym;
	m_Original = str;
	m_Line = l;
	m_WhiteAndComment = white;
#ifdef _UNICODE
#ifdef _DEBUG
	m_pszOriginal = NULL;
	int nLen = m_Original.GetLength();
	if (nLen)
	{
		m_pszOriginal = new char[nLen+1];
		nLen = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, m_Original, nLen,
									m_pszOriginal, nLen, NULL, NULL);
		m_pszOriginal[nLen] = 0;
	}
#endif	
#endif	
}
#ifdef _UNICODE
#ifdef _DEBUG
CToken::~CToken()
{
	if (m_pszOriginal) delete m_pszOriginal;
}
#endif	
#endif	

/////////////////////////////////////////////////////////////////////////////
CTokenArray::CTokenArray()
{
}
/////////////////////////////////////////////////////////////////////////////
CTokenArray::~CTokenArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CTokenArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAt(i)) 
		{ 
			delete (CToken*) GetAt(i); 
		} 
	} 
	RemoveAll(); 
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CScanner::CScanner(const CString& sourceFileName,BOOL bStoreWhiteAndComment /*= FALSE*/)
{
	m_PathName = sourceFileName;
	m_pszSource = NULL;
	m_Index = 0;
	m_CurrentLine = 1;
	m_MaxRead = 0;
	m_iPos = 0;
	m_bStoreWhiteAndComment = bStoreWhiteAndComment;
	m_bTolerant = FALSE;

	int p;
	p = m_PathName.ReverseFind('\\');
	if (p!=-1)
	{
		m_FileName = m_PathName.Mid(p+1);
	}
	else
	{
		m_FileName = m_PathName;
	}
}
CScanner::~CScanner()
{
	if (m_pszSource)
		delete m_pszSource;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::Read()
{
	CFile file;
	CFileException cfe;
	CFileStatus cfs;
	int len;
	if (CFile::GetStatus(LPCTSTR(m_PathName), cfs))
	{
		len = (int)cfs.m_size;
	}
	else
	{
		return FALSE;
	}

	if (m_pszSource)
	{
		delete m_pszSource;
	}

	m_pszSource = new BYTE[len];

	if (!file.Open(m_PathName,CFile::modeRead,&cfe))
	{
		delete m_pszSource;
		return FALSE;
	}

	TRY
	{
		file.Read((LPVOID)m_pszSource,len);
	}
	CATCH (CFileException, e)
	{
		delete m_pszSource;
		file.Close();
		return FALSE;
	}
	END_CATCH
	
	m_MaxRead = len;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::Scan()
{
	if (!Read())
	{
		return FALSE;
	}

	c = m_pszSource[0];

	while (TRUE)
	{
		if (IsLetter())
		{
			OnLetter();
			continue;
		}
		if (IsWhiteSpace())
		{
			OnWhiteSpace();
			continue;
		}
		if (IsDelimiter())
		{
			OnDelimiter();
			continue;
		}
		if (IsDigit())
		{
			OnDigit();
			continue;
		}
		if (IsBrace())
		{
			OnBrace();
			continue;
		}
		if (IsSeparator())
		{
			OnSeparator();
			continue;
		}
		if (IsStringChar())
		{
			OnStringChar();
			continue;
		}
		if (IsCharChar())
		{
			OnCharChar();
			continue;
		}
		if (IsEOF())
		{
			OnEof();
			break;
		}
		// something on current char went wrong
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::AddToken(ESymbol sym, const CString& str)
{
	if (m_bStoreWhiteAndComment)
	{
		if (sym==S_STRING_LITERAL)
		{
			m_sWhiteAndCommentBuffer +='\"';
		}
		if (sym==S_CHAR_LITERAL)
		{
			m_sWhiteAndCommentBuffer +='\'';
		}
		m_TokenList.Add(new CToken(sym,str,m_sWhiteAndCommentBuffer,m_CurrentLine));
		m_sWhiteAndCommentBuffer.Empty();
		if (sym==S_STRING_LITERAL)
		{
			m_sWhiteAndCommentBuffer +='\"';
		}
		if (sym==S_CHAR_LITERAL)
		{
			m_sWhiteAndCommentBuffer +='\'';
		}
	}
	else
	{
		m_TokenList.Add(new CToken(sym,str,m_CurrentLine));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::BackToFirst()
{
	m_iPos = 0;
}
/////////////////////////////////////////////////////////////////////////////
CToken* CScanner::NextToken()
{
	if (m_iPos<m_TokenList.GetSize())
	{
		return (CToken*)m_TokenList.GetAt(m_iPos++);
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
const CTokenArray* CScanner::GetTokens()
{
	return &m_TokenList;
}
/////////////////////////////////////////////////////////////////////////////
CString CScanner::GetFileName()
{
	return m_FileName;
}
/////////////////////////////////////////////////////////////////////////////
CString CScanner::GetPathName()
{
	return m_PathName;
}
/////////////////////////////////////////////////////////////////////////////
inline _TCHAR CScanner::NextChar()
{
	m_Index++;
	if (m_Index==m_MaxRead)
	{
		c = (_TCHAR)(-1);
	}
	else
	{
		c = m_pszSource[m_Index];
	}
	if (c=='\n')
	{
		m_CurrentLine++;
	}

	return c;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::OnLetter()
{
	CString identifier;
	ESymbol iSymbol;

	while (IsLetter() || IsDigit() || IsIdChar())
	{
		identifier += c;
		NextChar();
	}
	
	iSymbol = CheckForReservedWord(identifier);
	if (iSymbol != S_error)
	{
		AddToken(iSymbol,identifier);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::OnDigit()
{
	CString identifier;
	BOOL bIsHex = FALSE;
	BOOL bIsOct = FALSE;

	// remember TODO hex or octal nums 0x 0
	
	if (c=='0')
	{
		identifier += c;
		NextChar();
		if ((c=='x') || (c=='X'))
		{
			// hexadezimal
			identifier += c;
			NextChar();
			while (IsHexDigit())
			{
				identifier += c;
				NextChar();
			}
			bIsHex = TRUE;
		}
		else if (c=='.')
		{
			// float
		}
		else
		{
			// octal
			while (IsOctalDigit())
			{
				identifier += c;
				NextChar();
			}
			bIsOct = true;
		}
	}

	if (!bIsHex && !bIsOct)
	{
		while (IsDigit())
		{
			identifier += c;
			NextChar();
		}

		if (c=='.')
		{
			// float
			identifier += c;
			NextChar();
			while (IsDigit())
			{
				identifier += c;
				NextChar();
			}
		}

		if (c=='E')
		{
			// float
			identifier += c;
			NextChar();

			if (c=='-')
			{
				identifier += c;
				NextChar();
			}
			else if (c=='+')
			{
				identifier += c;
				NextChar();
			}
			while (IsDigit())
			{
				identifier += c;
				NextChar();
			}
		}
		else if ( (c=='l') || (c=='L'))
		{
			// integer with suffix
			identifier += c;
			NextChar();
		}
		else
		{
			// integer
		}
	}
	AddToken(S_LITERAL,identifier);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnBrace()
{
	ESymbol iSymbol = S_error;
	CString brace = c;
	
	switch (c)
	{
		case '(':
			iSymbol = S_OPEN1;
			NextChar();
			break;
		case ')':
			iSymbol = S_CLOSE1;
			NextChar();
			break;
		case '{':
			iSymbol = S_OPEN3;
			NextChar();
			break;
		case '}':
			iSymbol = S_CLOSE3;
			NextChar();
			break;

		// [] or [ white space ] must be OP_DIM
		case '[':
			do
			{
				NextChar();
			}
			while ( IsWhiteSpace() && !IsEOF());

			if (c==']')
			{
				brace += "]";
				NextChar();
				iSymbol = S_OP_DIM;
			}
			else
			{
				iSymbol = S_OPEN2;
			}
			break;
		case ']':
			iSymbol = S_CLOSE2;
			NextChar();
			break;
	}
	
	AddToken(iSymbol,brace);
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnSeparator()
{
	ESymbol iSymbol = S_error;
	CString separator = c;
	
	switch (c)
	{
		case ';':
			iSymbol = S_SEMICOLON;
			break;
		case '.':
			iSymbol = S_POINT;
			break;
		case ',':
			iSymbol = S_COMMA;
			break;
	}
	AddToken(iSymbol,separator);
	NextChar();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::OnDelimiter()
{
	ESymbol iSymbol = S_error;
	CString opOrAss;

	opOrAss += c;
	switch (c)
	{
	// == =
	case '=':
		NextChar();
		if (c=='=')
		{
			// ==
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_EQ;
		}
		else
		{
			// =
			iSymbol = S_EQUALS_SIGN;
		}
		break;
	
	// < <= << <<=
	case '<':
		NextChar();
		if (c=='<')
		{
			opOrAss+= c;
			NextChar();
			if (c=='=')
			{
				// <<=
				opOrAss+= c;
				NextChar();
				iSymbol = S_ASS_SHL;
			}
			else
			{
				// <<
				opOrAss+= c;
				NextChar();
				iSymbol = S_OP_SHL;
			}
		}
		else if (c=='=')
		{
			// <=
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_LE;
		}
		else
		{
			// <
			iSymbol = S_OP_L;
		}

		break;

	// > >= >> >>= 
	case '>':
		NextChar();
		if (c=='>')
		{
			opOrAss+= c;
			NextChar();
			if (c=='=')
			{
				// >>=
				opOrAss+= c;
				NextChar();
				iSymbol = S_ASS_SHR;
			}
			else
			{
				// >>
				iSymbol = S_OP_SHR;
			}
		}
		else if (c=='=')
		{
			// >=
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_GE;
		}
		else
		{
			// >
			iSymbol = S_OP_G;
		}
		break;
	
	// ! !=
	case '!':
		NextChar();
		if (c=='=')
		{
			// !=
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_NE;
		}
		else
		{
			// !
			iSymbol = S_EXCLAMATION;
		}
		break;

	// ~
	case '~':
		NextChar();
		iSymbol = S_TILDE;
		break;
	
	// :
	case ':':
		NextChar();
		iSymbol = S_COLON;
		break;

	// ?
	case '?':
		NextChar();
		iSymbol = S_QUESTION_MARK;
		break;
	
	// + += ++
	case '+':
		NextChar();
		if (c=='=')
		{
			// +=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_ADD;
		}
		else if (c=='+')
		{
			// ++
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_INC;
		}
		else
		{
			// +
			iSymbol = S_PLUS;
		}
		break;
	
	// - -= --
	case '-':
		NextChar();
		if (c=='=')
		{
			// -=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_SUB;
		}
		else if (c=='-')
		{
			// --
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_DEC;
		}
		else
		{
			// -
			iSymbol = S_MINUS;
		}
		break;
	
	// * *=
	case '*':
		NextChar();
		if (c=='=')
		{
			// *=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_MUL;
		}
		else
		{
			// *
			iSymbol = S_STAR;
		}
		break;
	
	// / /= // /*
	case '/':
		NextChar();
		if (c=='=')
		{
			// /=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_DIV;
		}
		else if (c=='/')
		{
			NextChar();
			OnCommentSlash();
			return TRUE;
		}
		else if (c=='*')
		{
			NextChar();
			if (!OnCommentAstr())
			{
				return FALSE;
			}
		}
		else
		{
			// /
			iSymbol = S_OP_DIV;
		}
		break;

	// & &= &&
	case '&':
		NextChar();
		if (c=='=')
		{
			// &=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_AND;
		}
		else if (c=='&')
		{
			// &&
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_LAND;
		}
		else
		{
			// &
			iSymbol = S_OP_AND;
		}
		break;

	// | |= ||
	case '|':
		NextChar();
		if (c=='=')
		{
			// |=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_OR;
		}
		else if (c=='|')
		{
			// ||
			opOrAss+= c;
			NextChar();
			iSymbol = S_OP_LOR;
		}
		else
		{
			// |
			iSymbol = S_OP_OR;
		}
		break;

	// ^ ^=
	case '^':
		NextChar();
		if (c=='=')
		{
			// ^=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_XOR;
		}
		else
		{
			// ^
			iSymbol = S_OP_XOR;
		}
		break;
	
	// % %=
	case '%':
		NextChar();
		if (c=='=')
		{
			// %=
			opOrAss+= c;
			NextChar();
			iSymbol = S_ASS_MOD;
		}
		else
		{
			// %
			iSymbol = S_OP_MOD;
		}
		break;

	default:
		return FALSE;
	}

	AddToken(iSymbol,opOrAss);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnStringChar()
{
	CString constString;

	NextChar();

	while (true)
	{
		if (IsStringChar())
		{
			NextChar();
			break;
		}
		else if (c=='\\')
		{
			constString += OnEscChar();
			NextChar();
		}
		else
		{
			constString += c;
			NextChar();
		}
	}

	AddToken(S_STRING_LITERAL,constString);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::OnCharChar()
{
	CString constString;

	NextChar();

	if (c=='\\')
	{
		constString += OnEscChar();
		NextChar();
	}		
	else
	{
		constString += c;
		NextChar();
	}

	if (IsCharChar())
	{
		NextChar();
		AddToken(S_CHAR_LITERAL,constString);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CScanner::OnEscChar()
{
	CString ret;
	NextChar();
	
	ret = "\\";
	ret += c;

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnWhiteSpace()
{
	while ( IsWhiteSpace() && !IsEOF())
	{
		if (m_bStoreWhiteAndComment)
		{
			m_sWhiteAndCommentBuffer += c;
		}
		NextChar();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnCommentSlash()
{
	if (m_bStoreWhiteAndComment)
	{
		m_sWhiteAndCommentBuffer += _T("//");
	}
	while ( !IsEOL() && !IsEOF())
	{	
		if (m_bStoreWhiteAndComment)
		{
			m_sWhiteAndCommentBuffer += c;
		}
		NextChar();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CScanner::OnCommentAstr()
{
	if (m_bStoreWhiteAndComment)
	{
		m_sWhiteAndCommentBuffer += _T("/*");
	}
	while (TRUE)
	{
		if (c=='*')
		{
			if (m_bStoreWhiteAndComment)
			{
				m_sWhiteAndCommentBuffer += c;
			}
			NextChar();
			if (c=='/')
			{
				NextChar();
				break;
			}
			else if (IsEOF())
			{
				return FALSE;
			}
		}
		if (m_bStoreWhiteAndComment)
		{
			m_sWhiteAndCommentBuffer += c;
		}
		NextChar();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CScanner::OnEof()
{
	// append the End of File token
	AddToken(S_EOF,_T(""));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsDigit()
{
	return (c>='0') && (c<='9');	
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsIdChar()
{
	return (c=='_') || (c=='$');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsLetter()
{
	return ((c>='A') && (c<='Z')) || ((c>='a') && (c<='z')) || (c=='_')|| (c=='#');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsHexDigit()
{
	return ((c>='0') && (c<='9')) || ((c>='A') && (c<='F')) || ((c>='a') && (c<='f'));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsOctalDigit()
{
	return (c>='0') && (c<='7');	
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsNonZeroDigit()
{
	return (c>='1') && (c<='9');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsBrace()
{
	return (c=='(') || (c==')') || (c=='[') || (c==']') || (c=='{') || (c=='}');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsWhiteSpace()
{
	return (c=='\b') || (c=='\t') || (c=='\f') || (c=='\r') || (c=='\n') || (c==' ');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsSeparator()
{
	return (c==';') || (c=='.') || (c==',');	
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsDelimiter()
{
	return	(c=='=') || (c=='<') || (c=='>') || (c=='!') || (c=='~') ||
			(c=='?') ||	(c=='+') || (c=='-') || (c=='*') || (c=='/') ||
			(c=='&') || (c=='|') ||	(c=='^') || (c=='%') || (c==':');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsEOL()
{
	return (c=='\n');
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsEOF()
{
	return m_Index == m_MaxRead;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsStringChar()
{
	return c=='\"';
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CScanner::IsCharChar()
{
	return c=='\'';
}
/////////////////////////////////////////////////////////////////////////////
ESymbol CScanner::CheckForReservedWord(const CString& identifier)
{
	if (identifier.IsEmpty())
	{
		return S_IDENTIFIER;
	}
	switch (identifier[0])
	{
	case 'A':
		if (identifier==_T("ACCELERATORS"))	return R_ACCELERATORS;
		if (identifier==_T("ALT")) return R_ALT;

	case 'B':
		if (identifier==_T("BITMAP")) return R_BITMAP;
		if (identifier==_T("BUTTON")) return R_BUTTON;
		if (identifier==_T("BEGIN")) return R_BEGIN;
		if (identifier==_T("BLOCK")) return R_BLOCK;
	case 'b':
		if (identifier==_T("break")) return S_BREAK;

	case 'c':
		if (identifier==_T("case"))	return S_CASE;
		if (identifier==_T("char"))	return S_CHAR;
		if (identifier==_T("class")) return S_CLASS;
		if (identifier==_T("continue"))	return S_CONTINUE;
		if (identifier==_T("const")) return S_CONST;
	case 'C':
		if (identifier==_T("CONTROL")) return R_CONTROL;
		if (identifier==_T("CTEXT")) return R_CTEXT;
		if (identifier==_T("CAPTION")) return R_CAPTION;

	case 'D':
		if (identifier==_T("DISCARDABLE")) return R_DISCARDABLE;
		if (identifier==_T("DIALOG")) return R_DIALOG;
		if (identifier==_T("DIALOGEX")) return R_DIALOGEX;
		if (identifier==_T("DEFPUSHBUTTON")) return R_DEFPUSHBUTTON;
	case 'd':
		if (identifier==_T("default")) return S_DEFAULT;
		if (identifier==_T("do")) return S_DO;
		if (identifier==_T("double")) return S_DOUBLE;
		if (identifier==_T("defined")) return P_DEFINED;

	case 'E':
		if (identifier==_T("END")) return R_END;
	case 'e':
		if (identifier==_T("else"))	return S_ELSE;

	case 'F':
		if (identifier==_T("FONT")) return R_FONT;
	case 'f':
		if (identifier==_T("float")) return S_FLOAT;
		if (identifier==_T("for")) return S_FOR;

	case 'G':
		if (identifier==_T("GROUPBOX")) return R_GROUPBOX;
	case 'g':
		if (identifier==_T("goto"))	return S_GOTO;

	case 'I':
		if (identifier==_T("ICON"))	return R_ICON;
	case 'i':
		if (identifier==_T("if")) return S_IF;
		if (identifier==_T("int")) return S_INT;

	case 'l':
		if (identifier==_T("long"))	return S_LONG;
	case 'L':
		if (identifier==_T("LTEXT")) return R_LTEXT;

	case 'M':
		if (identifier==_T("MENU")) return R_MENU;
		if (identifier==_T("MOVEABLE"))	return R_MOVEABLE;
		if (identifier==_T("MENUITEM"))	return R_MENUITEM;

	case 'n':
		if (identifier==_T("new")) return S_NEW;

	case 'o':
		if (identifier==_T("operator"))	return S_OPERATOR;

	case 'p':
		if (identifier==_T("private")) return S_PRIVATE;
		if (identifier==_T("protected")) return S_PROTECTED;
		if (identifier==_T("public")) return S_PUBLIC;
	case 'P':
		if (identifier==_T("PURE"))	return R_PURE;
		if (identifier==_T("PRELOAD")) return R_PRELOAD;
		if (identifier==_T("POPUP")) return R_POPUP;
		if (identifier==_T("PUSHBUTTON")) return R_PUSHBUTTON;
		if (identifier==_T("PUSHBOX")) return R_PUSHBOX;

	case 'r':
		if (identifier==_T("return")) return S_RETURN;
	case 'R':
		if (identifier==_T("RTEXT")) return R_RTEXT;

	case 's':
		if (identifier==_T("short")) return S_SHORT;
		if (identifier==_T("static")) return S_STATIC;
		if (identifier==_T("switch")) return S_SWITCH;
	case 'S':
		if (identifier==_T("SEPARATOR")) return R_SEPARATOR;
		if (identifier==_T("SHIFT")) return R_SHIFT;
		if (identifier==_T("STYLE")) return R_STYLE;
		if (identifier==_T("STRINGTABLE")) return R_STRINGTABLE;
		if (identifier==_T("SCROLLBAR")) return R_SCROLLBAR;
		if (identifier==_T("STATE3")) return R_STATE3;

	case 't':
		if (identifier==_T("this"))	return S_THIS;
	case 'T':
		if (identifier==_T("TOOLBAR")) return R_TOOLBAR;

	case 'v':
		if (identifier==_T("void"))	return S_VOID;
		if (identifier==_T("volatile"))	return S_VOLATILE;
	case 'V':
		if (identifier==_T("VIRTKEY")) return R_VIRTKEY;
		if (identifier==_T("VERSIONINFO")) return R_VERSIONINFO;
		if (identifier==_T("VALUE")) return R_VALUE;

	case 'w':
		if (identifier==_T("while")) return S_WHILE;

	case '#':
		// pragmas
		if (identifier==_T("#include"))	return P_INCLUDE;
		if (identifier==_T("#define")) return P_DEFINE;
		if (identifier==_T("#ifdef")) return P_IFDEF;
		if (identifier==_T("#else")) return P_ELSE;
		if (identifier==_T("#elsif")) return P_ELSIF;
		if (identifier==_T("#endif")) return P_ENDIF;
		if (identifier==_T("#undef")) return P_UNDEF;
		if (identifier==_T("#ifndef")) return P_IFNDEF;
		if (identifier==_T("#pragma")) return P_PRAGMA;
		if (identifier==_T("#if")) return P_IF;
	}

	// unused reserved words
	if (identifier==_T("bool"))	return S_error;
	if (identifier==_T("future")) return S_error;
	if (identifier==_T("generic")) return S_error;
	if (identifier==_T("inner")) return S_error;
	if (identifier==_T("outer")) return S_error;
	if (identifier==_T("rest"))	return S_error;
	if (identifier==_T("transient")) return S_error;
	if (identifier==_T("var")) return S_error;
	if (identifier==_T("catch")) return S_error;
	if (identifier==_T("byte"))	return S_error;
	if (identifier==_T("throw")) return S_error;
	if (identifier==_T("throws")) return S_error;
	if (identifier==_T("try")) return S_error;

	return S_IDENTIFIER;
}

