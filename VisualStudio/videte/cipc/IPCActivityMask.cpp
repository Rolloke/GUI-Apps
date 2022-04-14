// IPCActivityMask.cpp: implementation of the CIPCActivityMask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"

#include "IPCActivityMask.h"
#include "CipcExtraMemory.h"
#include "wkclasses\wk_trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCActivityMask::CIPCActivityMask()
{
	m_bVersion = MASK_VERSION;
	m_Size.cx = 0;
	m_Size.cy = 0;
	m_pMask = NULL;
	m_eType = AM_INVALID;
}

//////////////////////////////////////////////////////////////////////
CIPCActivityMask::CIPCActivityMask(MaskType eType)
{
	m_bVersion	= MASK_VERSION;
	m_Size.cx	= 0;
	m_Size.cy	= 0;
	m_pMask		= NULL;
	m_eType		= eType;
}

//////////////////////////////////////////////////////////////////////
CIPCActivityMask::CIPCActivityMask(CSize s, MaskType eType, WORD* pMask/* = NULL*/,BYTE bVersion/* = 0*/)
{
	m_Size = s;
	m_eType = eType;
	if (pMask)
	{
		DWORD dwBufferLen = m_Size.cx * m_Size.cy;
		m_pMask = new WORD[dwBufferLen];
		CopyMemory(m_pMask,pMask,dwBufferLen*sizeof(WORD));
	}
	else
		m_pMask = NULL;
	m_bVersion = bVersion;
}
//////////////////////////////////////////////////////////////////////
CIPCActivityMask::CIPCActivityMask(const CIPCActivityMask& src)
{
	m_Size = src.m_Size;
	m_eType = src.m_eType;
	m_pMask = NULL;
	if (src.m_pMask)
	{
		WK_DELETE_ARRAY(m_pMask);
		DWORD dwBufferLen = m_Size.cx * m_Size.cy;
		m_pMask = new WORD[dwBufferLen];
		CopyMemory(m_pMask,src.m_pMask,dwBufferLen*sizeof(WORD));
	}
	m_bVersion = src.m_bVersion;
}
//////////////////////////////////////////////////////////////////////
CIPCActivityMask::~CIPCActivityMask()
{
		WK_DELETE_ARRAY(m_pMask);
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCActivityMask::Create(MaskType eType, int nXDim, int nYDim)
{
	WK_DELETE_ARRAY(m_pMask);
	m_Size.cx	= nXDim;
	m_Size.cy	= nYDim;

	m_pMask		= new WORD[m_Size.cx * m_Size.cy];
	m_eType		= eType;
	m_bVersion	= MASK_VERSION;
	Clear();

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
const CIPCActivityMask& CIPCActivityMask::operator = (const CIPCActivityMask& src)
{
	m_Size = src.m_Size;
	m_eType = src.m_eType;
	if (src.m_pMask)
	{
		WK_DELETE_ARRAY(m_pMask);
		DWORD dwBufferLen = m_Size.cx * m_Size.cy;
		m_pMask = new WORD[m_Size.cx * m_Size.cy];
		CopyMemory(m_pMask,src.m_pMask,dwBufferLen*sizeof(WORD));
	}
	m_bVersion = src.m_bVersion;
	return *this;
}
//////////////////////////////////////////////////////////////////////
void CIPCActivityMask::SetType(MaskType eType)
{
	m_eType = eType;
}

//////////////////////////////////////////////////////////////////////
WORD CIPCActivityMask::GetAt(int nX, int nY) const
{
	WORD wResult = 0;
	if ((nX >= 0) && (nX < m_Size.cx) && (nY >= 0) && (nY < m_Size.cy))
		wResult = m_pMask[nX + nY * m_Size.cx];
	return wResult;
}
//////////////////////////////////////////////////////////////////////
void CIPCActivityMask::Invert()
{
	// Maske invertieren
	if (m_pMask)
	{
		for (int nY=0; nY < m_Size.cy; nY++)
		{
			for (int nX=0; nX < m_Size.cx; nX++)
				SetAt(nX, nY, (WORD)((WORD)AM_UPPER_BOUNDARY - GetAt(nX, nY)));
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCActivityMask::SetAt(int nX, int nY, WORD wValue)
{
	BOOL bResult = FALSE;
	if (m_pMask)
	{
		if ((nX >= 0) && (nX < m_Size.cx) && (nY >= 0) && (nY < m_Size.cy))
		{
			m_pMask[nX + nY * m_Size.cx] = wValue;
			bResult = TRUE;
		}
	}
	return bResult;

}
//////////////////////////////////////////////////////////////////////
void CIPCActivityMask::Clear()
{
	if (m_pMask)
	{
		FillMemory(m_pMask, m_Size.cx * m_Size.cy * sizeof(WORD), AM_LOWER_BOUNDARY);
	}
}
//////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CIPCActivityMask::BubbleFromMask(CIPC *pCipc) const
{
	// calc the length;
	DWORD dwSum = 0;
	CIPCExtraMemory* pBubble = NULL;

	if (m_pMask && pCipc)
	{
		dwSum  = sizeof(BYTE)							// m_bVersion 
			  +  sizeof(BYTE)							// m_eType
			  +  2*sizeof(int)							// m_Size
			  +  m_Size.cx * m_Size.cy * sizeof(WORD);	// m_pMask
		pBubble = new CIPCExtraMemory();
		if (pBubble->Create(pCipc,dwSum))
		{
			BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();
			DWORD dw;

			*pByte++ = m_bVersion;
			*pByte++ = (BYTE)m_eType;
			dw = m_Size.cx;
			CIPCExtraMemory::DWORD2Memory(pByte,dw);
			dw = m_Size.cy;
			CIPCExtraMemory::DWORD2Memory(pByte,dw);

			CopyMemory(pByte,m_pMask,m_Size.cx * m_Size.cy * sizeof(WORD));
			pByte += m_Size.cx * m_Size.cy * sizeof(WORD);
			
			DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
			WK_ASSERT(dwDelta==dwSum);
		}
		else
		{
			WK_DELETE(pBubble);
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in MaskFromFields\n"));
		}
	}
	else
		WK_TRACE_WARNING(_T("CIPCActivityMask::BubbleFromMask m_pMask or pCipc = NULL!\n"));

	return pBubble;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCActivityMask::MaskFromBubble(const CIPCExtraMemory* pExtraMem)
{
	if (pExtraMem)
	{
		WK_DELETE(m_pMask);
		const BYTE *pByte = (const BYTE*)pExtraMem->GetAddress();

		m_bVersion = *pByte++;
		m_eType = (MaskType)*pByte++;
		m_Size.cx = CIPCExtraMemory::Memory2DWORD(pByte);
		m_Size.cy = CIPCExtraMemory::Memory2DWORD(pByte);

		m_pMask = new WORD[m_Size.cx * m_Size.cy];
		CopyMemory(m_pMask,pByte,m_Size.cx * m_Size.cy*sizeof(WORD));

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CIPCActivityMask::LoadMaskFromFile(const CString& sFileName)
{
	BOOL bResult = FALSE;

	TRY
	{
		DWORD dwMagicHeader;
		CFile file(sFileName, CFile::modeRead);
		file.Read(&dwMagicHeader, sizeof(dwMagicHeader));
		if (dwMagicHeader == MAGIC_FILE_MASK_HEADER)
		{
			file.Read(&m_bVersion,	sizeof(m_bVersion));
			file.Read(&m_eType,		sizeof(m_eType));
			file.Read(&m_Size.cx,	sizeof(m_Size.cx));
			file.Read(&m_Size.cy,	sizeof(m_Size.cy));
			WK_DELETE_ARRAY(m_pMask);
			m_pMask = new WORD[m_Size.cx * m_Size.cy];
			if (m_pMask)
			{
				file.Read(m_pMask,	m_Size.cx * m_Size.cy * sizeof(WORD));
				bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR(_T("LoadMaskFromFile: Wrong magic header\n"));

	}
	CATCH( CFileException, e )
	{
		bResult = FALSE;
	}
	END_CATCH

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CIPCActivityMask::SaveMaskToFile(const CString& sFileName)
{
	BOOL	bResult = FALSE;

	if (!m_pMask)
		return FALSE;
	TRY
	{
		CFile file(sFileName, CFile::modeWrite | CFile::modeCreate);
		DWORD dwMagicHeader = MAGIC_FILE_MASK_HEADER;
		file.Write(&dwMagicHeader,	sizeof(dwMagicHeader));
		file.Write(&m_bVersion,		sizeof(m_bVersion));
		file.Write(&m_eType,		sizeof(m_eType));
		file.Write(&m_Size.cx,		sizeof(m_Size.cx));
		file.Write(&m_Size.cy,		sizeof(m_Size.cy));
		file.Write(m_pMask,			m_Size.cx * m_Size.cy * sizeof(WORD));
		
		bResult = TRUE;
	}
	CATCH( CFileException, e )
	{
		WK_TRACE_ERROR(_T("SaveMaskToFile: Failed to save ActivityMask\n"));
		bResult = FALSE;
	}
	END_CATCH

	return bResult;
}