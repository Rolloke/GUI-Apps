/////////////////////////////////////////////////////////////////////////////
#ifndef _TRANSLATE_STRING_H_
#define _TRANSLATE_STRING_H_

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// string defines
static _TCHAR BASED_CODE szTODO[] = _T("TODO! ");
static _TCHAR BASED_CODE szOriginal[]		= _T("Original");
static _TCHAR BASED_CODE szTranslation[]	= _T("Translation");

/////////////////////////////////////////////////////////////////////////////
enum enumRSType
{
	RST_DEFAULT=0,
	RST_MENUITEM=1,	
	RST_POPUP=2,	
	RST_CAPTION=3,	
	RST_LTEXT=4,	
	RST_CTEXT=5,	
	RST_RTEXT=6,	
	RST_DEFPUSHBUTTON=7,
	RST_PUSHBUTTON=8,
	RST_GROUPBOX=9,
	RST_CONTROL=10,
	RST_PUSHBOX=11,
	RST_SCROLLBAR=12,
	RST_STATE3=13,
	RST_STRING=14,
};


#ifdef _UNICODE
/////////////////////////////////////////////////////////////////////////////
#define TEXTIO_ANSI              0x10000000
#define TEXTIO_MCBS              0x20000000
#define TEXTIO_UNICODE_BIGENDIAN 0x0000fffe
#define TEXTIO_UNICODE           0x0000feff
#define TEXTIO_UNICODE_UTF8      0x00efbbbf
#define CARIAGE_RETURN				_T("\r")
#define LINE_FEED						_T("\n")

/////////////////////////////////////////////////////////////////////////////
class CStdioFileU : public CStdioFile
{
	DECLARE_DYNAMIC(CStdioFileU)
public:
	CStdioFileU();
	CStdioFileU(FILE* pOpenStream);
	CStdioFileU(LPCTSTR lpszFileName, UINT nOpenFlags);
	virtual BOOL ReadString(CString& rString);
	virtual void WriteString(LPCTSTR lpsz);

	bool IsFileAnsi()      { return (m_dwUnicodeFile == TEXTIO_ANSI             ) ? true : false; };
	bool IsFileMCBS()      { return ((m_dwUnicodeFile&TEXTIO_MCBS)==TEXTIO_MCBS ) ? true : false; };
	bool IsFileUnicode()   { return (m_dwUnicodeFile == TEXTIO_UNICODE          ) ? true : false; };
	bool IsFileUnicodeBE() { return (m_dwUnicodeFile == TEXTIO_UNICODE_BIGENDIAN) ? true : false; };
	bool IsFileUndefined() { return (m_dwUnicodeFile == 0                       ) ? true : false; };

	void SetFileAnsi()    { m_dwUnicodeFile = TEXTIO_ANSI;    };
	void SetFileMCBS(WORD wCodePage)    { m_dwUnicodeFile = TEXTIO_MCBS|wCodePage;};
	void SetFileUnicode() { m_dwUnicodeFile = TEXTIO_UNICODE; };

private:
	DWORD m_dwUnicodeFile;
	fpos_t m_fposition;
};

#endif // _UNICODE

/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////

CString Enum2Name(enumRSType rst);
enumRSType Name2Enum(const CString& sName);

/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
class CTranslateString
{
// construction
public:
	CTranslateString();

	// constuctor for string
	CTranslateString(const CString& sResourceFile,
					 const CString& sID,
					 const CString& sText);

	// constuctor for dialog control
	CTranslateString(enumRSType	rst,
					 const CString& sResourceFile,
					 const CString& sDialogID,
					 const CString& sControlID,
					 const CString& sText);
	// constuctor for menu
	CTranslateString(enumRSType	rst,
					 const CString& sResourceFile,
					 const CString& sMenuID,
					 int   iPopup,
					 const CString& sItemID,
					 const CString& sText);
	~CTranslateString();

// Helper functions
	static	BOOL	CheckDataAll(CString& sOriginal, CString& sTranslation, UINT& uMaxLength);
	static	BOOL	CheckFormatSpecifier(CString& sOriginal, CString& sTranslation);
	static	BOOL	CheckMaxLength(CString& sTranslation, UINT uMaxLength);
	static	BOOL	CheckSpecialStartChars(CString& sOriginal, CString& sTranslation, CString& sStartChars);
	static	BOOL	CheckSpecialCharsToAvoid(CString& sOriginal, CString& sTranslation, CString& sChars);
	static	BOOL	CheckTODO(CString& sTranslation);

// access
public:

// attributes
public:
	inline const CString	GetText() const;
		   const CString	GetTranslate(BOOL bIgnoreTODO = FALSE) const;
	inline const UINT		GetMaxLength() const;
	inline const CString	GetComment() const;
	inline const CString&	GetResourceID() const;
	inline const CString&	GetFullID() const;
	inline enumRSType		GetRSType() const;
	inline const int		GetNumOfWords() const;
				 BOOL		CheckTODO();
// operations
public:
	const CString GetSuggestion();
	void SetTranslate(const CString& sTranslate);
	void SetMaxLength(UINT uMaxLength);
	void SetComment(const CString& sComment);

