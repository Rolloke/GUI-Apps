// IntelPicture.h: interface for the CIntelPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTELPICTURE_H__2B50CD62_0DE1_4CED_ADE0_AD330908DFCD__INCLUDED_)
#define AFX_INTELPICTURE_H__2B50CD62_0DE1_4CED_ADE0_AD330908DFCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIntelPicture  
{
	// Construction/Destruction
public:
	CIntelPicture();
	virtual ~CIntelPicture();

	// attributes
public:
	inline int  GetWidth() const;
	inline int  GetHeight() const;
	inline BOOL IsEmpty() const;
	inline int  GetColorSpace() const;
	inline BYTE* GetData();

	// operations
public:
	void SetData(int iWidth, int iHeight, const BYTE* pBuffer, IJL_COLOR color);
	void Resize(int iWidth, int iHeight);

	// Implementation
protected:
	void Init();
	void Deinit();

	// data member
private:
	IplImage	m_Image;
	int			m_iColorSpace;
};
//////////////////////////////////////////////////////////////////////
inline int CIntelPicture::GetWidth() const
{
	return m_Image.width;
}
//////////////////////////////////////////////////////////////////////
inline int CIntelPicture::GetHeight() const
{
	return m_Image.height;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIntelPicture::IsEmpty() const
{
	return m_Image.imageData == NULL;
}
//////////////////////////////////////////////////////////////////////
inline int CIntelPicture::GetColorSpace() const
{
	return m_iColorSpace;
}
//////////////////////////////////////////////////////////////////////
inline BYTE* CIntelPicture::GetData()
{
	return (unsigned char*)m_Image.imageData;
}

#endif // !defined(AFX_INTELPICTURE_H__2B50CD62_0DE1_4CED_ADE0_AD330908DFCD__INCLUDED_)
