// MemoryMappedFile.h: Schnittstelle für die Klasse CMemoryMappedFile.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMAPPEDFILE_H__34BAD383_D6B7_4880_9100_707B9DE2989C__INCLUDED_)
#define AFX_MEMORYMAPPEDFILE_H__34BAD383_D6B7_4880_9100_707B9DE2989C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemoryMappedFile  
{
public:
	bool IsValid();
	CMemoryMappedFile();
	~CMemoryMappedFile();

	bool   Create(LPCTSTR, DWORD, DWORD, bool, bool, bool, bool, bool);
	bool   Alloc(DWORD, DWORD, bool , bool, bool, bool);
	void   Close();
   void * MapViewOfFile(DWORD, DWORD, SIZE_T);

private:
   HANDLE   m_hFileForMapping;   // Dateihandle der Mapdatei
   HANDLE   m_hFileMapping;      // Mappinghandle
   void    *m_pPointer;          // Mapping Pointer
   CString  m_strFileName;       // Name des MMF
   bool     m_bShared;           // Sharing mit anderem Handle
   bool     m_bTemporary;        // File wird wieder gelöscht
};

#endif // !defined(AFX_MEMORYMAPPEDFILE_H__34BAD383_D6B7_4880_9100_707B9DE2989C__INCLUDED_)
