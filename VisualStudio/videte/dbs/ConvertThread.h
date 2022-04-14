// ConvertThread.h: interface for the CConvertThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTTHREAD_H__20EA8298_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
#define AFX_CONVERTTHREAD_H__20EA8298_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConvertThread : public CWK_Thread  
{
public:
	CConvertThread();
	virtual ~CConvertThread();

	// attributes
public:

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);

};

#endif // !defined(AFX_CONVERTTHREAD_H__20EA8298_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
