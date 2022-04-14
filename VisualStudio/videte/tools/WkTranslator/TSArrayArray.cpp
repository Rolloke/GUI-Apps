// TSArrayArray.cpp

#include "stdafx.h"
#include "TSArrayArray.h"

/////////////////////////////////////////////////////////////////////////////
CTSArrayArray::CTSArrayArray()
{
}
/////////////////////////////////////////////////////////////////////////////
CTSArrayArray::~CTSArrayArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CTSArrayArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAt(i)) 
		{ 
			delete (CTSArray*) GetAt(i); 
		} 
	} 
	RemoveAll(); 
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::CheckData()
{
	BOOL bReturn = TRUE;
	CTSArray* pTSA;

	for (int i=0 ; bReturn && i<GetSize() ; i++)
	{
		pTSA = GetAt(i);
		bReturn = pTSA->CheckData();
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::Save(const CString& sDirName)
{
	BOOL bReturn = TRUE;
	int i;
	CTSArray* pTSA;
	CString sPathName;

	for (i=0;i<GetSize();i++)
	{
		pTSA = GetAt(i);
		sPathName = sDirName + '\\' + pTSA->GetName();
		bReturn &= pTSA->Save(sPathName);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::ExportAsGlossary(const CString& sPathName, BOOL bGerman)
{
	BOOL bReturn = TRUE;
	int i;
	CTSArray* pTSA;

	for (i=0;i<GetSize();i++)
	{
		pTSA = GetAt(i);
		bReturn &= pTSA->ExportAsGlossary(sPathName, bGerman);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::ExportAsTextOnly(const CString& sPathName)
{
	BOOL bReturn = TRUE;
	int i;
	CTSArray* pTSA;

	for (i=0;i<GetSize();i++)
	{
		pTSA = GetAt(i);
		bReturn &= pTSA->ExportAsTextOnly(sPathName);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::ExportAsOriginalLengthComment(const CString& sPathName)
{
	BOOL bReturn = TRUE;
	int i;
	CTSArray* pTSA;

	for (i=0;i<GetSize();i++)
	{
		pTSA = GetAt(i);
		bReturn &= pTSA->ExportAsOriginalLengthComment(sPathName);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::ImportFromTxtFile(const CString& sPathName)
{
	BOOL bReturn = TRUE;
	int i;
	CTSArray* pTSA;

	for (i=0;i<GetSize();i++)
	{
		pTSA = GetAt(i);
		bReturn &= pTSA->ImportFromTxtFile(sPathName);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::Load(const CString& sFilePath, BOOL bAllFiles)
{
	CStringArray saFiles;
	CString sPathName, sDirName, sFileName;
	CTSArray* pTSA;
	int i,c;

	Translator_SplitPath(sFilePath, sDirName, sFileName);

	if (bAllFiles)
	{
		// Lade alle lng-Dateien im Verzeichnis
//		WK_TRACE(_T("Load all lng-files in directory=%s\n"),(const char*)sDirName);
		Translator_SearchFiles(saFiles,sDirName,_T("*.lng"),FALSE);
	}
	else
	{
		saFiles.Add(sFileName);
	}

	c = saFiles.GetSize();
	for (i=0;i<c;i++)
	{
		pTSA = new CTSArray();
		sPathName = sDirName + '\\' + saFiles[i];
		if (pTSA->Load(sPathName))
		{
			Add(pTSA);
		}
		else
		{
			delete pTSA;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArrayArray::AutomaticTranslate(const CMapStringToString& mapAutomatic)
{
	BOOL bReturn = 0;

	for (int i=0;i<GetSize();i++)
	{
		bReturn += GetAt(i)->AutomaticTranslate(mapAutomatic);
	}
	return bReturn;
}
