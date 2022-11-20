
#include "DisplayType.h"
#include <QMap>

bool CDisplayType::m_different_endian = false;
CDisplayType::type_map CDisplayType::m_display;

CDisplayType::CDisplayType()
{
   m_nBytes = 0;
}

size_t CDisplayType::GetByteLength(const std::uint8_t*) const
{ 
    return m_nBytes; 
}

void CDisplayType::CopyInverse(const std::uint8_t*pIn, std::uint8_t*pOut, int nSize) const
{
    int i, j;
    for (i=0, j=nSize-1; i<nSize; i++, j--)
    {
        pOut[i] = pIn[j];
    }
}

const char* CDisplayType::getNameOfType(eType aType)
{
    switch (aType)
    {
    case Char:            return "signed char";
    case UChar:           return "unsigned char";
    case Short:           return "signed short";
    case UShort:          return "unsigned short";
    case Long:            return "signed long";
    case ULong:           return "unsigned long";
    case LongLong:        return "signed long long";
    case ULongLong:       return "unsigned long long";
    case Float:           return "float";
    case Double:          return "double";
    case HEX16:           return "HEX16";
    case HEX32:           return "HEX32";
    case HEX64:           return "HEX64";
    case HEX128:          return "HEX128";
    case Ascii:           return "ascii";
    case Unicode:         return "unicode";
    case Binary:          return "binary";
    case FormatFile:      return "Type Format File";
//    case AsciiLine:       return "ascii_line";
//    case UnicodeLine:     return "unicode_line";
//    case Matrix:          return "matrix";
//    case Structure:       return "structure";
//    case RepeatAttribute: return "repeat";
//    case BytesAttribute:  return "bytes";
//    case InLineAttribute: return "inline";
//    case FactorAttribute: return "factor";
    default: return "";
    }
}

CDisplayType::eType CDisplayType::getTypeOfName(const QString& aName)
{
    eType fType = Unknown;
    static QMap<QString, eType> fTypes;
    if (fTypes.size() == 0)
    {
        for (int i=0; i<=FormatFile; i++)
        {
            fType = static_cast<eType>(i);
            fTypes.insert(getNameOfType(fType), fType);
        }
        fTypes.insert("int8", Char);
        fTypes.insert("uint8", UChar);
        fTypes.insert("int16", Short);
        fTypes.insert("uint16", UShort);
        fTypes.insert("int32", Long);
        fTypes.insert("uint32", ULong);
        fTypes.insert("int64", LongLong);
        fTypes.insert("uint64", ULongLong);
        fTypes.insert("char", Char);
        fTypes.insert("uchar", UChar);
        fTypes.insert("short", Short);
        fTypes.insert("ushort", UShort);
        fTypes.insert("long", Long);
        fTypes.insert("ulong", ULong);
        fTypes.insert("longlong", LongLong);
        fTypes.insert("ulonglong", ULongLong);
        fTypes.insert("string", Ascii);
        fTypes.insert("wstring", Unicode);
    }
    auto found_key = fTypes.find(aName);
    if (found_key != fTypes.end())
    {
        fType = found_key.value();

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
    default: break;
    }
    return true;
}

void CDisplayType::setDifferentEndian(bool de)
{
    m_different_endian = de;
}

bool CDisplayType::getDifferentEndian()
{
    return m_different_endian;
}

CDisplayType::type_map& CDisplayType::get_type_map()
{
    if (m_display.size() == 0)
    {
        m_display[CDisplayType::Char]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayChar));
        m_display[CDisplayType::UChar]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayUChar));
        m_display[CDisplayType::Short]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayShort));
        m_display[CDisplayType::UShort]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayUShort));
        m_display[CDisplayType::Long]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayLong));
        m_display[CDisplayType::ULong]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayULong));
        m_display[CDisplayType::LongLong]  = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayLongLong));
        m_display[CDisplayType::ULongLong] = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayULongLong));
        m_display[CDisplayType::Float]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayFloat));
        m_display[CDisplayType::Double]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayDouble));
        m_display[CDisplayType::HEX16]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX2));
        m_display[CDisplayType::HEX32]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX4));
        m_display[CDisplayType::HEX64]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX8));
        m_display[CDisplayType::HEX128]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX16));
        m_display[CDisplayType::Binary]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayBinary));
        m_display[CDisplayType::Ascii]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayAscii));
        m_display[CDisplayType::Unicode]   = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayUnicode));
    }
    return m_display;
}

