/* GlobalReplace: PictData6 --> COldPictData */
/* GlobalReplace: Header6Entry --> COldPictDataHeaderRecord */

#define MAX_PICT_IN_FILE	2000

enum DATEI_ZUSTAND
{
	CLOSED,
	OPENED,
	CREATED
};

class COldPictDataHeaderRecord
{
public:
	inline COldPictDataHeaderRecord();
	inline void	Set(DWORD dwPosition, DWORD	dwLen);

	BOOL	IsUsed()	{	return (m_dwUsed==1);	}
	DWORD	Position()	{	return m_dwPosition;	}
	DWORD	Len()		{	return m_dwLen;			}
	inline void	Reset();
protected:
	void AlreadyInUseError() const;
	// CAVEAT order is important, there is a direct read in ReadHeader
	DWORD	m_dwUsed;
	DWORD	m_dwPosition;
	DWORD	m_dwLen;
};

class COldPictData : public CFile
{
public:
	COldPictData();
	virtual ~COldPictData();

	virtual void Close();

	DWORD GetPictCount() const { return m_dwPictCount; }
	DWORD	GetPictData	(DWORD dwNr, void* pData);
	DWORD	GetPictLen	(DWORD dwNr);

	BOOL	IsFile(const char *szName);
	CString ConstructFileName(const char *szName);
	const char *GetDatName()	{	return m_sName;		}

	void	ReadHeader();

	//
	BOOL	MyOpen(const char *szName);	// always ReadOnly

	DWORD m_dwFileSize;	// NEW HEDU
protected:

	UINT	HeaderSize();

	CString			m_sName;
	DATEI_ZUSTAND	m_OpenTyp;
	DWORD			m_dwPictCount;
	COldPictDataHeaderRecord	Header[MAX_PICT_IN_FILE];
};



//////////////////////////////////////////////////////////////////////
inline COldPictDataHeaderRecord::COldPictDataHeaderRecord()
{
	m_dwUsed		=	0;
	m_dwPosition	=	0;
	m_dwLen			=	0;
}

inline void COldPictDataHeaderRecord::Reset()
{
	m_dwUsed		=	0;
	m_dwPosition	=	0;
	m_dwLen			=	0;
}

inline void COldPictDataHeaderRecord::Set(DWORD dwPosition, DWORD	dwLen)
{
	if (m_dwUsed){
		AlreadyInUseError();
	}
	m_dwUsed	=	1;
	m_dwPosition=	dwPosition;
	m_dwLen		=	dwLen;
}
