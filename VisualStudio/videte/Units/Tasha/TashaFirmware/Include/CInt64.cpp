#include "CInt64.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInt64::CInt64()
{
	m_Data64.LowPart	= 0;
	m_Data64.HighPart	= 0;
}
//////////////////////////////////////////////////////////////////////
CInt64::CInt64(const CInt64& source)
{
	m_Data64.LowPart  = source.m_Data64.LowPart;
	m_Data64.HighPart = source.m_Data64.HighPart;
}

//////////////////////////////////////////////////////////////////////
CInt64::CInt64(int i)
{
	m_Data64.LowPart = (int)i;
	m_Data64.HighPart = 0;
}
//////////////////////////////////////////////////////////////////////
CInt64::CInt64(DWORD dwLo)
{
	m_Data64.LowPart	= (DWORD)dwLo;
	m_Data64.HighPart	= 0;
}

//////////////////////////////////////////////////////////////////////
CInt64::CInt64(DWORD dwLo, DWORD dwHi)
{
	m_Data64.LowPart	= (DWORD)dwLo;
	m_Data64.HighPart	= (DWORD)dwHi;
}
//////////////////////////////////////////////////////////////////////
CInt64::CInt64(const LARGE_INTEGER& ul)
{
	m_Data64.LowPart  = ul.LowPart;
	m_Data64.HighPart = ul.HighPart;
}

//////////////////////////////////////////////////////////////////////
const CInt64 & CInt64::operator =(const CInt64 &source)
{
	m_Data64.LowPart  = source.m_Data64.LowPart;
	m_Data64.HighPart = source.m_Data64.HighPart;
	return *this;
}

//////////////////////////////////////////////////////////////////////
const CInt64 & CInt64::operator +=(const CInt64 &source)
{
	DWORD dwResult;

  // DP(("Add((%x,%x) + (%x,%x) = ", m_Data64.HighPart, m_Data64.LowPart, source.m_Data64.HighPart, source.m_Data64.LowPart));

	dwResult = m_Data64.LowPart + source.m_Data64.LowPart;
	if (dwResult < m_Data64.LowPart)
		m_Data64.HighPart++;
	m_Data64.HighPart	+= source.m_Data64.HighPart;
	m_Data64.LowPart    = dwResult;

  // DP(("(%x,%x)\n", m_Data64.HighPart, m_Data64.LowPart));

   return *this;
}

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator +(const CInt64 &source)
{
	CInt64 tmp64 = *this;	
	return (tmp64 += source);	// <-- += Muß mit einer Kopie dieses Objektes aufgerufen werden!!

}

#if (0)
//////////////////////////////////////////////////////////////////////
const CInt64 & CInt64::operator -=(const CInt64 &source)
{
   // DP(("Sub((%x,%x) - (%x,%x) = ", m_Data64.HighPart, m_Data64.LowPart, source.m_Data64.HighPart, source.m_Data64.LowPart));

	m_Data64.HighPart -= source.m_Data64.HighPart;
	if (m_Data64.LowPart < source.m_Data64.LowPart)
		m_Data64.HighPart--;
	
	m_Data64.LowPart -= source.m_Data64.LowPart;
   // DP(("(%x,%x)\n", m_Data64.HighPart, m_Data64.LowPart));

   return *this;
}

#else
//////////////////////////////////////////////////////////////////////
const CInt64 & CInt64::operator -=(const CInt64 &source)
{
   // DP(("Sub((%x,%x) - (%x,%x) = ", m_Data64.HighPart, m_Data64.LowPart, source.m_Data64.HighPart, source.m_Data64.LowPart));

   if (m_Data64.HighPart >= source.m_Data64.HighPart)
   {
      m_Data64.HighPart -= source.m_Data64.HighPart;
      if (m_Data64.LowPart < source.m_Data64.LowPart)
	  {
         if (m_Data64.HighPart)
		 {
            m_Data64.HighPart--;
            m_Data64.LowPart -= source.m_Data64.LowPart;
         }
		 else
		 {
            m_Data64.LowPart  = 0;
         }
      }
	  else
	  {
         m_Data64.LowPart -= source.m_Data64.LowPart;
      }
   }
   else
   {
      m_Data64.HighPart = 0;
      m_Data64.LowPart   = 0;
   }

   // DP(("(%x,%x)\n", m_Data64.HighPart, m_Data64.LowPart));

   return *this;
}
#endif

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator -(const CInt64 &source)
{
	CInt64 tmp64 = *this;	
	return (tmp64 -= source);	// <-- -= Muß mit einer Kopie dieses Objektes aufgerufen werden!!

}

