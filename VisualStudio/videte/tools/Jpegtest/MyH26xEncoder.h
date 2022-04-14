// MyH26xEncoder.h: interface for the CMyH26xEncoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYH26XENCODER_H__890D673B_B4DF_40C8_A0B4_58CEDED52E39__INCLUDED_)
#define AFX_MYH26XENCODER_H__890D673B_B4DF_40C8_A0B4_58CEDED52E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDecodeThread;
class CMyH26xEncoder : public CH26xEncoder  
{
public:
	CMyH26xEncoder(CDecodeThread* pDecoder);
	virtual ~CMyH26xEncoder();

public:
	virtual void OnEncodedFrame();

private:
	CDecodeThread* m_pDecoder;
};

#endif // !defined(AFX_MYH26XENCODER_H__890D673B_B4DF_40C8_A0B4_58CEDED52E39__INCLUDED_)
