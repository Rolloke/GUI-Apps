
#ifndef _CStatValue_H
#define _CStatValue_H

#include "wk.h"

class CStatValue {
public:
	inline CStatValue(const CTime &t, DWORD dwValue, DWORD dwTick);
	//
	CTime m_time;
	DWORD m_dwTick;
	DWORD m_dwValue;
};

typedef CStatValue* CStatValuePtr;
WK_PTR_ARRAY(CStatValueArray,CStatValuePtr);

class CStatValues : public CStatValueArray
{
public:
	CStatValues() { }
	const CStatValues & operator =(const CStatValues &src);
};

/////////////////////////////////////
class CStatRecord {
public:
	inline CStatRecord(const CString &name);
	CStatRecord(const CStatRecord &src);
	inline ~CStatRecord();
	//
	inline const CString &GetName() const;
	void AddValue(CTime t, DWORD dwValue, DWORD dwTick);
	DWORD GetMinMax(CTime &startTime, CTime &endTime) const;
	int GetAverage();

	CString m_sName;
	CStatValues m_values;
	CString m_sType;
	DWORD m_dwMaxValue;
	DWORD m_dwMinValue;
	double m_Sum;
};

typedef CStatRecord* CStatRecordPtr;
WK_PTR_ARRAY(CStatRecordArray,CStatRecordPtr);

class CStatRecords : public CStatRecordArray
{
public:
	inline ~CStatRecords();
	void AddRecord( const CString &sLog,
					const CString &sName, CTime t, DWORD dwValue,
						DWORD dwTick,
						BOOL bAsStart=FALSE);
	DWORD GetMinMax(CTime &startTime, CTime &endTime) const;
};
extern CStatRecords *ScanFile(const CString &sFilename,
								CTime startTime,
								CTime endTime,
								const CStringArray &includePatterns,
								const CStringArray &excludePatterns,
								BOOL bDoProcesses=TRUE,
								BOOL bDoTimer=TRUE,
								BOOL bDoAlarms=TRUE
								);
/////////////////////////////////////
inline CStatValue::CStatValue(const CTime &t, DWORD dwValue, DWORD dwTick)
{
	m_time=t;
	m_dwTick = dwTick;
	m_dwValue = dwValue;
}

inline CStatRecord::CStatRecord(const CString &name)
{
	m_sName = name;
	m_dwMaxValue = 0;
	m_dwMinValue = 0;
	m_Sum = 0;
}
inline const CString &CStatRecord::GetName() const
{
	return m_sName;
}
inline CStatRecord::~CStatRecord()
{
	m_values.DeleteAll();
}
inline CStatRecords::~CStatRecords()
{
	DeleteAll();
}

inline void ValidateYear(int &iYear)
{
	if (iYear<1900 && iYear>90) {
		iYear += 1900;
	}
	if (iYear<90 && iYear>=0) {
		iYear += 2000;
	}

}

inline BOOL StringHasYear(const CString &sDate)
{
	BOOL bHasYear=FALSE;

	int iDotCount=0;
	for (int i=0;i<sDate.GetLength();i++) {
		if (sDate[i]=='.') {
			iDotCount++;
		}
	}
	if (iDotCount>1) {
		bHasYear=TRUE;
	}

	return bHasYear;

}

#endif
