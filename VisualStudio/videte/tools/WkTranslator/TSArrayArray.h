/////////////////////////////////////////////////////////////////////////////
#ifndef _TS_ARRAY_ARRAY_H_
#define _TS_ARRAY_ARRAY_H_
// TSArrayArray.h

#include "TranslateString.h"

/////////////////////////////////////////////////////////////////////////////
class CTSArrayArray : public CTypedPtrArray<CPtrArray,CTSArray*>
{
public:
	CTSArrayArray();
	~CTSArrayArray();

	// operations
public:
	void    DeleteAll();

	BOOL	CheckData();
	BOOL	Save(const CString& sDirName);
	BOOL	Load(const CString& sFilePath, BOOL bAllFiles);
	BOOL	ExportAsGlossary(const CString& sPathName, BOOL bGerman);
	BOOL	ExportAsTextOnly(const CString& sPathName);
	BOOL	ExportAsOriginalLengthComment(const CString& sPathName);
	BOOL	ImportFromTxtFile(const CString& sPathName);

	BOOL	AutomaticTranslate(const CMapStringToString& mapAutomatic);
};
#endif
