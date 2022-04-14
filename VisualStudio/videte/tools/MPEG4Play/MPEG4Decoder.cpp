// MPEG4Decoder.cpp: implementation of the CMPEG4Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MPEG4Play.h"
#include "MPEG4Decoder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDC *theDC = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMPEG4Decoder::CMPEG4Decoder(CString lpszInputFilePathName)
{
	m_pDibImageData      = NULL;
	m_pInputFrameBuffer  = NULL;
	m_pMP4DecHandle      = NULL;
	m_pH263DecHandle     = NULL;
	m_pOutFrameYUV420    = NULL;
//	m_pOutFrameYUV420->lum = NULL;
//	m_pOutFrameYUV420->cb  = NULL;
//	m_pOutFrameYUV420->cr  = NULL;
	m_pInputFileStream   = NULL;
	m_hDrawDib           = NULL;
	m_hDIB               = NULL;
	m_pMPEG4Data         = NULL;
	m_dwMPEG4DataLen     = 0;
	m_bThreadIsRunning   = FALSE;
	m_hDC                = NULL;
	m_iInputFrameLen     = 0;
	m_iFrameNr           = 0;
	m_iPrevTimestamp     = 0;
	m_bIsH263            = FALSE;

	m_lpszInputFilePathName = lpszInputFilePathName;

	ZeroMemory(&m_bmi,sizeof(m_bmi));
	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

//	// MPEG4Decoder initialisieren
//	if(!Init())
//		TRACE(_T("MPEG4-Init() fehlgeschlagen\n"));
}

CMPEG4Decoder::~CMPEG4Decoder()
{
	// noch vorhandene Bilder in m_listYUVFrameBuffer loeschen
	if(!m_listYUVFrameBuffer.IsEmpty())
	{
		POSITION pos = m_listYUVFrameBuffer.GetHeadPosition();
		while(pos!=NULL)
		{
			tBaseVideoFrame* pCurOutFrameYUV = m_listYUVFrameBuffer.GetNext(pos);
			delete pCurOutFrameYUV->cb;
			pCurOutFrameYUV->cb = NULL;
			delete pCurOutFrameYUV->cr;
			pCurOutFrameYUV->cr = NULL;
			delete pCurOutFrameYUV->lum;
			pCurOutFrameYUV->lum = NULL;
			delete pCurOutFrameYUV;
			pCurOutFrameYUV = NULL;
		}
		m_listYUVFrameBuffer.RemoveAll();
	}

	if(m_pMPEG4Data != NULL)
	{
		delete [] m_pMPEG4Data;
		m_pMPEG4Data = NULL;
	}

	// MP4-Datei wieder schliessen
	if(m_pInputFileStream != NULL)
	{
		fclose(m_pInputFileStream);
	}

	if(m_hDIB != NULL)
	{
		DeleteObject(m_hDIB);
		m_hDIB = NULL;
	}

	// Speicher fuer Input-Frame-Stream freigeben
	if(m_pInputFrameBuffer != NULL)
	{
		delete m_pInputFrameBuffer;
		m_pInputFrameBuffer = NULL;
	}

	// MP4Decoder-Handle freigeben
	if(m_pMP4DecHandle != NULL)
	{
		m_pMP4DecHandle->vDelete(m_pMP4DecHandle);
		m_pMP4DecHandle = NULL;
	}

	// H263Decoder-Handle freigeben
	if(m_pH263DecHandle != NULL)
	{
		m_pH263DecHandle->vDelete(m_pH263DecHandle);
		m_pH263DecHandle = NULL;
	}

	if(m_pOutFrameYUV420 != NULL)
	{
//		// Speicher fuer Y-Werte freigeben
//		if(m_pOutFrameYUV420->lum != NULL)
//		{
//			free(m_pOutFrameYUV420->lum);
//			m_pOutFrameYUV420->lum = NULL;
//		}
//
//		// Speicher fuer U-Werte freigeben
//		if(m_pOutFrameYUV420->cb != NULL)
//		{
//			free(m_pOutFrameYUV420->cb);
//			m_pOutFrameYUV420->cb = NULL;
//		}
//		
//		// Speicher fuer V-Werte freigeben
//		if(m_pOutFrameYUV420->cr != NULL)
//		{
//			free(m_pOutFrameYUV420->cr);
//			m_pOutFrameYUV420->cr = NULL;
//		}

//		delete m_pOutFrameYUV420;
//		m_pOutFrameYUV420 = NULL;
	}


	DrawDibClose(m_hDrawDib);

	if (m_pDecodeThread)
	{
//		if(m_pDecodeThread->m_hThread)
//			WaitForSingleObject(m_pDecodeThread->m_hThread,5000);
		if(m_bThreadIsRunning)
		{
			delete m_pDecodeThread;
			m_pDecodeThread = NULL;
		}
	}
}



