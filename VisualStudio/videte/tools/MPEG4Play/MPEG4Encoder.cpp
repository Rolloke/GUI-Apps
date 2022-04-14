// MPEG4Encoder.cpp: implementation of the CMPEG4Encoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MPEG4Encoder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMPEG4Encoder::CMPEG4Encoder(CString lpszInputFilePathName)
{
	m_lpszInputFilePathName = lpszInputFilePathName;
	m_bThreadIsRunning      = FALSE;
	m_pMP4EncHandle         = NULL;
	m_pInputFrameBuffer     = NULL;
	m_pOutputFrameBuffer    = NULL;
    m_pPacketSizes          = NULL;
    m_pInputFileStream      = NULL;
    m_pOutputFileStream     = NULL;
	m_dwByteCnt             = 0;
    m_lOutBufSize           = 0;
    m_lInputFrameSize       = 0;

	// Encoding-Parameter
    m_lVideoEncParam.VOPHeight            = 0;
    m_lVideoEncParam.VOPWidth             = 0;
    m_lVideoEncParam.VOPRate              = 0;
    m_lVideoEncParam.BitRate              = 0;
    m_lVideoEncParam.PacketSize           = 0;
    m_lVideoEncParam.ResyncMarkerDisable  = 0; 
    m_lVideoEncParam.ReversibleVLC        = 0;       
    m_lVideoEncParam.DataPartitioning     = 0;   
    m_lVideoEncParam.UnrestrictedMV       = 0;      
    m_lVideoEncParam.Enable4MV            = 0;           
    m_lVideoEncParam.Level                = 0;     
    m_lVideoEncParam.BufferSize           = 0;
    m_lVideoEncParam.GOVLength            = 0;
    m_lVideoEncParam.MAIR                 = 0;
    m_lVideoEncParam.MAPS                 = 0;
    m_lVideoEncParam.RemoveImpulseNoise   = 0;
    m_lVideoEncParam.DeInterlace          = 0;
    m_lVideoEncParam.RCModel              = 0;
    m_lVideoEncParam.ShortVideoHeader     = 0;
    m_lVideoEncParam.EncoderPerformance   = 0;
    m_lVideoEncParam.TimerResolution      = 0;
    m_lVideoEncParam.SearchRange          = 0;
    m_lVideoEncParam.AdaptiveSearchRange  = 0;
    m_lVideoEncParam.BitErrorRate         = 0;
    m_lVideoEncParam.PacketLossRate       = 0;
    m_lVideoEncParam.NumGOBHeaders        = 0;
    m_lVideoEncParam.SceneChangeDetection = 0;
    m_lVideoEncParam.ACPrediction         = 0;
    m_lVideoEncParam.HeaderExtCode        = 0;
    m_lVideoEncParam.IntraDCVLCThr        = 0;
}

CMPEG4Encoder::~CMPEG4Encoder()
{
	// MP4EncoderHandle wieder loeschen
	if(m_pMP4EncHandle != NULL)
	{
	    gDestroyMPEG4VideoEncode(m_pMP4EncHandle);
		m_pMP4EncHandle = NULL;
	}

	// m_pInputFrameBuffer wieder loeschen
	if(m_pInputFrameBuffer != NULL)
	{
	    delete m_pInputFrameBuffer;
		m_pInputFrameBuffer = NULL;
	}

	// m_pOutputFrameBuffer wieder loeschen
	if(m_pOutputFrameBuffer != NULL)
	{
	    delete m_pOutputFrameBuffer;
		m_pOutputFrameBuffer = NULL;
	}

	// m_pPacketSizes wieder loeschen
	if(m_pPacketSizes != NULL)
	{
	    delete m_pPacketSizes;
		m_pPacketSizes = NULL;
	}

	// Output-Datei wieder schliessen
	if(m_pInputFileStream != NULL)
	{
		fclose(m_pInputFileStream);
	}

	// In- und Output-File schliessen
	if(m_pInputFileStream != NULL)
	{
		fclose(m_pInputFileStream);
	}

	if(m_pOutputFileStream != NULL)
	{
		fclose(m_pOutputFileStream);
	}

	if (m_pEncodeThread)
	{
//		if(m_pDecodeThread->m_hThread)
//			WaitForSingleObject(m_pDecodeThread->m_hThread,5000);
		if(m_bThreadIsRunning)
		{
			delete m_pEncodeThread;
			m_pEncodeThread = NULL;
		}
	}
}