	void			Update(CTranslateString* pTS);
	BOOL			AutomaticTranslate(const CMapStringToString& mapAutomatic);
	BOOL			CheckData();
	const CString	ToString();
	BOOL			FromString(const CString& str);

// implementation
protected:
	const CString	CalcFullID();
	BOOL			IdFrom(const CString& id);
	void			CountWords();

// data
private:
	CString		m_sResourceFile;	// the original file

	CString		m_sText;			// the text to translate
	CString		m_sTranslate;		// the translated text
	CString		m_sSuggestion;		// the suggestion for

	UINT		m_uMaxLength;		// maximum length of text
	CString		m_sComment;			// comment to maximum length

	CString		m_sResourceID;		// the resource ID, if exists
	enumRSType	m_rst;				// the type of res string

	CString		m_sDialogID;		// the ID of the dialog containing this control, if ex
	CString		m_sMenuID;			// the ID of the menu containing this menu item, if ex
	CString		m_sPopup;			// the number of popup in menu

///	CTime		m_tScan;			// the time the resource was scanned last time
//	CTime		m_tTranslate;		// the time the resource was translated last time
//	CTime		m_tInsert;			// the time the resource was inserted last time

	CString		m_sFullID;
	int			m_iNumOfWords;

	static _TCHAR m_cDelimiter;
public:
	static BOOL gm_bReplace;
	static HWND gm_hProgress;
};
/////////////////////////////////////////////////////////////////////////////
inline const CString CTranslateString::GetText() const
{
	return m_sText;
}
/////////////////////////////////////////////////////////////////////////////
inline const UINT CTranslateString::GetMaxLength() const
{
	return m_uMaxLength;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CTranslateString::GetComment() const
{
	return m_sComment;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CTranslateString::GetResourceID() const
{
	return m_sResourceID;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CTranslateString::GetFullID() const
{
	return m_sFullID;
}
/////////////////////////////////////////////////////////////////////////////
inline enumRSType CTranslateString::GetRSType() const
{
	return m_rst;
}
/////////////////////////////////////////////////////////////////////////////
inline const int CTranslateString::GetNumOfWords() const
{
	return m_iNumOfWords;
}
/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
class CTSArray : public CTypedPtrArray<CPtrArray,CTranslateString*>
{
public:
	CTSArray();
	~CTSArray();

// attributes
public:
	inline const CString&	GetName();
	inline const int		GetNumOfWords();

	// operations
public:
	BOOL	CheckData();
	int		Load(const CString& sFilename);
	BOOL	Save(const CString& sFilename);
	BOOL	ExportAsGlossary(const CString& sPathName, BOOL bGerman);
	BOOL	ExportAsTextOnly(const CString& sPathName);
	BOOL	ExportAsOriginalLengthComment(const CString& sPathName);
	BOOL	ImportFromTxtFile(const CString& sPathName);
	void	AddReplace(CTranslateString* pTS);
	BOOL	AutomaticTranslate(const CMapStringToString& mapAutomatic);
	void    DeleteAll();

	CTranslateString* GetTSFromFullID(const CString& id); 
	CTranslateString* GetTSFromResourceID(const CString& id); 

// data
private:
	CString	m_sName;
	int		m_iNumOfWords;
};
/////////////////////////////////////////////////////////////////////////////
inline const CString& CTSArray::GetName()
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////////////
inline const int CTSArray::GetNumOfWords()
{
	return m_iNumOfWords;
}
/////////////////////////////////////////////////////////////////////////////
#endif

