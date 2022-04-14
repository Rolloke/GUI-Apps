#include <Vfw.h>         // Video for Windows support

#define TEXT_HEIGHT	20
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE 260

class CAVIFile : public CObject
{
public:
	CAVIFile(LPCTSTR szFileName, int iFps, int iXDim=-1, int iYDim=-1);
	virtual ~CAVIFile();

	BOOL	IsOK() const;
	BOOL	AddFrame(CBitmap& bmp);
	BOOL	Compress();
	CString	GetFileName() const;

private:
	CString					m_sFileName;
	int						m_iXDim;
	int						m_iYDim;

	AVISTREAMINFO			m_AviStreamInfo;
	PAVIFILE				m_pAviFile;
	PAVISTREAM				m_pAviStream;
	PAVISTREAM				m_pAviStreamCompressed;
	PAVISTREAM				m_pAviStreamText;
	AVICOMPRESSOPTIONS		m_AviCompressOptions;
	AVICOMPRESSOPTIONS FAR*	aopts[1];
	DWORD					m_dwTextFormat;
	char					m_szText[BUFSIZE];
	int						m_iFrames;
	int						m_iFps;
	BOOL					m_bOK;
};

