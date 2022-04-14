// getbits.cpp, bit level routines for tmndecode (H.263 decoder)

#include "stdafx.h"
#include "H263Decoder.h"

//////////////////////////////////////////////////////////////////////////////
CBits::CBits()
{
	init();
	m_bitMsk[0] = 128;
	m_bitMsk[1] = 64;
	m_bitMsk[2] = 32;
	m_bitMsk[3] = 16;
	m_bitMsk[4] = 8;
	m_bitMsk[5] = 4;
	m_bitMsk[6] = 2;
	m_bitMsk[7] = 1;
	m_pData = NULL;
	m_dwDataLen = 0;
}
//////////////////////////////////////////////////////////////////////////////
CBits::~CBits()
{
	if (m_pData!=NULL)
	{
		free(m_pData);
	}
}
//////////////////////////////////////////////////////////////////////////////
BOOL CBits::Load(const CString& sFileName)
{
	CFile file;
	CFileException cfe;
	CFileStatus cfs;
	BOOL bRet = FALSE;

	if (m_pData!=NULL)
	{
		free(m_pData);
	}

	init();

	if (file.Open(sFileName,CFile::modeRead,&cfe))
	{
		if (file.GetStatus(cfs))
		{
			m_dwDataLen = cfs.m_size;
			m_pData = (BYTE*)malloc(m_dwDataLen+6);
			if (m_dwDataLen!=file.Read(m_pData,m_dwDataLen))
			{
				free(m_pData);
				m_pData = NULL;
				m_dwDataLen = 0;
			}
			else
			{
				bRet = TRUE;
				m_pData[m_dwDataLen-6] = 0;
				m_pData[m_dwDataLen-5] = 0;
				m_pData[m_dwDataLen-4] = (1<<7) | (SE_CODE<<2);
				m_pData[m_dwDataLen-3] = 0;
				m_pData[m_dwDataLen-2] = 0;
				m_pData[m_dwDataLen-1] = (1<<7) | (SE_CODE<<2);
			}
		}
		file.Close();
	}
	return bRet;
}

