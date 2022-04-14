///////////////////////////////////////////////////////////////
// FILE   : $Workfile: TestTool.h $ 
// ARCHIVE: $Archive: /Project/Tools/UnitTest/TestTool.h $
// DATE   :	$Date: 23.04.97 18:09 $ 
// VERSION: $Revision: 1 $
// LAST   : $Modtime: 11.04.97 10:54 $ 
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**headerref **************************************************
 * name    : TestTool.h
 * class   : CTestTool
 * derived : nil
 *         : 
 * remark  : See TestTool.cpp for the implementation and  
 *         : documentation of this class 
 * author  : Hajo Fierke 1997 w+k
 * history : 09.04.97 hajo 
 *		   : 10.04.97 hajo added coco test params
 *		   : 
 **************************************************************/
#ifndef CTESTTOOL_H
	#define CTESTTOOL_H

#include "PictureDef.h"

class CTestTool 
{
public:
	CTestTool();
	~CTestTool();
	// access:
	inline	BOOL	GetDoVerify() const;
	inline	CString	GetTestFileName() const;
	inline	CString	GetHost() const;
	inline	int		GetDestination() const;
	inline	int		GetSizeofpData() const;
	inline	int		GetLoop() const;
	inline	int		GetDoDelete() const;
	void	SplitArgs(const CString &sCmdLine);
	CString	ParseOptions();
	CString	FindHostNumberByName(const CString &sHostsname)	const;
	//new ones for coconut
	inline	CString		GetShmName() const;
	inline	Compression	GetCompression() const;
	inline	Resolution	GetResolution() const;
	inline	WORD		GetNumPictures() const;
	inline	DWORD		GetBitrateScale() const;
	inline	DWORD		GetUserData() const;

private:	// data
	CString			m_szHost;
	CString			m_szUserDummy;
	CString			m_sTestFileName;	
	int				m_iDestination;	
	int				m_iSizeofpData;	
	CStringArray	m_lpCmdLineArray;
	BOOL			m_bDoVerify;
	BOOL			m_bDoDelete;
	int				m_iLoop;			
	//new ones for coconut
	CString			m_shmName;
	Compression		m_Compression;
	Resolution		m_Resolution;
	WORD			m_wNumPictures;
	DWORD			m_dwBitrateScale;

};

inline BOOL CTestTool::GetDoVerify() const
{
	return m_bDoVerify;
}

inline BOOL CTestTool::GetDoDelete() const
{
	return m_bDoDelete;
}

inline CString CTestTool::GetTestFileName() const
{
	return m_sTestFileName;
}

inline CString CTestTool::GetHost() const
{
	return m_szHost;
}

inline int CTestTool::GetDestination() const
{
	return m_iDestination;
}

inline int CTestTool::GetSizeofpData()const
{
	return m_iSizeofpData;
}

inline int 	CTestTool::GetLoop() const
{
	return m_iLoop;
}
// these are the brand new keys				 
inline CString CTestTool::GetShmName() const
{
	return m_shmName;
}
inline Compression CTestTool::GetCompression() const
{
	return m_Compression;
}
inline Resolution CTestTool::GetResolution() const
{
	return m_Resolution;
}
inline WORD CTestTool::GetNumPictures() const
{
	return m_wNumPictures;
}
inline DWORD CTestTool::GetBitrateScale() const
{
	return m_dwBitrateScale;
}

#endif