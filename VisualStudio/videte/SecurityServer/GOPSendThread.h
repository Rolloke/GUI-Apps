#pragma once

//////////////////////////////////////////////////////////////////////
class CGOPItem : public CIPCPictureRecords
{
	// construction / destruction
public:
	CGOPItem(CSecID camID);
	virtual~ CGOPItem() {};

	// attributes
public:
	inline CSecID GetCamID() const;
	inline DWORD  GetLastSend() const;

	// operations
public:
	void			   AddPictureRecord(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY);
	CIPCPictureRecord* GetNextPicture(DWORD dwTick, DWORD& dwX,DWORD& dwY);

private:
	CSecID m_camID;
	DWORD  m_dwIndex;
	DWORD  m_dwLastSend;
	LARGE_INTEGER m_liLastSend;
	CDWordArray m_dwMDXs;
	CDWordArray m_dwMDYs;
};
//////////////////////////////////////////////////////////////////////
inline CSecID CGOPItem::GetCamID() const
{
	return m_camID;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CGOPItem::GetLastSend() const
{
	return m_dwLastSend;
}
//////////////////////////////////////////////////////////////////////
typedef CGOPItem* CGOPItemPtr;
WK_PTR_ARRAY_CS(CGOPItemArray,CGOPItemPtr,CGOPItemArrayCS)
class CIPCOutputServerClient;
//////////////////////////////////////////////////////////////////////
class CGOPSendThread :	public CWK_Thread
{
	// construction / destruction
public:
	CGOPSendThread();
	virtual ~CGOPSendThread();

	// attributes
public:

	// operations
public: 
	void SetParent(CIPCOutputServerClient* pParent);
	void AddPictureRecord(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY);
	virtual	BOOL Run(LPVOID lpContext);


private:
	CIPCOutputServerClient* m_pParent;
	CGOPItemArrayCS			m_GOPs;
};
