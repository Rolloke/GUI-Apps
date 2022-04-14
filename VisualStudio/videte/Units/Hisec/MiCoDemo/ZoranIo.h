#include "zr050_io.h"

class ZoranIo : public Z050_IO       
{
public:
	ZoranIo(WORD wIOBASE);

    virtual BYTE Read050( WORD Address); 

    virtual void Write050( WORD Address, BYTE Data); 

    virtual BOOL Check050End( ); 

private:
	WORD	m_wIOBase;
	CIo		m_IO;
};