QString CDisplayType::toHexString(const std::uint8_t*pData) const
{
    QString text;
# if __BYTE_ORDER == __LITTLE_ENDIAN
    if (!m_different_endian)
#else
    if (m_different_endian)
#endif
    {
        for (int i=GetByteLength()-1; i>=0; --i)
        {
            text += QString("%1").arg(pData[i], 2, 16);
        }
    }
    else
    {
        for (int i=0; i < static_cast<int>(GetByteLength()); ++i)
        {
            text += QString("%1").arg(pData[i], 2, 16);
        }
    }
    for (auto& c : text)
    {
        if (c == ' ') c = '0';
    }
    return text;
}

CDisplayChar::CDisplayChar()
{
   m_nBytes = sizeof(std::int8_t);
   mType = Char;
}

QString CDisplayChar::Display(const std::uint8_t*pData)const
{
   return QString("%1").arg(static_cast<int>(*reinterpret_cast<const char*>(pData)));
}

bool CDisplayChar::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    *reinterpret_cast<char*>(pData) = static_cast<char>(sValue.toInt(&bok));
    return bok;
}

CDisplayUChar::CDisplayUChar()
{
   m_nBytes = sizeof(std::uint8_t);
   mType = UChar;
}

QString CDisplayUChar::Display(const std::uint8_t*pData)const
{
   return QString("%1").arg(*pData);
}

bool CDisplayUChar::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    *pData = static_cast<std::uint8_t>(sValue.toUInt(&bok));
    return bok;
}

CDisplayShort::CDisplayShort()
{
   m_nBytes = sizeof(std::int16_t);
   mType = Short;
}

QString CDisplayShort::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<std::int16_t>(pData);
}

