// Mit Microsoft Visual C++ automatisch erstellte IDispatch-Kapselungsklasse(n).

// HINWEIS: Die Inhalte dieser Datei nicht ändern. Wenn Microsoft Visual C++
// diese Klasse erneuert, werden Ihre Änderungen überschrieben.


#include "stdafx.h"
#include "picture.h"

#ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Eigenschaften CPicture 

long CPicture::GetHandle()
{
	long result;
	GetProperty(0x0, VT_I4, (void*)&result);
	return result;
}

long CPicture::GetHPal()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void CPicture::SetHPal(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

short CPicture::GetType()
{
	short result;
	GetProperty(0x3, VT_I2, (void*)&result);
	return result;
}

long CPicture::GetWidth()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

long CPicture::GetHeight()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// Operationen CPicture 
#endif // _DEBUG