//////////////////////////////////////////////////////////////////////
BOOL CMPEG4Decoder::Init(CWnd* pWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	tStreamInfo			lStrmInfo;				// Streaminfo
	int32				lBufLength;
    int32				iRet;
	tRGBWindowParam		wndParam;
    //int32             lPostprocType  = VD_COMPLEX_DEBLOCKING;
	BOOL				bRet = TRUE;
	int					nFilePos = 0;

	m_lPostprocType  = VD_COMPLEX_DEBLOCKING;

	m_pWnd		= pWnd;
	m_uMsg		= msg;
	m_wParam	= wParam;
	m_lParam	= lParam;

	// Soll ein H263-File dekodiert werden?
	CString str(m_lpszInputFilePathName);
	if(str.Find(_T(".263")) >= 0)
		m_bIsH263 = TRUE;

	m_hDrawDib	= DrawDibOpen(); 

	// MP4-Datei oeffnen
	m_pInputFileStream = _tfopen(m_lpszInputFilePathName, _T("rb"));

    if (m_pInputFileStream == NULL)
	{
		TRACE(_T("Input file does not exist \n"));
		bRet = FALSE;
        goto END_INIT;
	}

    // Buffer fuer Input-Frame-Stream allozieren
	m_pInputFrameBuffer = (uint8 *) malloc (sizeof(uint8) * COMPRESSED_FRAME_SIZE);
	if(m_pInputFrameBuffer == NULL)
	{
		TRACE(_T("Memory allocation error\n"));
		bRet = FALSE;
        goto END_INIT;
	}
#if (1)
	fpos_t FileLen;
	fseek(m_pInputFileStream, 0, SEEK_END);
	fgetpos(m_pInputFileStream, &FileLen);
	fseek(m_pInputFileStream, 0, SEEK_SET);
	do
	{
		// configuration stream auslesen
		if(m_bIsH263)
		{
			gReadVOPH263(m_pInputFrameBuffer, &lBufLength, m_pInputFileStream);
			
			// File-Pointer wieder an den Anfang des files setzen
			// --> Unterschied zu MPEG4!
			fseek(m_pInputFileStream, 0, SEEK_SET);
		}
		else
			gReadVOP(m_pInputFrameBuffer, &lBufLength, m_pInputFileStream);

		iRet = gGetStreamInfo(m_pInputFrameBuffer, lBufLength, &lStrmInfo);
		if (iRet != E_SUCCESS)
		{
			TRACE(_T("Getting streamInfo failed (FilePos=%d Len=%d) \n"), nFilePos, lBufLength);
			nFilePos+=lBufLength;
			fseek(m_pInputFileStream, nFilePos, SEEK_SET);
		}
	}
	while ((iRet != E_SUCCESS) && (nFilePos < FileLen));
	if (iRet != E_SUCCESS)
	{
		bRet = FALSE;
		goto END_INIT;
	}
#else
	// configuration stream auslesen
	if(m_bIsH263)
	{
		gReadVOPH263(m_pInputFrameBuffer, &lBufLength, m_pInputFileStream);
		
		// File-Pointer wieder an den Anfang des files setzen
		// --> Unterschied zu MPEG4!
		fseek(m_pInputFileStream, 0, SEEK_SET);
		
		iRet = gGetStreamInfo(m_pInputFrameBuffer, lBufLength, &lStrmInfo);
		if (iRet != E_SUCCESS)
		{
			TRACE(_T("Getting streamInfo failed\n"));
			AfxMessageBox("Getting streamInfo failed",MB_OK);
			bRet = FALSE;
			goto END_INIT;
		}
	}
	else
	{
		// Ist StreamInfo vorhanden?
		gReadVOP(m_pInputFrameBuffer, &lBufLength, m_pInputFileStream);
		//if(countStreamInfo == 0)
		if(lBufLength == STREAM_INFO2_LENGTH)
		{
			// Ja...also normal dekodieren
			//gReadVOP(m_pInputFrameBuffer, &lBufLength, m_pInputFileStream);
	
			iRet = gGetStreamInfo(m_pInputFrameBuffer, lBufLength, &lStrmInfo);
			if (iRet != E_SUCCESS)
			{
				TRACE(_T("Getting streamInfo failed\n"));
				AfxMessageBox("Getting streamInfo failed",MB_OK);
				bRet = FALSE;
				goto END_INIT;
			}
		
			CopyMemory(streamInfoBuffer, m_pInputFrameBuffer, STREAM_INFO2_LENGTH);
			//CopyMemory(&streamInfoDummy, &lStrmInfo, sizeof(tStreamInfo));
			//countStreamInfo++;
		}
		else
		{
			// Nein...also Stream-Pointer wieder an den Anfang des Streams setzen
			// und zuvor gespeicherten StreamInfo uebernehmen
			fseek(m_pInputFileStream, 0, SEEK_SET);

			CopyMemory(m_pInputFrameBuffer, streamInfoBuffer, STREAM_INFO2_LENGTH);
			iRet = gGetStreamInfo(m_pInputFrameBuffer, STREAM_INFO2_LENGTH, &lStrmInfo);
			if (iRet != E_SUCCESS)
			{
				TRACE(_T("Getting streamInfo failed\n"));
				AfxMessageBox("Getting streamInfo failed",MB_OK);
				bRet = FALSE;
				goto END_INIT;
			}

			CopyMemory(&lStrmInfo, &streamInfoDummy, sizeof(tStreamInfo));
			lBufLength = STREAM_INFO2_LENGTH;
		}
*/	}

#endif
	m_streamInfo = lStrmInfo;

	wndParam.wndWidth   = lStrmInfo.width;
	wndParam.wndHeight  = lStrmInfo.height;
	wndParam.xOffset    = 0;	
	wndParam.yOffset    = 0;	
	wndParam.rotateFlag = 0;


	switch(lStrmInfo.streamType)
	{
	case VD_H263_BASELINE_STREAM:
		TRACE(_T("Typ des komprimierten streams: VD_H263_BASELINE_STREAM\n"));
		iRet = gCreateH263Decoder(&m_pH263DecHandle,
			                      lStrmInfo.width,
								  lStrmInfo.height,
								  m_lPostprocType,
								  &wndParam);
		break;
	case VD_MPEG4_SP_STREAM:
		TRACE(_T("Typ des komprimierten streams: VD_MPEG4_SP_STREAM\n"));
		// MPEG-4 Simple Profile Decoder erstellen
		iRet = gCreateMP4SPDecoder(	&m_pMP4DecHandle, 
									m_pInputFrameBuffer,
									//streamInfoBuffer,
									&lBufLength, 
									m_lPostprocType,
									&wndParam);
		break;
	case VD_NOT_SUPPORTED_STREAM:
		TRACE(_T("Typ des komprimierten streams: VD_NOT_SUPPORTED_STREAM\n"));
		break;
	}

    if (iRet != E_SUCCESS)
    {
		TRACE(_T("Decoder creation failed \n"));
		bRet = FALSE;
        goto END_INIT;
    }

	// aktuelle Framegroesse
	iRet = lStrmInfo.extWidth * lStrmInfo.extHeight;
	//iRet = lStrmInfo.width * lStrmInfo.height;
	m_iInputFrameLen = iRet;

//	// Speicher fuer Y-Werte des Frames allozieren
//	// -> iRet = Anzahl der Y-Werte des Frames
//	m_OutFrameYUV420.lum = (uint8 *) malloc(iRet);
//
//	// Speicher fuer U-Werte des Frames allozieren
//	// -> iRet >> 2 = Anzahl der U-Werte des Frames
//	m_OutFrameYUV420.cb  = (uint8 *) malloc(iRet >> 2);
//
//	// Speicher fuer V-Werte des Frames allozieren
//	// -> iRet >> 2 = Anzahl der V-Werte des Frames
//	m_OutFrameYUV420.cr  = (uint8 *) malloc(iRet >> 2);
//
//	if(    (m_OutFrameYUV420.lum == NULL) 
//		|| (m_OutFrameYUV420.cb  == NULL) 
//		|| (m_OutFrameYUV420.cr  == NULL))
//	{
//		TRACE(_T("Output buffer allocation error\n"));
//		bRet = FALSE;
//        goto END_INIT;
//	}

	if(!m_bThreadIsRunning)
	{
		m_bThreadIsRunning = TRUE;
		m_pDecodeThread = AfxBeginThread(DecodeThreadProc,(LPVOID)this, THREAD_PRIORITY_LOWEST);
		if(m_pDecodeThread == NULL)
		{
			TRACE(_T("Decoder-Thread konnte nicht erstellt werden!\n"));
		}
	}

END_INIT:
	return bRet;
}


