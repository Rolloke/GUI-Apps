/*-------------------------------------------------------------------------- *
 *
 *   QuickStep P2C Translator Library                       V 2.06-TP
 *
 *   (c) Copyright 1987-90 by LAUER & WALLWITZ GmbH, D-6200 Wiesbaden
 *
 *   P2C.H : Constants, macros, base functions and memory management
 *
 *-------------------------------------------------------------------------- */

#include <P2CRESC.H>

#define odd(i)      ((i)&1)
#define pred(i)     ((i)-1)
#define succ(i)     ((i)+1)
#define shl(a,b)    ((a)<<(b))
#define shr(a,b)    ((a)>>(b))
#define trunc(r)    ((SHORT)(r))
#define LOBYTE(a)  *((PBYTE)&(a))
#define HIBYTE(a)  *((PBYTE)(&(a))+1)

#if !defined(TURBOC) && !defined(TOPC)
  extern  SHORT  _argc;
  extern  PSZ   *_argv;
#endif
#if defined(TURBOC) || defined(TOPC)
  typedef BYTE FILLPATTERNTYPE[8]; /* graph.tpu -> graphics.h */
  typedef struct {SHORT x1,y1,x2,y2,clip;} VIEWPORTTYPE;
#endif

typedef CHAR STRING[256];

#define _VER40          /* TP 4.0 $DEFINE's */
#define _VER50          /* TP 5.0 $DEFINE's */
#define _CPU86
#define _MSDOS

VOID p2c_init(PVOID pool,USHORT poolsize,SHORT iomode);
PVOID p2calloc(USHORT size);
#define p2cdup(x) memmove(p2calloc(sizeof(x)),x,sizeof(x))
PSZ strncpy0(PSZ dest,PSZ src,USHORT destsize);
CHAR upcase(CHAR c);
SHORT swap(SHORT v);
ULONG memavail(VOID);
ULONG maxavail(VOID);
SHORT xscanf(CHAR (* next)(VOID),PSZ fmt,PSZ argp);
/*#if P2CLIB == 'a'
extern VOID (* exitproc)(VOID);
#else
extern VOID (far *exitproc)(VOID);
#endif  */
