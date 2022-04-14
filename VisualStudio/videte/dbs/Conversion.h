// Conversion.h: interface for the CConversion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERSION_H__20EA8296_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
#define AFX_CONVERSION_H__20EA8296_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CConversion  
{
	// construction / destruction
public:
	CConversion(const CString& sDatabaseDir,
				WORD wArchivNr,
				WORD wSequenceNr, 
				DWORD dwSize,
				const CSystemTime& dbfTime,
				BOOL bWasSafeRing,
				const CString& sDbf);
	virtual ~CConversion();

	// attributes
public:
	inline WORD		GetArchivNr() const;
	inline WORD		GetSequenceNr() const;
	inline CString 	GetDatabaseDir() const;
	inline CString 	GetDbf() const;
		   CString  GetDbfPath() const;

	// operation
public:
	BOOL DeleteFiles();

	// data 
private:
	CString		m_sDatabaseDir;
	WORD		m_wArchivNr;
	WORD		m_wSequenceNr; 
	DWORD		m_dwSize;
	CSystemTime m_dbfTime;
	BOOL		m_bWasSafeRing;
	CString		m_sDbf;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD CConversion::GetArchivNr() const
{
	return m_wArchivNr;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CConversion::GetSequenceNr() const
{
	return m_wSequenceNr;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CConversion::GetDatabaseDir() const
{
	return m_sDatabaseDir;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CConversion::GetDbf() const
{
	return m_sDbf;
}
/////////////////////////////////////////////////////////////////////////////
typedef CConversion* CConversionPtr;
WK_PTR_ARRAY_CS(CConversionArray,CConversionPtr,CConversions)
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CONVERSION_H__20EA8296_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
