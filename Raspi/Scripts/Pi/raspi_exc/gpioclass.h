#ifndef _GPIO_CLASS_H_
#define _GPIO_CLASS_H_

#include <string>

class CGPIO
{
public: 
	enum eDirection { None, In, Out };
	
	CGPIO(int aPort, eDirection aOutput=None);
	~CGPIO();
	
	// getter / setter
	int  value();
	void value(int aValue);
	
	const std::string&  port();
	void  direction(eDirection aOutput);
	
private:
	
	std::string mPort;
	int mFile; 
};

int delay(unsigned long millis);
int udelay(unsigned long micros);


#endif
