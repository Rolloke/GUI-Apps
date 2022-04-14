// FileOperations.h: interface for the CFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEOPERATIONS_H__A67F98D1_BBC2_4394_9E64_475C37632F89__INCLUDED_)
#define AFX_FILEOPERATIONS_H__A67F98D1_BBC2_4394_9E64_475C37632F89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void Translator_SplitPathAll(const CString sFullPath,
		  CString &sDrive,
		  CString &sPath, 
		  CString &sFilename,
		  CString &sExt);
void Translator_SplitPath(const CString &sFullPath,
		  CString &sResultPath, 
		  CString &sResultFilename
		  );

int Translator_SearchFiles(
			   CStringArray &result,// adds files no clear!
			   const CString sPathname,
			   const CString &sPattern,
			   BOOL bIncludeDirectories=FALSE
			   );

#endif // !defined(AFX_FILEOPERATIONS_H__A67F98D1_BBC2_4394_9E64_475C37632F89__INCLUDED_)
