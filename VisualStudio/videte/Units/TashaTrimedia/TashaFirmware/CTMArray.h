/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CTMArray.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CTMArray.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
#ifndef __CTMARRAY_H__
#define __CTMARRAY_H__

class CTMArray
{

public:

// Construction
	CTMArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	void* GetAt(int nIndex) const;
	void SetAt(int nIndex, void* newElement);

	void*& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const void** GetData() const;
	void** GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, void* newElement);

	int Add(void* newElement);

	int Append(const CTMArray& src);
	void Copy(const CTMArray& src);

	// overloaded operator helpers
	void* operator[](int nIndex) const;
	void*& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, void* newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CTMArray* pNewArray);

// Implementation
protected:
	void** m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount


public:
	~CTMArray();

protected:
	// local typedefs for class templates
	typedef void* BASE_TYPE;
	typedef void* BASE_ARG_TYPE;
};

inline int CTMArray::GetSize() const { return m_nSize; }
inline int CTMArray::GetUpperBound() const { return m_nSize-1; }
inline void CTMArray::RemoveAll() { SetSize(0); }
inline void* CTMArray::GetAt(int nIndex) const { return m_pData[nIndex]; }
inline void CTMArray::SetAt(int nIndex, void* newElement){	m_pData[nIndex] = newElement; }
inline void*& CTMArray::ElementAt(int nIndex){	return m_pData[nIndex]; }
inline const void** CTMArray::GetData() const { return (const void**)m_pData; }
inline void** CTMArray::GetData() { return (void**)m_pData; }
inline void* CTMArray::operator[](int nIndex) const { return GetAt(nIndex); }
inline void*& CTMArray::operator[](int nIndex) { return ElementAt(nIndex); }
inline int CTMArray::Add(void* newElement)
{
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex;
}

#endif // __CTMARRAY_H__



