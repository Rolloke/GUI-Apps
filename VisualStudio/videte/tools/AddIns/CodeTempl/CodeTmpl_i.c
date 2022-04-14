/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.01.75 */
/* at Sat Jun 20 13:22:46 1998
 */
/* Compiler settings for CodeTmpl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ICTAddIn = {0x9EDADF6F,0xC069,0x11D1,{0xA4,0x46,0x00,0x00,0xE8,0xD2,0xCD,0x0A}};


const IID LIBID_CODETMPLLib = {0x9EDADF62,0xC069,0x11D1,{0xA4,0x46,0x00,0x00,0xE8,0xD2,0xCD,0x0A}};


const CLSID CLSID_CTAddIn = {0x9EDADF70,0xC069,0x11D1,{0xA4,0x46,0x00,0x00,0xE8,0xD2,0xCD,0x0A}};


#ifdef __cplusplus
}
#endif

