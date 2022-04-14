// Converter.h: interface for the CConverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTER_H__B7E3D526_72F2_11D2_B4E2_00C095EC9EFA__INCLUDED_)
#define AFX_CONVERTER_H__B7E3D526_72F2_11D2_B4E2_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CConverter scans all drives for files to convert
// it constructs an array of CConversions / an old sequence
// 

class CConverter  
{
	// construction / destruction
public:
	CConverter(const CString& sDatabaseDir);
	virtual ~CConverter();

	// operations
public:
	BOOL Convert();

	// implementation
protected:
	BOOL ConvertArchivDir(WORD wArchivNr);
	BOOL ConvertSequenceDir(WORD wArchivNr, WORD wSequenceNr);
	BOOL ConvertSequence(WORD wArchivNr, 
						 WORD wSequenceNr, 
						 DWORD dwSize,
						 const CSystemTime& dbfTime,
						 BOOL bWasSafeRing,
						 const CString& sDbf);
	BOOL DeleteForeignDir(const CString& sForeignDir);
	BOOL DeleteForeignFile(const CString& sForeignFile);

private:
	CString m_sDatabaseDir;
};

#endif // !defined(AFX_CONVERTER_H__B7E3D526_72F2_11D2_B4E2_00C095EC9EFA__INCLUDED_)
