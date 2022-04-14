/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: picturelengthstatistic.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/picturelengthstatistic.h $
// CHECKIN:		$Date: 17.03.04 13:47 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 17.03.04 13:19 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _PICT_LEN_STAT_HEADER_H_
#define _PICT_LEN_STAT_HEADER_H_

class CIPCOutputIdipClient;

/////////////////////////////////////////////////////////////////////////////
class CPictureLengthStatistic 
{
	// construction
public:
	CPictureLengthStatistic(CIPCOutputIdipClient* pOutput);
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

	CIPCOutputIdipClient* m_pOutput;
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

