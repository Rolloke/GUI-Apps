/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: picturerecord.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/picturerecord.cpp $
// CHECKIN:		$Date: 14.06.06 8:38 $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 12.06.06 8:03 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcOutput.h"
#include "CipcExtraMemory.h"
#include "VideoJob.h"
#include "CipcOutputRecord.h"

// 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FRAME_TIME_STEP_PRESENT 0x00000001
#define MD_POINTS_PRESENT		0x00000002

/*********************************************************************************************
 Klasse  : CIPCPictureRecord
 Funktion: CIPCPictureRecord
 Zweck   : Konstruktor mit Parametern.
           
 Parameter:  
  pCipc        : (E): Entsprechendes CIPC Objekt, um Shared Memory für die Bilddaten zu erzeugen.  (const CIPC*)
  pData        : (E): Buffer mit den Bilddaten  (const BYTE*)
  dwDataLen    : (E): Größe des Buffers in Bytes  (DWORD)
  camID        : (E): ID der Kamera  (CSecID )
  res          : (E): Auflösung des Bildes  (Resolution)
  comp         : (E): Kompressionsrate des Bildes  (Compression)
  ct           : (E): Kompressionstyp des Bildes  (CompressionType)
  stTimeStamp  : (E): Zeit zu der das Bild gegrabbed wurde  (const CSystemTime)
  dwJobTime    : (E): Systemzeit: wird nur benötigt bei Komunikation mit Server  (DWORD)
  szInfoString : (E): zusätzlicher Infotext (alter Alarmtext für ATMs)  (LPCTSTR)
  dwBitrate    : (E): Bitrate in Bits pro Sekunde für die Kompression mit H263 und MPEG  (DWORD)
  pictureType  : (E): Bildtyp  (CIPCSavePictureType pictureType /*=)
  wBlockNr     : (E): Blocknummer des Bildes (WORD)

 <TITLE CIPCPictureRecord>
*********************************************************************************************/
CIPCPictureRecord::CIPCPictureRecord(const CIPC* pCipc, 
									 const BYTE* pData, 
									 DWORD dwDataLen,
									 CSecID  camID,
									 Resolution res,
									 Compression comp,
									 CompressionType ct,
									 const CSystemTime stTimeStamp,
									 DWORD dwJobTime,
									 LPCTSTR(szInfoString) /*=NULL*/,
									 DWORD dwBitrate /*=0*/,
									 CIPCSavePictureType pictureType /*= SPT_FULL_PICTURE*/,
									 WORD wBlockNr, /*=0*/
									 void *pHeader /*= NULL*/,
									 int nHeaderLen /*=0*/
									 )
{
	m_pBuffer = NULL;
	m_dwBufferLen = 0;

	m_pCipc				= pCipc;
	m_camID				= camID;
	m_resolution		= res;
	m_compression		= comp;
	m_compressionType	= ct;
	m_dwJobTime			= dwJobTime;
	m_pictureType		= pictureType;
	m_pBubble			= NULL;
	m_dwBitrate			= dwBitrate;
	m_dwBlockNr			= wBlockNr;
	m_stTimeStamp		= stTimeStamp;
	m_dwTimeOffset		= 17;
	m_pHeader			= pHeader;
	m_nHeaderLen		= nHeaderLen;
	m_dwLengths.Add(dwDataLen);
	m_dwFlags			= 0;
	m_nNoOfMDPoints		= 0;
	m_pMDPoints			= NULL;

	if (szInfoString) 
	{
		m_sInfoString = szInfoString;
	}

	if (m_pCipc)
	{
		CreateBubble(pData,dwDataLen);
	}
	else
	{
		m_dwBufferLen = dwDataLen;
		m_pBuffer = new BYTE[m_dwBufferLen];
		CopyMemory(m_pBuffer,pData,m_dwBufferLen);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : CIPCPictureRecord
 Description: Creates a picture record as a group of pictures (GOP)

 Parameters:   
  pCipc            : (E): current CIPC object  (const CIPC*)
  camID            : (E): Camera ID of the picutre  (CSecID )
  res              : (E): Resolution of the Picture  (Resolution)
  comp             : (E): Compression of the pictures  (Compression)
  ct               : (E): Compresssion Type ofthe picture  (CompressionType)
  bIsVideoPresent  : (E): Video is present  (BOOL)
  wBlockNr         : (E): Block No of the GOP  (WORD)
  datas            : (E): Pointer array with pointers to the picture data  (const CPtrArray&)
  dwLengths        : (E): Array with the lengths of the picture data according to the pointer  (const CDWordArray&)
  times            : (E): Grabbing times of the pictures  (const CSystemTimeArray&)
  [dwFrameTimeStep]: (E): Timestep for one picture (Default=0, timestep is calculated)  (DWORD)
  [dwNoOfMDPoints] : (E): Number of motion detection points. Default=0  (DWORD)
  [pMDPoints]      : (E): Array of MD points. Default = NULL  (CPoint*)

 Result type:  (none)
 Author: Rolf Kary-Ehlers
 created: June, 16 2005
 <TITLE CIPCPictureRecord>
*********************************************************************************************/
CIPCPictureRecord::CIPCPictureRecord(const CIPC *pCipc, 
									 CSecID  camID,
										Resolution res,
										Compression comp,
										CompressionType ct,
										BOOL bIsVideoPresent,
										WORD wBlockNr,
										const CPtrArray& datas,
										const CDWordArray& dwLengths,
										const CSystemTimeArray& times,
										DWORD dwFrameTimeStep/*=0*/,
										DWORD dwNoOfMDPoints/*=0*/,
										CPoint *pMDPoints/*=NULL*/
										)
{
	m_pBuffer = NULL;
	m_dwBufferLen = 0;

	m_pCipc = pCipc;
	m_camID = camID;
	m_resolution = res;
	m_compression = comp;
	m_compressionType = ct;
	m_dwJobTime = bIsVideoPresent;
	m_pictureType = SPT_GOP_PICTURE;
	m_pBubble = NULL;
	m_dwBitrate = dwFrameTimeStep;
	m_dwBlockNr = wBlockNr;
	m_nNoOfMDPoints = dwNoOfMDPoints;
	m_pMDPoints = pMDPoints;
	m_dwFlags = 0;

	if (times.GetSize())
	{
		m_stTimeStamp = *times.GetAtFast(0);
	}
	m_dwTimeOffset = 17;

	m_pHeader = NULL;
	m_nHeaderLen = 0;

	// m_sInfoString;

	if (m_pCipc)
	{
		CreateBubble(datas,dwLengths,times);
	}
	else
	{
		m_nNoOfMDPoints = 0;
		m_pMDPoints = NULL;
		WK_TRACE_ERROR(_T("creating SPT_GOP_PICTURE no cipc\n"));
	}
}
/*********************************************************************************************
 Klasse  : CIPCPictureRecord
 Funktion: CIPCPictureRecord
 Zweck   : Konstruktor zum Duplizieren

 Parameter:  
  src: (E): CIPCPictureRecord Objekt  (const CIPCPictureRecord&)

 <TITLE CIPCPictureRecord>
*********************************************************************************************/
CIPCPictureRecord::CIPCPictureRecord(const CIPCPictureRecord &src)
{
	m_pBuffer = NULL;
	m_dwBufferLen = 0;

	m_pCipc				= src.m_pCipc;
	m_camID				= src.m_camID;
	m_resolution		= src.m_resolution;
	m_compression		= src.m_compression;
	m_compressionType	= src.m_compressionType;
	m_dwJobTime			= src.m_dwJobTime;
	m_pictureType		= src.m_pictureType;
	m_pBubble			= NULL;
	m_dwBitrate			= src.m_dwBitrate;
	m_dwBlockNr			= src.m_dwBlockNr;
	m_sInfoString		= src.m_sInfoString;
	m_stTimeStamp		= src.m_stTimeStamp;
	m_dwTimeOffset		= src.m_dwTimeOffset;
	m_pHeader			= src.m_pHeader;
	m_nHeaderLen		= src.m_nHeaderLen;
	m_dwLengths.RemoveAll();
	m_dwLengths.Append(src.m_dwLengths);
	m_nNoOfMDPoints		= src.m_nNoOfMDPoints;
	m_pMDPoints			= src.m_pMDPoints;
	m_dwFlags			= src.m_dwFlags;

	if (src.m_pBubble) 
	{
		m_pBubble = new CIPCExtraMemory(*src.m_pBubble);	
	}
	else if (src.m_pBuffer)
	{
		m_dwBufferLen = src.m_dwBufferLen;
		m_pBuffer = new BYTE[m_dwBufferLen];
		CopyMemory(m_pBuffer,src.m_pBuffer,m_dwBufferLen);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
int CIPCPictureRecord::GetDataOffset() const
{
	return (6*sizeof(DWORD));
}
//////////////////////////////////////////////////////////////////////////////////////
//private
void CIPCPictureRecord::CreateBubble(const BYTE *pData,DWORD dwDataLen)
{
	if (dwDataLen>1024*1024) 
	{
		WK_TRACE_ERROR(_T("PictureBubble %d exceeds 1MB\n"),dwDataLen);
		return;
	}

	m_dwTimeOffset = 17;
#ifdef _UNICODE
	CWK_String sInfoString(m_sInfoString);
	BOOL bUnicode = m_pCipc->GetCodePage() == CODEPAGE_UNICODE;
	DWORD dwBubbleLen =   dwDataLen 
		                 + GetDataOffset()
						     + sInfoString.CopyToMemory(bUnicode, NULL, -1, m_pCipc->GetCodePage())
						     + m_dwTimeOffset;
#else
	DWORD dwBubbleLen =   dwDataLen 
		                 + GetDataOffset()
						     + m_sInfoString.GetLength()
						     + m_dwTimeOffset;
#endif
	if (m_pHeader)	// ist der Header Extra
	{
		dwBubbleLen += m_nHeaderLen;
	}

	m_pBubble = new CIPCExtraMemory();

	if (m_pBubble->Create(m_pCipc,dwBubbleLen))
	{
		// first fill header with parameters
		DWORD *pDword = (DWORD *)m_pBubble->GetAddressForWrite();
	
		pDword[0] = 0x45000000 // magic high byte
					|((DWORD)m_resolution & 0xff )<<16
					|((DWORD)m_compression & 0xff )<<8
					|((DWORD)m_compressionType & 0xff);
		pDword[1] = m_camID.GetID();
		ULONGLONG ulTime = m_stTimeStamp.GetCTime().GetTime();
		WK_ASSERT(ULONG_MAX >= ulTime);
		pDword[2] = (DWORD)ulTime;
		pDword[3] = ((m_dwJobTime & 0xffff)<<16) | (WORD)m_pictureType;
		
		// infoStrings are limited for WORD length
		WK_ASSERT(m_sInfoString.GetLength() <= 0xffff);
		// for 4.0 + 17 Bytes for 0 and CSystemTime
		pDword[4] = ((m_sInfoString.GetLength()+17)<<16) | m_dwBlockNr;
		pDword[5] = m_dwBitrate;

		BYTE *pByte = (BYTE*)m_pBubble->GetAddressForWrite()+GetDataOffset();
		if (m_pHeader)	// ist der Header Extra
		{
			CopyMemory(pByte, m_pHeader, m_nHeaderLen);
			pByte += m_nHeaderLen;
		}
		// then add the picture data
		if (pData != NULL)
		{
			CopyMemory(pByte, pData, dwDataLen);
		}
		pByte += dwDataLen;
		
		// append string ?
#ifdef _UNICODE
		int nLen = sInfoString.CopyToMemory(bUnicode, pByte, -1, m_pCipc->GetCodePage());
		pByte += nLen;
		*pByte++ = 0; 
#else
		if (m_sInfoString.GetLength()) 
		{
			CopyMemory(pByte, LPCTSTR(m_sInfoString), m_sInfoString.GetLength());
			pByte += m_sInfoString.GetLength();
		}
		// append a CSystemTime in 4.0
		*pByte++ = 0; 
#endif
		// the 0 is necessary for old versions, using a strcpy, which stops at 0
		m_stTimeStamp.WriteIntoBubble(pByte);
		DWORD dwDelta=pByte-(BYTE*)m_pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwBubbleLen);
	}
	else
	{
		WK_DELETE(m_pBubble);
		WK_TRACE_ERROR(_T("cannot create PictureRecord Bubble Len %d\n"),dwBubbleLen);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCPictureRecord::CreateBubble(const CPtrArray& datas,
									 const CDWordArray& dwLengths,
									 const CSystemTimeArray& times)
{
	DWORD dwBubbleLen = 6*sizeof(DWORD);
	DWORD dwDL = 0;

	// calc the bubble length
#ifdef _DEBUG
	if (datas.GetSize()==1)
	{
		ASSERT(dwLengths.GetSize() == times.GetSize());
	}
	else
	{
		ASSERT(datas.GetSize() == dwLengths.GetSize());
		ASSERT(datas.GetSize() == times.GetSize());
	}
#endif

	m_dwLengths.RemoveAll();
	m_dwLengths.Append(dwLengths);

	LONG iNumPics = m_dwLengths.GetSize();
	int i = 0;
	int j = 0;

	// number of pics
	dwBubbleLen += sizeof(DWORD);
	// the data lengths
	dwBubbleLen += iNumPics * sizeof(DWORD);
	// the time stamps
	dwBubbleLen += iNumPics * 8 * sizeof(WORD);

	if (m_dwBitrate)
	{
		m_dwFlags |= FRAME_TIME_STEP_PRESENT;
		dwBubbleLen += sizeof(DWORD);
	}
	if (m_nNoOfMDPoints && m_pMDPoints)
	{
		m_dwFlags |= MD_POINTS_PRESENT;
		dwBubbleLen += sizeof(DWORD);
		dwBubbleLen += m_nNoOfMDPoints*sizeof(CPoint);
	}

	// add the picture lengths
	DWORD *pdwLengths = m_dwLengths.GetData();
	for (i=0;i<iNumPics;i++)
	{
		dwBubbleLen += pdwLengths[i];
		dwDL += pdwLengths[i];
	}


	m_pBubble = new CIPCExtraMemory();

	if (m_pBubble->Create(m_pCipc,dwBubbleLen))
	{
		// TODO write into bubble
		i = 0;
		
		// first fill header with parameters
		// same like old code 
		// m_pictureType is important!
		DWORD* pDword = (DWORD*)m_pBubble->GetAddressForWrite();

		pDword[i++] = 0x45000000 // magic high byte
			|((DWORD)m_resolution & 0xff )<<16
			|((DWORD)m_compression & 0xff )<<8
			|((DWORD)m_compressionType & 0xff);
		pDword[i++] = m_camID.GetID();
		ULONGLONG ulTime = m_stTimeStamp.GetCTime().GetTime();
		WK_ASSERT(ULONG_MAX >= ulTime);
		pDword[i++] = (DWORD)ulTime;
		pDword[i++] = ((m_dwJobTime & 0xffff)<<16) | (WORD)m_pictureType;
		pDword[i++] = m_dwBlockNr;
		pDword[i++] = m_dwFlags; // reserved for future use.
		
		pDword[i++] = iNumPics;
		// write the picture lengths
		for (j=0;j<iNumPics;j++)
		{
//			pDword[i++] = m_dwLengths.GetAt(j);
			pDword[i++] = pdwLengths[j];
		}
		// ab jetzt als BYTE pointer
		BYTE* pByte = (BYTE*)&pDword[i];

		// write the time stamps
		for (j=0;j<iNumPics;j++)
		{
			times.GetAtFast(j)->WriteIntoBubble(pByte);
		}

		// copy the picture datas
		if (iNumPics==datas.GetSize())
		{
			for (j=0;j<iNumPics;j++)
			{
				if (datas.GetAt(j) != NULL)
				{
					CopyMemory(pByte, datas.GetAt(j), pdwLengths[j]);
				}
				pByte += pdwLengths[j];
			}
		}
		else if (datas.GetSize()==1)
		{
			CopyMemory(pByte, datas.GetAt(0), dwDL);
			pByte += dwDL;
		}

		pDword = (DWORD*)pByte;
		i=0;
		if (m_dwBitrate)
		{
			pDword[i++] = m_dwBitrate;
		}
		if (m_nNoOfMDPoints && m_pMDPoints)
		{
			DWORD dwDataLen = m_nNoOfMDPoints*sizeof(CPoint);
			pDword[i++] = m_nNoOfMDPoints;
			pByte = (BYTE*)&pDword[i];
			CopyMemory(pByte, m_pMDPoints, dwDataLen);
			m_pMDPoints = (CPoint*)pByte;
			pByte += dwDataLen;
		}
		// to be continued

		DWORD dwDelta=pByte-(BYTE*)m_pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwBubbleLen);
	}
	else
	{
		WK_DELETE(m_pBubble);
		WK_TRACE_ERROR(_T("cannot create PictureRecord Bubble Len %d\n"),dwBubbleLen);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCPictureRecord::ReleaseBubble()
{
	if (m_pBubble)
	{
		WK_DELETE_ARRAY(m_pBuffer);
		m_dwBufferLen = GetDataLength();
		m_pBuffer = new BYTE[m_dwBufferLen];

		CopyMemory(m_pBuffer,GetData(),m_dwBufferLen);

		WK_DELETE(m_pBubble);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// private !
CIPCPictureRecord::CIPCPictureRecord(const CIPCExtraMemory &src)
{
	m_pBuffer		= NULL;
	m_dwBufferLen	= 0;
	m_dwTimeOffset	= 0;
	m_pCipc			= src.GetCIPC();
	m_pHeader		= NULL;
	m_nHeaderLen	= 0;
	m_dwBitrate		= 0;
	m_nNoOfMDPoints	= 0;
	m_pMDPoints		= NULL;
	m_dwFlags		= 0;

	m_pBubble = new CIPCExtraMemory(src);
	const DWORD *pDword = (const DWORD *)m_pBubble->GetAddress();
	
	BYTE bMagic = HIBYTE(HIWORD(pDword[0]));
	if (bMagic==0x45) 
	{
		// Resoultion
		m_resolution=(Resolution)LOBYTE(HIWORD(pDword[0]));
		if (m_resolution==0xff) 
		{
			m_resolution = RESOLUTION_NONE;
		}
		// Compression
		m_compression=(Compression)HIBYTE(LOWORD(pDword[0]));
		if (m_compression==0xff) 
		{
			m_compression=COMPRESSION_NONE;
		}
		m_compressionType=(CompressionType)LOBYTE(LOWORD(pDword[0]));
		m_camID=pDword[1];

		CTime timestamp = pDword[2];
		m_stTimeStamp.FromTime(timestamp);

		m_dwJobTime=HIWORD(pDword[3]);
		m_pictureType =(CIPCSavePictureType) LOWORD(pDword[3]);
		DWORD dwStringLen = HIWORD(pDword[4]);
		m_dwBlockNr = LOWORD(pDword[4]);

		if (m_pictureType == SPT_GOP_PICTURE)
		{
			m_dwFlags = pDword[5];
			DWORD dwNumberOfPictures = pDword[6];
			ASSERT(dwNumberOfPictures>0 && dwNumberOfPictures<100);
			m_dwLengths.SetSize(dwNumberOfPictures);
			for (DWORD i=0;i<dwNumberOfPictures;i++)
			{
				m_dwLengths.SetAt(i,pDword[i+7]);
			}
			PBYTE pByteTimes = GetTimes();
			m_stTimeStamp.ReadFromBubble(pByteTimes);

			int nIndex=0;
			pDword = (DWORD*)GetData(dwNumberOfPictures);
			if (m_dwFlags & FRAME_TIME_STEP_PRESENT)
			{
				m_dwBitrate = pDword[nIndex++];
			}
			if (m_dwFlags & MD_POINTS_PRESENT)
			{
				m_nNoOfMDPoints = pDword[nIndex++];
				m_pMDPoints		= (CPoint*)&pDword[nIndex];
			}
		}
		else
		{
			// is a string appended at the end of the bubble ?
			m_dwBitrate=pDword[5];

			if (dwStringLen) 
			{
				BYTE* pByte = (BYTE*)m_pBubble->GetAddress() + m_pBubble->GetLength() - dwStringLen;
				// CAVEAT String may be only 0, followed by SystemTime
#ifdef _UNICODE
				CWK_String sInfoString;
				for (DWORD i=0; i<dwStringLen; i++)
					if (pByte[i] == 0) break;
				sInfoString.InitFromMemory(src.GetCIPC()->GetCodePage() == CODEPAGE_UNICODE, pByte, i, src.GetCIPC()->GetCodePage());
				//			CopyMemory(m_sInfoString.GetBufferSetLength(dwStringLen/2), pByte, dwStringLen);
				//			MultiByteToWideChar(CP_ACP, 0, (char*)pByte, dwStringLen, m_sInfoString.GetBufferSetLength(dwStringLen), dwStringLen);
				sInfoString.ReleaseBuffer();
				m_sInfoString = sInfoString;
				if ((DWORD)m_sInfoString.GetLength()<dwStringLen)
				{
					m_dwTimeOffset = 17;
					// there is a new 4.0 extra data
					// a CSystemTime
					pByte = (BYTE*)m_pBubble->GetAddress() + m_pBubble->GetLength() - 16;
					m_stTimeStamp.ReadFromBubble(pByte);
				}
#else
				CopyMemory(m_sInfoString.GetBufferSetLength(dwStringLen), pByte, dwStringLen);
				m_sInfoString.ReleaseBuffer();
				//			WK_TRACE(_T("CIPCPictureRecord(%s)\n"), m_sInfoString);
				if ((DWORD)m_sInfoString.GetLength()<dwStringLen)
				{
					m_dwTimeOffset = 17;
					// there is a new 4.0 extra data
					// a CSystemTime
					pByte = (BYTE*)m_pBubble->GetAddress() + m_pBubble->GetLength() - 16;
					m_stTimeStamp.ReadFromBubble(pByte);
				}
#endif
			}
		}
	} 
	else 
	{
		WK_TRACE(_T("CIPCPictureRecord missing magic 0xf... in %x\n"),pDword[0]);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// destructor deletes the bubble or decrements the counter*/
CIPCPictureRecord::~CIPCPictureRecord()
{
	WK_DELETE(m_pBubble);
	WK_DELETE_ARRAY(m_pBuffer);
}
//////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : *CIPCPictureRecord
 Function   : GetData
 Description: Retrieves the data pointer of a picture

 Parameters:   
  [i]: (): Index of the picture, Default=0  (int)

 Result type:  (const BYTE)
 Author: Rolf Kary-Ehlers
 created: June, 16 2005
 <TITLE GetData>
*********************************************************************************************/
const BYTE *CIPCPictureRecord::GetData(int i/*=0*/) const
{
	if (m_pBubble) 
	{
		if (m_pictureType == SPT_GOP_PICTURE)
		{
			PBYTE pByte = GetTimes();
			// Zeiten überspringen
			pByte += 16 * m_dwLengths.GetSize();
			for (int j=0;j<i;j++)
			{
				pByte += m_dwLengths[j];
			}
			return pByte;
		}
		else
		{
			return (const BYTE *)(m_pBubble->GetAddress())+GetDataOffset();
		}
	}
	else if (m_pBuffer)
	{
		return m_pBuffer;
	}
	else
	{
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetDataLength
 Description: Retrieves the length of the data for the picture

 Parameters:   
  [i]: (E): Index of the picture, Default=0  (int)

 Result type:  (DWORD)
 created: June, 16 2005
 <TITLE GetDataLength>
*********************************************************************************************/
DWORD CIPCPictureRecord::GetDataLength(int i/*=0*/) const
{
	if (m_pBubble) 
	{
		if (m_pictureType == SPT_GOP_PICTURE)
		{
			if (i==-1)
			{
				DWORD dwTotal = 0;
				for (int i=0;i<m_dwLengths.GetSize();i++)
				{
					dwTotal += m_dwLengths[i];
				}
				return dwTotal;
			}
			else
			{
				return m_dwLengths[i];
			}
		}
		else
		{
#ifdef _UNICODE
			CWK_String sInfoString(m_sInfoString);
			ASSERT(m_pCipc != NULL);
			return   m_pBubble->GetLength()
				- GetDataOffset()
				- sInfoString.CopyToMemory(m_pCipc->GetCodePage() == CODEPAGE_UNICODE, NULL, -1, m_pCipc->GetCodePage())
				- m_dwTimeOffset;
#else
			return   m_pBubble->GetLength()
				- GetDataOffset()
				- m_sInfoString.GetLength()
				- m_dwTimeOffset;
#endif
		}
	}
	else if (m_pBuffer)
	{
		return m_dwBufferLen;
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
CSystemTime CIPCPictureRecord::GetTimeStamp(int i=0) const
{
	if (m_pictureType == SPT_GOP_PICTURE)
	{
		ASSERT(i<(int)GetNumberOfPictures());
		CSystemTime t;
		PBYTE pByteTimes = GetTimes();
		pByteTimes += 16*i;
		t.ReadFromBubble(pByteTimes);
		return t;
	}
	else
	{
		return m_stTimeStamp;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// Timestep between GOP-Pictures in µs
DWORD CIPCPictureRecord::GetFrameTimeStep() const
{
	if (m_pictureType == SPT_GOP_PICTURE)
	{
		if (m_dwBitrate && m_dwFlags & FRAME_TIME_STEP_PRESENT)
		{
			return m_dwBitrate;
		}
		else
		{
			int nNumPics = GetNumberOfPictures();
			if (nNumPics == 1)
			{
				return 0;
			}
			int nStart = nNumPics > 3 ? 1 : 0;
			int nEnd   = nNumPics-1;
			CSystemTime Time0 = GetTimeStamp(nStart);// calculate the time step
			CSystemTime TimeN = GetTimeStamp(nEnd);
			LARGE_INTEGER liGOPTimeStep;
			LARGE_INTEGER liTime0 = Time0;
			LARGE_INTEGER liTimeN = TimeN;			// times are save in 100 ns steps
			liGOPTimeStep.QuadPart = liTimeN.QuadPart - liTime0.QuadPart;
			liGOPTimeStep.QuadPart = liGOPTimeStep.QuadPart / ((nEnd - nStart)*10);
			if (liGOPTimeStep.LowPart > 1000000)		// bigger than 1 second
			{										// is not likely
				WK_TRACE(_T("Time %d too big\n"), liGOPTimeStep.LowPart);
				liGOPTimeStep.LowPart = 0;
			}
			return liGOPTimeStep.LowPart;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCPictureRecord::GetNumberOfMDPoints() const
{
	if (   m_pictureType == SPT_GOP_PICTURE
		&& m_dwFlags & MD_POINTS_PRESENT)
	{
		return m_nNoOfMDPoints;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
CPoint CIPCPictureRecord::GetMDPoint(int i) const
{
	if (   m_pictureType == SPT_GOP_PICTURE
		&& m_dwFlags & MD_POINTS_PRESENT 
		&& m_pMDPoints
		&& IsInArray(i, m_nNoOfMDPoints))
	{
		return m_pMDPoints[i];
	}
	return CPoint(0,0);
}
