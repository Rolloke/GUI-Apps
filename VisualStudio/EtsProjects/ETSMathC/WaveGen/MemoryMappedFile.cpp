// MemoryMappedFile.cpp: Implementierung der Klasse CMemoryMappedFile.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryMappedFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMemoryMappedFile::CMemoryMappedFile()
{
   m_hFileForMapping = INVALID_HANDLE_VALUE;
   m_hFileMapping    = INVALID_HANDLE_VALUE;
   m_pPointer        = NULL;
   m_strFileName.Empty();
   m_bShared         = false;
   m_bTemporary      = false;
}

CMemoryMappedFile::~CMemoryMappedFile()
{
   Close();
}

void CMemoryMappedFile::Close()
{
   if (m_hFileMapping != INVALID_HANDLE_VALUE)
   {                                                        // bei MemoryMappedFile
      if (m_pPointer)                                       // Zeiger unmappen
      {
         ::UnmapViewOfFile((void*)m_pPointer);
      }
      ::CloseHandle(m_hFileMapping);                        // File-Mapping schlieﬂen
   }

   if (m_hFileForMapping != INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(m_hFileForMapping);
   }

   if (!m_strFileName.IsEmpty())
   {
      if (m_bTemporary)
      {
         ::DeleteFile(m_strFileName);
      }
      m_strFileName.Empty();
   }

   m_hFileForMapping = INVALID_HANDLE_VALUE;
   m_hFileMapping    = INVALID_HANDLE_VALUE;
   m_pPointer        = NULL;
   m_bShared         = false;
}

bool CMemoryMappedFile::Alloc(DWORD nSizeHigh, DWORD nSizeLow, 
     bool bTemporary, bool bShared, bool bRead, bool bWrite)
{
   if (bShared)
   {
      DWORD nBufferLength = 0;
      char szPath[MAX_PATH];
      nBufferLength = GetTempPath(MAX_PATH, szPath);
      if (nBufferLength)
      {
         char szTempMMF[MAX_PATH*2];
         if (GetTempFileName(szPath, "mmf", 0, szTempMMF))
            m_strFileName = _T(szTempMMF);
      }
   }
   if (!m_strFileName.IsEmpty())
      return Create(m_strFileName, nSizeHigh, nSizeLow, bTemporary, bShared, bRead, bWrite, true);
   else
      return Create(NULL, nSizeHigh, nSizeLow, bTemporary, bShared, bRead, bWrite, false);
}

bool CMemoryMappedFile::Create(LPCTSTR pszFileName, DWORD nSizeHigh, DWORD nSizeLow, 
     bool bTemporary, bool bShared, bool bRead, bool bWrite, bool bExisting)
{
   DWORD dwDesiredAccess       = 0;
   DWORD dwShareMode           = 0;
   DWORD dwCreationDisposition = 0;
   DWORD dwFlagsAndAttributes  = 0;
   DWORD flProtect             = SEC_COMMIT;
   if (bExisting)
   {
      dwCreationDisposition    |= OPEN_EXISTING;
      if (bShared) dwShareMode |= FILE_SHARE_DELETE;
   }
   else dwCreationDisposition  |= OPEN_ALWAYS;

   if (bRead)
   {
      dwDesiredAccess |= GENERIC_READ;
      if (bShared) dwShareMode |= FILE_SHARE_READ;
      if (bWrite) flProtect = PAGE_READWRITE;
      else
      {
         flProtect = PAGE_READONLY;
         dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;   // nur lesen
      }
   }
   if (bWrite) 
   {
      dwDesiredAccess |= GENERIC_WRITE;
      if (bShared) dwShareMode |= FILE_SHARE_WRITE;
      dwFlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;        // lesen und schreiben
   }
   m_bTemporary = bTemporary;
   if (pszFileName != NULL)
   {
      m_hFileForMapping  = ::CreateFile(pszFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL);
   }

   m_hFileMapping = ::CreateFileMapping(m_hFileForMapping, NULL, flProtect, nSizeHigh, nSizeLow, NULL);
   if (m_hFileMapping != NULL) return true;

   return false;
}

void * CMemoryMappedFile::MapViewOfFile(DWORD nOffsetHigh, DWORD nOffsetLow, SIZE_T nMapBytes)
{
   if (IsValid())
   {
      DWORD dwDesiredAccess = 0;
      if (m_pPointer) return m_pPointer;
      DWORD dwAttr = 0;
      if (!m_strFileName.IsEmpty()) dwAttr = GetFileAttributes(m_strFileName);
      if (dwAttr & FILE_ATTRIBUTE_READONLY) dwDesiredAccess |= FILE_MAP_READ;
      else                                  dwDesiredAccess |= FILE_MAP_READ|FILE_MAP_WRITE;

      m_pPointer = ::MapViewOfFile(m_hFileMapping, dwDesiredAccess, nOffsetHigh, nOffsetLow, nMapBytes);
      ASSERT(m_pPointer != NULL);
   }
   return m_pPointer;
}

bool CMemoryMappedFile::IsValid()
{
   return (m_hFileMapping != INVALID_HANDLE_VALUE) ? true : false;
}
