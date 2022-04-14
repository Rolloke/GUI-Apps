/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
// Sequences.h: interface for the CSequences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCES_H__A30FED62_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_SEQUENCES_H__A30FED62_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Sequence.h"

class CDrive;
/////////////////////////////////////////////////////////////////////////////
typedef CSequence* CSequencePtr;
WK_PTR_ARRAY_CS(CSequenceArray,CSequencePtr,CSequenceArrayCS)
/////////////////////////////////////////////////////////////////////////////
class CSequences : public CSequenceArrayCS 
{
	// construction / destruction
public:
	CSequences();
	virtual ~CSequences();

	// attributes
public:
	CSequence* GetSequenceID(WORD wID);
	CSequence* GetSequenceIndex(int i);
	CSequence* GetFirstSequence(CIPCDrive* pDrive);
	CIPCInt64  GetSizeBytes();
	DWORD	   GetSizeMB();
	DWORD	   GetNumberOfPictures();

	// operations
public:
	void	   AddScannedSequence(CSequence* pSequence);
	CSequence* AddNewSequence(BOOL bSuspect,CArchiv* pArchiv, CIPCDrive* pDrive,CSequence* pSourceSequence=NULL);
	BOOL	   DeleteSequence(WORD wID);
	BOOL	   RemoveSequence(WORD wID);
	BOOL	   DeleteOldestSequence();
	void	   Sort(BOOL& bCancel);
	void	   Trace();
	BOOL	   Finalize(BOOL bForceClose, DWORD dwMaxStorageTime);

	// implementation
protected:
	WORD	   GetNewID();

	// data member
private:
	WORD		m_wID;
};

#endif // !defined(AFX_SEQUENCES_H__A30FED62_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_)
