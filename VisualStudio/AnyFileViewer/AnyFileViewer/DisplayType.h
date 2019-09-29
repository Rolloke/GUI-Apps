// AnyFileViewer.h : Hauptheaderdatei für die AnyFileViewer-Anwendung
//
#pragma once

class CDisplayType
{
public:
      enum eType {
      Unknown=-1,
      Char=0,       //  8 bit
      UChar,        //  
      Short,        // 16 bit
      UShort,       //  
      Long,         // 32 bit
      ULong,        //  
      LongLong,     // 64 bit
      ULongLong,    //  
      Float,        // 32 bit
      Double,       // 64 bit
      HEX2,         //  8 bit
      HEX4,         // 16 bit
      HEX8,         // 32 bit
      HEX16,        // 64 bit
      Ascii,        // ascii text
      Unicode,      // unicode text
      Binary,       // binary data
      AsciiLine,    // ascii line
      UnicodeLine,  // unicode line
      Structure,    // Image position for structure
      Matrix,       // Data Matrix  // TODO! define Datatype Matrix?
      TreeViewData, // Data Tree    // TODO! define Datatype for Treeview?
      RepeatAttribute,      // Image position for repeat attribute
      InLineAttribute,      // Image position for inline attribute
      FactorAttribute,      // Image position for factor attribute
      BytesAttribute,       // Image position for bytes attribute
      LastAttribute,
      LastType=Structure             // last display type
    };

   CDisplayType();

   virtual      CString Display(BYTE*) const { return _T(""); };
   virtual      int    Write(BYTE*, CString ) const { return TRUE; };
   virtual      int    GetByteLength(BYTE*pData=0) const;
   const        CString Type() const { return getNameOfType(mType); };
   static const LPCTSTR getNameOfType(eType aType);
   static       eType getTypeOfName(const CString& aName);
   eType        getType() const { return mType; };
   void         CopyInverse(BYTE*pIn, BYTE*pOut, int nSize) const;
   void         SetBytes(int n);
   bool         isSizeFixed();
protected:
   int   m_nBytes;
   eType mType;
};

class CDisplayChar : public CDisplayType
{
public:
   CDisplayChar();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayUChar : public CDisplayType
{
public:
   CDisplayUChar();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayShort : public CDisplayType
{
public:
   CDisplayShort();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayUShort : public CDisplayType
{
public:
   CDisplayUShort();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayLong : public CDisplayType
{
public:
   CDisplayLong();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayULong : public CDisplayType
{
public:
   CDisplayULong();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayLongLong : public CDisplayType
{
public:
   CDisplayLongLong();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayULongLong : public CDisplayType
{
public:
   CDisplayULongLong();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayFloat: public CDisplayType
{
public:
   CDisplayFloat();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayDouble: public CDisplayType
{
public:
   CDisplayDouble();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayHEX2: public CDisplayType
{
public:
   CDisplayHEX2();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayHEX4: public CDisplayType
{
public:
   CDisplayHEX4();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayHEX8: public CDisplayType
{
public:
   CDisplayHEX8();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayHEX16: public CDisplayType
{
public:
   CDisplayHEX16();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};

class CDisplayAscii: public CDisplayType
{
public:
   CDisplayAscii();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
   virtual int    GetByteLength(BYTE*pData) const;
};

class CDisplayUnicode: public CDisplayType
{
public:
   CDisplayUnicode();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
   virtual int    GetByteLength(BYTE*pData) const;
};

class CDisplayBinary: public CDisplayType
{
public:
   CDisplayBinary();
   virtual CString Display(BYTE*pData) const;
   virtual int    Write(BYTE*pData, CString sValue) const;
};
