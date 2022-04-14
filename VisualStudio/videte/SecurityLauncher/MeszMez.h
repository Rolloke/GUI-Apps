// MeszMez.h: interface for the CMeszMez class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESZMEZ_H__1E0FD7B1_E405_11D1_B511_00C095EC9EFA__INCLUDED_)
#define AFX_MESZMEZ_H__1E0FD7B1_E405_11D1_B511_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMeszMez  
{
	// construction/destruction
public:
	CMeszMez();
	virtual ~CMeszMez();

public:
	inline BOOL	IsMeszMez() const;
	inline BOOL	IsDCF() const;

	// operations
public:
	void Init();
	void Check();

	// implementation
private:
	void Check95();
	void CheckNT();
	BOOL IsAfterMesz2Mez(LPSYSTEMTIME lpST);
	BOOL IsAfterMez2Mesz(LPSYSTEMTIME lpST);
	BOOL IsBeforeMesz2Mez(LPSYSTEMTIME lpST);
	BOOL IsBeforeMez2Mesz(LPSYSTEMTIME lpST);
	int  DaysOfMonth(LPSYSTEMTIME lpSystemTime);
	BOOL IsLastSundayOfMonth(LPSYSTEMTIME lpSystemTime);

	// data
private:
	BOOL m_bDCF;
	BOOL m_bAutoMESZ;
	UINT m_month2MESZ;
	UINT m_month2MEZ;
};

inline BOOL	CMeszMez::IsMeszMez() const
{
	return m_bAutoMESZ;
}
inline BOOL	CMeszMez::IsDCF() const
{
	return m_bDCF;
}

#endif // !defined(AFX_MESZMEZ_H__1E0FD7B1_E405_11D1_B511_00C095EC9EFA__INCLUDED_)
