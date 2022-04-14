// Parser.h

#ifndef _PARSER_H_
#define _PARSER_H_

#include <afxtempl.h>

class CScanner;
class CTSArray;

class CParser
{ 
	// construction
public:
	// constructor for extracting the resource strings
	CParser(CScanner* pScanner, CTSArray* pTSArray, CTSArray* pTSArrayEnu, BOOL bCL);
	// constructor for inserting the resource strings
	CParser(CScanner* pScannerSource, CTSArray* pTSArrayReplace, 
			const CString& destinationFile, BOOL bCL);
	~CParser();

public:
	BOOL Parse(BOOL bIgnoreTODO = FALSE);


private:
	CScanner* m_pScanner;
	CTSArray* m_pTSArray;		// Translation table (array)
	CTSArray* m_pTSArrayEnu;	// Additional array for Length & Comment info, can be NULL
	BOOL	  m_bCommandLine;	// Called from Commandline, avoid Msg-Boxes
	CString	  m_sDestRCFile;	// Destination RC file

	enum enumCondition
	{
		C_ERROR,
		C_READ,
		C_STRINGTABLE,
		C_STRINGTABLE_BEGIN,
		C_STRINGTABLE_IDENTIFIER,
		C_STRINGTABLE_END,
		C_MENU,
		C_DIALOG,
		C_DIALOG_ID,
		C_DIALOG_CAPTION,
		C_DIALOG_BEGIN,
		C_DIALOG_CONTROL,
		C_DIALOG_CONTROL_COMMA,
		C_DIALOG_CONTROL_COMMA_IDENTIFIER,
		C_MENU_ID,
		C_MENU_BEGIN,
		C_MENU_END,
		C_POPUP,
		C_MENUITEM,
		C_MENUITEM_STR,
		C_MENUITEM_STR_COMMA,
		C_POPUP_STR,
		C_POPUP_BEGIN,
		C_POPUP_MENUITEM,
		C_POPUP_MENUITEM_STR,
		C_POPUP_MENUITEM_STR_COMMA,
		C_POPUP_MENU_ITEM_STR_COMMA_IDENTIFIER,
	};

};

#endif