//////////////////////////////////////////////////////////////////////
void CMPEG4Decoder::OnDraw(CDC *pDC, const CRect &rectDest, const CRect &rectSrc/*=CRect(0,0,0,0)*/)
{
	m_csDecoder.Lock();

	if(!m_listYUVFrameBuffer.IsEmpty())
	{
		tBaseVideoFrame *pCurOutFrameYUV420;

		pCurOutFrameYUV420 = m_listYUVFrameBuffer.RemoveHead();

		m_bmi.bmiHeader.biBitCount      = 24;
		m_bmi.bmiHeader.biClrImportant  = 0;
		m_bmi.bmiHeader.biClrUsed       = 0;
		m_bmi.bmiHeader.biCompression   = 0;
		m_bmi.bmiHeader.biHeight        = pCurOutFrameYUV420->height;
		m_bmi.bmiHeader.biPlanes        = 1;
		m_bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);;
		m_bmi.bmiHeader.biSizeImage     = pCurOutFrameYUV420->width * pCurOutFrameYUV420->height * 3;
		m_bmi.bmiHeader.biWidth         = pCurOutFrameYUV420->width;
		m_bmi.bmiHeader.biXPelsPerMeter = 0;
		m_bmi.bmiHeader.biYPelsPerMeter = 0;

		// device-independent bitmap (DIB) erstellen
		m_hDIB = CreateDIBSection(pDC->m_hDC, &m_bmi, DIB_RGB_COLORS, (void **)&m_pDibImageData, NULL, 0);

		// Konvertierung: YUV 4:2:0 -> RGB24
		gYUV420toRGB24(pCurOutFrameYUV420, m_pDibImageData);

//		DrawDibDraw(m_hDrawDib,
//					pDC->m_hDC,
//					rectDest.left,
//					rectDest.top,
//					rectDest.Width(),
//					rectDest.Height(),
//					&m_bmi.bmiHeader,
//					m_pDibImageData,
//					0,
//					0,
//					pCurOutFrameYUV420->width,
//					pCurOutFrameYUV420->height,
//					0);

		TRACE(_T("rectDest.Width() = %d   rectDest.Height() = %d   m_streamInfo.width = %d   m_streamInfo.height = %d\n"),rectDest.Width(), rectDest.Height(), m_streamInfo.width,m_streamInfo.height);

		DrawDibDraw(m_hDrawDib,
					pDC->m_hDC,
					rectDest.left,
					rectDest.top,
					rectDest.Width(),
					rectDest.Height(),
//					m_streamInfo.width,
//					m_streamInfo.height,
					&m_bmi.bmiHeader,
					m_pDibImageData,
					0,
					0,
					m_streamInfo.width,
					m_streamInfo.height,
					0);


		if(m_hDIB != NULL)
		{
			DeleteObject(m_hDIB);
			m_hDIB = NULL;
		}

		delete pCurOutFrameYUV420->cb;
		pCurOutFrameYUV420->cb = NULL;
		delete pCurOutFrameYUV420->cr;
		pCurOutFrameYUV420->cr = NULL;
		delete pCurOutFrameYUV420->lum;
		pCurOutFrameYUV420->lum = NULL;

		delete pCurOutFrameYUV420;
		pCurOutFrameYUV420 = NULL;
	}

	m_csDecoder.Unlock();
}



//////////////////////////////////////////////////////////////////////
/* 
*  einen MPEG4-Frame aus dem komprimierten Datenstrom lesen
*/
void CMPEG4Decoder::gReadVOP(uint8 *buffer, int32 *length, FILE *fp)
{
	unsigned char  *lBuffer;
	uint16  lCode;
	int32   lBlocksize = COMPRESSED_FRAME_SIZE;
	int32	lTemp, rear;
	int32   i;
    int32   lFlag2, lCount;

	lFlag2 = 1;
	lBuffer = (unsigned char *) buffer;

    if ((lTemp = fread ((void *) lBuffer, 1, lBlocksize - 5, fp)) <= 0)
	{
		/* Raise expection */
        *length = 0;
        return;
	}
	
	if (!(lBuffer[0] | lBuffer[1]) && (lBuffer[2] == 1) && (lBuffer[3] == 0xB3))
	{
		lFlag2 = 0;
	}

	lCount = lTemp;
	if (lCount > (lBlocksize - 8))
		lCount = lBlocksize - 8;

    rear = lCount;

	for(i = 3; i < lCount; i++)
	{
		lCode = (uint16) (lBuffer[i] | lBuffer[i + 1]);
		if (!lCode)
		{
			// lCode = 0000 0000
			if (lBuffer[i + 2] == 1)
			{				
				if(lBuffer[i + 3] == 0xB3)
				{
					// configuration stream-Ende erreicht
					// 0xB3 bedeutet: group_of_vop_start_code (siehe MPEG-Standard)
					// -> ab hier liegen also die VOPs (=Frames) vor
					rear = i;
					break;
				}
				if(lBuffer[i + 3] == 0xB6)
				{
					// Uebergang zum naechsten VOP
					// 0xB6 bedeutet: vop_start_code (siehe MPEG-Standard)
					if (lFlag2 == 0)
					{
						lFlag2 = 1;
					}
					else
					{
						// Frame-Ende erreicht
						rear = i;
						break;
					}
				}
			}
		}
	}

	fseek(fp, rear - lTemp, SEEK_CUR);

    *length = rear;

	return;
}


