/////////////////////////////////////////////////////////////////////////////
#ifndef _TRANSLATE_STRING_H_
#define _TRANSLATE_STRING_H_

#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
enum ResourceStringType {
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
	CTranslateString(ResourceStringType	rst,
					 const CString& sResourceFile,
					 const CString& sDialogID,
					 const CString& sControlID,
					 const CString& sText);
	// constuctor for menu
	CTranslateString(ResourceStringType	rst,
					 const CString& sResourceFile,
					 const CString& sMenuID,
					 int   iPopup,
					 const CString& sItemID,
					 const CString& sText);
	~CTranslateString();
	// access
public:
	const CString ToString();
		  BOOL	  FromString(const CString& str);
	inline const CString& GetID() const;
	inline const int GetNumOfWords() const;

	// attributes
public:
		   const CString CalcID();
	inline const CString GetText();
	inline const CString GetTranslate();
	// operations
public:
	void SetTranslate(CString& sTranslate);
	void Update(CTranslateString* pTS);
	void AutomaticTranslate(const CMapStringToString& mapAutomatic);

	// implementation
protected:
	BOOL IdFrom(const CString& id);
	void CountWords();

	// data
private:
	CString	m_sResourceFile;	// the original file

	CString m_sText;			// the text to translate
	CString m_sTranslate;		// the translated text

	CString m_sID;				// the resource ID, if exists
	ResourceStringType	m_rst;	// the type of res string

	CString m_sDialogID;		// the ID of the dialog containing this control, if ex
	CString m_sMenuID;			// the ID of the menu containing this menu item, if ex
	CString m_sPopup;			// the number of popup in menu

	CTime	m_tScan;			// the time the resource was scanned last time
	CTime	m_tTranslate;		// the time the resource was translated last time
	CTime	m_tInsert;			// the time the resource was inserted last time

	CString m_sFullID;
	int		m_iNumOfWords;

	static char m_cDelimiter;
};
inline const CString CTranslateString::GetText()
{
	return m_sText;
}
inline const CString CTranslateString::GetTranslate()
{
	return m_sTranslate;
}
inline const CString& CTranslateString::GetID() const
{
	return m_sFullID;
}
inline const int CTranslateString::GetNumOfWords() const
{
	return m_iNumOfWords;
}
/////////////////////////////////////////////////////////////////////////////
class CTSArray : public CTypedPtrArray<CPtrArray,CTranslateString*>
{
public:
	CTSArray();
	~CTSArray();

	// operations
public:
	void    DeleteAll();

	BOOL	Save(const CString& sFilename);
	BOOL	ExportAsGlossary(const CString& sPathName);
	BOOL	Load(const CString& sFilename);

	void	AddReplace(CTranslateString* pTS);
	CTranslateString* GetTSFromID(const CString& id); 

	void	AutomaticTranslate(const CMapStringToString& mapAutomatic);
	inline const CString& GetName();
	inline const int GetNumOfWords();

private:
	CString	m_sName;
	int		m_iNumOfWords;
};
inline const CString& CTSArray::GetName()
{
	return m_sName;
}
inline const int CTSArray::GetNumOfWords()
{
	return m_iNumOfWords;
}
/////////////////////////////////////////////////////////////////////////////
CString Enum2Name(ResourceStringType rst);
ResourceStringType Name2Enum(const CString& sName);

#endif

