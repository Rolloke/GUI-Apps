/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Memory test stand-alone program Eagle-35/Hawk-35/EZ-Kit Lite, header file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/BlackFin/include/mem_tst.h $
 * 
 * 1     5.01.04 9:53 Martin.lueck
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _MEM_TEST_H_
#define _MEM_TEST_H_

//..............................................................................

#if defined (_EZKITLITE_) || defined (_HAWK_35_) // [ _EZKITLITE_ || _HAWK_35_

#ifdef _HAWK_35_                   // [ _HAWK_35_

#define START_EXT_L2SDRAM (uint32 *)0x0
#define END_EXT_L2SDRAM (uint32 *)0x02000000 // 32 MByte or 8M x 32 bits

#else                              // ] [ _EZKITLITE_

#define START_EXT_L2SDRAM (uint32 *)0x0
#define END_EXT_L2SDRAM (uint32 *)0x01000000 // 16 MByte or 4M x 32 bits

#endif                             //  _EZKITLITE_ ]

// EBIU_SDRRC

#define	kSDRRC	\
		ZET(bEBIU_RDIV,2071)  /* RDIV */

// EBIU_SDBCTL

#define	kSDBCTL	\
		ZET(bEBIU_EB0E,1)+    /* SDRAM external bank 0 enable */       \
		ZET(bEBIU_EB0SZ,2)+   /* SDRAM external bank 0 64 MB */        \
		ZET(bEBIU_EB0CAW,0)+  /* Bank 0 column address width 8 bits */ \
		ZET(bEBIU_EB1E,1)+    /* SDRAM external bank 1 enable */       \
		ZET(bEBIU_EB1SZ,2)+   /* SDRAM external bank 1 64 MB */        \
		ZET(bEBIU_EB1CAW,0)+  /* Bank 1 column address width 8 bits */ \
		ZET(bEBIU_EB2E,0)+    /* SDRAM external bank 2 disable */      \
		ZET(bEBIU_EB2SZ,2)+   /* SDRAM external bank 2 64 MB */        \
		ZET(bEBIU_EB2CAW,0)+  /* Bank 2 column address width 8 bits */ \
		ZET(bEBIU_EB3E,0)+    /* SDRAM external bank 3 disable */      \
		ZET(bEBIU_EB3SZ,2)+   /* SDRAM external bank 3 64 MB */        \
		ZET(bEBIU_EB3CAW,0)   /* Bank 3 column address width 8 bits */             


// EBIU_SDGCTL

#define kSDGCTL \
		ZET(bEBIU_SCTLE,1)+   \
                                  /* Enable SCLK[0],SRAS,SCAS,SWE,SDQM[3:0] */ \
		ZET(bEBIU_SCK1E,1)+   /* Enable SCLK[1] */                     \
		ZET(bEBIU_CL,2)+      /* SDRAM CAS latency 2 cycles */         \
		ZET(bEBIU_PFE,1)+     /* SDRAM prefetch enable */              \
		ZET(bEBIU_PFP,1)+     /* Prefetch priority over AMC requests*/ \
		ZET(bEBIU_TRAS,7)+    /* SDRAM tRAS in SCLK cycles */          \
		ZET(bEBIU_TRP,3)+     /* SDRAM tRP in SCLK cycles */           \
		ZET(bEBIU_TRCD,3)+    /* SDRAM tRCD in SCLK cycles */          \
		ZET(bEBIU_TWR,2)+     /* SDRAM tWR in SCLK cycles */           \
		ZET(bEBIU_PSM,0)+     /* SDRAM power-up sequence, */           \
           /* Precharge, 8 CBR refresh cycles, mode register set  */           \
		ZET(bEBIU_PSSE,1)+    /* Enables SDRAM power-up sequence */    \
                                      /* on next SDRAM access. */              \
		ZET(bEBIU_SRFS,0)+    /* SDRAM self-refresh mode start, */     \
                                      /* no effect */                          \
		ZET(bEBIU_EBUFE,0)+   /* External buffering timing disabled */ \
		ZET(bEBIU_X16DE,0)    /* SDRAM external datapath width 32 bits*/     

#else                               // ] [ !(_EZKITLITE_ || _HAWK_35_)

#ifdef _EAGLE_35_                   // [ _EAGLE_35_


// EBIU_SDRRC

#define	kSDRRC	\
		ZET(bEBIU_RDIV,2071)  /* RDIV */


#if 0  // [ KMM366S1623CT, 128M, 2 rows, 16 chips

#define START_EXT_L2SDRAM (uint32 *)0x0
#define END_EXT_L2SDRAM (uint32 *)0x08000000 // 128 MByte DIMM PC-133

// EBIU_SDBCTL

