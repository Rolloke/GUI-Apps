// H263Decoder.h: interface for the CH263Decoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H263DECODER_H__265171F9_5DC4_11D1_905F_444553540000__INCLUDED_)
#define AFX_H263DECODER_H__265171F9_5DC4_11D1_905F_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "tmndec.h"

#include "H263DirectDraw.h"
#include "YUVToRGB.h"

//#define _REFIDCT
//#define _IDCT_INLINE
//////////////////////////////////////////////////////////////////////
typedef struct {
	char run, level, len;
} DCTtab;
//////////////////////////////////////////////////////////////////////
typedef struct {
	int val, run, sign;
} RunCoef;
//////////////////////////////////////////////////////////////////////
typedef struct {
  int val, len;
} VLCtab;
//////////////////////////////////////////////////////////////////////
class CBits
{
public:
	CBits();
	~CBits();

	BOOL				Load(const CString& sFileName);
	inline void			init();
	inline unsigned int	showbits(int n);
	inline unsigned int	getbits1();
	inline void			flushbits(int n);
	inline unsigned int	getbits(int n);

private:
	int		m_ByteCount;
	int		m_BitCount;
	BYTE	m_bitMsk[8];

	BYTE*	m_pData;
	DWORD	m_dwDataLen;
};
//////////////////////////////////////////////////////////////////////
/* Some macros */
#define mmax(a, b)        ((a) > (b) ? (a) : (b))
#define mmin(a, b)        ((a) < (b) ? (a) : (b))
#define mnint(a)        ((a) < 0 ? (int)(a - 0.5) : (int)(a + 0.5))
#define sign(a)         ((a) < 0 ? -1 : 1)

UINT DecodeThreadProc(LPVOID lpParam);

//////////////////////////////////////////////////////////////////////
class CH263Decoder  
{
public:
	CH263Decoder();
	virtual ~CH263Decoder();

	// operation
public:
	BOOL	GetFit();
	void	SetFit(BOOL bFit);
	
	int		GetZoom();
	void	SetZoom(int iZoom);

	int		GetHeight();
	int		GetWidth();

	// operation
public:
	void Init(HWND hWnd);
	void Decode(const CString& sFilename);
	void Exit();
	void OnDraw(HDC hDC);
	// Implementation
private:
	// H263Decoder.cpp
	void initdecoder();
	int	 initDisplay (int pels, int lines);
	int	 closeDisplay ();
	int  displayImage (unsigned char *lum, unsigned char *Cr, unsigned char *Cb);
	int  DrawDIB();

	// getbits.cpp

	// getblk.cpp
	void			getblock(int comp, int mode);
	void			get_sac_block(int comp, int mode);
	RunCoef			vlc_word_decode (int symbol_word, int *last); 
	RunCoef			Decode_Escape_Char (int intra, int *last);
	int				DecodeTCoef (int position, int intra);

	// gethdr.cpp
	int	 getheader();
	void startcode();
	void getpicturehdr();

	// getpic.cpp
	void getpicture(int *framenum);
	void putlast(int framenum);
	void getMBs(int framenum);
	void clearblock(int comp);
	void addblock(int comp,int bx,int by,int addflag);
	void reconblock_b(int comp,int bx,int by,int mode,int bdx,int bdy);
	int	 motion_decode(int vec,int pmv);
	int	 find_pmv(int x, int y, int block,int comp);


	// getvlc.cpp
	int	getTMNMV();
	int	getMCBPC();
	int	getMODB();
	int	getMCBPCintra();
	int	getCBPY();

	// idct.cpp		
#ifdef _IDCT_INLINE
	inline void idct(short *block);
	inline void init_idct();
	inline void idctrow(short *blk);
	inline void idctcol(short *blk);
#else
	void idct(short *block);
	void init_idct();
	void idctrow(short *blk);
	void idctcol(short *blk);
#endif

	// idctref.cpp
	void init_idctref();
	void idctref(short *block);

	// tmndec.cpp
	void error(char *text);
	void printbits(int code, int bits, int len);


	// recon.c
	void reconstruct(int bx, int by, int P, int bdx, int bdy);
	void recon_comp (unsigned char *src, unsigned char *dst,
									   int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int flag);
	void recon_comp_obmc (unsigned char *src, unsigned char *dst,int lx,int lx2,int comp,int w,int h,int x,int y);
	void rec (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void recc (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void reco (unsigned char *s, int *d, int lx, int lx2, int addflag,int c, int xa, int xb, int ya, int yb);
	void rech (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void rechc (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void recho (unsigned char *s, int *d, int lx, int lx2, int addflag,int c, int xa, int xb, int ya, int yb);
	void recv (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void recvc (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void recvo (unsigned char *s, int *d, int lx, int lx2, int addflag,int c, int xa, int xb, int ya, int yb);
	void rec4 (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void rec4c (unsigned char *s, unsigned char *d, int lx, int lx2, int h);
	void rec4o (unsigned char *s, int *d, int lx, int lx2, int addflag,int c, int xa, int xb, int ya, int yb);

	// sac.c
	int	 decode_a_symbol(int cumul_freq[ ]);
	void decoder_reset( );
	void bit_out_psc_layer();

	// display.c
	void init_display(char *name);
	void exit_display();
	void dither(unsigned char *src[]);
	void init_dither();

	// data members
private:
	CString			m_sFPS;
	CH263DirectDraw	m_DD;

	CYUVToRGB		m_YUVToRGB;

	CBits	m_Bits;
	BYTE*	m_clp;
	BYTE*   m_refframe[3];
	BYTE*	m_oldrefframe[3];
	BYTE*	m_bframe[3];
	BYTE*	m_newframe[3];
	BYTE*	m_edgeframe[3];
	BYTE*	m_edgeframeorig[3];

	BOOL	m_bFit;
	BOOL	m_bGDI;
	BOOL	m_bDD;

	BOOL		m_bRun;
	CWinThread*	m_pDecodeThread;
	CEvent		m_EventPictureArrived;
	CEvent		m_EventPictureBlitted;

	HWND m_hWnd;
	HDRAWDIB m_hDrawDib;
	BITMAPINFOHEADER m_biHeader;
	BOOL m_bImageIsReady;
	BYTE* m_pBufRGB;
	BYTE* m_pSrc[3];

	int m_iWidth, m_iHeight;
	int m_iZoom;
	
	/* block data */
	short block[12][64];

	BOOL m_bQuiet;
	int trace;
	int fault;
	int bquant;
	int quant;
	int temp_ref, prev_temp_ref, source_format;
	int trd, trb;
	int pict_type,newgob;
	int mv_outside_frame,syntax_arith_coding,adv_pred_mode,pb_frame;
	int long_vectors;
	int MV[2][5][MBR+1][MBC+2];
	int modemap[MBR+1][MBC+2];
	int horizontal_size,vertical_size,mb_width,mb_height;
	int coded_picture_width, coded_picture_height;
	int chrom_width,chrom_height,blk_cnt;

	short iclip[1024]; /* clipping table */
	short *iclp;
	double c[8][8];

	friend UINT DecodeThreadProc(LPVOID lpParam);

};
//////////////////////////////////////////////////////////////////////
#include "Bits.inl"
//////////////////////////////////////////////////////////////////////
#ifdef _IDCT_INLINE
	#include "idct.inl"
#endif
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_H263DECODER_H__265171F9_5DC4_11D1_905F_444553540000__INCLUDED_)
