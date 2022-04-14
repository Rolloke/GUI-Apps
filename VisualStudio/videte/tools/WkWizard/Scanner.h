// Scanner.h

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <afxtempl.h>

enum ESymbol 
{
	S_EOF = 0,
	S_error = 1,
	S_ABSTRACT = 2,
	S_BOOLEAN = 3,
	S_BREAK = 4,
	S_BYTE = 5,
	S_CASE = 6,
	S_CATCH = 7,
	S_CHAR = 8,
	S_CLASS = 9,
	S_CONTINUE = 10,
	S_DEFAULT = 11,
	S_DO = 12,
	S_DOUBLE = 13,
	S_ELSE = 14,
	S_EXTENDS = 15,
	S_FINAL = 16,
	S_FINALLY = 17,
	S_FLOAT = 18,
	S_FOR = 19,
	S_IF = 20,
	S_IMPLEMENTS = 21,
	S_IMPORT = 22,
	S_INSTANCEOF = 23,
	S_INT = 24,
	S_INTERFACE = 25,
	S_NATIVE = 27,
	S_LONG = 26,
	S_NEW = 28,
	S_NULL = 29,
	S_PACKAGE = 30,
	S_PRIVATE = 31,
	S_PROTECTED = 32,
	S_PUBLIC = 33,
	S_RETURN = 34,
	S_SHORT = 35,
	S_STATIC = 36,
	S_SUPER = 37,
	S_SWITCH = 38,
	S_SYNCHRONIZED = 39,
	S_THIS = 40,
	S_THROW = 41,
	S_THROWS = 42,
	S_TRY = 43,
	S_VOID = 44,
	S_WHILE = 45,
	S_PLUS = 46,
	S_MINUS = 47,
	S_STAR = 48,
	S_OP_DIV = 49,
	S_OP_MOD = 50,
	S_OP_AND = 51,
	S_OP_OR = 52,
	S_OP_XOR = 53,
	S_OP_INC = 54,
	S_OP_DEC = 55,
	S_OP_SHL = 56,
	S_OP_SHR = 57,
	S_OP_SHRR = 58,
	S_OP_GE = 59,
	S_OP_LE = 60,
	S_OP_EQ = 61,
	S_OP_NE = 62,
	S_OP_G = 63,
	S_OP_L = 64,
	S_OP_LAND = 65,
	S_OP_LOR = 66,
	S_OP_DIM = 67,
	S_ASS_MUL = 68,
	S_ASS_DIV = 69,
	S_ASS_MOD = 70,
	S_ASS_ADD = 71,
	S_ASS_SUB = 72,
	S_ASS_SHL = 73,
	S_ASS_SHR = 74,
	S_ASS_SHRR = 75,
	S_ASS_AND = 76,
	S_ASS_XOR = 77,
	S_ASS_OR = 78,
	S_IDENTIFIER = 79,
	S_LITERAL = 80,
	S_COMMA = 81,
	S_SEMICOLON = 82,
	S_POINT = 83,
	S_EQUALS_SIGN = 84,
	S_COLON = 85,
	S_QUESTION_MARK = 86,
	S_TILDE = 87,
	S_EXCLAMATION = 88,
	S_OPEN1 = 89,
	S_CLOSE1 = 90,
	S_OPEN2 = 91,
	S_CLOSE2 = 92,
	S_OPEN3 = 93,
	S_CLOSE3 = 94,
	S_CONST = 95,
	S_GOTO = 96,
	S_OPERATOR = 97,
	S_VOLATILE = 98,
	S_STRING_LITERAL = 99,
	S_CHAR_LITERAL = 100,

	R_DISCARDABLE = 101,
	R_BITMAP = 102,
	R_MOVEABLE = 103,
	R_PURE = 104,
	R_TOOLBAR = 105,
	R_SEPARATOR = 106,
	R_BUTTON = 107,
	R_PRELOAD = 108,
	R_BEGIN = 109,
	R_END = 110,
	R_POPUP = 111,
	R_MENUITEM = 112,
	R_ACCELERATORS = 113,
	R_VIRTKEY = 114,
	R_ALT = 115,
	R_SHIFT = 116,
	R_CONTROL = 117,
	R_DIALOG = 118,
	R_STYLE = 119,
	R_FONT = 120,
	R_LTEXT = 121,
	R_CTEXT = 122,
	R_RTEXT = 123,
	R_DEFPUSHBUTTON = 124,
	R_VERSIONINFO = 125,
	R_BLOCK = 126,
	R_VALUE = 127,
	R_STRINGTABLE = 128,
	R_CAPTION = 129,
	R_GROUPBOX = 130,
	R_PUSHBOX = 131,
	R_SCROLLBAR = 132,
	R_STATE3 = 133,
	R_MENU = 134,
	R_PUSHBUTTON = 135,
	R_ICON = 136,
	R_DIALOGEX = 137,

