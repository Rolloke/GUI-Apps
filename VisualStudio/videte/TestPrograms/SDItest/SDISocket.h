#pragma once

class CSDITestDoc;

// CSDISocket command target

typedef CByteArray* CByteArrayPtr;
WK_PTR_ARRAY_CS(CByteArrayArrayPlain, CByteArrayPtr, CByteArrays)


class CSDISocket : public CAsyncSocket
{
	// construction / destruction
public:
	CSDISocket();
	virtual ~CSDISocket();

	// operations
public:
	void SetDocument(CSDITestDoc* pDoc);
	void AddSendData(const CByteArray& bytes);

	// overrides
public:
	virtual void OnSend(int nErrorCode);

	// data
private:
	CSDITestDoc* m_pDocument;
	CByteArrays  m_SendData;
public:
	virtual void OnConnect(int nErrorCode);
};