//////////////////////////////////////////////////////////////////////
/*
*	Konvertierung: YUV 4:2:0 -> RGB24
*/
void CMPEG4Decoder::gYUV420toRGB24(tBaseVideoFrame *yuvFrame, uint8 *rgbFrame)
{
    tBaseVideoFrame  *lEextFrame;    
    uint8  *lCrPtr, *lCbPtr, *Lum1, *Lum2, *rgb1, *rgb2;
    int32    lY1, lCr, lCb;
    int32    lTemp, lTemp1, lTemp2, lTemp3;
    int32    lRGBFrameWidth, lLumWidth, lModuloWidth;
	int32    lActWidth, lActHeight;
    int32    i, j;
	int32    lLumWidth1;
	int32    lRGBFrameWidth1;
	int32    lExtraWidth;

	lEextFrame = (tBaseVideoFrame *) yuvFrame;
	
    lLumWidth = yuvFrame->width;
#ifdef ACTUALWIDTH
	lActWidth = lEextFrame->actWidth;
	lActHeight = lEextFrame->actHeight;
#else
	lActWidth = lEextFrame->width;
	lActHeight = lEextFrame->height;
#endif
	lRGBFrameWidth = 3 * lActWidth;    
	lExtraWidth    = (lActWidth & 7);
    lModuloWidth = lActWidth - (lActWidth & 7);  

    Lum1 = yuvFrame->lum;	
	Lum2 = Lum1 + lLumWidth;	

	lCbPtr = yuvFrame->cb;
    lCrPtr = yuvFrame->cr;

	
	rgb1 = rgbFrame + lRGBFrameWidth * (lActHeight - 2);
	rgb2 = rgb1 + lRGBFrameWidth;
	
	lLumWidth1     = (lLumWidth << 1) - lModuloWidth;
	lRGBFrameWidth1 = 	(lRGBFrameWidth  - lExtraWidth) * 3;

	sYUV420toRGB24Frame(Lum1, Lum2, lCbPtr, lCrPtr, rgb1, rgb2, lModuloWidth,
                        lLumWidth1,lRGBFrameWidth1,lActHeight); 
/*	
	if (lExtraWidth)
	{
		for (i = 0; i < lActHeight; i += 2)
		{
			Lum1 += (lModuloWidth);		
			Lum2 += (lModuloWidth);		

			lCrPtr  += (lModuloWidth >> 1);
			lCbPtr  += (lModuloWidth >> 1);	
				
			rgb1 += (lModuloWidth * 3);
			rgb2 += (lModuloWidth * 3); 
			
		    for (j = lModuloWidth ; j < lActWidth; j += 2)
			{
				lCr = lCrPtr[0];
				lCb = lCbPtr[0];

				lCr -= 128;
				lCb -= 128;

				lTemp1 = ((25802 * lCr) + COLOR_CONV_ROUND) >> COLOR_CONV_PRECISION;
				lTemp2 = ((3072 * lCb) + (7671 * lCr) + COLOR_CONV_ROUND) >>
															COLOR_CONV_PRECISION;
				lTemp3 = ((30397 * lCb) + COLOR_CONV_ROUND) >> COLOR_CONV_PRECISION;				

				///First Pixel				
				lY1 = Lum1[0];
				lTemp = (lY1 + lTemp3);
				rgb2[0] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 - lTemp2);
				rgb2[1] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 + lTemp1);
				rgb2[2] = SATURATE_RGB(lTemp);
        
				//Second Pixel
				lY1 = Lum1[1];				
				lTemp = (lY1 + lTemp3);
				rgb2[3] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 - lTemp2);
				rgb2[4] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 + lTemp1);
				rgb2[5] = SATURATE_RGB(lTemp);

				//Third Pixel
				lY1 = Lum2[0];
				lTemp = (lY1 + lTemp3);
				rgb1[0] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 - lTemp2);
				rgb1[1] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 + lTemp1);
				rgb1[2] = SATURATE_RGB(lTemp);

				//Fourth Pixel
				lY1 = Lum2[ 1];				
				lTemp = (lY1 + lTemp3);
				rgb1[3] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 - lTemp2);
				rgb1[4] = SATURATE_RGB(lTemp);				
				lTemp = (lY1 + lTemp1);
				rgb1[5] = SATURATE_RGB(lTemp);
				
				rgb1 += 6;
				rgb2 += 6;
				Lum1 += 2;
				Lum2 += 2;				
				lCrPtr++;
				lCbPtr++; 		
			}
			
			rgb1 -= (3 * lRGBFrameWidth);        
			rgb2 -= (3 * lRGBFrameWidth);

			Lum1 += ((lLumWidth << 1) - lActWidth);
			Lum2 += ((lLumWidth << 1) - lActWidth);

			lCrPtr += ((lLumWidth  - lActWidth) >> 1);
			lCbPtr += ((lLumWidth  - lActWidth) >> 1); 			
		}
    }	
*/
    return;
}



