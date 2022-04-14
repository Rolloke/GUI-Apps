// Param.h: interface for the CParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAM_H__E5210C25_2C39_11D2_B56C_00C095EC9EFA__INCLUDED_)
#define AFX_PARAM_H__E5210C25_2C39_11D2_B56C_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CParam  
{
	// construction/destruction
public:
	CParam();
	virtual ~CParam();

	// attributes
public:

private:
	CString m_sName;
	CString m_sValue;
};

class CParamArray : public CTypedPtrArray<CPtrArray,CParam*>
{
	// construction/destruction
public:
	CParamArray();
	virtual ~CParamArray();
};

#endif // !defined(AFX_PARAM_H__E5210C25_2C39_11D2_B56C_00C095EC9EFA__INCLUDED_)
