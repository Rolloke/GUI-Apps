// IPCamera.cpp: implementation of the CIPCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "viewcam.h"
#include "IPCamera.h"
#include "IPCOutputViewcam.h"
#include "viewcamDlg.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCamera::CIPCamera(const char* shmName)
:CIPCOutputClient(shmName,FALSE)
{
	g_i=0;
}

CIPCamera::~CIPCamera()
{

}

void CIPCamera::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData)
{

}
