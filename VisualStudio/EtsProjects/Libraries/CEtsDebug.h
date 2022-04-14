/**************************************+****************************************

                                  CEtsDebug.h
                                  -----------

                    Standard Komponenten Debugger User Header

                     (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 26.08.1999


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#if !defined(__cplusplus)
   #error C++ compiler required !
#endif

#ifdef _CETSDEBUG_HEADER_
   #error CEtsDebug-Header included more the ones !
#else

   #define _CETSDEBUG_HEADER_

   #ifdef BEGIN
      #undef BEGIN
   #endif

   #ifdef END
      #undef END
   #endif

   #ifdef REPORT
      #undef REPORT
   #endif

   #if defined(_DEBUG)
      #define ETSDEBUG_MODE true
   #else
      #define ETSDEBUG_MODE false
   #endif

   #if defined(ETSDEBUG_INCLUDE)
      class CEtsDebug
      {
         public: 
            CEtsDebug(bool ,const char * szFileName,const bool * = NULL);
            ~CEtsDebug();

                   void        Destructor();     // darf nicht static sein
            static void        Begin(const char * szText);
            static void        End();
            static void _cdecl Report(const char *,...);
      };

      class CEtsDebugBegin
      {  
         public:

         CEtsDebugBegin(const char * szText){CEtsDebug::Begin(szText);};
         ~CEtsDebugBegin(void){CEtsDebug::End();};
      };

      #define BEGIN(s)       CEtsDebugBegin localfunctioninstance = CEtsDebugBegin(s);
      #define END            ;                   // 26.08.1999 ab jetzt nicht mehr nötig !
      #define REPORT         CEtsDebug::Report
      #define ETSDEBUGEND    g_etsdebuginstance.Destructor();

      #if defined(ETSDEBUG_CREATEDATA)
         CEtsDebug g_etsdebuginstance = CEtsDebug(ETSDEBUG_MODE,ETSDEBUG_CREATEDATA);
      #endif

   #else
      #define BEGIN(s)      ;
      #define END           ;
      #define REPORT        ;
      #define ETSDEBUGEND   ;
   #endif
#endif
