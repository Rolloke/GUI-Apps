#pragma once

class CCamera;
//////////////////////////////////////////////////////////////////////
class CGOPItem : public CIPCPictureRecords
{
	// construction / destruction
public:
	CGOPItem(CSecID camID, CCamera* pCamera);
	virtual~ CGOPItem() {};

	// attributes
public:
	inline CSecID GetCamID() const;
	inline CCamera* GetCamera() const;
	inline DWORD  GetLastSend() const;

	// operations
public:
	void			   AddPictureRecord(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY);
	CIPCPictureRecord* GetNextPicture(DWORD dwTick, DWORD& dwX,DWORD& dwY);

private:
	CSecID m_camID;
	CCamera* m_pCamera;
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
inline CCamera* CGOPItem::GetCamera() const
{
	return m_pCamera;
}
//////////////////////////////////////////////////////////////////////
typedef CGOPItem* CGOPItemPtr;
WK_PTR_ARRAY_CS(CGOPItemArray,CGOPItemPtr,CGOPItemArrayCS)
class CIPCOutputDVClient;
//////////////////////////////////////////////////////////////////////
class CGOPSendThread :	public CWK_Thread
{
public:
	CGOPSendThread();
	virtual ~CGOPSendThread();

	// attributes
public:

	// operations
public: 
	void SetParent(CIPCOutputDVClient* pParent);
	void AddPictureRecord(CCamera* pCamera, const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY);
	virtual	BOOL Run(LPVOID lpContext);


private:
	CIPCOutputDVClient* m_pParent;
	CGOPItemArrayCS		m_GOPs;
};