//////////////////////////////////////////////////////////////////////
const CInt64 & CInt64::operator *=(const CInt64 &source)
{
   double fFloat, fDiv, fMaxInt;

   // DP(("Mul((%x,%x) * (%x,%x) = ", m_Data64.HighPart, m_Data64.LowPart, source.m_Data64.HighPart, source.m_Data64.LowPart));

   fMaxInt        = 4294967296.0;
   fFloat          = ((double)m_Data64.HighPart * fMaxInt) + (double)m_Data64.LowPart;
   fFloat         *= ((double)source.m_Data64.HighPart * fMaxInt) + (double) source.m_Data64.LowPart;
   fDiv           = fFloat / fMaxInt;
   m_Data64.HighPart  = (DWORD)fDiv;
   m_Data64.LowPart   = (DWORD)(fFloat-((double)m_Data64.HighPart * fMaxInt));

   // DP(("(%x,%x)\n", m_Data64.HighPart, m_Data64.LowPart));

   return *this;
}

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator *(const CInt64 &source)
{
	CInt64 tmp64 = *this;	
	return (tmp64 *= source); // <-- *= Muß mit einer Kopie dieses Objektes aufgerufen werden!!
}

//////////////////////////////////////////////////////////////////////
// Es scheint, als ob dieser Operator nicht korrekt arbeitet.
// CInt64 GetTime() verwendet diesen Operator. und scheint damit 
// des öfteren fehlerhafte Resultate zu liefern!
const CInt64 & CInt64::operator /=(const CInt64& source )
{
   double fFloat, fDiv, fMaxInt;

   // DP(("Div((%x,%x) / (%x,%x) = ", m_Data64.HighPart, m_Data64.LowPart, source.m_Data64.HighPart, source.m_Data64.LowPart));
   fMaxInt        = 4294967296.0;
   fFloat          = ((double)m_Data64.HighPart * fMaxInt) + (double)m_Data64.LowPart;
   fFloat         /= ((double)source.m_Data64.HighPart * fMaxInt) + (double)source.m_Data64.LowPart;

   fDiv           = fFloat / fMaxInt;
   m_Data64.HighPart  = (DWORD)fDiv;
   m_Data64.LowPart    = (DWORD)(fFloat-((double)m_Data64.HighPart * fMaxInt));

   // DP(("(%x,%x)\n", m_Data64.HighPart, m_Data64.LowPart));

   return *this;
}

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator /(const CInt64& source )
{
	CInt64 tmp64 = *this;	
	return (tmp64 /= source); // <-- /= Muß mit einer Kopie dieses Objektes aufgerufen werden!!

}

//////////////////////////////////////////////////////////////////////
const CInt64& CInt64::operator ++()
{
	return *this += 1;
}

//////////////////////////////////////////////////////////////////////
const CInt64& CInt64::operator --()
{
	return *this -= 1;
}

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator ++(int)
{
	CInt64 tmp = *this;
	*this += 1;
	return tmp;
}

//////////////////////////////////////////////////////////////////////
CInt64 CInt64::operator --(int)
{
	CInt64 tmp = *this;
	*this -= 1;
	return tmp;
}

//////////////////////////////////////////////////////////////////////
LARGE_INTEGER CInt64::GetInt64() const
{
	return m_Data64;
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator > (const CInt64& s)
{
	if (s.m_Data64.HighPart < m_Data64.HighPart)
		return TRUE;
	else if (s.m_Data64.HighPart == m_Data64.HighPart)
	{
		if (s.m_Data64.LowPart < m_Data64.LowPart)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator < (const CInt64& s)
{
	return !(*this > s);
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator == (const CInt64& s)
{
	return ((m_Data64.LowPart == s.m_Data64.LowPart) && (m_Data64.HighPart == s.m_Data64.HighPart));
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator != (const CInt64& s)
{
	return (!(*this == s));
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator >= (const CInt64& s)
{
	if (s.m_Data64.HighPart < m_Data64.HighPart)
		return TRUE;
	else if (s.m_Data64.HighPart == m_Data64.HighPart)
	{
		if (s.m_Data64.LowPart <= m_Data64.LowPart)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CInt64::operator <= (const CInt64& s)
{
	return (!(*this > s));
}

//////////////////////////////////////////////////////////////////////
CInt64::operator DWORD()
{
	return m_Data64.LowPart;
}

//////////////////////////////////////////////////////////////////////
/*inline const CInt64 CInt64::operator %(const CInt64 &source)
{
	CInt64 tmp64 = *this;
	tmp64 /= source;
	tmp64 *= source;
	return (*this - tmp64);
}
*/
//////////////////////////////////////////////////////////////////////
CInt64::operator const char*()
{
	char sBuffer[22] = {0};
	
	CInt64 a	= 0;
	CInt64 b	= *this;
	int nI		= 0;
	int nJ		= 0;

	do
	{
		a = (BYTE)(b % 10);
		sBuffer[nI++] = '0'+(BYTE)a;
		b = (b-a);
		b /= 10;

	}while (b != (CInt64)0);

	nI--;
	for (nJ = 0; nJ <= nI; nJ++)
		m_sBuffer[nJ]= sBuffer[nI-nJ];

	m_sBuffer[nJ]= '\0';
	
	return m_sBuffer;
}