BOOL CMPEG4Encoder::Init(int VOPWidth, int VOPHeight, int fps)
{
	//uint32				lFrmSize;
	//uint32				lOutBufSize;
	int32				lMaxPackets;
	int32				lReturn;
	BOOL                bRet = TRUE;

	ZeroMemory(m_pStreamInfoHeader, STREAM_INFO2_LENGTH);
	
/*
1. VOP width
176
2. VOP Height
144
3. Bit rate
384000
4. Frame rate
15
5. Packet size
8192
6. GOV Length
5
7. Buffer Size
655360
8. Resyncmarker Disable. OFF = 0, ON = 1
0
9. Reversible VLC. OFF = 0, ON = 1
0
10. Data partitioning. OFF = 0, ON = 1
0
11. Unrestricted motion vector. OFF = 0, ON = 1
1
12. Enable four motion vector. OFF = 0, ON = 1
0
13. Level Info.(supproted levels - 1,2,3,4,5,10 (level-0B))
3
14. Motion Adaptive Intra Refresh. OFF = 0, ON = 1
0
15. Motion Adaptive Packet Size.  OFF = 0, ON = 1
0
16. Removal of Impulse Noise. OFF = 0, CONSERVATIVE SMOOTHING = 1, MEDIAN = 2.
0
17. De-interlacing. OFF = 0, ON = 1
0
18. Rate Control Model. 0 for CBR, 1 for constrained VBR, 2 for unconstrained VBR
2
19. Short video header mode OFF = 0; ON = 1 
0
21. Encoder performance. best performance = 0; best quality = 1
1
22. Adaptive Search Range. OFF = 0; ON = 1
0
23.Bit Error Rate
0
24.Packet loss rate
0
25.Number of GOB Headers
0
26.Scene Change Detection
1
26.Timer Resolution
30
27.Search Range
31
28.AC prediction. OFF = 0, ON = 1
1
29.Header Extension Code. OFF = 0, ON = 1
0
30.Intra DC VLC Threshold. Value should be between 0 and 7. Value should be 0 for mobile multimedia.
0
*/

	// Parameter-Datei auslesen
	if(ReadParamFile())
	{
		// YUV-Datei oeffnen
		m_pInputFileStream = _tfopen(m_lpszInputFilePathName, _T("rb"));
		if(m_pInputFileStream == NULL)
		{
			TRACE(_T("YUV-Datei konnte nicht geoeffnet werden!\n"));
			bRet =  FALSE;
		}
		else
		{
			// MP4-Output-Datei oeffnen bzw. erstellen
			m_pOutputFileStream = _tfopen(_T("tempout.mp4"), _T("wb"));
			if(m_pOutputFileStream == NULL)
			{
				TRACE(_T("MP4-Output-Datei konnte nicht geoeffnet bzw. erstellt werden!\n"));
				bRet = FALSE;
			}
			else
			{
				// MPEG4SP-Encoder erstellen
				lReturn = gCreateMPEG4VideoEncode(&m_pMP4EncHandle, &m_lVideoEncParam);
				if(lReturn != E_SUCCESS)
				{
					TRACE(_T("Encoder creation failed\n"));
					switch(lReturn)
					{
					case E_INVALID_PARAMS:
						TRACE(_T("Invalid input parameters.\n"));
						break;
					case E_INVALID_SIZE:
						TRACE(_T("Invalid input video size.\n"));
						break;
					case E_OUT_OF_MEMORY:
						TRACE(_T("Insufficient Memory.\n"));
						break;
					default:
						TRACE(_T("Creation failed.\n"));
					}
					bRet = FALSE;
				}
				else
				{
					// YUV-Input-Frame-Buffer erstellen
					// - Y - Werte
					m_lInputFrameSize = m_lVideoEncParam.VOPWidth * m_lVideoEncParam.VOPHeight;
					// - U- und V - Werte
					m_lInputFrameSize += (m_lInputFrameSize >> 1);
    
					m_pInputFrameBuffer = (uint8 *) malloc(m_lInputFrameSize);

					if(m_pInputFrameBuffer == NULL)
					{
						TRACE(_T("InputBuffer memory allocation failed.\n"));
						bRet = FALSE;
					}
					else
					{
						// Output-Buffer fuer einen MP4-Frame erstellen
						gGetParamMPEG4Enc(m_pMP4EncHandle, GET_MAX_FRAME_SIZE, &m_lOutBufSize);

						m_pOutputFrameBuffer = (uint8 *) malloc(m_lOutBufSize);

						if(m_pOutputFrameBuffer == NULL)
						{
							TRACE(_T("OutputBuffer memory allocation failed.\n"));
							bRet = FALSE;
						}
						else
						{
							// Anzahl der packets ermitteln, die pro Video-Frame
							// angezeigt werden können.
							gGetParamMPEG4Enc(m_pMP4EncHandle,GET_MAX_NUM_PACKETS, (uint32 *)&lMaxPackets);

							m_pPacketSizes = (uint32*) malloc(lMaxPackets * sizeof(uint32));

							if(m_pPacketSizes == NULL)
							{
								TRACE(_T("Anzahl der packets pro Video-Frame konnte nicht ermittelt werden.\n"));
								bRet = FALSE;
							}
							else
							{
								if(!m_bThreadIsRunning)
								{
									m_bThreadIsRunning = TRUE;
									m_pEncodeThread = AfxBeginThread(EncodeThreadProc,(LPVOID)this, THREAD_PRIORITY_LOWEST);
									if(m_pEncodeThread == NULL)
									{
										TRACE(_T("Decoder-Thread konnte nicht erstellt werden!\n"));
									}
								}
							}
						}
					}
				}
			}
		}
	}

// -------------------

//	lTimeStamp = 0;
//
//
//	while(fread(lInputBuf, 1, lFrmSize, lInputFilePtr) == lFrmSize)
//	{
//		lNumBytes = lOutBufSize;
//
//		/* Encode input frame */
//		lReturn = gMP4SPEncodeFrame(m_pMP4EncHandle, lInputBuf, lOutputBuf,
//						&lNumBytes, lPacketSizes, lTimeStamp);
//
//		if(lReturn != E_SUCCESS)
//		{
//			TRACE(_T("Error code %d\n"),lReturn);
//			return FALSE;
//		}
//
//		TRACE(_T("."));
//
//		if(lNumBytes > 0)
//		{
//			fwrite(lOutputBuf,1,lNumBytes,lOutputFilePtr);
//		}
//
//		lTimeStamp += ((uint32)(m_lVideoEncParam.TimerResolution/m_lVideoEncParam.VOPRate));
//	}
//
//	/* Sequence End code */
//	lNumBytes = 0xB1010000;
//    fwrite(&lNumBytes,1,4, lOutputFilePtr);

//    gDestroyMPEG4VideoEncode(m_pMP4EncHandle);

//	/* Free all allocated buffers */
//	free(lPacketSizes);
//	free(lInputBuf);
//	free(lOutputBuf);

//	/* Close input and output files */
//	fclose(lInputFilePtr);
//	fclose(lOutputFilePtr);

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMPEG4Encoder::ReadParamFile()
{
	BOOL  bRet = TRUE;
	FILE* lPar;
    int8  lLine[256];

//	CFile paramFile;
//	char pLine[256];

	// Datei oeffnen
//	if(!paramFile.Open("param.txt", CFile::modeRead))
//	{
//		TRACE(_T("Parameter-Datei konnte nicht geoeffnet werden!\n"));
//		bRet = FALSE;
//	}
//	else
//	{
//		// Parameter auslesen:
//
//		// VOP Width
//		paramFile.Read(pLine, 256);
//		paramFile.Read(pLine, 256);
//		//sscanf((const char *)pLine, "%d", &m_lVideoEncParam.VOPWidth);
//		sscanf(pLine, "%d", &m_lVideoEncParam.VOPWidth);
//
//		// Datei schliessen
//		paramFile.Close();
//	}


	/* Open parameter file */
	lPar = fopen("param.txt","r");
	if(lPar == NULL)
	{
		TRACE(_T("Parameter-Datei konnte nicht geoeffnet werden!\n"));		
        bRet = FALSE;
	}
	else
	{
		/*VOP Width*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.VOPWidth);

		/*VOP Height*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.VOPHeight);

		/*Bit rate*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.BitRate);

		/*Frame rate*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%f", &m_lVideoEncParam.VOPRate);

		/*Packet Size*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.PacketSize);

		/*GOV Length*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.GOVLength);

		/*Buffer Size*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.BufferSize);

		/*Resync marker disable*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.ResyncMarkerDisable);

		/*Reversible VLC info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.ReversibleVLC);

		/*Data Partitioning info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.DataPartitioning);

		/*Unrestricted MV info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.UnrestrictedMV);

		/*Four MV info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.Enable4MV);

		/*Level info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.Level);

		/*Motion Adaptive Intra Refresh info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.MAIR);

		/*Motion Adaptive Packet Size info*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.MAPS);

		/*Removal of Impulse Noise*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.RemoveImpulseNoise);

		/*Removal of Impulse Noise*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.DeInterlace);

		/*Rate Control Modle*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.RCModel);

		/*Short video header mode*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.ShortVideoHeader);

		/*Encoder Performance*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.EncoderPerformance);

		/*Adaptive search range*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.AdaptiveSearchRange);

		/*Bit error rate*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.BitErrorRate);

		/*Packet loss rate*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.PacketLossRate);

		/*Number of GOB headers*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.NumGOBHeaders);

		/*Scene change Detection*/
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.SceneChangeDetection);

		/* Timer Resolution */
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.TimerResolution);

		/* Search Range */
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.SearchRange);

		/* AC prediction */
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.ACPrediction);

		/* Header Extension Code */
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.HeaderExtCode);

		/* Intra DC VLC Threshold */
		fgets((char *)lLine, 254, lPar);
		fgets((char *)lLine, 254, lPar);
		sscanf((const char *)lLine, "%d", &m_lVideoEncParam.IntraDCVLCThr);
	}


	return bRet;
}


