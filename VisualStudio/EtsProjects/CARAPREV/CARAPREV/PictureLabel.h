// PictureLabel.h: Schnittstelle für die Klasse CPictureLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURELABEL_H__1DF76B02_09CF_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_PICTURELABEL_H__1DF76B02_09CF_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RectLabel.h"

#define DIB_FORMAT            0x0DBFA
#define META_FORMAT           0x0EAFA
#define BITMAP_FILE           0x0DBFE

class CDibSection;
class CFileHeader;

class AFX_EXT_CLASS CPictureLabel : public CRectLabel  
{
   DECLARE_SERIAL(CPictureLabel)
public:
	CSize GetPrintSize();
	static int DeleteBWBmp(CLabel *pl, void *pParam);
	CString & GetFileName();
	void SetFileName(CString &);
   CPictureLabel();
   virtual ~CPictureLabel();
   virtual bool Draw(CDC*, int bAtr = 1);
   virtual bool DrawShape(CDC * pDC, bool draw);
   virtual void Serialize(CArchive &);
   virtual void SetPoint(CPoint);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif

   void         AttachMetaFile(HENHMETAFILE, bool bSetSize = false);
   HENHMETAFILE CPictureLabel::DetachMetaFile();
   void         AttachDIB(CDibSection *, bool bSetSize = false);
   CDibSection* DetachDIB();
   UINT         GetFormat();
   void         SetRopMode(DWORD nMode)        {m_nMode = nMode;};
   int          GetRopMode()                   {return m_nMode;};
   void         SetStretchBltMode(DWORD nMode) {m_nStretchMode = nMode;};
   int          GetStretchBltMode()            {return m_nStretchMode;};
   void         SetSaveInFile(bool bSIF)       {m_bSaveInFile = bSIF;};
   bool         SaveInFile()                   {return m_bSaveInFile;};

   bool         m_bNoFile;
private:
   CDibSection* m_pDibSection;
   HENHMETAFILE m_hMetaFile;
   DWORD        m_nMode;
   int          m_nStretchMode;
   bool         m_bSaveInFile;
	CString      m_strFileName;
   CSize        m_szPrintSize;
};

#endif // !defined(AFX_PICTURELABEL_H__1DF76B02_09CF_11D2_9DB9_0000B458D891__INCLUDED_)
