#ifndef _CCARAHELP_
#define _CCARAHELP_

class CCaraHelp
{
public:
   ~CCaraHelp();

   static void CreateContextWnd(const char*,int,int);
   static void CreateErrorWnd(const char*,int,int); 
};
#endif