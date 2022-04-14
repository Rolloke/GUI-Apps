/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: picturelengthstatistic.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/picturelengthstatistic.h $
// CHECKIN:		$Date: 5.06.02 13:03 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 4.06.02 14:21 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _PICT_LEN_STAT_HEADER_H_
#define _PICT_LEN_STAT_HEADER_H_

class CIPCOutputVision;

/////////////////////////////////////////////////////////////////////////////
class CPictureLengthStatistic 
{
	// construction
public:
	CPictureLengthStatistic(CIPCOutputVision* pOutput);
	~CPictureLengthStatistic();

	// access
public:
	inline const int GetTuning() const;	
	inline const int GetDelay() const;	
	// operations
public:
	void operator +=(int iPictureLength);
	BOOL OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend);
	BOOL CalcTuning();
	void Reset();

private:
	void AddPictureLength(int iPictureLength);
	void CalculateAverageBitrate();

	// data
private:
	DWORD			m_Tuning; // tuning parameter for video source encoder
	DWORD			m_Delay; // 

	DWORD			m_dwCurBitTick;
	DWORD			m_dwMedBitTick;
	DWORD			m_dwAllBitTick;

	DWORD			m_AllTime;
	DWORD			m_MedTime;
	DWORD			m_CurTime;

	double			m_CurBitrate; // current (last m_dwCurBitTick milliseconds)
	double			m_MedBitrate; // medium (last m_dwMedBitTick milliseconds)
	double			m_AllBitrate; // all (last m_dwAllBitTick milliseconds)
	
	double			m_AveBitrate;

	double			m_AllDataSum;
	double			m_MedDataSum;
	double			m_CurDataSum;

	DWORD			m_dwPics;

	DWORD			m_LastUpdated;
	BOOL		    m_bDelayed;

	CIPCOutputVision* m_pOutput;
};
/////////////////////////////////////////////////////////////////////////////
inline const int CPictureLengthStatistic::GetTuning() const
{
	return m_Tuning;
}
/////////////////////////////////////////////////////////////////////////////
inline const int CPictureLengthStatistic::GetDelay() const
{
	return m_Delay;
}
/////////////////////////////////////////////////////////////////////////////


#endif