bool CDisplayShort::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    std::int16_t value = static_cast<std::int16_t>(sValue.toShort(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayUShort::CDisplayUShort()
{
   m_nBytes = sizeof(std::uint16_t);
   mType = UShort;
}

QString CDisplayUShort::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<std::uint16_t>(pData);
}

bool CDisplayUShort::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    std::uint16_t value = static_cast<std::uint16_t>(sValue.toUShort(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayLong::CDisplayLong()
{
   m_nBytes = sizeof(std::int32_t);
   mType = Long;
}

QString CDisplayLong::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<std::int32_t>(pData);
}

bool CDisplayLong::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    std::int32_t value = static_cast<std::int32_t>(sValue.toLong(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayULong::CDisplayULong()
{
   m_nBytes = sizeof(std::uint32_t);
   mType = ULong;
}

QString CDisplayULong::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<std::uint32_t>(pData);
}

bool CDisplayULong::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    std::uint32_t value = static_cast<std::uint32_t>(sValue.toULong(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayLongLong::CDisplayLongLong()
{
   m_nBytes = sizeof(qlonglong);
   mType = LongLong;
}

QString CDisplayLongLong::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<qlonglong>(pData);
}

bool CDisplayLongLong::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    std::int64_t value = static_cast<std::int64_t>(sValue.toLongLong(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayULongLong::CDisplayULongLong()
{
   m_nBytes = sizeof(qulonglong);
   mType = ULongLong;
}

QString CDisplayULongLong::Display(const std::uint8_t*pData)const
{
    return CDisplayType::toString<qulonglong>(pData);
}

bool CDisplayULongLong::Write(std::uint8_t*pData, const QString& sValue) const
{
    bool bok {false};
    std::uint64_t value = static_cast<std::uint64_t>(sValue.toULongLong(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayFloat::CDisplayFloat()
{
   m_nBytes = sizeof(float);
   mType = Float;
}

QString CDisplayFloat::Display(const std::uint8_t*pData)const
{
    float value;
    const auto *value_pointer = reinterpret_cast<const float*>(pData);
    if (m_different_endian)
    {
        value_pointer = &value;
        CopyInverse(pData, (std::uint8_t*)value_pointer, sizeof(float));
    }
    return QString("%1").arg(*value_pointer,0, 'g', 7);
}

bool CDisplayFloat::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    float value = static_cast<float>(sValue.toFloat(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayDouble::CDisplayDouble()
{
   m_nBytes = sizeof(double);
   mType = Double;
}

QString CDisplayDouble::Display(const std::uint8_t*pData)const
{
    double value;
    const auto *value_pointer = reinterpret_cast<const double*>(pData);
    if (m_different_endian)
    {
        value_pointer = &value;
        CopyInverse(pData, (std::uint8_t*)value_pointer, sizeof(double));
    }
    return QString("%1").arg(*value_pointer,0, 'g', 14);
}

bool CDisplayDouble::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    double value = static_cast<double>(sValue.toDouble(&bok));
    setData(pData, value, bok);
    return bok;
}

CDisplayHEX2::CDisplayHEX2()
{
   m_nBytes = sizeof(std::uint8_t);
   mType = HEX16;
}

QString CDisplayHEX2::Display(const std::uint8_t*pData)const
{
    return toHexString(pData);
}

bool CDisplayHEX2::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    if (sValue.size()<= 2)
    {
        *reinterpret_cast<char*>(pData) = static_cast<char>(sValue.toUInt(&bok, 16));
    }
    return bok;
}

CDisplayHEX4::CDisplayHEX4()
{
   m_nBytes = sizeof(std::uint16_t);
   mType = HEX32;
}

QString CDisplayHEX4::Display(const std::uint8_t*pData)const
{
    return toHexString(pData);
}

bool CDisplayHEX4::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    if (sValue.size()<= 4)
    {
        std::uint16_t value = static_cast<double>(sValue.toUShort(&bok, 16));
        setData(pData, value, bok);
    }
    return bok;
}

CDisplayHEX8::CDisplayHEX8()
{
   m_nBytes = sizeof(std::uint32_t);
   mType = HEX64;
}

QString CDisplayHEX8::Display(const std::uint8_t*pData)const
{
    return toHexString(pData);
}

bool CDisplayHEX8::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    if (sValue.size()<= 8)
    {
        std::uint32_t value = static_cast<double>(sValue.toULong(&bok, 16));
        setData(pData, value, bok);
    }
    return bok;
}

CDisplayHEX16::CDisplayHEX16()
{
   m_nBytes = sizeof(std::uint64_t);
   mType = HEX128;
}

QString CDisplayHEX16::Display(const std::uint8_t*pData)const
{
    return toHexString(pData);
}

bool CDisplayHEX16::Write(std::uint8_t*pData, const QString& sValue)const
{
    bool bok {false};
    if (sValue.size()<= 16)
    {
        std::uint64_t value = static_cast<double>(sValue.toULongLong(&bok, 16));
        setData(pData, value, bok);
    }
    return bok;
}



CDisplayBinary::CDisplayBinary()
{
   m_nBytes = sizeof(unsigned char);
   mType = Binary;
}


QString CDisplayBinary::Display(const std::uint8_t*pData)const
{
    const int bits = 8;
    int i, j, k, n = m_nBytes * bits;
    std::uint8_t cMask;
    QString str;
    str.reserve(n+m_nBytes);
    for (i=m_nBytes-1, k=0; i>=0; --i)
    {
        cMask = 0x80;
        for (j=0; j<bits; ++j, cMask >>= 1, ++k)
        {
            str.append(pData[i] & cMask ? '1' : '0');
        }
        str.append(' ');
    }
    return str;
}

bool CDisplayBinary::Write(std::uint8_t*pData, const QString& sValue) const
{
    const int bits = 8;
    QString value(sValue);
    value.replace(" ", "");
    bool ret {false};
    if (pData)
    {
        int i, j, char_n, size = value.size();
        int rest  = size % bits;
        int bytes = size / bits;
        if (rest) bytes++;
        int desired_size = bytes * bits;

        for (; size<desired_size; ++size)
        {
            value = '0' + value;
        }
        std::uint8_t cMask;
        for (i=m_nBytes-1, char_n=0; i>=0 && char_n<size; --i)
        {
            cMask = 0x80;
            pData[i] = 0;
            for (j=0; j<bits && char_n<size; j++, cMask >>= 1, ++char_n)
            {
                if (value[char_n] == '1')
                {
                    pData[i] |= cMask;
                }
            }
            if (char_n >= size) break;
        }
        ret = true;
    }
    return ret;
}


#ifdef DISPLAY_STRINGS

CDisplayAscii::CDisplayAscii()
{
   m_nBytes = 1;
   mType = Ascii;
}

QString CDisplayAscii::Display(const std::uint8_t*pData)const
{
    if (m_nBytes == 0)
    {
        size_t nLen = GetByteLength(pData);
        QByteArray byte_array(reinterpret_cast<const char*>(pData), static_cast<int>(nLen));
        return byte_array;
    }
    else
    {
        QByteArray byte_array(reinterpret_cast<const char*>(pData), static_cast<int>(m_nBytes));
        return byte_array;
    }
}

size_t CDisplayAscii::GetByteLength(const std::uint8_t*pData) const
{
    size_t nLen = 0;
    if (pData)
    {
        const char *fText = reinterpret_cast<const char *>(pData);
        const char *fEndline = strstr(fText, "\r\n");
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

bool CDisplayAscii::Write(std::uint8_t*pData, const QString& sValue) const
{
    if (pData == 0) return false;
    int i, n = std::min(static_cast<int>(m_nBytes), sValue.size());
    for (i=0; i<n; ++i)
    {
        pData[i] = sValue.at(i).cell();
    }
    return n > 0;
}

CDisplayUnicode::CDisplayUnicode()
{
   m_nBytes = 2;
   mType = Unicode;
}

QString CDisplayUnicode::Display(const std::uint8_t*pData)const
{
    if (m_nBytes == 0)
    {
        size_t nLen = GetByteLength(pData);
        QString str(reinterpret_cast<const QChar*>(pData),  static_cast<int>(nLen));
        return str;
    }
    else
    {
       QString str(reinterpret_cast<const QChar*>(pData),  static_cast<int>(m_nBytes/2));
       return str;
    }
}

size_t CDisplayUnicode::GetByteLength(const std::uint8_t*pData) const
{
    size_t nLen = 0;
    if (pData)
    {
        const wchar_t *fText = reinterpret_cast<const wchar_t *>(pData);
        const wchar_t *fEndline = wcsstr(fText, L"\r\n");
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
    return static_cast<int>(nLen*sizeof(wchar_t));
}

bool CDisplayUnicode::Write(std::uint8_t*pData, const QString& sValue) const
{
    if (pData == 0) return false;
    int i, n = std::min(static_cast<int>(m_nBytes/2), sValue.size());
    wchar_t *pwData = (wchar_t*)pData;
    const QChar* pstr = sValue.data();
    for (i=0; i<n; i++)
    {
        if (m_different_endian)
        {
            CopyInverse((std::uint8_t*)&pstr[i], (std::uint8_t*)&pwData[i], 2);
        }
        else
        {
            pwData[i] = pData[i];
        }
    }
    return n > 0;
}
#endif

