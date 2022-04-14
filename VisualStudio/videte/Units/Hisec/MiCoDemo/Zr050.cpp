

/*************************************************************************
 *
 *   ZR050.CPP
 *
 *   050 coder functions definitions
 *
 *
 *   Copyright (c) 1993 Zoran Corporation.  All Rights Reserved.
 *
 *
 *************************************************************************/

#include <windows.h>

#include "zr050_io.h"
#include "zr050.h"

//
   // --------------------------------------------------------------
   //   DEFINITIONS OF QUANTITIES FOR BASELINE JPEG. This section can    
   //   be modified if another compression scheme is needed.
   // --------------------------------------------------------------

#define NO_OF_COMPONENTS          0x3                     //Y,U,V
#define BASELINE_PRECISION        0x8                     // ???
#define NO_OF_SCAN1_COMPONENTS    0x3                     //Y,U,V



// assignment of Q and H tables to components.
// max NO_OF_COMPONENTS is 8.

static BYTE QTid[8] = {0, 1, 1, 0, 0, 0, 0, 0};

static BYTE SCAN1_HDCid[8] = {0, 1, 1, 0, 0, 0, 0, 0};
static BYTE SCAN1_HACid[8] = {0, 1, 1, 0, 0, 0, 0, 0};


static DWORD SOFMarkerSize;
static DWORD SOSMarkerSize;
static DWORD APPMarkerSize;
static DWORD COMMarkerSize;
static DWORD DRIMarkerSize;
static DWORD DQTMarkerSize;
static DWORD DHTMarkerSize;
static DWORD DNLMarkerSize;
static DWORD DQTIMarkerSize;
static DWORD DHTIMarkerSize;

//*********************************************************************************************
//          class Constructor
//             Receives as parameter pointer to the CLASS which defines
//             the IO SCHEME for read/write to the coder                 
//*********************************************************************************************
Z050_Coder::Z050_Coder(Z050_IO* Brd_IO)
{
	Board_IO = Brd_IO;
}



//*********************************************************************************************
//          class Destructor
//             
//*********************************************************************************************
Z050_Coder::~Z050_Coder()
{
	return;
}


//*********************************************************************************************
//          MakeSOFMarker    
//             Create the SOF marker segment for a given configuration
//             of compression/expansion  
//*********************************************************************************************
DWORD Z050_Coder::MakeSOFMarker(LP_Z050_CODER_CONFIG Cfg)
{
	WORD Org = SOF_INDX;

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, SOF0_MARKER);	                // SOF0

	Board_IO->Write050(Org+0x2, 0);                             // len < 256
	Board_IO->Write050(Org+0x3, (BYTE)(0x8 + (NO_OF_COMPONENTS * 3)));
	DWORD Mlen = (DWORD)(0x8 + (NO_OF_COMPONENTS * 3) +2);

	Board_IO->Write050(Org+0x4, BASELINE_PRECISION);            // P [8]

	Board_IO->Write050(Org+0x5, (BYTE)(Cfg->Height / 0x100));   // Y(h)
	Board_IO->Write050(Org+0x6, (BYTE)(Cfg->Height % 0x100));   // Y(l)
	Board_IO->Write050(Org+0x7, (BYTE)(Cfg->Width / 0x100));	// X(h)
	Board_IO->Write050(Org+0x8, (BYTE)(Cfg->Width % 0x100));	// X(l)

	Board_IO->Write050(Org+9, NO_OF_COMPONENTS);   				// YUV - 3 components

	for (int j=0; j<NO_OF_COMPONENTS; j++)
	{																  
		Board_IO->Write050(Org+0xA+(j*3), j);					     // C1 - id 0
		Board_IO->Write050(Org+0xB+(j*3),(Cfg->HSampRatio[j] * 0x10) +
										  Cfg->VSampRatio[j]);        // H,V 
		Board_IO->Write050(Org+0xC+(j*3), QTid[j]);					  // Qtbl
	}
	return(Mlen);
};


//*********************************************************************************************
//          MakeSOS1Marker    
//             Create the SOS1 marker segment for a given configuration
//             of compression/expansion  
//*********************************************************************************************
DWORD Z050_Coder::MakeSOS1Marker()
{
	WORD Org = SOS1_INDX;

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, SOS_MARKER);	                // SOS

	Board_IO->Write050(Org+0x2, 0);                             // len < 256
	BYTE Len = (BYTE)(0x3 + (NO_OF_SCAN1_COMPONENTS * 2) + 3);
	Board_IO->Write050(Org+0x3, Len);
	DWORD Mlen = (DWORD)(Len + 2);

	Board_IO->Write050(Org+4, NO_OF_SCAN1_COMPONENTS);		     // YUV - 3 components

	for (int j=0; j<NO_OF_SCAN1_COMPONENTS; j++)
	{
		Board_IO->Write050(Org+0x5+(j*2), j);					 // C1 - id 0
		Board_IO->Write050(Org+0x6+(j*2),(SCAN1_HDCid[j] * 0x10) +
										  SCAN1_HACid[j]);       // AC, DC tbls
	}

	Board_IO->Write050((Org+ (WORD)Len - 1), 0x00);              //00
	Board_IO->Write050((Org+ (WORD)Len), 0x3F);                  //3F
	Board_IO->Write050((Org+ (WORD)Len + 1), 0x00);              //00

	return(Mlen);
};


