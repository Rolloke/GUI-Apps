// FindFiles.h: interface for the FindFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDFILES_H__BC2B7256_C0EA_42E7_B71D_729FF3AC88B7__INCLUDED_)
#define AFX_FINDFILES_H__BC2B7256_C0EA_42E7_B71D_729FF3AC88B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// This class is a helper for enumerating a directory tree.
class CFindFiles  
{

public:
	_finddata_t m_fd;
	long m_lHandle;
	bool m_bValid;

	CFindFiles (LPCSTR psPath);
	~CFindFiles ();

	inline bool IsValidEntry (void);
	void FindNext (void);
	bool IsSubDir (void);

};

/*********************************************************************************************
 Class		   : CFindFiles
 Function      : IsValidEntry 
 Description   : Check if entry is valid

 Parameters:  --

 Result type:  returns TRUE if entry is valid  (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE IsValidEntry >
 <GROUP CFindFiles>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CFindFiles, IsValidEntry>
*********************************************************************************************/
inline bool CFindFiles::IsValidEntry (void)
{
	return m_bValid;
}

/*********************************************************************************************
 Class		   : CFindFiles
 Function      : FindNext 
 Description   : Find the next entry and set valid flag

 Parameters:  --

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE FindNext >
 <GROUP CFindFiles>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CFindFiles, FindNext>
*********************************************************************************************/
inline void CFindFiles::FindNext (void)
{
	// _findnext returns 0 for success
	if (0 == _findnext (m_lHandle, &m_fd))
	{
		m_bValid = true;
	}
	else
	{
		m_bValid = false;
	}

}

/*********************************************************************************************
 Class		   : CFindFiles
 Function      : IsSubDir 
 Description   : Check if entry is a subdirectory

 Parameters:  --

 Result type:  returns TRUE if entry is a subdirectory  (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE IsSubDir >
 <GROUP CFindFiles>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CFindFiles, IsSubDir>
*********************************************************************************************/
inline bool CFindFiles::IsSubDir (void)
{
	if (0 != (m_fd.attrib & _A_SUBDIR))
	{
		return true;
	}
	else
	{
		return false;
	}
}
#endif // !defined(AFX_FINDFILES_H__BC2B7256_C0EA_42E7_B71D_729FF3AC88B7__INCLUDED_)
