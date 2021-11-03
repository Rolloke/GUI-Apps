// TestCara3D.h: Schnittstelle für die Klasse CTestCara3D.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTCARA3D_H__6B7FE942_D07D_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_TESTCARA3D_H__6B7FE942_D07D_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ETS3DGL.h"
#include "CVector.h"

#define TEX_SIZE 4
#define stripeImageWidth 32

class CTestCara3D : public CEts3DGL
{
public:     // virtual Memberfunctions
	DWORD GetThreadID();
	virtual ~CTestCara3D();
   virtual void      InitGLLists();
   virtual void      DestroyGLLists();
   virtual void      OnRenderSzene();
   virtual LRESULT   OnTimer(long, BOOL);
	virtual bool      OnMessage(UINT, WPARAM, LPARAM);
	virtual LRESULT   OnCommand(WORD, WORD, HWND);
   virtual void      AutoDelete();
   virtual void      OnDrawToRenderDC(HDC, int);

            // Memberfunctions
	CTestCara3D();
	CTestCara3D(CTestCara3D*);
	void InitTest();
	void LoadBitmap(char *pszFile);
	void DetachData();

private:
   int      m_nPhi;
   bool     m_bRun;
   BYTE    *m_pTexImage;
   int      m_nWidth;
   int      m_nHeight;
   CVector  m_vObjectPos;
   CGlLight m_Light;

private:
	static void LightCommand(CTestCara3D*, int, int);
	static void ListMatrix(  CTestCara3D*, int, int);
	static void ListObject(  CTestCara3D*, int, int);
	static void ListLight(   CTestCara3D*, int, int);
};

#endif // !defined(AFX_TESTCARA3D_H__6B7FE942_D07D_11D2_9E4E_0000B458D891__INCLUDED_)
