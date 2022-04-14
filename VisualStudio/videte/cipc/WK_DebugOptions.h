#ifndef _CWKDebugOptions_h
#define _CWKDebugOptions_h

#include <afxwin.h>

class AFX_EXT_CLASS CWKDebugOptions 
{
public:
	//
	BOOL ReadFromFile(const CString &sFilename);
	BOOL ReadFromFile(CFile& file);
	BOOL WriteToFile(const CString &sFilename);
	//
	void SetValue(const CString &sKey, int iValue,
						LPCTSTR szComment = _T("")	// only used if not empty
						);
	int GetValue(const CString &sKey, int iDefault=0);		// slow !
	inline int GetNumKeys() const;
	inline const CString &GetFilename() const;
	//
	void Clear();
private:
	CString m_sFilename;
	CStringArray m_keys;
	CDWordArray m_values;
	CStringArray m_comments;
};

inline int CWKDebugOptions::GetNumKeys() const
{
	return m_keys.GetSize();
}
inline const CString & CWKDebugOptions::GetFilename() const
{
	return m_sFilename;
}


#endif
