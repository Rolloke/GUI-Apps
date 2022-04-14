// VDBCameraMap.h: interface for the CVDBCameraMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VDBCAMERAMAP_H__DC299A1B_25CA_4C5C_A13D_6741B3DCC304__INCLUDED_)
#define AFX_VDBCAMERAMAP_H__DC299A1B_25CA_4C5C_A13D_6741B3DCC304__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVDBCameraMap : public CMapStringToString  
{
	// construction/destruction
public:
	CVDBCameraMap();
	virtual ~CVDBCameraMap();

	// operations
public:
	void Load(const CString& sPathname);
	void Save(const CString& sPathname);
	void DeleteContent(void);

	// data member
private:
	Code4		m_CodeBase;
	Field4info  m_Fields;
};

#endif // !defined(AFX_VDBCAMERAMAP_H__DC299A1B_25CA_4C5C_A13D_6741B3DCC304__INCLUDED_)