//////////////////////////////////////////////////////////////////////
/*
*	Konvertierung: YUV 4:2:0 -> RGB24
*/
void CMPEG4Decoder::sYUV420toRGB24Frame(uint8 *Lum1, uint8 *Lum2, uint8 *Cb, uint8 *Cr,
                                        uint8 *rgb1, uint8 *rgb2, int32 lWidth,int32 lumWidth,
                                        int32 rgbWidth,int32 actHeight)
{
__asm	
	{
   		push        ebx            
		mov			ebx, esp		
		and         esp,0FFFFFFF0h
		push		ebx        
		push        edi
		push        esi
		
		sub         esp,74h		
		        
		mov         eax, Lum1               
		mov         ecx, rgb1
		mov         dword ptr [esp+48h],eax		
		
		mov         eax, Lum2              
		mov         ebx, rgb2                
		mov         dword ptr [esp+4Ch],eax
		
		mov         edi, lWidth
		mov         esi, 0
		shr         edi, 1          
		mov         dword ptr [esp],esi
		mov         dword ptr [esp+0Ch],edi			    
		mov         edi,actHeight
		mov         dword ptr [esp+52h],edi
		mov         edi,lumWidth
		mov         dword ptr [esp+56h],edi
		add         edi,lWidth
		shr         edi,1
		sub         edi,lWidth
		shr         edi,1
		mov         dword ptr [esp+60h],edi
		mov         edi,rgbWidth
		mov         dword ptr [esp+64h],edi
		mov         edx, Cr
		mov         eax, Cb
	
loop1:
		mov         esi,dword ptr [esp+48h]
		prefetcht0  [esi+20h]
		movq        mm4,mmword ptr [esi]
		movq        mmword ptr [esp+40h],mm4
		add         esi,8
		mov         edi,dword ptr [esp+4Ch]
		mov         dword ptr [esp+48h],esi
		movq        mm2,mmword ptr [edi]
		movd        mm3,dword ptr [edx]
		punpcklbw   mm4,mm4
		psllw       mm4,8
		psrlw       mm4,7
		movq        mm6,mm4
		punpcklwd   mm6,mm6
		movq        mm1,mm6
		punpckldq   mm1,mm1
		psrlq       mm1,10h
		movq        mm0,mm6
		pslld       mm0,10h
		por         mm1,mm0
		prefetcht0  [edi+20h]
		add         edi,8
		punpckhwd   mm4,mm4
		mov         dword ptr [esp+4Ch],edi
		pshufw      mm0,mm4,0FCh
		psllq       mm4,20h
		punpckhdq   mm6,mm4
		punpcklbw   mm3,mm3
		psllw       mm3,8
		psrlw       mm3,4
		psubsw      mm3,Tab07
		movq        mm4,mm3
		punpcklwd   mm4,mm4
		movq        mm7,mm4
		punpckldq   mm7,mm7
		movq        mm5,mm4
		punpckhdq   mm5,mm5
		pmulhw      mm5,Tab03
		movq        mmword ptr [esp+10h],mm5
		prefetcht0  [edx+20h]
		add         edx,4
		paddsw      mm0,mm5
		pmulhw      mm7,Tab01
		paddsw      mm1,mm7
		movq        mmword ptr [esp+18h],mm7
		pmulhw      mm4,Tab02
		paddsw      mm6,mm4
		movq        mmword ptr [esp+20h],mm4
		movd        mm4,dword ptr [eax]
		punpcklbw   mm4,mm4
		psllw       mm4,8
		psrlw       mm4,4
		psubsw      mm4, Tab07
		movq        mm7,mm4
		punpcklwd   mm7,mm7
		movq        mm5,mm7
		punpckldq   mm5,mm5
		prefetcht0  [eax+20h]
		add         eax,4
		pmulhw      mm5,Tab04
		paddsw      mm1,mm5
		pmulhw      mm7,Tab05
		movq        mmword ptr [esp+28h],mm5
		paddsw      mm6,mm7
		psraw       mm1,1
		movq        mmword ptr [esp+30h],mm7
		psrlq       mm7,10h
		movq        mmword ptr [esp+38h],mm7
		paddsw      mm0,mm7
		psraw       mm6,1
		packuswb    mm1,mm6
		movntq      mmword ptr [ebx],mm1
		movq        mm6,mm2
		punpcklbw   mm6,mm6
		psllw       mm6,8
		psrlw       mm6,7
		movq        mm7,mm6
		punpcklwd   mm7,mm7
		movq        mm5,mm7
		punpckldq   mm5,mm5
		psrlq       mm5,10h
		movq        mm1,mm7
		pslld       mm1,10h
		por         mm5,mm1
		punpckhwd   mm6,mm6
		pshufw      mm1,mm6,0FCh
		psllq       mm6,20h
		punpckhdq   mm7,mm6
		paddsw      mm5,mmword ptr [esp+18h]
		paddsw      mm7,mmword ptr [esp+20h]
		paddsw      mm5,mmword ptr [esp+28h]
		paddsw      mm7,mmword ptr [esp+30h]
		psraw       mm5,1
		psraw       mm7,1
		packuswb    mm5,mm7
		movntq      mmword ptr [ecx],mm5
		paddsw      mm1,mmword ptr [esp+10h]
		paddsw      mm1,mmword ptr [esp+38h]
		movq        mm5,mmword ptr [esp+40h]
		punpckhbw   mm5,mm5
		psllw       mm5,8
		psrlw       mm5,7
		punpckhwd   mm3,mm3
		punpckhwd   mm4,mm4
		movq        mmword ptr [esp+40h],mm5
		punpcklwd   mm5,mm5
		movq        mm6,mm5
		punpckldq   mm6,mm6
		psrlq       mm6,10h
		movq        mm7,mm5
		pslld       mm7,10h
		por         mm6,mm7
		movq        mm7,mm3
		punpckldq   mm7,mm7
		pmulhw      mm7,Tab01
		movq        mmword ptr [esp+30h],mm7
		paddsw      mm6,mm7
		movq        mm7,mm4
		punpckldq   mm7,mm7
		pmulhw      mm7,Tab04
		paddsw      mm6,mm7
		movq        mmword ptr [esp+28h],mm7
		psraw       mm0,1
		psraw       mm6,1
		packuswb    mm0,mm6
		movntq      mmword ptr [ebx + 8],mm0
		punpckhbw   mm2,mm2
		psllw       mm2,8
		psrlw       mm2,7
		movq        mm0,mm2
		punpcklwd   mm0,mm0
		movq        mm7,mm0
		punpckldq   mm7,mm7
		psrlq       mm7,10h
		movq        mm6,mm0
		pslld       mm6,10h
		por         mm7,mm6
		paddsw      mm7,mmword ptr [esp+30h]
		paddsw      mm7,mmword ptr [esp+28h]
		psraw       mm1,1
		psraw       mm7,1
		packuswb    mm1,mm7
		movntq      mmword ptr [ecx + 8],mm1
		punpckhwd   mm2,mm2
		pshufw      mm6,mm2,0FCh
		psllq       mm2,20h
		punpckhdq   mm0,mm2
		movq        mm1,Tab02
		movq        mm2,mm3
		pmulhw      mm2,mm1
		paddsw      mm0,mm2
		movq        mm7,Tab05
		movq        mm1,mm4
		pmulhw      mm1,mm7
		paddsw      mm0,mm1
		punpckhdq   mm3,mm3
		pmulhw      mm3,Tab03
		paddsw      mm6,mm3
		punpckhdq   mm4,mm4
		pmulhw      mm4,Tab06
		paddsw      mm6,mm4
		psraw       mm0,1
		psraw       mm6,1
		packuswb    mm0,mm6
		movntq      mmword ptr [ecx+16],mm0
		add         ecx,18h
		movq        mm0,mmword ptr [esp+40h]
		punpckhwd   mm0,mm0
		pshufw      mm6,mm0,0FCh
		psllq       mm0,20h
		punpckhdq   mm5,mm0
		paddsw      mm5,mm2
		paddsw      mm5,mm1
		paddsw      mm6,mm4
		paddsw      mm6,mm3
		psraw       mm5,1
		psraw       mm6,1
		packuswb    mm5,mm6
		movntq      mmword ptr [ebx+16],mm5
		add         ebx,18h
		mov         esi,dword ptr [esp]
		add         esi,4
		mov         dword ptr [esp],esi
		mov         edi,dword ptr [esp+0Ch]
		cmp         esi,edi
		jl          loop1
		
	
		mov         esi,dword ptr [esp+56h]
		add			dword ptr [esp+48h],esi
		add			dword ptr [esp+4ch],esi

		mov         esi,dword ptr [esp+60h]
		add         eax,esi
		add         edx,esi

		mov         esi,dword ptr [esp+64h]
		sub         ecx,esi
		sub         ebx,esi
		
		mov         esi, 0
		mov         dword ptr [esp],esi

		dec         dword ptr [esp+52h]
		dec         dword ptr [esp+52h]
		cmp	        dword ptr [esp+52h],0
		jg          loop1 

		add         esp,74h		
		pop         esi
		pop         edi
		pop			ebx
		mov			esp, ebx    
		pop         ebx		
		emms
	}
	return;
}


