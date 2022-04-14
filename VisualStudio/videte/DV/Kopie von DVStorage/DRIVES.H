// Drives.h: interface for the CDrives class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVES_H)
#define AFX_DRIVES_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDrives : public CIPCDrives
{
	// construction / destruction
public:
	CDrives();
	virtual ~CDrives();

public:
	// Attributes
	CIPCDrive* GetFreeDrive(WORD wCollectionIDpart);
	inline int GetNrOfDBDrives() const;
	WORD GetCollectionsOfDrive(CString sDrive);

	// operations
	void Initialize();
	void SetCollectionsToDrive(CString sDrive, WORD wCollectionIDpart, BOOL bAdd);

private:
	int m_nNrOfDBDrives;
	CMap<WORD,WORD, WORD, WORD> m_mapDriveToCollections;	// counts the Collections per drive 
	CMap<WORD,WORD, WORD, WORD> m_mapCollectionToDrive;		// assigns the collections subid partly to the drive
};

inline int CDrives::GetNrOfDBDrives() const
{
	return m_nNrOfDBDrives;
}

#endif // !defined(AFX_DRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
