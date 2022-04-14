// Scanner.cpp
#include "stdafx.h"
#include "Scanner.h"
/////////////////////////////////////////////////////////////////////////////
CString NameOfEnum(ESymbol es)
{
	CString ret;
	switch (es)
	{
		case S_EOF: ret = "S_EOF";break;
		case S_error: ret = "S_error";break;
		case S_ABSTRACT: ret = "S_ABSTRACT";break;
		case S_BOOLEAN: ret = "S_BOOLEAN";break;
		case S_BREAK: ret = "S_BREAK";break;
		case S_BYTE: ret = "S_BYTE";break;
		case S_CASE: ret = "S_CASE";break;
		case S_CATCH: ret = "S_CATCH";break;
		case S_CHAR: ret = "S_CHAR";break;
		case S_CLASS: ret = "S_CLASS";break;
		case S_CONTINUE: ret = "S_CONTINUE";break;
		case S_DEFAULT: ret = "S_DEFAULT";break;
		case S_DO: ret = "S_DO";break;
		case S_DOUBLE: ret = "S_DOUBLE";break;
		case S_ELSE: ret = "S_ELSE";break;
		case S_EXTENDS: ret = "S_EXTENDS";break;
		case S_FINAL: ret = "S_FINAL";break;
		case S_FINALLY: ret = "S_FINALLY";break;
 		case S_FLOAT: ret = "S_FLOAT";break;
 		case S_FOR: ret = "S_FOR";break;
 		case S_IF: ret = "S_IF";break;
 		case S_IMPLEMENTS: ret = "S_IMPLEMENTS";break;
 		case S_IMPORT: ret = "S_IMPORT";break;
 		case S_INSTANCEOF: ret = "S_INSTANCEOF";break;
 		case S_INT: ret = "S_INT";break;
 		case S_INTERFACE: ret = "S_INTERFACE";break;
 		case S_NATIVE: ret = "S_NATIVE";break;
 		case S_LONG: ret = "S_LONG";break;
 		case S_NEW: ret = "S_NEW";break;
 		case S_NULL: ret = "S_NULL";break;
 		case S_PACKAGE: ret = "S_PACKAGE";break;
 		case S_PRIVATE: ret = "S_PRIVATE";break;
 		case S_PROTECTED: ret = "S_PROTECTED";break;
 		case S_PUBLIC: ret = "S_PUBLIC";break;
 		case S_RETURN: ret = "S_RETURN";break;
 		case S_SHORT: ret = "S_SHORT";break;
 		case S_STATIC: ret = "S_STATIC";break;
 		case S_SUPER: ret = "S_SUPER";break;
 		case S_SWITCH: ret = "S_SWITCH";break;
 		case S_SYNCHRONIZED: ret = "S_SYNCHRONIZED";break;
 		case S_THIS: ret = "S_THIS";break;
 		case S_THROW: ret = "S_THROW";break;
 		case S_THROWS: ret = "S_THROWS";break;
 		case S_TRY: ret = "S_TRY";break;
 		case S_VOID: ret = "S_VOID";break;
 		case S_WHILE: ret = "S_WHILE";break;
 		case S_PLUS: ret = "S_PLUS";break;
 		case S_MINUS: ret = "S_MINUS";break;
 		case S_STAR: ret = "S_STAR";break;
 		case S_OP_DIV: ret = "S_OP_DIV";break;
 		case S_OP_MOD: ret = "S_OP_MOD";break;
 		case S_OP_AND: ret = "S_OP_AND";break;
 		case S_OP_OR: ret = "S_OP_OR";break;
 		case S_OP_XOR: ret = "S_OP_XOR";break;
 		case S_OP_INC: ret = "S_OP_INC";break;
 		case S_OP_DEC: ret = "S_OP_DEC";break;
 		case S_OP_SHL: ret = "S_OP_SHL";break;
 		case S_OP_SHR: ret = "S_OP_SHR";break;
 		case S_OP_SHRR: ret = "S_OP_SHRR";break;
 		case S_OP_GE: ret = "S_OP_GE";break;
 		case S_OP_LE: ret = "S_OP_LE";break;
 		case S_OP_EQ: ret = "S_OP_EQ";break;
 		case S_OP_NE: ret = "S_OP_NE";break;
 		case S_OP_G: ret = "S_OP_G";break;
 		case S_OP_L: ret = "S_OP_L";break;
 		case S_OP_LAND: ret = "S_OP_LAND";break;
 		case S_OP_LOR: ret = "S_OP_LOR";break;
 		case S_OP_DIM: ret = "S_OP_DIM";break;
 		case S_ASS_MUL: ret = "S_ASS_MUL";break;
 		case S_ASS_DIV: ret = "S_ASS_DIV";break;
 		case S_ASS_MOD: ret = "S_ASS_MOD";break;
 		case S_ASS_ADD: ret = "S_ASS_ADD";break;
 		case S_ASS_SUB: ret = "S_ASS_SUB";break;
 		case S_ASS_SHL: ret = "S_ASS_SHL";break;
 		case S_ASS_SHR: ret = "S_ASS_SHR";break;
 		case S_ASS_SHRR: ret = "S_ASS_SHRR";break;
 		case S_ASS_AND: ret = "S_ASS_AND";break;
 		case S_ASS_XOR: ret = "S_ASS_XOR";break;
 		case S_ASS_OR: ret = "S_ASS_OR";break;
 		case S_IDENTIFIER: ret = "S_IDENTIFIER";break;
 		case S_LITERAL: ret = "S_LITERAL";break;
 		case S_STRING_LITERAL: ret = "S_STRING_LITERAL";break;
 		case S_CHAR_LITERAL: ret = "S_CHAR_LITERAL";break;
 		case S_COMMA: ret = "S_COMMA";break;
 		case S_SEMICOLON: ret = "S_SEMICOLON";break;
 		case S_POINT: ret = "S_POINT";break;
 		case S_EQUALS_SIGN: ret = "S_EQUALS_SIGN";break;
 		case S_COLON: ret = "S_COLON";break;
 		case S_QUESTION_MARK: ret = "S_QUESTION_MARK";break;
 		case S_TILDE: ret = "S_TILDE";break;
 		case S_EXCLAMATION: ret = "S_EXCLAMATION";break;
 		case S_OPEN1: ret = "S_OPEN1";break;
 		case S_CLOSE1: ret = "S_CLOSE1";break;
 		case S_OPEN2: ret = "S_OPEN2";break;
 		case S_CLOSE2: ret = "S_CLOSE2";break;
 		case S_OPEN3: ret = "S_OPEN3";break;
 		case S_CLOSE3: ret = "S_CLOSE3";break;
 		case S_CONST: ret = "S_CONST";break;
 		case S_GOTO: ret = "S_GOTO";break;
 		case S_OPERATOR: ret = "S_OPERATOR";break;
 		case S_VOLATILE: ret = "S_VOLATILE";break;

 		case R_ICON: ret = "R_ICON";break;
 		case R_DISCARDABLE: ret = "R_DISCARDABLE";break;
 		case R_BITMAP: ret = "R_BITMAP";break;
 		case R_MOVEABLE: ret = "R_MOVEABLE";break;
 		case R_PURE: ret = "R_PURE";break;
 		case R_TOOLBAR: ret = "R_TOOLBAR";break;
 		case R_SEPARATOR: ret = "R_SEPARATOR";break;
 		case R_BUTTON: ret = "R_BUTTON";break;
 		case R_PRELOAD: ret = "R_PRELOAD";break;
 		case R_BEGIN: ret = "R_BEGIN";break;
 		case R_END: ret = "R_END";break;
 		case R_POPUP: ret = "R_POPUP";break;
 		case R_MENUITEM: ret = "R_MENUITEM";break;
 		case R_ACCELERATORS: ret = "R_ACCELERATORS";break;
 		case R_VIRTKEY: ret = "R_VIRTKEY";break;
 		case R_ALT: ret = "R_ALT";break;
 		case R_SHIFT: ret = "R_SHIFT";break;
 		case R_CONTROL: ret = "R_CONTROL";break;
 		case R_DIALOG: ret = "R_DIALOG";break;
 		case R_DIALOGEX: ret = "R_DIALOGEX";break;
 		case R_STYLE: ret = "R_STYLE";break;
 		case R_FONT: ret = "R_FONT";break;
 		case R_LTEXT: ret = "R_LTEXT";break;
 		case R_CTEXT: ret = "R_CTEXT";break;
 		case R_RTEXT: ret = "R_RTEXT";break;
 		case R_DEFPUSHBUTTON: ret = "R_DEFPUSHBUTTON";break;
 		case R_PUSHBUTTON: ret = "R_PUSHBUTTON";break;
 		case R_VERSIONINFO: ret = "R_VERSIONINFO";break;
 		case R_BLOCK: ret = "R_BLOCK";break;
 		case R_VALUE: ret = "R_VALUE";break;
 		case R_STRINGTABLE: ret = "R_STRINGTABLE";break;
 		case R_CAPTION: ret = "R_CAPTION";break;
 		case R_GROUPBOX: ret = "R_GROUPBOX";break;
 		case R_PUSHBOX: ret = "R_PUSHBOX";break;
 		case R_SCROLLBAR: ret = "R_SCROLLBAR";break;
 		case R_STATE3: ret = "R_STATE3";break;
 		case R_MENU: ret = "R_MENU";break;

 		case P_INCLUDE: ret = "P_INCLUDE";break;
 		case P_DEFINE: ret = "P_DEFINE";break;
 		case P_IFDEF: ret = "P_IFDEF";break;
 		case P_ELSE: ret = "P_ELSE";break;
 		case P_ENDIF: ret = "P_ENDIF";break;
 		case P_UNDEF: ret = "P_UNDEF";break;
 		case P_IFNDEF: ret = "P_IFNDEF";break;
 		case P_PRAGMA: ret = "P_PRAGMA";break;
 		case P_ELSIF: ret = "P_ELSIF";break;
 		case P_IF: ret = "P_IF";break;
 		case P_DEFINED: ret = "P_DEFINED";break;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
CToken::CToken(ESymbol sym, const CString& str, int l)
{
	m_Symbol = sym;
	m_Original = str;
	m_Line = l;
}
/////////////////////////////////////////////////////////////////////////////
CToken::CToken(ESymbol sym, const CString& str, const CString& white, int l)
{
	m_Symbol = sym;
	m_Original = str;
	m_Line = l;
	m_WhiteAndComment = white;
}
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
	CFileStatus cfs;
	CFileException cfe;
	int len;

	if (CFile::GetStatus(m_PathName,cfs))
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
inline char CScanner::NextChar()
{
	m_Index++;
	if (m_Index==m_MaxRead)
	{
		c = (char)(-1);
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
		m_sWhiteAndCommentBuffer += "//";
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
		m_sWhiteAndCommentBuffer += "/*";
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
	AddToken(S_EOF,"");
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
		if (identifier=="ACCELERATORS")	return R_ACCELERATORS;
		if (identifier=="ALT") return R_ALT;

	case 'B':
		if (identifier=="BITMAP") return R_BITMAP;
		if (identifier=="BUTTON") return R_BUTTON;
		if (identifier=="BEGIN") return R_BEGIN;
		if (identifier=="BLOCK") return R_BLOCK;
	case 'b':
		if (identifier=="break") return S_BREAK;

	case 'c':
		if (identifier=="case")	return S_CASE;
		if (identifier=="char")	return S_CHAR;
		if (identifier=="class") return S_CLASS;
		if (identifier=="continue")	return S_CONTINUE;
		if (identifier=="const") return S_CONST;
	case 'C':
		if (identifier=="CONTROL") return R_CONTROL;
		if (identifier=="CTEXT") return R_CTEXT;
		if (identifier=="CAPTION") return R_CAPTION;

	case 'D':
		if (identifier=="DISCARDABLE") return R_DISCARDABLE;
		if (identifier=="DIALOG") return R_DIALOG;
		if (identifier=="DIALOGEX") return R_DIALOGEX;
		if (identifier=="DEFPUSHBUTTON") return R_DEFPUSHBUTTON;
	case 'd':
		if (identifier=="default") return S_DEFAULT;
		if (identifier=="do") return S_DO;
		if (identifier=="double") return S_DOUBLE;
		if (identifier=="defined") return P_DEFINED;

	case 'E':
		if (identifier=="END") return R_END;
	case 'e':
		if (identifier=="else")	return S_ELSE;

	case 'F':
		if (identifier=="FONT") return R_FONT;
	case 'f':
		if (identifier=="float") return S_FLOAT;
		if (identifier=="for") return S_FOR;

	case 'G':
		if (identifier=="GROUPBOX") return R_GROUPBOX;
	case 'g':
		if (identifier=="goto")	return S_GOTO;

	case 'I':
		if (identifier=="ICON")	return R_ICON;
	case 'i':
		if (identifier=="if") return S_IF;
		if (identifier=="int") return S_INT;

	case 'l':
		if (identifier=="long")	return S_LONG;
	case 'L':
		if (identifier=="LTEXT") return R_LTEXT;

	case 'M':
		if (identifier=="MENU") return R_MENU;
		if (identifier=="MOVEABLE")	return R_MOVEABLE;
		if (identifier=="MENUITEM")	return R_MENUITEM;

	case 'n':
		if (identifier=="new") return S_NEW;

	case 'o':
		if (identifier=="operator")	return S_OPERATOR;

	case 'p':
		if (identifier=="private") return S_PRIVATE;
		if (identifier=="protected") return S_PROTECTED;
		if (identifier=="public") return S_PUBLIC;
	case 'P':
		if (identifier=="PURE")	return R_PURE;
		if (identifier=="PRELOAD") return R_PRELOAD;
		if (identifier=="POPUP") return R_POPUP;
		if (identifier=="PUSHBUTTON") return R_PUSHBUTTON;
		if (identifier=="PUSHBOX") return R_PUSHBOX;

	case 'r':
		if (identifier=="return") return S_RETURN;
	case 'R':
		if (identifier=="RTEXT") return R_RTEXT;

	case 's':
		if (identifier=="short") return S_SHORT;
		if (identifier=="static") return S_STATIC;
		if (identifier=="switch") return S_SWITCH;
	case 'S':
		if (identifier=="SEPARATOR") return R_SEPARATOR;
		if (identifier=="SHIFT") return R_SHIFT;
		if (identifier=="STYLE") return R_STYLE;
		if (identifier=="STRINGTABLE") return R_STRINGTABLE;
		if (identifier=="SCROLLBAR") return R_SCROLLBAR;
		if (identifier=="STATE3") return R_STATE3;

	case 't':
		if (identifier=="this")	return S_THIS;
	case 'T':
		if (identifier=="TOOLBAR") return R_TOOLBAR;

	case 'v':
		if (identifier=="void")	return S_VOID;
		if (identifier=="volatile")	return S_VOLATILE;
	case 'V':
		if (identifier=="VIRTKEY") return R_VIRTKEY;
		if (identifier=="VERSIONINFO") return R_VERSIONINFO;
		if (identifier=="VALUE") return R_VALUE;

	case 'w':
		if (identifier=="while") return S_WHILE;

	case '#':
		// pragmas
		if (identifier=="#include")	return P_INCLUDE;
		if (identifier=="#define") return P_DEFINE;
		if (identifier=="#ifdef") return P_IFDEF;
		if (identifier=="#else") return P_ELSE;
		if (identifier=="#elsif") return P_ELSIF;
		if (identifier=="#endif") return P_ENDIF;
		if (identifier=="#undef") return P_UNDEF;
		if (identifier=="#ifndef") return P_IFNDEF;
		if (identifier=="#pragma") return P_PRAGMA;
		if (identifier=="#if") return P_IF;
	}

	// unused reserved words
	if (identifier=="bool")	return S_error;
	if (identifier=="future") return S_error;
	if (identifier=="generic") return S_error;
	if (identifier=="inner") return S_error;
	if (identifier=="outer") return S_error;
	if (identifier=="rest")	return S_error;
	if (identifier=="transient") return S_error;
	if (identifier=="var") return S_error;
	if (identifier=="catch") return S_error;
	if (identifier=="byte")	return S_error;
	if (identifier=="throw") return S_error;
	if (identifier=="throws") return S_error;
	if (identifier=="try") return S_error;

	return S_IDENTIFIER;
}

