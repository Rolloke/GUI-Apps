// CJobQueue.h: interface for the CJobQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CJOBQUEUE_H__9AE3790C_6842_4E93_8FD0_36FD1C3A7A84__INCLUDED_)
#define AFX_CJOBQUEUE_H__9AE3790C_6842_4E93_8FD0_36FD1C3A7A84__INCLUDED_

#define FIFO_SIZE 8

class CJobQueue  
{
public:
	CJobQueue();
	~CJobQueue();

	BOOL	Push(void* pData, DWORD dwLen);
	void*	Pop();
	void*   Pop2();
	void*	Get();
	int		ReadLevel(){return m_nLevel;}

private:
	void*	m_pArray[FIFO_SIZE];
	BOOL	m_bValid[FIFO_SIZE];

	int		m_nRdIndex;
	int		m_nWrIndex;
	int		m_nLevel;

};

#endif // !defined(AFX_CJOBQUEUE_H__9AE3790C_6842_4E93_8FD0_36FD1C3A7A84__INCLUDED_)