//////////////////////////////////////////////////////////////////////
void CMPEG4Decoder::ConvertYUV420FrameToDIB()
{
/*
	m_csDecoder.Lock();

	m_bmi.bmiHeader.biBitCount      = 24;
	m_bmi.bmiHeader.biClrImportant  = 0;
	m_bmi.bmiHeader.biClrUsed       = 0;
	m_bmi.bmiHeader.biCompression   = 0;
	m_bmi.bmiHeader.biHeight        = m_OutFrameYUV420.height;
	m_bmi.bmiHeader.biPlanes        = 1;
	m_bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);;
	m_bmi.bmiHeader.biSizeImage     = m_OutFrameYUV420.width * m_OutFrameYUV420.height * 3;
	m_bmi.bmiHeader.biWidth         = m_OutFrameYUV420.width;
	m_bmi.bmiHeader.biXPelsPerMeter = 0;
	m_bmi.bmiHeader.biYPelsPerMeter = 0;

	// device-independent bitmap (DIB) erstellen
//	if(m_hDC)
//	{
		m_hDIB = CreateDIBSection(m_hDC, &m_bmi, DIB_RGB_COLORS, (void **)&m_pDibImageData, NULL, 0);

		// Konvertierung: YUV 4:2:0 -> RGB24
		gYUV420toRGB24(&m_OutFrameYUV420, m_pDibImageData);
//	}

	m_csDecoder.Lock();
*/
}



