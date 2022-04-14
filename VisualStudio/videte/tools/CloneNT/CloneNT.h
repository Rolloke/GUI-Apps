
#if !defined(AFX_CLONENT_H__B63E2DE6_8518_4381_A1BB_54CAB78FB358__INCLUDED_)
#define AFX_CLONENT_H__B63E2DE6_8518_4381_A1BB_54CAB78FB358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

class CloneNT
{
public:
	CloneNT(const CString& sSourceDir, const CString& sTargetDir, BOOL bStartRDisk);
	virtual ~CloneNT(); 
	BOOL CloneWinNT();

private:
	BOOL CreateDirStructure();
	BOOL CopyFileList();
	BOOL CopyFile(const CString &sFrom, const CString &sTo);
	BOOL ExpandLockedFiles();
	BOOL ExtractResource(int nResourceID, char*& pResource, DWORD& dwResource);
	BOOL ExpandAndCopy(const CString& sSource, const CString& sTarget);
	BOOL ModifyBootIni();

private:
	CString m_sSourceDir;
	CString m_sTargetDir;
	BOOL	m_bStartRDisk;

};



#endif // !defined(AFX_CLONENT_H__B63E2DE6_8518_4381_A1BB_54CAB78FB358__INCLUDED_)