//*********************************************************************************************
//          MakeDRIMarker    
//             Create the DRI marker segment for a given configuration
//             of compression/expansion  
//*********************************************************************************************
DWORD Z050_Coder::MakeDRIMarker(LP_Z050_CODER_CONFIG Cfg)
{
	WORD Org = DRI_INDX;

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, DRI_MARKER);	            // DRI

	Board_IO->Write050(Org+0x2, 0);                         // len < 256
	Board_IO->Write050(Org+0x3, 4);							// fixed len

	Board_IO->Write050(Org+0x4, (BYTE)(Cfg->DRI / 0x100));	// RI(h)
	Board_IO->Write050(Org+0x5, (BYTE)(Cfg->DRI % 0x100));	// RI(l)

	return(6);												// fixed len
};


//*********************************************************************************************
//          MakeQTables    
//             The Q tables are given as a pointer to an array of 130   
//             chars. The array contains two contiguous Q tables including
//             the Pq,Tq byte before each table.
//             The organization of the Q tables array is expected to be
//             as follows:
//             [0]      - Pq,Tq       Precision and Identifier of first table
//             [1:64]   - Qk          64 elements of first table
//             [65]     - Pq,Tq       Precision and Identifier of second table
//             [66:129] - Qk          64 elements of second table
//*********************************************************************************************
DWORD Z050_Coder::MakeQTable(char FAR* QTable)
{
	WORD Org = DQT_INDX;

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, DQT_MARKER);	                 // DQT

	WORD Len = ((QTABLE_SIZE + 1) * 2) + 2;
	DWORD Mlen = (DWORD)(Len + 2);

	Board_IO->Write050(Org+0x2, (BYTE)(Len / 0x100));            
	Board_IO->Write050(Org+0x3, (BYTE)(Len % 0x100));

	for (int j=0; j<(int)((QTABLE_SIZE + 1) * 2); j++)
	{
		Board_IO->Write050(Org+0x4+j, (BYTE)QTable[j]);				 
	}

	return(Mlen);
};

//*********************************************************************************************
//          MakeHuffTables    
//             The Huff tables are given as a pointer to an array of    
//             420 chars. The array is expected to contain two AC and
//             two DC tables including the ID byte for each table.
//             The organization of each table is expected to be as follows:
//             DC table: 
//             [0]      - 0,Th        DC indicator (0) and Identifier of
//                                    the DC table
//             [1:28]   - DCk         28 elements of DC table
//             AC table: 
//             [0]      - 1,Th        AC indicator (1) and Identifier of
//                                    the AC table
//             [1:178]  - ACk         178 elements of AC table
//             The tables can come in any order DC,DC,AC,AC or AC,DC,AC,DC
//             or whatever.
//*********************************************************************************************
DWORD Z050_Coder::MakeHuffTable(char FAR* HuffTable)
{
	WORD Org = DHT_INDX;

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, DHT_MARKER);	                 // DHT

	WORD Len = ((HUFF_AC_SIZE + 1) * 2) + ((HUFF_DC_SIZE + 1) * 2) + 2;
	DWORD Mlen = (DWORD)(Len + 2);

	Board_IO->Write050(Org+0x2, (BYTE)(Len / 0x100));            
	Board_IO->Write050(Org+0x3, (BYTE)(Len % 0x100));

	for (int j=0; j<(int)(((HUFF_AC_SIZE + 1) + (HUFF_DC_SIZE + 1)) * 2); j++)
	{
		Board_IO->Write050(Org+0x4+j, (BYTE)HuffTable[j]);				 
	}

	return(Mlen);
};


//*********************************************************************************************
//          MakeAPPString    
//             The APP string is given as a pointer to an array of    
//             64 chars. Only 64 chars are supported.
//*********************************************************************************************
DWORD Z050_Coder::MakeAPPString(char FAR* APPString)
{
	WORD Org = APP_INDX;

	if (APPString == NULL)
		return(0);

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, APP_MARKER);	                 // APP

	Board_IO->Write050(Org+0x2, 0x0);            
	Board_IO->Write050(Org+0x3, (BYTE)(APP_SIZE + 2));
	DWORD Mlen = (DWORD)(APP_SIZE + 4);

	for (int j=0; j<APP_SIZE; j++)
	{
		Board_IO->Write050(Org+0x4+j, (BYTE)APPString[j]);				 
	}

	return(Mlen);
};