UINT CMPEG4Decoder::DecodeThreadProc(LPVOID pParam)
{
	CMPEG4Decoder*   pDec = (CMPEG4Decoder*)pParam;
	int32            lFrameBufLength = 0;
	int              iRet = E_FAILURE;
	tBaseVideoFrame* pCurOutFrameYUV = NULL;
	tStreamInfo      lStrmInfo;	// Streaminfo
	tRGBWindowParam  wndParam;

	
//	CWK_PerfMon perfMon("DecodeCounter1");

	// Decoder-Performance-Messung
	// ---------------------------
	//
	// Bei der Performance-Messung per CWK_PerfMon auf einem 2,4 GHz P4
	// konnten am 20.02.2004 folgende Zeiten ermittelt werden:
	// 
	// H263:
	// -----
	//        a.) CIF (352x288):  ~ 3083 us
	//            QCIF (176x144): ~ 765 us
	//
	//        b.) CIF (352x288):  ~ 2710 us
	//            QCIF (176x144): ~ 725 us
	//
	// MPEG4:
	// ------
	//        a.) QCIF (176x144): ~ 545 us
	//
	//        b.) QCIF (176x144): ~ 265 us
	//
	// Legende:
	// a.) H263-Frame aus dem Stream nehmen
	//       -> Buffer fuer YUV-Frame erzeugen
	//         -> Frame mittels vDecodeFrame(...) dekodieren
	//
	// b.) nur Frame mittels vDecodeFrame(...) dekodieren

	while(1)
	{
//		// Performance-Messung starten
//		perfMon.Start();

		if(pDec->m_streamInfo.streamType == VD_MPEG4_SP_STREAM)
		{
			// Read the compressed stream for a frame
			pDec->gReadVOP(pDec->m_pInputFrameBuffer,	// MP4-Frame
						   &lFrameBufLength,			// Buffer-Groesse des Frames
						   pDec->m_pInputFileStream);	// MP4-Input-File

			// Auslesen eines MPEG4SP-Frames fehlgeschlagen
			if(!lFrameBufLength)
				break;

			// Wurde StreamInfo oder Frame ausgelesen?
			if(lFrameBufLength == STREAM_INFO2_LENGTH)
			{
				// es liegt StreamInfo vor!
				// -> also StreamInfo-Struktur auslesen und Decoder reinitialisieren
				iRet = gGetStreamInfo(pDec->m_pInputFrameBuffer, lFrameBufLength, &lStrmInfo);
				if (iRet != E_SUCCESS)
				{
					TRACE(_T("Getting streamInfo failed\n"));
					break;
					//bRet = FALSE;
					//goto END_INIT;
				}
				else
				{
					pDec->m_streamInfo = lStrmInfo;

					wndParam.wndWidth   = lStrmInfo.width;
					wndParam.wndHeight  = lStrmInfo.height;
					wndParam.xOffset    = 0;	
					wndParam.yOffset    = 0;	
					wndParam.rotateFlag = 0;

					// alten MPEG-4 Simple Profile Decoder loeschen
					if(pDec->m_pMP4DecHandle != NULL)
					{
						pDec->m_pMP4DecHandle->vDelete(pDec->m_pMP4DecHandle);
						pDec->m_pMP4DecHandle = NULL;
					}

					// MPEG-4 Simple Profile Decoder erstellen
					iRet = gCreateMP4SPDecoder(	&pDec->m_pMP4DecHandle, 
												pDec->m_pInputFrameBuffer,
												//streamInfoBuffer,
												&lFrameBufLength, 
												pDec->m_lPostprocType,
												&wndParam);

				    if (iRet != E_SUCCESS)
					{
						TRACE(_T("Decoder creation failed \n"));
						break;
						//bRet = FALSE;
						//goto END_INIT;
					}

					// Decoder ok, jetzt nachfolgenden Frame auslesen
					pDec->gReadVOP(pDec->m_pInputFrameBuffer,	// MP4-Frame
								   &lFrameBufLength,			// Buffer-Groesse des Frames
								   pDec->m_pInputFileStream);	// MP4-Input-File

					// Auslesen des MPEG4SP-Frames fehlgeschlagen
					if(!lFrameBufLength)
						break;
				}
			}
		}

		if(pDec->m_streamInfo.streamType == VD_H263_BASELINE_STREAM)
		{
			// Read the compressed stream for a frame
			pDec->gReadVOPH263(pDec->m_pInputFrameBuffer,	// H263-Frame
						       &lFrameBufLength,			// Buffer-Groesse des Frames
						       pDec->m_pInputFileStream);	// H263-Input-File

			// Auslesen eines H263-Frames fehlgeschlagen
			if(lFrameBufLength <= 4)
				break;
		}

		pCurOutFrameYUV = pDec->CreateOutFrameYUV();
		//pDec->CreateOutFrameYUV();

		// MPEG4SP: einen Frame dekodieren 
		if(pDec->m_streamInfo.streamType == VD_MPEG4_SP_STREAM)
		{
			iRet = pDec->m_pMP4DecHandle->vDecodeFrame(pDec->m_pMP4DecHandle, 
													   pDec->m_pInputFrameBuffer, 
													   &lFrameBufLength,
													   pCurOutFrameYUV);
//			// Performance-Messung stoppen
//			perfMon.Stop();

		}

		// H263: einen Frame dekodieren 
		if(pDec->m_streamInfo.streamType == VD_H263_BASELINE_STREAM)
		{
			iRet = pDec->m_pH263DecHandle->vDecodeFrame(pDec->m_pH263DecHandle, 
													    pDec->m_pInputFrameBuffer, 
													    &lFrameBufLength,
													    pCurOutFrameYUV);
		}
	
		if(iRet)
		{
			TRACE(_T("error\n"));

			// Allozierten Frame wieder loeschen
			if(pCurOutFrameYUV != NULL)
			{
				if(pCurOutFrameYUV->cb != NULL)
				{
					delete pCurOutFrameYUV->cb;
					pCurOutFrameYUV->cb = NULL;
				}
				if(pCurOutFrameYUV->cr != NULL)
				{
					delete pCurOutFrameYUV->cr;
					pCurOutFrameYUV->cr = NULL;
				}
				if(pCurOutFrameYUV->lum != NULL)
				{
					delete pCurOutFrameYUV->lum;
					pCurOutFrameYUV->lum = NULL;
				}

				delete pCurOutFrameYUV;
				pCurOutFrameYUV = NULL;
			}
		}
		else
		{
			pDec->m_iFrameNr++;
			TRACE(_T("Y-width = %d   Y-height = %d   curtimestamp = %d ms   prevtimestamp = %d   FrameNr = %d\n"),pCurOutFrameYUV->width, pCurOutFrameYUV->height, pCurOutFrameYUV->timeStamp, pDec->m_iPrevTimestamp, pDec->m_iFrameNr);
			pDec->GetYUVFrameBufferList()->AddTail(pCurOutFrameYUV);
			TRACE(_T("YUVFrameBufferList count: %d\n"),pDec->GetYUVFrameBufferList()->GetCount());

			if(pDec->m_iPrevTimestamp >= 0)
			{
				TRACE(_T("framerate = %d ms \n"),pCurOutFrameYUV->timeStamp - pDec->m_iPrevTimestamp);
//				Sleep(pCurOutFrameYUV->timeStamp - pDec->m_iPrevTimestamp);
			}


			pDec->m_iPrevTimestamp = pCurOutFrameYUV->timeStamp;

//			if(pDec->m_iFrameNr == 2)
//				pDec->m_iPrevTimestamp = pCurOutFrameYUV->timeStamp;
//
//			Sleep(pDec->m_iPrevTimestamp);

			// Bild ist dekodiert -> also anzeigen
			pDec->m_pWnd->PostMessage(pDec->m_uMsg, pDec->m_wParam, pDec->m_lParam);

		}
	}

//	// Ergebnis der Performance-Messung ausgeben
//	perfMon.ShowResults();

	pDec->m_bThreadIsRunning = FALSE;

	AfxEndThread(0);

	return 0x0DEC;
}


