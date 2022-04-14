// GeldDaten.h
//
#ifndef __GELDDATEN_H__
#define __GELDDATEN_H__

					
#define GDC_DATUM	(0x01)
#define GDC_ZEIT	(0x02)
#define GDC_TA		(0x04)
#define GDC_GAA		(0x08)
#define GDC_BLZ		(0x10)
#define GDC_KTNR	(0x20)
#define GDC_BETRAG	(0x40)

#define	DATUM_LEN	(6)
#define	ZEIT_LEN	(4)
#define	TA_LEN		(4)
#define	GAA_LEN		(6)
#define	BLZ_LEN		(8)
#define	KTONR_LEN	(10)
#define	BETRAG_LEN	(4)

#define	GDC_GESAMT_LEN (DATUM_LEN+ZEIT_LEN+TA_LEN+GAA_LEN+BLZ_LEN+KTONR_LEN+BETRAG_LEN)

/////////////////////////////////////////////////////////////////
class CGeldDaten
{
	// construction
public:
	CGeldDaten();
	virtual ~CGeldDaten(){}
	CGeldDaten(CGeldDaten& source);
	CGeldDaten(LPCSTR lpszDaten);

	// attributes
public:
	inline const char*	GetDatum() const;
	inline const char*	GetZeit() const;
	inline const char*	GetTransaktion() const;
	inline const char*	GetAutomat() const;
	inline const char*	GetBankleitzahl() const;
	inline const char*	GetKontonummer() const;
	inline const char*	GetBetrag() const;
	inline CString		GetFormatedZeit() const;
	inline CString		GetFormatedDatum() const;
	inline BOOL		IsCorrected();

	inline CTime	GetTime();
	inline void		SetTime();

	// operations
public:
	void	Clear();
	void	Validate();
	void	Trace();
	CString Format();
	CString FormatAusgabeString();
	void	Set(LPCSTR lpszDaten);
	
	// members
public:
	char m_Datum		[DATUM_LEN+1];
	char m_Zeit			[ZEIT_LEN+1];
	char m_Transaktion	[TA_LEN+1];
	char m_Automat		[GAA_LEN+1];
	char m_Bankleitzahl	[BLZ_LEN+1];
	char m_Kontonummer	[KTONR_LEN+1];
	char m_Betrag		[BETRAG_LEN+1];

protected:
	enum { D=1,Z=2,T=3,A=4,B=5,K=6,W=7 };

	BOOL	m_bCorrected;
	void	Clear(int iWhat);
	BOOL	CheckDigitOrSpace(int iWhat);
	CTime m_Time;
};
/////////////////////////////////////////////////////////////////
inline const char*	CGeldDaten::GetDatum() const
{
	return m_Datum;
}
inline const char*	CGeldDaten::GetZeit() const
{
	return m_Zeit;
}
inline CString CGeldDaten::GetFormatedZeit() const
{
	CString s = GetZeit();
	s.TrimRight();
	CString sZeit = "     ";
	if (s.GetLength() == 4){
		sZeit.SetAt(0, s[0]);
		sZeit.SetAt(1, s[1]);
		sZeit.SetAt(2, ':');
		sZeit.SetAt(3, s[2]);
		sZeit.SetAt(4, s[3]);
	}else{
		sZeit = "----";
		// Das darf nicht geaendert werden, da der PH den Rückgabewert
		// dieser Funktion auf (sZeit == "----") abprüft.
	}
	return sZeit;
}

inline CString CGeldDaten::GetFormatedDatum() const
{
	CString s = GetDatum();
	s.TrimRight();
	CString sDatum = "--------";
	if (s.GetLength() == 6){
		sDatum.SetAt(0, s[4]);
		sDatum.SetAt(1, s[5]);
		sDatum.SetAt(2, '.');
		sDatum.SetAt(3, s[2]);
		sDatum.SetAt(4, s[3]);
		sDatum.SetAt(5, '.');
		sDatum.SetAt(6, s[0]);
		sDatum.SetAt(7, s[1]);
	}
	return sDatum;
}

inline const char*	CGeldDaten::GetTransaktion() const
{
	return m_Transaktion;
}
inline const char*	CGeldDaten::GetAutomat() const
{
	return m_Automat;
}
inline const char*	CGeldDaten::GetBankleitzahl() const
{
	return m_Bankleitzahl;
}
inline const char*	CGeldDaten::GetKontonummer() const
{
	return m_Kontonummer;
}
inline const char*	CGeldDaten::GetBetrag() const
{
	return m_Betrag;
}
inline BOOL CGeldDaten::IsCorrected()
{
	return m_bCorrected;
}
inline CTime CGeldDaten::GetTime()
{
	return m_Time;
}
inline void	CGeldDaten::SetTime()
{
	m_Time = CTime::GetCurrentTime();
}

#endif