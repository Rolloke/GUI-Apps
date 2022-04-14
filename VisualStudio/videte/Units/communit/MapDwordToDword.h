// MapDwordToDword.h: interface for the CMapDwordToDword class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDWORDTODWORD_H__71300221_680D_41D8_BB0C_4DB5AC939FA8__INCLUDED_)
#define AFX_MAPDWORDTODWORD_H__71300221_680D_41D8_BB0C_4DB5AC939FA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapDwordToDword : public CMapPtrToPtr  
{
public:
	CMapDwordToDword();
	virtual ~CMapDwordToDword();

	// Lookup
	BOOL Lookup(DWORD key, DWORD& rValue) const;

// Operations
	// Lookup and add if not there
	DWORD& operator[](DWORD key);

	// add a new (key, value) pair
	void SetAt(DWORD key, DWORD newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(DWORD key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, DWORD& rKey, DWORD& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(DWORD key) const;
};

#endif // !defined(AFX_MAPDWORDTODWORD_H__71300221_680D_41D8_BB0C_4DB5AC939FA8__INCLUDED_)
