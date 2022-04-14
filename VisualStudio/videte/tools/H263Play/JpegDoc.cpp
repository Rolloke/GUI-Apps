// JpegDoc.cpp : implementation file
//

#include "stdafx.h"
#include "h263play.h"
#include "JpegDoc.h"

#include "cdjpeg\JpegEncoder.h"
#include "cdjpeg\JpegDecoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJpegDoc

IMPLEMENT_DYNCREATE(CJpegDoc, CDocument)

CJpegDoc::CJpegDoc()
{
	m_pBuffer = NULL;
	m_dwLen	  = 0;
}

BOOL CJpegDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CJpegDoc::~CJpegDoc()
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
	}
	m_dwLen = 0;
}


BEGIN_MESSAGE_MAP(CJpegDoc, CDocument)
	//{{AFX_MSG_MAP(CJpegDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpegDoc diagnostics

#ifdef _DEBUG
void CJpegDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJpegDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJpegDoc serialization

void CJpegDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJpegDoc commands

BOOL CJpegDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	CFile file;
	CFileException cfe;

	if (file.Open(lpszPathName,CFile::modeRead,&cfe))
	{
		if (m_pBuffer)
		{
			delete m_pBuffer;
			m_dwLen = 0;
		}
		m_dwLen = file.GetLength();
		m_pBuffer = new BYTE[m_dwLen];

		file.Read(m_pBuffer,m_dwLen);
		SetPathName(lpszPathName,TRUE);
		GetJpegSize(m_pBuffer, m_dwLen, m_Size);
	}

	UpdateAllViews(NULL,NULL,NULL);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LPBYTE CJpegDoc::GetBuffer()
{
	return m_pBuffer;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CJpegDoc::GetBufferLength()
{
	return m_dwLen;
}
/////////////////////////////////////////////////////////////////////////////
CSize CJpegDoc::GetJpegSize()
{
	return m_Size;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CJpegDoc::GetJpegSize(LPBYTE lpBuffer, DWORD dwLen, SIZE &Size)
{
	BYTE byVal1, byVal2;

	if (!lpBuffer)
		return FALSE;

	// Find SOF
	for (DWORD dwI = 0; dwI < dwLen; dwI+=2)
	{
		byVal1 = lpBuffer[dwI];
		byVal2 = lpBuffer[dwI+1];
		if ((byVal1 == 0xff)	&& (byVal2 == 0xc0))
		{
			if (dwI <= dwLen - 8)
			{
				Size.cy = (int)MAKEWORD(lpBuffer[dwI+6], lpBuffer[dwI+5]);
				Size.cx = (int)MAKEWORD(lpBuffer[dwI+8], lpBuffer[dwI+7]);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CJpegDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	char szTempPathName[_MAX_PATH];
	char szTempFileName1[_MAX_PATH];
	char szTempFileName2[_MAX_PATH];
	GetTempPath(_MAX_PATH,szTempPathName);
	GetTempFileName(szTempPathName,"bmp",0,szTempFileName1);
	GetTempFileName(szTempPathName,"bmp",0,szTempFileName2);

	CSize size;
	GetJpegSize(m_pBuffer,m_dwLen,size);

	if ((size.cx == 704) && (size.cy = 280))
	{
		CJpegDecoder d;

		d.SetSource(lpszPathName);
		d.SetDestination(szTempFileName1);
		d.Decode();

		StretchBmpToDoubleHeigth(szTempFileName1,szTempFileName2);

		CJpegEncoder e;
		e.SetSource(szTempFileName2);
		e.SetDestination(lpszPathName);
		e.Encode();
		
		DeleteFile(szTempFileName1);
		DeleteFile(szTempFileName2);
	}
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CJpegDoc::OnFileSaveAs() 
{
	CString sFilter;
	sFilter.LoadString(IDS_SAVE_FILTER);

	CFileDialog dlg(FALSE,"bmp",NULL,OFN_HIDEREADONLY,sFilter);

	if (dlg.DoModal())
	{
		CString sExt = dlg.GetFileExt();
		if (0==sExt.CompareNoCase("bmp"))
		{
			CJpegDecoder d;

			d.SetSource(GetPathName());
			d.SetDestination(dlg.GetPathName());
			d.Decode();
		}
		else if (0==sExt.CompareNoCase("jpg"))
		{
			OnSaveDocument(dlg.GetPathName());
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CJpegDoc::StretchBmpToDoubleHeigth(const CString &sSource,
					   const CString &sDest)
{
	CFile source;
	CFile dest;
	BOOL bRet = TRUE;
	BITMAPFILEHEADER bfHS;
	BITMAPINFOHEADER biHS;
	BITMAPFILEHEADER bfHD;
	BITMAPINFOHEADER biHD;
	LPBYTE pBuffer = NULL; 
	DWORD  dwLen = 0; 
	int i;

	TRY
	{
		if (source.Open(sSource,CFile::modeRead))
		{
			source.Read(&bfHS,sizeof(bfHS));
			source.Read(&biHS,sizeof(biHS));
			bfHD = bfHS;
			biHD = biHS;

			biHD.biHeight = biHS.biHeight * 2;

			bfHD.bfSize = sizeof(bfHD) + sizeof(biHD) +	
						  biHS.biWidth * biHS.biHeight * (biHS.biBitCount/8) * 2;

			if (dest.Open(sDest,CFile::modeCreate|CFile::modeWrite))
			{
				dest.Write(&bfHD,sizeof(bfHD));
				dest.Write(&biHD,sizeof(biHD));

				dwLen = biHS.biWidth * (biHS.biBitCount/8);
				pBuffer = new BYTE[dwLen];

				for (i=0;i<biHS.biHeight;i++)
				{
					source.Read(pBuffer,dwLen);
					dest.Write(pBuffer,dwLen);
					dest.Write(pBuffer,dwLen);
				}

				delete [] pBuffer;
				
				source.Close();
				
				dest.Flush();
				dest.Close();
			}
		}
	}
	CATCH (CFileException, cfe)
	{
		bRet = FALSE;
	}
	END_CATCH

	return bRet;
}