CList<tBaseVideoFrame*, tBaseVideoFrame*> * CMPEG4Decoder::GetYUVFrameBufferList()
{
	return &m_listYUVFrameBuffer;
}

tBaseVideoFrame* CMPEG4Decoder::CreateOutFrameYUV()
{
	tBaseVideoFrame* pCurOutFrameYUV = NULL;

//	if(m_pOutFrameYUV420)
//	{
//		if(m_pOutFrameYUV420 != NULL)
//		{
//			// Speicher fuer Y-Werte freigeben
//			if(m_pOutFrameYUV420->lum != NULL)
//			{
//				free(m_pOutFrameYUV420->lum);
//				m_pOutFrameYUV420.lum = NULL;
//			}
//
//			// Speicher fuer U-Werte freigeben
//			if(m_pOutFrameYUV420->cb != NULL)
//			{
//				free(m_pOutFrameYUV420.cb);
//				m_pOutFrameYUV420->cb = NULL;
//			}
//			
//			// Speicher fuer V-Werte freigeben
//			if(m_pOutFrameYUV420->cr != NULL)
//			{
//				free(m_pOutFrameYUV420.cr);
//				m_pOutFrameYUV420->cr = NULL;
//			}
//
//			delete m_pOutFrameYUV420;
//			m_pOutFrameYUV420 = NULL
//		}
//	}

//	// neuen OutFrameYUV erstellen
//	m_pOutFrameYUV420 = new tBaseVideoFrame;
//	
//	// Speicher fuer Y-Werte des Frames allozieren
//	// -> iRet = Anzahl der Y-Werte des Frames
//	m_pOutFrameYUV420->lum = (uint8 *) malloc(m_iInputFrameLen);
//
//	// Speicher fuer U-Werte des Frames allozieren
//	// -> iRet >> 2 = Anzahl der U-Werte des Frames
//	m_pOutFrameYUV420->cb  = (uint8 *) malloc(m_iInputFrameLen >> 2);
//
//	// Speicher fuer V-Werte des Frames allozieren
//	// -> iRet >> 2 = Anzahl der V-Werte des Frames
//	m_pOutFrameYUV420->cr  = (uint8 *) malloc(m_iInputFrameLen >> 2);
//
//	m_pOutFrameYUV420->height = 0;
//	m_pOutFrameYUV420->width = 0;

	//return pCurOutFrameYUV;


	// neuen OutFrameYUV erstellen
	pCurOutFrameYUV = new tBaseVideoFrame;
	
	// Speicher fuer Y-Werte des Frames allozieren
	// -> iRet = Anzahl der Y-Werte des Frames
	pCurOutFrameYUV->lum = (uint8 *) malloc(m_iInputFrameLen);

	// Speicher fuer U-Werte des Frames allozieren
	// -> iRet >> 2 = Anzahl der U-Werte des Frames
	pCurOutFrameYUV->cb  = (uint8 *) malloc(m_iInputFrameLen >> 2);

	// Speicher fuer V-Werte des Frames allozieren
	// -> iRet >> 2 = Anzahl der V-Werte des Frames
	pCurOutFrameYUV->cr  = (uint8 *) malloc(m_iInputFrameLen >> 2);

	pCurOutFrameYUV->height = 0;
	pCurOutFrameYUV->width = 0;

	return pCurOutFrameYUV;
}

tStreamInfo CMPEG4Decoder::GetStreamInfo()
{
	return m_streamInfo;
}


//////////////////////////////////////////////////////////////////////
/* 
*  einen H263-Frame aus dem komprimierten Datenstrom lesen
*/
void CMPEG4Decoder::gReadVOPH263(uint8 *buffer, int32 *length, FILE *fp)
{
	unsigned char  *lBuffer;
	uint16  lCode;
	int32   lBlocksize = COMPRESSED_FRAME_SIZE;
	int32	lTemp, rear;
	int32   i;
    int32   lFlag, lCount;

	lFlag = 1;
	lBuffer = (unsigned char *) buffer;

    if ((lTemp = fread ((void *) lBuffer, 1, lBlocksize - 5, fp)) <= 0)
	{
		/* Raise expection */
        *length = 0;
        return;
	}
	
	if (!(buffer[0] | buffer[1]) && (buffer[2] == 1) && (buffer[3] == 0xB0 /*VOS_START_CODE*/))
	{
		lFlag = 0;
	}

	lCount = lTemp;
	if (lCount > (lBlocksize - 8))
		lCount = lBlocksize - 8;

    rear = lCount;

	for(i = 3; i < lCount; i++)
	{
		lCode = (uint16) (lBuffer[i] | lBuffer[i + 1]);
		if (!lCode)
		{
			if (((buffer[i + 2] & 0xfc) == 0x80 /*short_video_start_marker*/)
				|| ((buffer[i + 2] == 1) && (buffer[i + 3] == 0xB1) /*VOS_END_CODE*/))
			{
				if (lFlag == 0)
				{
					lFlag = 1;
				}
				else
				{
					rear = i;
					break;
				}
			}
		}
	}

	fseek(fp, rear - lTemp, SEEK_CUR);

    *length = rear;

	return;
}
