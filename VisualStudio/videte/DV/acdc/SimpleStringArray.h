// SimpleStringArray.h: interface for the SimpleStringArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLESTRINGARRAY_H__289A73CD_ADE7_4527_A7AA_B99C19F4AD85__INCLUDED_)
#define AFX_SIMPLESTRINGARRAY_H__289A73CD_ADE7_4527_A7AA_B99C19F4AD85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Simple vector of strings

class CSimpleStringArray  
{
public:
	int GetSize() const;
	std::vector<LPSTR> vect;
	bool m_bOwnData;
	
	CSimpleStringArray () {m_bOwnData = true;}
	~CSimpleStringArray ();

	void Add (LPSTR psString);
	void Clear ();

};

#endif // !defined(AFX_SIMPLESTRINGARRAY_H__289A73CD_ADE7_4527_A7AA_B99C19F4AD85__INCLUDED_)
