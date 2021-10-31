// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	CARA3DV.pch ist die vorcompilierte Header-Datei
//	stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"

#ifdef _DEBUG
void glcLastError()
{
   int error = glGetError();
   if (error != GL_NO_ERROR)
   {
      if (error == GL_INVALID_OPERATION) 
      {
         TRACE("GL_INVALID_OPERATION\n");
      }
      else
      {
         ASSERT(false);
         switch (error)
         {
            case GL_INVALID_ENUM :      TRACE("An unacceptable value is specified for an enumerated argument.\nThe offending function is ignored, having no side effect other\nthan to set the error flag.\n");         break;
            case GL_INVALID_VALUE :     TRACE("A numeric argument is out of range. The offending function is ignored,\nhaving no side effect other than to set the error flag.\n");                                     break;
            case GL_STACK_OVERFLOW :    TRACE("This function would cause a stack overflow. The offending function\nis ignored, having no side effect other than to set the error flag.\n");                             break;
            case GL_STACK_UNDERFLOW :   TRACE("This function would cause a stack underflow. The offending function\nis ignored, having no side effect other than to set the error flag.\n");                            break;
            case GL_OUT_OF_MEMORY :     TRACE("There is not enough memory left to execute the function. The state\nof OpenGL is undefined, except for the state of the error flags,\nafter this error is recorded.\n"); break;
         }
      }
   }
}

void AssertAndReportInt(int nError)
{
   if (nError) 
   {
//      ASSERT(false); 
      LPVOID lpMsgBuf;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                    GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf, 0, NULL );                             // Process any inserts in lpMsgBuf.
      TRACE("%d : %s", nError, lpMsgBuf);
      LocalFree( lpMsgBuf );
   }
}
#endif
