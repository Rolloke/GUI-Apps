/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DirContent.h $
// ARCHIVE:		$Archive: /Project/Tools/UpdateHandler/DirContent.h $
// CHECKIN:		$Date: 13.12.05 12:01 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 12.12.05 16:15 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _DIR_CONTENT_HEADER_H_
#define _DIR_CONTENT_HEADER_H_

#define FLAG_FILE_DATA_IS_UNICODE 0x80000000

/////////////////////////////////////////////////////////////////////////////
class CDirContent
{
	// construction
public:
	CDirContent(const CString& sDir, const void* pData, DWORD dwDataLen);
	~CDirContent();

	// access
public:
	BOOL  IsEmpty() const;
	BOOL  IsContinued() const;
	const int GetSize() const;
	const LPWIN32_FIND_DATA GetAt(int i) const;
	const CString& GetDirectory() const;
	// operations
public:
	BOOL  Remove(const CString& sFile);

	// data
private:
	CPtrArray	m_Files;
	CString		m_sDirectory;
	BOOL		m_bContinued;
};
/////////////////////////////////////////////////////////////////////////////
#endif

