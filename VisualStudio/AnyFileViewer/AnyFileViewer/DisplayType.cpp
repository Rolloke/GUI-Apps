
#include "stdafx.h"
#include "AnyFileViewer.h"

#include "DisplayType.h"


CDisplayType::CDisplayType()
{
   m_nBytes = 0;
}

int CDisplayType::GetByteLength(BYTE*) const
{ 
    return m_nBytes; 
}

void CDisplayType::CopyInverse(BYTE*pIn, BYTE*pOut, int nSize) const
{
    int i, j;
    for (i=0, j=nSize-1; i<nSize; i++, j--)
    {
        pOut[i] = pIn[j];
    }
}

const LPCTSTR CDisplayType::getNameOfType(eType aType)
{
    switch (aType)
    {
    case Char: return _T("char");
    case UChar: return _T("unsigned_char");
    case Short: return _T("short");
    case UShort: return _T("unsigned_short");
    case Long: return _T("long");
    case ULong: return _T("unsigned_long");
    case LongLong: return _T("LONGLONG");
    case ULongLong: return _T("ULONGLONG");
    case Float: return _T("float");
    case Double: return _T("double");
    case HEX2: return _T("HEX2");
    case HEX4: return _T("HEX4");
    case HEX8: return _T("HEX8");
    case HEX16: return _T("HEX16");
    case Ascii: return _T("ascii");
    case Unicode: return _T("unicode");
    case Binary: return _T("binary");
    case AsciiLine: return _T("ascii_line");
    case UnicodeLine: return _T("unicode_line");
    case Matrix: return _T("matrix");
    case Structure: return _T("structure");
    case RepeatAttribute: return _T("repeat");
    case BytesAttribute: return _T("bytes");
    case InLineAttribute: return _T("inline");
    case FactorAttribute: return _T("factor");
    default: return _T("");
    }
}

CDisplayType::eType CDisplayType::getTypeOfName(const CString& aName)
{
    eType fType;
    static CMap<CString, LPCTSTR, eType, eType&> fTypes;
    if (fTypes.GetCount() == 0)
    {
        for (int i=0; i<=LastAttribute; i++)
        {
            fType = static_cast<eType>(i);
            fTypes.SetAt( getNameOfType(fType), fType);
        }
    }
    if (!fTypes.Lookup(aName, fType))
    {
        fType = Unknown;

    }
    return fType;
}

void CDisplayType::SetBytes(int n)
{
    if (!isSizeFixed())
    {
        m_nBytes = n; 
    }
}

bool CDisplayType::isSizeFixed()
{
    switch (mType)
    {
    case Ascii: case Unicode: case Binary:
        return false; 
        break;
    }
    return true;
}

CDisplayChar::CDisplayChar()
{
   m_nBytes = sizeof(char);
   mType = Char;
}

CString CDisplayChar::Display(BYTE*pData)const
{
   CString str;
   str.Format(_T("%d"), *((char*)pData));
   return str;
}

int CDisplayChar::Write(BYTE*pData, CString sValue)const
{
   long c = 0;
   CStringA s(sValue);
   if (sscanf_s(s, "%d", &c) == 1)
   {
      *((char*)pData) = (char)c;
      return TRUE;
   }
   return FALSE;
}

CDisplayUChar::CDisplayUChar()
{
   m_nBytes = sizeof(unsigned char);
   mType = UChar;
}

CString CDisplayUChar::Display(BYTE*pData)const
{
   CString str;
   str.Format(_T("%u"), *((unsigned char*)pData));
   return str;
}

int CDisplayUChar::Write(BYTE*pData, CString sValue)const
{
   unsigned long c = 0;
   CStringA s(sValue);
   if (sscanf_s(s, "%u", &c) == 1)
   {
      *((unsigned char*)pData) = (unsigned char)c;
      return TRUE;
   }
   return FALSE;
}

CDisplayShort::CDisplayShort()
{
   m_nBytes = sizeof(short);
   mType = Short;
}

CString CDisplayShort::Display(BYTE*pData)const
{
   CString str;
   short s, *ps = (short*)pData;
   if (theApp.isDifferentEndian())
   {
       ps = &s;
       CopyInverse((BYTE*)pData, (BYTE*)ps, sizeof(short)); 
   }
   str.Format(_T("%d"), *ps);
   return str;
}