#define	kSDBCTL	\
		ZET(bEBIU_EB0E,1)+    /* SDRAM external bank 0 enable */       \
		ZET(bEBIU_EB0SZ,1)+   /* SDRAM external bank 0 32 MB */        \
		ZET(bEBIU_EB0CAW,1)+  /* Bank 0 column address width 9 bits */ \
		ZET(bEBIU_EB1E,1)+    /* SDRAM external bank 1 enable */       \
		ZET(bEBIU_EB1SZ,1)+   /* SDRAM external bank 1 32 MB */        \
		ZET(bEBIU_EB1CAW,1)+  /* Bank 1 column address width 9 bits */ \
		ZET(bEBIU_EB2E,1)+    /* SDRAM external bank 2 enable */      \
		ZET(bEBIU_EB2SZ,1)+   /* SDRAM external bank 2 32 MB */        \
		ZET(bEBIU_EB2CAW,1)+  /* Bank 2 column address width 9 bits */ \
		ZET(bEBIU_EB3E,1)+    /* SDRAM external bank 3 enable */      \
		ZET(bEBIU_EB3SZ,1)+   /* SDRAM external bank 3 32 MB */        \
		ZET(bEBIU_EB3CAW,1)   /* Bank 3 column address width 9 bits */             

#else  // ] [ No-name DIMM, 64M, 1 row, 8 chips (?)

#define START_EXT_L2SDRAM (uint32 *)0x0
#define END_EXT_L2SDRAM (uint32 *)0x04000000 // 64 MByte DIMM PC-133

// EBIU_SDBCTL

#define	kSDBCTL	\
		ZET(bEBIU_EB0E,1)+    /* SDRAM external bank 0 enable */       \
		ZET(bEBIU_EB0SZ,1)+   /* SDRAM external bank 0 32 MB */        \
		ZET(bEBIU_EB0CAW,1)+  /* Bank 0 column address width 9 bits */ \
		ZET(bEBIU_EB1E,1)+    /* SDRAM external bank 1 enable */       \
		ZET(bEBIU_EB1SZ,1)+   /* SDRAM external bank 1 32 MB */        \
		ZET(bEBIU_EB1CAW,1)+  /* Bank 1 column address width 9 bits */ \
		ZET(bEBIU_EB2E,0)+    /* SDRAM external bank 2 enable */      \
		ZET(bEBIU_EB2SZ,1)+   /* SDRAM external bank 2 32 MB */        \
		ZET(bEBIU_EB2CAW,1)+  /* Bank 2 column address width 9 bits */ \
		ZET(bEBIU_EB3E,0)+    /* SDRAM external bank 3 enable */      \
		ZET(bEBIU_EB3SZ,1)+   /* SDRAM external bank 3 32 MB */        \
		ZET(bEBIU_EB3CAW,1)   /* Bank 3 column address width 9 bits */             

#endif     // ]

// EBIU_SDGCTL

#define kSDGCTL \
		ZET(bEBIU_SCTLE,1)+   \
                                  /* Enable SCLK[0],SRAS,SCAS,SWE,SDQM[3:0] */ \
		ZET(bEBIU_SCK1E,1)+   /* Enable SCLK[1] */                     \
		ZET(bEBIU_CL,2)+      /* SDRAM CAS latency 2 cycles */         \
		ZET(bEBIU_PFE,1)+     /* SDRAM prefetch enable */              \
		ZET(bEBIU_PFP,1)+     /* Prefetch priority over AMC requests*/ \
		ZET(bEBIU_TRAS,7)+    /* SDRAM tRAS in SCLK cycles */          \
		ZET(bEBIU_TRP,3)+     /* SDRAM tRP in SCLK cycles */           \
		ZET(bEBIU_TRCD,3)+    /* SDRAM tRCD in SCLK cycles */          \
		ZET(bEBIU_TWR,2)+     /* SDRAM tWR in SCLK cycles */           \
		ZET(bEBIU_PSM,0)+     /* SDRAM power-up sequence, */           \
           /* Precharge, 8 CBR refresh cycles, mode register set  */           \
		ZET(bEBIU_PSSE,1)+    /* Enables SDRAM power-up sequence */    \
                                      /* on next SDRAM access. */              \
		ZET(bEBIU_SRFS,0)+    /* SDRAM self-refresh mode start, */     \
                                      /* no effect */                          \
		ZET(bEBIU_EBUFE,0)+   /* External buffering timing disabled */ \
		ZET(bEBIU_X16DE,0)    /* SDRAM external datapath width 32 bits*/     

#else                               // ] [ !_EAGLE_35_

#error "Board not defined"

#endif                              // ] _EAGLE_35_

#endif                              // ] (_EZKITLITE_ || _HAWK_35_)

//..............................................................................

#endif
