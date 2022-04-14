// avcodec_sample.cpp

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use
//
// g++ -o avcodec_sample avcodec_sample.cpp -lavformat -lavcodec -lz
//
// to build (assuming libavformat and libavcodec are correctly installed on
// your system).
//
// Run using
//
// avcodec_sample myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.
#include "stdafx.h"
#include "CAVCodec.h"

int  DecodeFromFile(const char* pFileName);
int  DecodeFromMemory(unsigned char* pBuffer, unsigned long dwLen);
bool GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, int videoStream, AVFrame *pFrame);
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// Open file
    FILE* pFile=_tfopen(CString(argv[1]), _T("rb"));
    if(pFile)
	{
		fpos_t nLen;
		fseek(pFile, 0, SEEK_END);
		fgetpos(pFile, &nLen);
		fseek(pFile, 0, SEEK_SET);
		uint8_t* pRawdata = new uint8_t[nLen];
		uint8_t* pDataBase = pRawdata;

		if (pRawdata)
		{
			int nBytes = fread(pRawdata, 1, nLen, pFile);
			fclose(pFile);
			
			if (nBytes == nLen)
			{
				int   nLens[10000] = {0};

				CAVCodec AVCodec;
				//if (AVCodec.Create(_T("m4v")))
				if (AVCodec.Open(argv[1]))
				{
					unsigned int nTC = GetTickCount();
					int nFrames = 0;
					do
					{
						int nDecodedBytes = AVCodec.DecodeNextFrame(pRawdata, nBytes);
						if (nDecodedBytes == 0)
							break;
						// Für den eMuzed Decoder die Frames separieren.
						nLens[nFrames] = nDecodedBytes;

						nBytes-= nDecodedBytes;
						pRawdata += nDecodedBytes;
						nFrames++;
					}while (nBytes > 0);

					unsigned int nDiff = GetTickCount() - nTC;
					float fTpF = (float)nDiff/(float)nFrames;
					float fFps = 1000.0/fTpF;
					_tprintf(_T("AVCodec: frames=%d, total time=%.02fs, time per frame=%.02fms, frames/s=%.02f\n"), nFrames, (float)nDiff/1000.0, fTpF, fFps);

	
					CMPEG4Decoder MPeg4Dec;
					nTC = GetTickCount();
					nFrames = 0;
					pRawdata = pDataBase;
					do
					{
						bool bRet = MPeg4Dec.DecodeMpeg4FromMemory(pRawdata, nLens[nFrames]);
						if (!bRet)
							break;
						pRawdata += nLens[nFrames];
						nFrames++;
					}while (nLens[nFrames] > 0);

					nDiff = GetTickCount() - nTC;
					fTpF = (float)nDiff/(float)nFrames;
					fFps = 1000.0/fTpF;
					_tprintf(_T("eMuzed:  frames=%d, total time=%.02fs, time per frame=%.02fms, frames/s=%.02f\n"), nFrames, (float)nDiff/1000.0, fTpF, fFps);


				}
			}

			delete pDataBase;
			pRawdata = NULL;
		}
	}

	_tprintf(_T("press a key to exit\n"));
	getch();

    return 0;
}