	P_INCLUDE = 200,
	P_DEFINE = 201,
	P_IFDEF = 202,
	P_ELSE = 203,
	P_ENDIF = 204,
	P_UNDEF = 205,
	P_IFNDEF = 206,
	P_PRAGMA = 207,
	P_ELSIF = 208,
	P_IF = 209,
	P_DEFINED = 210,

};
/////////////////////////////////////////////////////////////////////////////
CString NameOfEnum(ESymbol es);
/////////////////////////////////////////////////////////////////////////////
class CToken
{
	// construction
public:
	CToken(ESymbol sym, const CString& str, int l);
	CToken(ESymbol sym, const CString& str, const CString& white, int l);

	// attributes
public:
	inline ESymbol	GetSymbol();
	inline CString	GetOriginal();
	inline int		GetLine();
	inline CString	GetSymbolName();
	inline CString	GetWhiteAndComment();

private:
	ESymbol	m_Symbol;
	CString m_Original;
	int		m_Line;
	CString m_WhiteAndComment;
};
inline ESymbol CToken::GetSymbol()
{
	return m_Symbol;
}
inline CString CToken::GetOriginal()
{
	return m_Original;
}
inline int CToken::GetLine()
{
	return m_Line;
}
inline CString CToken::GetSymbolName()
{
	return NameOfEnum(m_Symbol);
}
inline CString CToken::GetWhiteAndComment()
{
	return m_WhiteAndComment;
}
/////////////////////////////////////////////////////////////////////////////
class CTokenArray : public CTypedPtrArray<CPtrArray,CToken*>
{
public:
	CTokenArray();
	~CTokenArray();

	// operations
public:
	void    DeleteAll();
};
/////////////////////////////////////////////////////////////////////////////

class CScanner
{ 
	// construction
public:
	CScanner(const CString& sourceFileName, BOOL bStoreWhiteAndComment = FALSE);
	~CScanner();

	// attributes
public:
	CString GetFileName();
	CString GetPathName();
	// operations
public:
	BOOL Scan();
	void BackToFirst();
	CToken* NextToken();
	const CTokenArray* GetTokens();
	inline const int GetCurrentTokenIndex();

protected:
	BOOL Read();
	inline char NextChar();
	
	
	BOOL OnLetter();
	BOOL OnDigit();
	void OnBrace();
	void OnSeparator();
	BOOL OnDelimiter();
	void OnStringChar();
	BOOL OnCharChar();
	CString OnEscChar();
	void OnWhiteSpace();
	void OnCommentSlash();
	BOOL OnCommentAstr();
	void OnEof();

protected:
	inline BOOL IsDigit();
	inline BOOL IsIdChar();
	inline BOOL IsLetter();
	inline BOOL IsHexDigit();
	inline BOOL IsOctalDigit();
	inline BOOL IsNonZeroDigit();
	inline BOOL IsBrace();
	inline BOOL IsWhiteSpace();
	inline BOOL IsSeparator();
	inline BOOL IsDelimiter();
	inline BOOL IsEOL();
	inline BOOL IsEOF();
	inline BOOL IsStringChar();
	inline BOOL IsCharChar();
	ESymbol CheckForReservedWord(const CString& identifier);

	void AddToken(ESymbol sym, const CString& str);

	// data members
private:
	CString m_FileName;
	CString m_PathName;
	PBYTE	m_pszSource;
	int m_Index;
	int m_MaxRead;
	int m_CurrentLine;
	char c;
	int m_iPos;
	CTokenArray m_TokenList;
	BOOL m_bStoreWhiteAndComment;

	CString m_sWhiteAndCommentBuffer;
};
inline const int CScanner::GetCurrentTokenIndex()
{
	return m_iPos;
}
/////////////////////////////////////////////////////////////////////////
#endif