//*********************************************************************************************
//          MakeCOMString    
//             The COM string is given as a pointer to an array of    
//             64 chars. Only 64 chars are supported.
//*********************************************************************************************
DWORD Z050_Coder::MakeCOMString(char FAR* COMString)
{
	WORD Org = COM_INDX;

	if (COMString == NULL)
		return(0);

	Board_IO->Write050(Org, 0xFF);
	Board_IO->Write050(Org+0x1, COM_MARKER);	                 // COM

	Board_IO->Write050(Org+0x2, 0x0);            
	Board_IO->Write050(Org+0x3, (BYTE)(COM_SIZE + 2));
	DWORD Mlen = (DWORD)(COM_SIZE + 4);
	for (int j=0; j<COM_SIZE; j++)
	{
		Board_IO->Write050(Org+0x4+j, (BYTE)COMString[j]);				 
	}

	return(Mlen);
};

//*********************************************************************************************
//          MakeMarkerSegments
//             Create the marker segments for a given configuration  
//             of compression/expansion  
//*********************************************************************************************
void Z050_Coder::MakeMarkerSegments(LP_Z050_CODER_CONFIG Cfg)
{
	SOFMarkerSize = MakeSOFMarker(Cfg);
	SOSMarkerSize = MakeSOS1Marker();
	DRIMarkerSize = MakeDRIMarker(Cfg);
	DQTMarkerSize = MakeQTable(Cfg->QTable);
	DHTMarkerSize = MakeHuffTable(Cfg->HuffTable);
	APPMarkerSize = MakeAPPString(Cfg->APPString);
	COMMarkerSize = MakeCOMString(Cfg->COMString);
	DHTIMarkerSize = 0;
	DQTIMarkerSize = 0;
	DNLMarkerSize = 0;
};


//*********************************************************************************************
//          CoderStart
//             Write 0 (any data would do) to the coder's GO register
//             to start coder operation  
//*********************************************************************************************
void Z050_Coder::CoderStart()
{
	Board_IO->Write050(GO, 0);
};

