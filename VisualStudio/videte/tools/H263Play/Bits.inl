// Bits.inl

#ifndef _BITS_INL
#define _BITS_INL

inline void CBits::init()
{
	m_ByteCount = 0;
	m_BitCount = 0;
}

// return next n bits (right adjusted) 
// without advancing
inline unsigned int CBits::showbits(int n)
{
	unsigned int r;

	r = (m_pData[m_ByteCount]<<24)   | 
		(m_pData[m_ByteCount+1]<<16) |
		(m_pData[m_ByteCount+2]<<8)  |
		(m_pData[m_ByteCount+3]);
	r <<= m_BitCount;
//  the following line is only
//	necessary showbits is ever called with n>24
	r |= (m_pData[m_ByteCount+4] >> (8-m_BitCount));
	// now r contains next 32 bits
	r >>= 32-n;
	return r;
}
/* return next bit (could be made faster than getbits(1)) */
// advance
inline unsigned int CBits::getbits1()
{
	unsigned int l;
	l = (m_pData[m_ByteCount] & m_bitMsk[m_BitCount])>>(7-m_BitCount);// 
	m_ByteCount += (m_BitCount+1)>>3;
	m_BitCount = (m_BitCount+1)&7;
	return l;
}


/* advance by n bits */

inline void CBits::flushbits(int n)
{
	m_ByteCount += (m_BitCount+n)>>3;
	m_BitCount = (m_BitCount+n)&7;
}


/* return next n bits (right adjusted) */

inline unsigned int CBits::getbits(int n)
{
	unsigned int l;
	
	l = showbits(n);
	flushbits(n);
	
	return l;
}
#endif