int CDisplayShort::Write(BYTE*pData, CString sValue)const
{
    long c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%d", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(short)); 
        }
        else
        {
            *((short*)pData) = (short)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayUShort::CDisplayUShort()
{
   m_nBytes = sizeof(unsigned short);
   mType = UShort;
}

CString CDisplayUShort::Display(BYTE*pData)const
{
   CString str;
   unsigned short s, *ps = (unsigned short*)pData;
   if (theApp.isDifferentEndian())
   {
       ps = &s;
       CopyInverse((BYTE*)pData, (BYTE*)ps, sizeof(unsigned short)); 
   }
   str.Format(_T("%u"), *ps);
   return str;
}

int CDisplayUShort::Write(BYTE*pData, CString sValue)const
{
    unsigned long c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%u", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(unsigned short)); 
        }
        else
        {
            *((unsigned short*)pData) = (unsigned short)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayLong::CDisplayLong()
{
   m_nBytes = sizeof(long);
   mType = Long;
}

CString CDisplayLong::Display(BYTE*pData)const
{
   CString str;
   long l, *pl = (long*)pData;
   if (theApp.isDifferentEndian())
   {
       pl = &l;
       CopyInverse((BYTE*)pData, (BYTE*)pl, sizeof(long)); 
   }
   str.Format(_T("%d"), *pl);
   return str;
}

int CDisplayLong::Write(BYTE*pData, CString sValue)const
{
    long c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%d", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(long)); 
        }
        else
        {
            *((long*)pData) = (long)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayULong::CDisplayULong()
{
   m_nBytes = sizeof(unsigned long);
   mType = ULong;
}

CString CDisplayULong::Display(BYTE*pData)const
{
   CString str;
   unsigned long l, *pl = (unsigned long*)pData;
   if (theApp.isDifferentEndian())
   {
       pl = &l;
       CopyInverse((BYTE*)pData, (BYTE*)pl, sizeof(unsigned long)); 
   }
   str.Format(_T("%u"), *pl);
   return str;
}

int CDisplayULong::Write(BYTE*pData, CString sValue)const
{
    unsigned long c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%u", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(unsigned long)); 
        }
        else
        {
            *((unsigned long*)pData) = (unsigned long)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayLongLong::CDisplayLongLong()
{
   m_nBytes = sizeof(LONGLONG);
   mType = LongLong;
}

CString CDisplayLongLong::Display(BYTE*pData)const
{
   CString str;
   LONGLONG l, *pl = (LONGLONG*)pData;
   if (theApp.isDifferentEndian())
   {
       pl = &l;
       CopyInverse((BYTE*)pData, (BYTE*)pl, sizeof(LONGLONG)); 
   }
   double d = (double)*pl;
   str.Format(_T("%.f"), d);
   return str;
}

int CDisplayLongLong::Write(BYTE*pData, CString sValue)const
{
    double c;
    CStringA s(sValue);
    if (sscanf_s(s, "%lf", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            LONGLONG value = (LONGLONG)c;
            CopyInverse((BYTE*)&value, (BYTE*)pData, sizeof(LONGLONG)); 
        }
        else
        {
            *((LONGLONG*)pData) = (LONGLONG)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayULongLong::CDisplayULongLong()
{
   m_nBytes = sizeof(ULONGLONG);
   mType = ULongLong;
}

CString CDisplayULongLong::Display(BYTE*pData)const
{
   CString str;
   ULONGLONG l, *pl = (ULONGLONG*)pData;
   if (theApp.isDifferentEndian())
   {
       pl = &l;
       CopyInverse((BYTE*)pData, (BYTE*)pl, sizeof(ULONGLONG)); 
   }
   double d = (double)*pl;
   str.Format(_T("%.f"), d);
   return str;
}

int CDisplayULongLong::Write(BYTE*pData, CString sValue)const
{
    double c;
    CStringA s(sValue);
    if (sscanf_s(s, "%lf", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            LONGLONG value = (ULONGLONG)c;
            CopyInverse((BYTE*)&value, (BYTE*)pData, sizeof(ULONGLONG)); 
        }
        else
        {
            *((ULONGLONG*)pData) = (ULONGLONG)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayFloat::CDisplayFloat()
{
   m_nBytes = sizeof(float);
   mType = Float;
}

CString CDisplayFloat::Display(BYTE*pData)const
{
   CString str;
   float f, *pf = (float*)pData;
   if (theApp.isDifferentEndian())
   {
       pf = &f;
       CopyInverse((BYTE*)pData, (BYTE*)pf, sizeof(float)); 
   }
   str.Format(theApp.getFloatFormat(), *pf);
   return str;
}

int CDisplayFloat::Write(BYTE*pData, CString sValue)const
{
    float c;
    CStringA s(sValue);
    if (sscanf_s(s, "%f", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(float)); 
        }
        else
        {
            *((float*)pData) = c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayDouble::CDisplayDouble()
{
   m_nBytes = sizeof(double);
   mType = Double;
}

CString CDisplayDouble::Display(BYTE*pData)const
{
   CString str;
   double d, *pd = (double*)pData;
   if (theApp.isDifferentEndian())
   {
       pd = &d;
       CopyInverse((BYTE*)pData, (BYTE*)pd, sizeof(double)); 
   }
   str.Format(theApp.getDoubleFormat(), *pd);
   return str;
}

int CDisplayDouble::Write(BYTE*pData, CString sValue)const
{
    double c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%lf", &c) == 1)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(double)); 
        }
        else
        {
            *((double*)pData) = (double)c;
        }
        return TRUE;
    }
    return FALSE;
}

CDisplayHEX2::CDisplayHEX2()
{
   m_nBytes = sizeof(unsigned char);
   mType = HEX2;
}

CString CDisplayHEX2::Display(BYTE*pData)const
{
   CString str;
   str.Format(_T("%02X"), *((unsigned char*)pData));
   return str;
}

int CDisplayHEX2::Write(BYTE*pData, CString sValue)const
{
   unsigned long c = 0;
   CStringA s(sValue);
   if (sscanf_s(s, "%02x", &c) == 1)
   {
      *((unsigned char*)pData) = (unsigned char)c;
      return TRUE;
   }
   return FALSE;
}

CDisplayHEX4::CDisplayHEX4()
{
   m_nBytes = sizeof(unsigned short);
   mType = HEX4;
}

CString CDisplayHEX4::Display(BYTE*pData)const
{
   CString str;
   unsigned short s, *ps = (unsigned short*)pData;
   ps = &s;
   CopyInverse((BYTE*)pData, (BYTE*)ps, sizeof(unsigned short)); 
   str.Format(_T("%04X"), *ps);
   return str;
}

int CDisplayHEX4::Write(BYTE*pData, CString sValue)const
{
    unsigned long c = 0;
    CStringA s(sValue);
    if (sscanf_s(s, "%04x", &c) == 1)
    {
        CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(unsigned short)); 
        return TRUE;
    }
    return FALSE;
}

CDisplayHEX8::CDisplayHEX8()
{
   m_nBytes = sizeof(unsigned long);
   mType = HEX8;
}

CString CDisplayHEX8::Display(BYTE*pData)const
{
   CString str;
   unsigned long l, *pl = (unsigned long*)pData;
   pl = &l;
   CopyInverse((BYTE*)pData, (BYTE*)pl, sizeof(unsigned long)); 
   str.Format(_T("%08X"), *pl);
   return str;
}

int CDisplayHEX8::Write(BYTE*pData, CString sValue)const
{
    unsigned long c;
    CStringA s(sValue);
    if (sscanf_s(s, "%08x", &c) == 1)
    {
        CopyInverse((BYTE*)&c, (BYTE*)pData, sizeof(unsigned long)); 
        return TRUE;
    }
    return FALSE;
}

CDisplayHEX16::CDisplayHEX16()
{
   m_nBytes = sizeof(unsigned long) * 2;
   mType = HEX16;
}

CString CDisplayHEX16::Display(BYTE*pData)const
{
   CString str;
   unsigned long l[2], *pl = (unsigned long *)pData;
   pl = &l[0];
   CopyInverse((BYTE*)pData, (BYTE*)pl, m_nBytes); 
   str.Format(_T("%08X%08X"), pl[1], pl[0]);
   return str;
}

int CDisplayHEX16::Write(BYTE*pData, CString sValue)const
{
    unsigned long c[2];
    CStringA s(sValue);
    if (sscanf_s(s, "%08x%08x", &c[1], &c[0]) == 2)
    {
        CopyInverse((BYTE*)&c, (BYTE*)pData, m_nBytes); 
        return TRUE;
    }
    return FALSE;
}

CDisplayAscii::CDisplayAscii()
{
   m_nBytes = 1;
   mType = Ascii;
}

CString CDisplayAscii::Display(BYTE*pData)const
{
    int i;
    if (m_nBytes == 0)
    {
        size_t nLen = GetByteLength(pData);
        CString str(reinterpret_cast<char*>(pData), static_cast<int>(nLen));
        return str;
    }
    else
    {
        CString str;
        LPTSTR pstr = str.GetBuffer(m_nBytes+1);
        for (i=0; i<m_nBytes; i++)
        {
            if (pData[i])
            {
                pstr[i] = (_TCHAR)pData[i];
            }
            else
            {
                pstr[i] = _T('0');
            }
        }
        pstr[i] = 0;
        str.ReleaseBuffer();
        return str;
    }
}

int CDisplayAscii::GetByteLength(BYTE*pData) const
{
    size_t nLen = 0;
    if (pData)
    {
        char *fText = reinterpret_cast<char *>(pData);
        char *fEndline = strstr(fText, "\r\n");
        if (fEndline) nLen = 2;
        else
        {
            fEndline = strstr(fText, "\r");
            if (fEndline) nLen = 1;
            else
            {
                fEndline = strstr(fText, "\n");
                if (fEndline) nLen = 1;
            }
        }
        if (fEndline)
        {
            nLen += fEndline - fText;
        }
        else
        {
            nLen = strlen(fText);
        }
    }
    return static_cast<int>(nLen);
}

int CDisplayAscii::Write(BYTE*pData, CString sValue)const
{
    if (pData == 0) return sValue.GetLength();
    int i, n = min(m_nBytes, sValue.GetLength());
    for (i=0; i<n; i++)
    {
        pData[i] = (BYTE)sValue[i];
    }
    return n;
}

CDisplayUnicode::CDisplayUnicode()
{
   m_nBytes = 2;
   mType = Unicode;
}

CString CDisplayUnicode::Display(BYTE*pData)const
{
    if (m_nBytes == 0)
    {
        int nLen = GetByteLength(pData);
        CString str(reinterpret_cast<WCHAR*>(pData),  static_cast<int>(nLen));
        return str;
    }
    else
    {
       CString str;
       int i, n = m_nBytes/2;
       LPTSTR pstr = str.GetBufferSetLength(n+1);
       WORD *pwData = (WORD*)pData;
       for (i=0; i<n; i++)
       {
          if (pwData[i])
          {
              if (theApp.isDifferentEndian())
              {
                  CopyInverse((BYTE*)&pwData[i], (BYTE*)&pstr[i], 2);
              }
              else
              {
                  pstr[i] = (_TCHAR)pwData[i];
              }
          }
          else
          {
             pstr[i] = _T('0');
          }
       }
       pstr[i] = 0;
       str.ReleaseBuffer();
       return str;
    }
}

int CDisplayUnicode::GetByteLength(BYTE*pData) const
{
    size_t nLen = 0;
    if (pData)
    {
        WCHAR *fText = reinterpret_cast<WCHAR *>(pData);
        WCHAR *fEndline = wcsstr(fText, L"\r\n");
        if (fEndline) nLen = 2;
        else
        {
            fEndline = wcsstr(fText, L"\r");
            if (fEndline) nLen = 1;
            else
            {
                fEndline = wcsstr(fText, L"\n");
                if (fEndline) nLen = 1;
            }
        }
        if (fEndline)
        {
            nLen += fEndline - fText;
        }
        else
        {
            nLen = wcslen(fText);
        }
    }
    return static_cast<int>(nLen*sizeof(WCHAR));
}

int CDisplayUnicode::Write(BYTE*pData, CString sValue)const
{
    if (pData == 0) return sValue.GetLength()*2;
    int i, n = min(m_nBytes/2, sValue.GetLength());
    WORD *pwData = (WORD*)pData;
    LPCTSTR pstr = sValue;
    for (i=0; i<n; i++)
    {
        if (theApp.isDifferentEndian())
        {
            CopyInverse((BYTE*)&pstr[i], (BYTE*)&pwData[i], 2);
        }
        else
        {
            pwData[i] = pstr[i];
        }
    }
    return n * 2;
}

CDisplayBinary::CDisplayBinary()
{
   m_nBytes = sizeof(unsigned char);
   mType = Binary;
}

CString CDisplayBinary::Display(BYTE*pData)const
{
   int i, j, k, n = m_nBytes*8; 
   BYTE cMask;
   CString str(_T('0'), n+m_nBytes);
   LPTSTR psz = (LPTSTR)LPCTSTR(str);
   for (i=0, k=0; i<m_nBytes; i++)
   {
       cMask = 0x80;
       for (j=0; j<8; j++, cMask >>= 1, k++)
       {
           if (pData[i] & cMask)
           {
               psz[k] = _T('1');
           }
       }
       psz[k++] = _T(' ');
   }
   return str;
}

int CDisplayBinary::Write(BYTE*pData, CString sValue)const
{
    sValue.Replace(_T(" "), _T(""));
    if (pData == 0) return sValue.GetLength()/8;
    int i, j, k, n = sValue.GetLength(); 
    BYTE cMask;
    LPCTSTR psz = LPCTSTR(sValue);
    for (i=0, k=0; i<m_nBytes; i++)
    {
        cMask = 0x80;
        pData[i] = 0;
        for (j=0; j<8; j++, cMask >>= 1, k++)
        {
            if (psz[k] == _T('1'))
            {
                pData[i] |= cMask;
            }
            //if (psz[k] == _T(' ')) k++;
            if (k >= n) break;
        }
        if (k >= n) break;
    }
    return i;
}