//*********************************************************************************************
//          CoderConfigureCompression
//             Configure the coder's internal registers and marker   
//             segments to BASELINE JPEG compression
//*********************************************************************************************
int  Z050_Coder::CoderConfigureCompression(LP_Z050_CODER_CONFIG Cfg)
{
	BYTE Tmp;

	Tmp = MSTR | NO_DMA | CFIS_1_CLK | NO_BELE;
	Board_IO->Write050(HARDWARE, Tmp);

	Tmp = COMP | NO_ATP | NO_PASS2 | TLM | NO_DCONLY | NO_BRC;  
	Board_IO->Write050(MODE, Tmp);

	Tmp = NSCN_1 | NO_OVF;
	Board_IO->Write050(OPTIONS, Tmp);

	Tmp = (BYTE) (Cfg->MaxBlockCodeVolume / 2);
	Board_IO->Write050(MBCV_050, Tmp);

	Tmp = 0;
	Board_IO->Write050(INT_REQ_0, Tmp);         // no interrupts on markers

	Tmp = 3;                                    // two lsb must be '1'
	Board_IO->Write050(INT_REQ_1, Tmp);         // NO intrpt enabled

	WORD WTmp = Cfg->ScaleFactor;
	Board_IO->Write050(SF_HI, (BYTE)(WTmp / 0x100));
	Board_IO->Write050(SF_LO, (BYTE)(WTmp % 0x100));

	Board_IO->Write050(AF_HI, 0xFF);
	Board_IO->Write050(AF_M, 0xFF);
	Board_IO->Write050(AF_LO, 0xFF);

	MakeMarkerSegments(Cfg);					// sets the MarkerSize variables

	Tmp = APP_OFF | COM_OFF | DRI_OFF | DQT_OFF | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_ON;
	Board_IO->Write050(MARKERS_EN, Tmp);

	CoderStart();								// start the coder for
												// Huff table preload
                                        
	DWORD Dly = 0;
	while ( !(Board_IO->Check050End( )) ) 
	{											// wait for TLM to end
		if ((Dly++) > 100000)
			return (0);
												// HERE: insert TIMEOUT checking
	}

	// HERE the 050 is in IDLE mode like after a RESET. Waits for GO.

	Tmp = COMP | NO_ATP | PASS2 | NO_TLM | NO_DCONLY | BRC;  
	Board_IO->Write050(MODE, Tmp);

//	Tmp = APP_ON | COM_OFF | DRI_OFF  | DQT_ON | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_OFF;
	Tmp = APP_ON | COM_OFF | DRI_OFF  | DQT_ON | DHT_ON | DNL_OFF | DQTI_OFF | DHTI_OFF;
	Board_IO->Write050(MARKERS_EN, Tmp);

	DWORD TotalMarkers = SOFMarkerSize + SOSMarkerSize + 4;
								  // SOF, SOS, SOI, EOI allways exist
	if (Tmp & APP_MASK) TotalMarkers += APPMarkerSize;
	if (Tmp & COM_MASK) TotalMarkers += COMMarkerSize;
	if (Tmp & DRI_MASK) TotalMarkers += DRIMarkerSize;
	if (Tmp & DQT_MASK) TotalMarkers += DQTMarkerSize;
	if (Tmp & DHT_MASK) TotalMarkers += DHTMarkerSize;
	if (Tmp & DNL_MASK) TotalMarkers += DNLMarkerSize;
	if (Tmp & DQTI_MASK) TotalMarkers += DQTIMarkerSize;
	if (Tmp & DHTI_MASK) TotalMarkers += DHTIMarkerSize;

	DWORD DTmp = Cfg->TotalCodeVolume - TotalMarkers;      // in BYTES !
	DTmp *= 8;                                             // in BITS  !

	WTmp = (WORD)(DTmp / 0x10000);
	Board_IO->Write050(TCV_NET_HI, (BYTE)(WTmp / 0x100));
	Board_IO->Write050(TCV_NET_MH, (BYTE)(WTmp % 0x100));

	WTmp = (WORD)(DTmp % 0x10000);
	Board_IO->Write050(TCV_NET_ML, (BYTE)(WTmp / 0x100));
	Board_IO->Write050(TCV_NET_LO, (BYTE)(WTmp % 0x100));

	DTmp -= (DTmp / 128);		              // minus STUFFING (SHMUEL)
	DTmp -= ((DTmp * 5) / 64);                // minus EOB (SHMUEL)

	WTmp = (WORD)(DTmp / 0x10000);
	Board_IO->Write050(TCV_DATA_HI, (BYTE)(WTmp / 0x100));    
	Board_IO->Write050(TCV_DATA_MH, (BYTE)(WTmp % 0x100));

	WTmp = (WORD)(DTmp % 0x10000);
	Board_IO->Write050(TCV_DATA_ML, (BYTE)(WTmp / 0x100));
	Board_IO->Write050(TCV_DATA_LO, (BYTE)(WTmp % 0x100));

	return 1;
};

//*********************************************************************************************
//          CoderConfigureExpansion   
//             Configure the coder's internal registers and marker   
//             segments to BASELINE JPEG expansion  
//*********************************************************************************************
int  Z050_Coder::CoderConfigureExpansion(LP_Z050_CODER_CONFIG Cfg)
{
	BYTE Tmp;

	Tmp = NO_BSWD | MSTR | NO_DMA | CFIS_2_CLK | NO_BELE;
	Board_IO->Write050(HARDWARE, Tmp);

	Tmp = NO_COMP | TLM;
	Board_IO->Write050(MODE, Tmp);				// Table Load Mode

	Tmp = 0;
	Board_IO->Write050(INT_REQ_0, Tmp);         // no interrupts on markers

	Tmp = 3;				                    // two lsb must be '1'
	Board_IO->Write050(INT_REQ_1, Tmp);         // NO intrpt enabled

	MakeHuffTable(Cfg->HuffTable);	            // only DHT relevant !!

	Tmp = DHTI_ON;
	Board_IO->Write050(MARKERS_EN, Tmp);

	CoderStart();								// start the coder for
												// Huff table preload
                                
	DWORD Dly = 0;
	while ( !(Board_IO->Check050End( )) ) 
	{											// wait for TLM to end
		if ((Dly++) > 100000)
			return (0);
												// HERE: insert TIMEOUT checking
	}

	// HERE the 050 is in IDLE mode like after a RESET. Waits for GO.
                                      
	Tmp = NO_COMP;
	Board_IO->Write050(MODE, Tmp);                // expansion 

	Tmp = APP_OFF | COM_OFF;                      // don't accept APP, COM   
	Board_IO->Write050(MARKERS_EN, Tmp);		  // into markers seg.

	return 1;
};

//*********************************************************************************************
//          CoderGetLastScaleFactor   
//             Return the Scale Factor used to compress the last frame
//*********************************************************************************************
WORD  Z050_Coder::CoderGetLastScaleFactor()
{
	BYTE LowSF  = Board_IO->Read050(SF_LO);
	BYTE HighSF = Board_IO->Read050(SF_HI);

	return (((WORD)HighSF * 0x100) + LowSF);
}

