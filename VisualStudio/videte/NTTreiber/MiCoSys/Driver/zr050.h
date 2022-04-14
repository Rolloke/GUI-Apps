
/*************************************************************************
 *
 *   ZR050.H
 *
 *   050 coder CLASS definitions
 *
 *
 *   Copyright (c) 1993 Zoran Corporation.  All Rights Reserved.
 *
 *
 *************************************************************************/

#ifndef Z050_CODER
#define Z050_CODER

#include "zr050_io.h"


//
   // --------------------------------------------------------------
   //   Indexes of 050 internal registers and of functional locations
   //   in the 050's INTERNAL MEMORY
   // --------------------------------------------------------------

#define GO                  0x000
// NOT USED                 0x001
#define HARDWARE            0x002
#define MODE                0x003
#define OPTIONS             0x004
#define MBCV_050            0x005        // !
#define MARKERS_EN          0x006
#define INT_REQ_0           0x007
#define INT_REQ_1           0x008
#define TCV_NET_HI          0x009
#define TCV_NET_MH          0x00a
#define TCV_NET_ML          0x00b
#define TCV_NET_LO          0x00c
#define TCV_DATA_HI         0x00d
#define TCV_DATA_MH         0x00e
#define TCV_DATA_ML         0x00f
#define TCV_DATA_LO         0x010
#define SF_HI               0x011
#define SF_LO               0x012
#define AF_HI               0x013
#define AF_M                0x014
#define AF_LO               0x015
#define ACV_HI              0x016
#define ACV_MH              0x017
#define ACV_ML              0x018
#define ACV_LO              0x019
#define ACT_HI              0x01a
#define ACT_MH              0x01b
#define ACT_ML              0x01c
#define ACT_LO              0x01d
#define ACV_TRUN_HI         0x01e
#define ACV_TRUN_MH         0x01f
#define ACV_TRUN_ML         0x020
#define ACV_TRUN_LO         0x021
#define STATUS_0            0x02e
#define STATUS_1            0x02f


   // --------------------------------------------------------------
   //   Indexes of JPEG Baseline Markers Segment 
   //   in the 050's INTERNAL MEMORY
   // --------------------------------------------------------------

#define SOF_INDX            0x040
#define SOS1_INDX           0x07a 
#define SOS2_INDX           0x08a 
#define SOS3_INDX           0x09a 
#define SOS4_INDX           0x0aa 
#define DRI_INDX            0x0c0
#define DNL_INDX            0x0c6
#define DQT_INDX            0x0cc
#define DHT_INDX            0x1d4
#define APP_INDX            0x380
#define COM_INDX            0x3c0


   // --------------------------------------------------------------
   //   Values of JPEG Baseline Markers 
   // --------------------------------------------------------------

#define SOF0_MARKER         0xC0 
#define SOS_MARKER          0xDA  
#define DRI_MARKER          0xDD 
#define DNL_MARKER          0xDC 
#define DQT_MARKER          0xDB 
#define DHT_MARKER          0xC4 
#define APP_MARKER          0xE0 
#define COM_MARKER          0xFE 


//
   // --------------------------------------------------------------
   //   Definitions of bits' masks in the 050's HARDWARE register
   // --------------------------------------------------------------

#define BSWD_MASK           0x80
#define BSWD                0x80
#define NO_BSWD             0x00

#define MSTR_MASK           0x40
#define MSTR                0x40
#define NO_MSTR             0x00

#define DMA_MASK            0x20
#define DMA                 0x20
#define NO_DMA              0x00

#define CFIS_MASK           0x1C
#define CFIS_1_CLK          0x00
#define CFIS_2_CLK          0x04
#define CFIS_3_CLK          0x08
#define CFIS_4_CLK          0x0C
#define CFIS_5_CLK          0x10
#define CFIS_6_CLK          0x14
#define CFIS_7_CLK          0x18
#define CFIS_8_CLK          0x1C

#define BELE_MASK           0x01
#define BELE                0x01
#define NO_BELE             0x00


   // --------------------------------------------------------------
   //   Definitions of bits' masks in the 050's MODE register
   // --------------------------------------------------------------

#define COMP_MASK           0x80
#define COMP                0x80
#define NO_COMP             0x00

#define ATP_MASK            0x40
#define ATP                 0x40
#define NO_ATP              0x00

#define PASS2_MASK          0x20
#define PASS2               0x20
#define NO_PASS2            0x00

#define TLM_MASK            0x10
#define TLM                 0x10
#define NO_TLM              0x00

#define DCONLY_MASK         0x08
#define DCONLY              0x08
#define NO_DCONLY           0x00

#define BRC_MASK            0x04
#define BRC                 0x04
#define NO_BRC              0x00


   // --------------------------------------------------------------
   //   Definitions of bits' masks in the 050's OPTIONS register
   // --------------------------------------------------------------

#define NSCN_MASK           0xE0
#define NSCN_1              0x00
#define NSCN_2              0x20
#define NSCN_3              0x40
#define NSCN_4              0x60
#define NSCN_5              0x80
#define NSCN_6              0xA0
#define NSCN_7              0xC0
#define NSCN_8              0xE0