//////////////////////////////////////////////////////////////////////
UINT CMPEG4Encoder::EncodeThreadProc(LPVOID pParam)
{
	CMPEG4Encoder*      pDec = (CMPEG4Encoder*)pParam;
	uint32				lTimeStamp;
	int32				lNumBytes;
	//uint32				lFrmSize;
	int32				lReturn;

	pDec->m_bThreadIsRunning = FALSE;

	lTimeStamp = 0;

	while(fread(pDec->m_pInputFrameBuffer,
                1,
				pDec->m_lInputFrameSize,
				pDec->m_pInputFileStream) == pDec->m_lInputFrameSize)
	{
		lNumBytes = pDec->m_lOutBufSize;

		// Input-Frame kodieren
		lReturn = gMP4SPEncodeFrame(pDec->m_pMP4EncHandle,
                                    pDec->m_pInputFrameBuffer,
									pDec->m_pOutputFrameBuffer,
									&lNumBytes,
									pDec->m_pPacketSizes,
									lTimeStamp);

		if(lReturn != E_SUCCESS)
		{
			TRACE(_T("Error code %d\n"),lReturn);
			return FALSE;
		}

		TRACE(_T("."));

		if(lNumBytes > 0)
		{
			// I- oder P-Frame
			if(pDec->IsStreamInfoHeader(pDec->m_pOutputFrameBuffer, lNumBytes))
			{
				TRACE(_T("---> I-Frame\n"));

				if(pDec->m_dwByteCnt == 0)
				{
					// erster Frame ueberhaupt, der den StreamInfo-Header beinhaltet
					// => also StreamInfo-Header merken
					CopyMemory(pDec->m_pStreamInfoHeader, pDec->m_pOutputFrameBuffer, STREAM_INFO2_LENGTH);
				}
				else
				{
					// erst StreamInfo-Header uebertragen...
					fwrite(pDec->m_pStreamInfoHeader, 1, STREAM_INFO2_LENGTH, pDec->m_pOutputFileStream);

					pDec->m_dwByteCnt += STREAM_INFO2_LENGTH;
				}
			}
			else
			{
				TRACE(_T("---> P-Frame\n"));
			}

			fwrite(pDec->m_pOutputFrameBuffer, 1, lNumBytes, pDec->m_pOutputFileStream);

			pDec->m_dwByteCnt += lNumBytes;
		}

		lTimeStamp += ((uint32)(pDec->m_lVideoEncParam.TimerResolution/pDec->m_lVideoEncParam.VOPRate));
	}

	// Sequence End code
	lNumBytes = 0xB1010000;
    fwrite(&lNumBytes, 1, 4, pDec->m_pOutputFileStream);

	fclose(pDec->m_pOutputFileStream);

	// Encoder-Thread beenden
	pDec->m_bThreadIsRunning = FALSE;
	AfxEndThread(0);

	return 0x0DEC;
}


BOOL CMPEG4Encoder::IsStreamInfoHeader(BYTE *buffer, DWORD length)
{
	BOOL bFound = FALSE;

	for(DWORD i = 0; i < length-3; i++)
	{
		if(   buffer[i] == 0
		   && buffer[i+1] == 0
		   && buffer[i+2] == 0x01
		   && buffer[i+3] == 0xB3)
		{
		   bFound = TRUE;
		   break;
		}
	}

	return bFound;

//	if(   buffer[28] == 0
//       && buffer[29] == 0
//	   && buffer[30] == 0x01
//       && buffer[31] == 0xB3)
//	{
//	   return TRUE;
//	}
//	else
//		return FALSE;
}


