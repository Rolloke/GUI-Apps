#include "StdAfx.h"
#include "CAVCodec.h"

/////////////////////////////////////////////////////////////////////////////////////
CAVCodec::CAVCodec(void)
{
	m_pAVInputFormat= NULL;
	m_pFormatCtx	= NULL;
	m_pCodecCtx		= NULL;
	m_pFrame		= NULL; 
	m_pFrameRGB		= NULL;
	m_pBuffer		= NULL;
	m_hDrawDib		= NULL;

	m_nFrames		= 0;

	ZeroMemory(&m_bmi,sizeof(m_bmi));
	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Register all formats and codecs
    av_register_all();

	m_hDrawDib	= DrawDibOpen(); 
}

/////////////////////////////////////////////////////////////////////////////////////
CAVCodec::~CAVCodec(void)
{
	if (m_hDrawDib)
		DrawDibClose(m_hDrawDib);

	Destroy();
}

/////////////////////////////////////////////////////////////////////////////////////
void CAVCodec::Destroy()
{
	if (m_pFrame)
	{
		// Free the YUV frame
		av_free(m_pFrame);
		m_pFrame = NULL;
	}

	if (m_pFrameRGB)
	{
		// Free the RGB image
	    av_free(m_pFrameRGB);
		m_pFrameRGB = NULL;
	}

	if (m_pBuffer)
	{
		delete[]m_pBuffer;
		m_pBuffer = NULL;
	}

	if (m_pCodecCtx)
	{
		// Close the codec
		avcodec_close(m_pCodecCtx);
		m_pCodecCtx = NULL;
	}

	if (m_pFormatCtx)
	{
		// Close the video file
		av_close_input_file(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CAVCodec::Create(const CString& sShortName)
{
	BOOL bResult = FALSE;
	Destroy();
	
	m_pAVInputFormat = av_find_input_format(CStringA(sShortName));
	if (m_pAVInputFormat)
	{
		m_pFormatCtx = av_alloc_format_context();
	
		if (m_pFormatCtx)
		{
			m_pFormatCtx->iformat	= m_pAVInputFormat;
			m_pFormatCtx->priv_data	= NULL;
			
			if (m_pAVInputFormat->read_header(m_pFormatCtx, NULL) >= 0)
			{
				m_pFormatCtx->streams[0]->codec.codec_type = CODEC_TYPE_VIDEO;

				// Get a pointer to the codec context for the video stream
				m_pCodecCtx = &m_pFormatCtx->streams[0]->codec;
				
				if (m_pCodecCtx)
				{
					// Find the decoder for the video stream
					AVCodec* pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
					if (pCodec)
					{
						// Inform the codec that we can handle truncated bitstreams -- i.e.,
						// bitstreams where frame boundaries can fall in the middle of packets
						//if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
						//	m_pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;
			
						// Open codec
						if (avcodec_open(m_pCodecCtx, pCodec) >= 0)
						{
							// Allocate video frame
							m_pFrame = avcodec_alloc_frame();
							if (m_pFrame)
							{
								// Allocate an AVFrame structure
								m_pFrameRGB = avcodec_alloc_frame();
								if (m_pFrameRGB)
								{
									// Determine required buffer size and allocate buffer
									int numBytes = avpicture_get_size(PIX_FMT_BGR24, MAX_HSIZE, MAX_VSIZE);
									m_pBuffer = new BYTE[numBytes];

									// Assign appropriate parts of buffer to image planes in pFrameRGB
									avpicture_fill((AVPicture *)m_pFrameRGB, m_pBuffer, PIX_FMT_BGR24, MAX_HSIZE, MAX_VSIZE);

									av_init_packet(&m_packet);
									bResult = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CAVCodec::Open(const CString& sFileName)
{
	BOOL bResult = FALSE;

	Destroy();

	// Open video file
    if (av_open_input_file(&m_pFormatCtx, CStringA(sFileName), NULL, 0, NULL) == 0)
	{
		// Retrieve stream information
		if (av_find_stream_info(m_pFormatCtx) >= 0)
		{
			// Dump information about file onto standard error
			//dump_format(m_pFormatCtx, 0, sFileName, false);

			// Find the first video stream
			int videoStream = -1;
			for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
			{
				if (m_pFormatCtx->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO)
				{
					videoStream = i;
					break;
				}
			}
			if (videoStream != -1)
			{
				// Get a pointer to the codec context for the video stream
				m_pCodecCtx = &m_pFormatCtx->streams[videoStream]->codec;
						
				if (m_pCodecCtx)
				{
					// Find the decoder for the video stream
					AVCodec* pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
					if (pCodec)
					{
						// Inform the codec that we can handle truncated bitstreams -- i.e.,
						// bitstreams where frame boundaries can fall in the middle of packets
						if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
							m_pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;
			
						// Open codec
						if (avcodec_open(m_pCodecCtx, pCodec) >= 0)
						{
							// Allocate video frame
							m_pFrame = avcodec_alloc_frame();
							if (m_pFrame)
							{
								// Allocate an AVFrame structure
								m_pFrameRGB = avcodec_alloc_frame();
								if (m_pFrameRGB)
								{
									// Determine required buffer size and allocate buffer
									int numBytes = avpicture_get_size(PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);
									m_pBuffer = new uint8_t[numBytes];

									// Assign appropriate parts of buffer to image planes in pFrameRGB
									avpicture_fill((AVPicture *)m_pFrameRGB, m_pBuffer, PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

									av_init_packet(&m_packet);
									bResult = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////
int CAVCodec::DecodeNextFrame(BYTE* pData, DWORD dwLen)
{
	int	frameFinished		= 0;
	int nBytesDecoded		= 0;

	m_packet.data			= pData;
	m_packet.size			= dwLen;
	m_packet.stream_index	= 0;
    m_packet.destruct		= NULL;
	m_packet.priv			= NULL; 
 
	m_pCodecCtx->idct_algo		= FF_IDCT_AUTO;

    m_pCodecCtx->hurry_up = 5;
	if (avcodec_decode_video(m_pCodecCtx, m_pFrame, &frameFinished, m_packet.data, m_packet.size) > 0)
	{
		// Decode the next chunk of data
		m_pCodecCtx->hurry_up = 0;
		nBytesDecoded = avcodec_decode_video(m_pCodecCtx, m_pFrame, &frameFinished, m_packet.data, m_packet.size);
		//TRACE(_T("BytesDecoded=%d frameFinished=%d, dwLen=%d\n"), nBytesDecoded, frameFinished, dwLen);

		if (nBytesDecoded > 0)
		{
			// Assign appropriate parts of buffer to image planes in pFrameRGB
			avpicture_fill((AVPicture *)m_pFrameRGB, m_pBuffer, PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);
		
			// Save the frame to disk
			// SaveFrame(m_pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height, ++m_nFrames);
		}
	}
	return nBytesDecoded;
}

BOOL CAVCodec::OnDraw(CDC *pDC, const CRect &rectDest, const CRect &rectSrc/*=CRect(0,0,0,0)*/)
{
	BOOL bRet = FALSE;
	CRect rectTo, rectFrom;			 


	// Ist ein Quellrechteck vorgegeben?
	if (rectSrc.IsRectEmpty())
	{
		// Nein, dann nimm die Originalgröße des MPEG4-Bildes.
		rectFrom.left   = 0;
		rectFrom.top	= 0;
		rectFrom.right	= m_pCodecCtx->width;
		rectFrom.bottom	= m_pCodecCtx->height;
	}
	else
	{
		// Ja, dann nimm dieses.
		rectFrom		= rectSrc;
		//rectFrom.bottom -= m_streamInfo.extHeight-m_streamInfo.height;
	}

	// ist ein Zielrechteck vorgegeben?
	if (rectDest.IsRectEmpty())
	{
		// Nein
		rectTo.left		= 0;
		rectTo.top		= 0;
		rectFrom.right	= m_pCodecCtx->width;
		rectFrom.bottom	= m_pCodecCtx->height;
	}
	else
	{
		// Ja, dann nimm das vorgegebene Zielrechteck.
		rectTo	 = rectDest;
	}

	// Parameter ok?
 	if ((rectTo.IsRectEmpty())||
		(rectFrom.Width() > m_pCodecCtx->width) ||(rectFrom.Height() > m_pCodecCtx->height))
	{
		return bRet;
	}	

	if (m_pFrameRGB)
	{
		// YUV420->RGB24
		img_convert((AVPicture *)m_pFrameRGB, PIX_FMT_BGR24, (AVPicture*)m_pFrame, 
								m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height);

		m_bmi.bmiHeader.biBitCount      = 24;
		m_bmi.bmiHeader.biClrImportant  = 0;
		m_bmi.bmiHeader.biClrUsed       = 0;
		m_bmi.bmiHeader.biCompression   = BI_RGB;
		m_bmi.bmiHeader.biHeight        = m_pCodecCtx->height; 
		m_bmi.bmiHeader.biWidth         = m_pCodecCtx->width; 
		m_bmi.bmiHeader.biPlanes        = 1;
		m_bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		m_bmi.bmiHeader.biSizeImage     = m_bmi.bmiHeader.biHeight*m_bmi.bmiHeader.biWidth * 3;
		m_bmi.bmiHeader.biXPelsPerMeter = 0;
		m_bmi.bmiHeader.biYPelsPerMeter = 0;

		FlipVertical();

		if (pDC->IsPrinting() || (m_hDrawDib==NULL))
		{
			pDC->SetStretchBltMode(COLORONCOLOR);

			// recalculate coordinates
			// to prevent from negative ones
			int h = rectFrom.Height();
			rectFrom.bottom = abs(m_bmi.bmiHeader.biHeight) - rectFrom.top;
			rectFrom.top = rectFrom.bottom - h; 

			int iRet = StretchDIBits(pDC->m_hDC,				// handle to device context
				rectTo.left,				// x-coordinate of upper-left corner of dest. rect.
				rectTo.top,               // y-coordinate of upper-left corner of dest. rect.
				rectTo.Width(),			// width of destination rectangle
				rectTo.Height(),          // height of destination rectangle
				rectFrom.left,			// x-coordinate of upper-left corner of source rect.
				rectFrom.top, 	        // y-coordinate of upper-left corner of source rect.
				rectFrom.Width(), 		// width of source rectangle
				rectFrom.Height(),		// height of source rectangle
				m_pFrameRGB->data[0],	// address of bitmap bits
				(BITMAPINFO*)&m_bmi,	// address of bitmap data
				DIB_RGB_COLORS,           // usage
				SRCCOPY			        // raster operation code
				);

			if (iRet == GDI_ERROR)
			{
				_tprintf(_T("mpeg StretchDIBits failed \n"));
			}
			else
			{
				bRet = TRUE;
			}
		}
		else
		{
			// TODO! RKE: Performance optimierung notwendig
			// PerformanceTests auf 2.4 GHz Hyperthreading Rechner:
			//  WaitOnCriticalSec            :  2 us, bei StretchDIBits(HALFTONE) bis zu 7 ms
			//  gYUV420toRGB24               :  1.7 ms
			//  Draw
			//  0 DrawDibDraw(COLORONCOLOR)  :  8.5 ms
			//  1 DrawDibDraw(HALFTONE)      :  8.5 ms, ändert die Ausgabe nicht
			//  2 StretchDIBits(COLORONCOLOR): 20   ms
			//  3 StretchDIBits(HALFTONE)    : 60   ms, Interpoliert sieht es aber sehr gut aus.
			bRet = DrawDibDraw(m_hDrawDib,
					pDC->m_hDC,
					rectTo.left,
					rectTo.top,
					rectTo.Width(),
					rectTo.Height(),
					&m_bmi.bmiHeader,
					m_pFrameRGB->data[0],
					rectFrom.left,
					rectFrom.top,
					rectFrom.Width(),
					rectFrom.Height(),
					0);
		}
	}
	else
	{
		TRACE(_T("[CMPEG4Decoder::OnDraw] Fehler bei der Erstellung des DIBs\n"));
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////////
void CAVCodec::SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    TCHAR szFilename[32];

    // Open file
    _stprintf(szFilename, _T("frame%05d.ppm"), iFrame);
    pFile=_tfopen(szFilename, _T("wb"));
    if (pFile)
	{
		// Write header
		_ftprintf(pFile, _T("P6\n%d %d\n255\n"), width, height);

		// Write pixel data
		for(int y = 0; y < height; y++)
			fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

		// Close file
		fclose(pFile);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CAVCodec::FlipVertical()
{
	// Bild vertikal spiegeln
	BYTE DibLine[768*3];
	int nSrcIndex = 0;
	int nDstIndex = 0;
	BYTE* pSrc		= m_pFrameRGB->data[0];
	BYTE* pDst		= m_pFrameRGB->data[0];
	int   nLineSize = m_pFrameRGB->linesize[0];

	for (int nY = 0; nY < m_pCodecCtx->height/2; nY++)
	{
		nDstIndex = (m_pCodecCtx->height-nY-1)*nLineSize;
		nSrcIndex = nY*nLineSize;
		memcpy(&DibLine[0],			&pDst[nDstIndex],	nLineSize); 
		memcpy(&pDst[nDstIndex],	&pSrc[nSrcIndex],	nLineSize); 
		memcpy(&pSrc[nSrcIndex],	&DibLine[0],		nLineSize); 
	}
	
	return TRUE;
}