#define OVF_MASK            0x10
#define OVF                 0x10
#define NO_OVF              0x00


   // --------------------------------------------------------------
   //   Definitions of bits' masks in the 050's MARKERS_EN register
   // --------------------------------------------------------------

#define APP_MASK            0x80
#define APP_ON              0x80
#define APP_OFF             0x00

#define COM_MASK            0x40
#define COM_ON              0x40
#define COM_OFF             0x00

#define DRI_MASK            0x20
#define DRI_ON              0x20
#define DRI_OFF             0x00

#define DQT_MASK            0x10
#define DQT_ON              0x10
#define DQT_OFF             0x00

#define DHT_MASK            0x08
#define DHT_ON              0x08
#define DHT_OFF             0x00

#define DNL_MASK            0x04
#define DNL_ON              0x04
#define DNL_OFF             0x00

#define DQTI_MASK           0x02
#define DQTI_ON             0x02
#define DQTI_OFF            0x00

#define DHTI_MASK           0x01
#define DHTI_ON             0x01
#define DHTI_OFF            0x00


   // --------------------------------------------------------------
   //   Defs of bits' masks in 050's STATUS_0 and INT_REQ_0 registers
   // --------------------------------------------------------------

   // NOTE : THE MAPPING OF BITS IN THIS REGISTER IS IDENTICAL TO
   //        THE ONE IN MARKERS_EN REGISTERS - EXCEPT LAST TWO BITS.
   //        THEREFORE SAME MASKS ARE USED.

#define RST_MASK            DRI_MASK        // two functions for this bit
#define SOF_MASK            0x02
#define SOS_MASK            0x02


   // --------------------------------------------------------------
   //   Defs of bits' masks in 050's STATUS_1 and INT_REQ_1 registers
   // --------------------------------------------------------------


#define DATRDY_MASK         0x80
#define MRKDET_MASK         0x40
#define RFM_MASK            0x10
#define RFD_MASK            0x08
#define END_MASK            0x04
#define TCVOVF_MASK         0x02
#define DATOVF_MASK         0x01


   // --------------------------------------------------------------
   //   Defs of sizes of marker segments
   // --------------------------------------------------------------


#define QTABLE_SIZE         0x40
#define HUFF_AC_SIZE        0xB2             //178 !!
#define HUFF_DC_SIZE        0x1C             //28 !!
#define APP_SIZE            0x3C             //60 - NET length !!       
#define COM_SIZE            0x3C			 //60 - NET length !! 


   // --------------------------------------------------------------
   //   Defs of PIXEL COMPONENTS indexes
   // --------------------------------------------------------------


#define Y_COMPONENT         0
#define U_COMPONENT         1
#define V_COMPONENT         2

   
//
   // ---------------------------------
   //   Coder class definition:
   // --------------------------------- 

 
typedef struct tagZ050_CODER_CONFIG
	{
	  WORD                   Width;
	  WORD                   Height;
	  BYTE                   HSampRatio[8];           // 8 is max components
	  BYTE                   VSampRatio[8];
	  DWORD                  TotalCodeVolume;
	  WORD                   MaxBlockCodeVolume;
	  WORD                   ScaleFactor;
	  WORD                   DRI;
	  char FAR *             QTable;
	  char FAR *             HuffTable;
	  char FAR *             APPString;
	  char FAR *             COMString;
	} Z050_CODER_CONFIG, FAR* LP_Z050_CODER_CONFIG;

 

#if 0
                                       

/*
 *------------------------------------------------------------------------------
 *
 * Class          : Z050_Coder
 *
 * Purpose        : This class defines the 050 coder as an objec. 
 *
 * Data Structure : Board_IO - a pointer to an object of class Z050_IO. This
 *                  class member functions provide the method for read and
 *                  write operations to the 050 coder.
 *
 * Methods        : 1. Z050_Coder - class constructor. Receives as input
 *                     parameter the actual pointer to be stored in
 *                     Board_IO.
 *                  2. 
 *
 *------------------------------------------------------------------------------
 */

                                        
class Z050_Coder       
{

private:

    Z050_IO* Board_IO;

// prototypes of member functions
public:

    Z050_Coder(Z050_IO* Brd_IO);

    ~Z050_Coder();

    void  MakeMarkerSegments(LP_Z050_CODER_CONFIG Cfg);

    DWORD MakeSOFMarker(LP_Z050_CODER_CONFIG Cfg);

    DWORD MakeSOS1Marker();

    DWORD MakeDRIMarker(LP_Z050_CODER_CONFIG Cfg);

    DWORD MakeQTable(char FAR* QTable);

    DWORD MakeHuffTable(char FAR* HuffTable);

    DWORD MakeAPPString(char FAR* APPString);

    DWORD MakeCOMString(char FAR* COMString);

    void  CoderStart();

	int   CoderConfigureCompression(LP_Z050_CODER_CONFIG Cfg);

	int   CoderConfigureExpansion(LP_Z050_CODER_CONFIG Cfg);

	WORD  CoderGetLastScaleFactor();
};
#endif
#endif
