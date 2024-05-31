#ifndef DISPLAY_TYPES_H
#define DISPLAY_TYPES_H

#include <QString>
#include <map>
#include <memory>
#include <unordered_map>

#define DISPLAY_STRINGS 1

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
      HEX8,         //  8 bit
      HEX16,        // 16 bit
      HEX32,        // 32 bit
      HEX64,        // 64 bit
      Binary,       // binary data
      Ascii,        // ascii text
      Unicode,      // unicode text
      FormatFile,
//      AsciiLine,    // ascii line
//      UnicodeLine,  // unicode line
//      Structure,    // Image position for structure
//      Matrix,       // Data Matrix  // TODO! define Datatype Matrix?
//      TreeViewData, // Data Tree    // TODO! define Datatype for Treeview?
//      RepeatAttribute,      // Image position for repeat attribute
//      InLineAttribute,      // Image position for inline attribute
//      FactorAttribute,      // Image position for factor attribute
//      BytesAttribute,       // Image position for bytes attribute
//      LastAttribute,
      LastType=FormatFile             // last display type
    };

    typedef std::map<CDisplayType::eType, std::unique_ptr<CDisplayType>> type_map;

   CDisplayType();
   virtual ~CDisplayType() {}

   virtual      QString Display(const std::uint8_t*) const { return ""; }
   virtual      bool    Write(std::uint8_t*, const QString& ) const { return true; }
   virtual      size_t  GetByteLength(const std::uint8_t*pData=0) const;
   const char*         Type() const { return getNameOfType(mType); }
   static const char* getNameOfType(eType aType);
   static       eType getTypeOfName(const QString& aName);
   eType        getType() const { return mType; }
   void         CopyInverse(const std::uint8_t*pIn, std::uint8_t*pOut, int nSize) const;
   void         SetBytes(int n);
   bool         isSizeVariable();
   void         limit_byte_length(size_t &length, const uint8_t *current_ptr) const;
   static bool  setDifferentEndian(bool de);
   static bool  getDifferentEndian();
   static type_map& get_type_map();
   static void  set_end_ptr(const uint8_t *ptr);
protected:

   QString      toHexString(const std::uint8_t*pData) const;

   template <class Type>
   QString toString(const std::uint8_t*pData)const
   {
       Type value;
       const Type *value_pointer = reinterpret_cast<const Type*>(pData);
       if (m_different_endian)
       {
           value_pointer = &value;
           CopyInverse(pData, (std::uint8_t*)value_pointer, sizeof(Type));
       }
       return QString("%1").arg(*value_pointer);
   }

   template <class Type>
   void setData(std::uint8_t*pData, Type value, bool bok)const
   {
       if (bok)
       {
           if (m_different_endian)
           {
               CopyInverse((std::uint8_t*)&value, (std::uint8_t*)pData, sizeof(Type));
           }
           else
           {
               *reinterpret_cast<Type*>(pData) = value;
           }
       }
   }

   size_t m_nBytes;
   eType  mType;
   static bool m_different_endian;
   static type_map m_display;
   static const std::uint8_t* m_end_ptr;
};

class CDisplayChar : public CDisplayType
{
public:
   CDisplayChar();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayUChar : public CDisplayType
{
public:
   CDisplayUChar();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayShort : public CDisplayType
{
public:
   CDisplayShort();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayUShort : public CDisplayType
{
public:
   CDisplayUShort();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayLong : public CDisplayType
{
public:
   CDisplayLong();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayULong : public CDisplayType
{
public:
   CDisplayULong();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayLongLong : public CDisplayType
{
public:
   CDisplayLongLong();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayULongLong : public CDisplayType
{
public:
   CDisplayULongLong();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayFloat: public CDisplayType
{
public:
   CDisplayFloat();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayDouble: public CDisplayType
{
public:
   CDisplayDouble();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayHEX2: public CDisplayType
{
public:
   CDisplayHEX2();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayHEX4: public CDisplayType
{
public:
   CDisplayHEX4();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayHEX8: public CDisplayType
{
public:
   CDisplayHEX8();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayHEX16: public CDisplayType
{
public:
   CDisplayHEX16();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

class CDisplayBinary: public CDisplayType
{
public:
   CDisplayBinary();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
};

#ifdef DISPLAY_STRINGS
class CDisplayAscii: public CDisplayType
{
public:
   CDisplayAscii();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
   virtual size_t  GetByteLength(const std::uint8_t*pData) const;
};

class CDisplayUnicode: public CDisplayType
{
public:
   CDisplayUnicode();
   virtual QString Display(const std::uint8_t*pData) const;
   virtual bool    Write(std::uint8_t*pData, const QString& sValue) const;
   virtual size_t  GetByteLength(const std::uint8_t*pData) const;
};
#endif

#endif
