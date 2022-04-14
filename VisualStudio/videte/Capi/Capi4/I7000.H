#ifdef __cplusplus
     #define EXPORTS extern "C" __declspec (dllimport)
#else
     #define EXPORTS
#endif


#define    Data5Bit                     5
#define    Data6Bit                     6
#define    Data7Bit                     7
#define    Data8Bit                     8

#define    NonParity                    0
#define    OddParity                    1
#define    EvenParity                   2

#define    OneStopBit                   0
#define    One5StopBit                  1
#define    TwoStopBit                   2

// return code
#define    NoError                      0
#define    FunctionError                1
#define    PortError                    2
#define    BaudRateError                3
#define    DataError                    4
#define    StopError                    5
#define    ParityError                  6
#define    CheckSumError                7
#define    ComPortNotOpen               8
#define    SendThreadCreateError        9
#define    SendCmdError                 10
#define    ReadComStatusError           11
#define    ResultStrCheckError          12
#define    CmdError                     13
#define    TimeOut                      15
#define    ModuleIdError                17
#define    AdChannelError               18
#define    UnderInputRange              19
#define    ExceedInputRange             20
#define    InvalidateCounterNo          21
#define    InvalidateCounterValue       22
#define    InvalidateGateMode           23
#define    InvalidateChannelNo          24


EXPORTS WORD  CALLBACK Open_Com(char cPort, DWORD dwBaudRate, 
              char cData, char cParity, char cStop);
EXPORTS BOOL  CALLBACK Close_Com(char cPort);

EXPORTS WORD  CALLBACK ComPortStatus(char port);
//  1: this port is used now
//  0: this port isn't used now

EXPORTS WORD  CALLBACK Send_Receive_Cmd(char cPort, char szCmd[], 
              char szResult[], WORD wTimeOut, WORD wChksum, WORD *wT);

EXPORTS WORD  CALLBACK Send_Cmd(char cPort, char szCmd[], 
              WORD wTimeout, WORD wChkSum);
EXPORTS WORD  CALLBACK Receive_Cmd(char cPort, char szResult[], 
              WORD wTimeOut, WORD wChkSum, WORD *wT);

EXPORTS WORD  CALLBACK Send_Str(char cPort, char szCmd[], WORD wTimeOut);
EXPORTS WORD  CALLBACK Receive_Str(char cPort, char szResult[], 
              WORD wTimeOut, WORD wLen, WORD *wT);

//EXPORTS WORD  CALLBACK Read_Com_Status(char cPort, char szResult[], 
//              WORD *wT);
//EXPORTS WORD  CALLBACK Send_Str(char cPort, char szCmd[], 
//              WORD wTimeOut, WORD wLenT, WORD wLenR);


//---------------------------------------------------------------------------

EXPORTS short CALLBACK Short_Sub_2(short nA, short nB);
EXPORTS float CALLBACK Float_Sub_2(float fA, float fB);
EXPORTS WORD  CALLBACK Get_Dll_Version(void);


EXPORTS WORD CALLBACK Test(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
// for driver testing
// strcpy(szSendTo7000,"Command String Send to I7000");
// strcpy(szReceiveFrom7000,"Command String Receive From I7000");
// for (i=0; i<4; i++)
//     {
//    w7000[i]++;
//    f7000[i]++;
//    }

EXPORTS WORD CALLBACK HostIsOK(WORD w7000[], float f7000[],
      char szSendTo7000[], char szReceiveFrom7000[]);

/*--- Read the module reset status, $AA5 --------*/
EXPORTS WORD CALLBACK ReadModuleResetStatus(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -----------------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // don't care
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ----------------------------------
//  w7000[5];   // w7000[5]=0: module has not been reset
                    // w7000[5]=1: module has been reset

/*--- To setup host watchdog, ~AA3ETT --------*/
EXPORTS WORD CALLBACK ToSetupHostWatchdog(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -----------------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // don't care
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // 0: Disable host watchdog
                    // 1: Enable host watchdog
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//  w7000[7];   // a time interval for watchdog, unit is 0.1 second,
                // e.x  when w7000[7]=45, the time interval is 4.5 second

/*--- To read host watchdog setup value, ~AA2 --------*/
EXPORTS WORD CALLBACK ToReadHostWatchdog(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -----------------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // don't care
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------------------
//  w7000[5];   // 0: Disable host watchdog
                    // 1: Enable host watchdog
//  w7000[7];   // a time interval for watchdog, unit is 0.1 second,
                // e.x  when w7000[7]=45, the time interval is 4.5 second




/*--- Read the module status about the host watchdog, ~AA0 --------*/
EXPORTS WORD CALLBACK ReadModuleHostWatchdogStatus(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -----------------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // don't care
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ----------------------------------
//  w7000[5];   // w7000[5]=0:  module is OK
                   //  w7000[5]=4:  host watchdog failure

/*--- Reset the module status about the host watchdog, ~AA1 --------*/
EXPORTS WORD CALLBACK ResetModuleHostWatchdogStatus(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -----------------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // don't care
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // don't care
//  w7000[6];   // flag: 0=no save, 1=save send/receive string



/*--Set SafeValue for all DO module(42/43/44/50/60/65/67) ---*/
EXPORTS WORD CALLBACK SetSafeValueForDo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ---------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // module ID: 0x7050/60/67/42/43/44
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // safe value
//  w7000[6];   // flag: 0=no save, 1=save send/receive string


/*--Set PowerOn value for all DO module(42/43/44/50/60/65/67) ---*/
EXPORTS WORD CALLBACK SetPowerOnValueForDo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // module ID: 0x7050/60/67/42/43/44
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // PowerOn value
//  w7000[6];   // flag: 0=no save, 1=save send/receive string

/*--Set SafeValue for all Ao module(21/24), ~AA5 ---*/
EXPORTS WORD CALLBACK SetSafeValueForAo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ---------------------------
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // module ID: 0x7021/0x7024
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // Channel No
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//  f7000[0];   // safe value

/*--Set PowerOn value for all Ao module(21/24), $AA4 ---*/
EXPORTS WORD CALLBACK SetPowerOnValueForAo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // module ID: 0x7021/0x7024
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // PowerOn value
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//  f7000[0];   // PowerOn value


/*-------- 7011/7012/7014 Set PowerOn/Safe value --------*/
EXPORTS WORD CALLBACK SetPowerOnSafeValue(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//  w7000[0];   // RS-232 port number: 1/2/3/4
//  w7000[1];   // module address: 0x00 to 0xFF
//  w7000[2];   // module ID: 0x7011/0x7012/0x7014
//  w7000[3];   // checksum: 0=disable, 1=enable
//  w7000[4];   // TimeOut constant: normal=100
//  w7000[5];   // PowerOn value
//  w7000[6];   // flag: 0=no save, 1=save send/receive string
//  w7000[7];   // safe value

/*--Read PowerOn value for all DO module(42/43/44/50/60/65/67) ---*/
EXPORTS WORD CALLBACK ReadPowerOnValueForDo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ---------------------------
// w7000[0];    // RS-232 port number: 1/2/3/4
// w7000[1];    // module address: 0x00 to 0xFF
// w7000[2];    // module ID: 0x7050/60/65/67/42/43/44
// w7000[3];    // checksum: 0=disable, 1=enable
// w7000[4];    // TimeOut constant: normal=100
// w7000[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------
//   w7000[5]:   PowerOn Value Value


/*--Read SafeValue for all DO module(42/43/44/50/60/65/67) ---*/
EXPORTS WORD CALLBACK ReadSafeValueForDo(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ---------------------------
// w7000[0];    // RS-232 port number: 1/2/3/4
// w7000[1];    // module address: 0x00 to 0xFF
// w7000[2];    // module ID: 0x7050/60/65/67/42/43/44
// w7000[3];    // checksum: 0=disable, 1=enable
// w7000[4];    // TimeOut constant: normal=100
// w7000[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------
//   w7000[5]:   Safe Value Value


/*-------- Read Config Status ---------*/
EXPORTS WORD CALLBACK  ReadConfigStatus(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];   // RS-232 port number: 1/2/3/4
// w7000[1];   // module address: 0x00 to 0xFF
// w7000[2];   // module ID
// w7000[3];   // checksum: 0=disable, 1=enable
// w7000[4];   // TimeOut constant: normal=100
// w7000[5];   // Don't Care
// w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- Output -------------------------------------
// w7000[7]:   module address
// w7000[8]:   module Range Code
// w7000[9]:   module baudrate
// w7000[10]:  module data format


EXPORTS WORD CALLBACK AnalogIn(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7011/12/13/14/16/17/18/33
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // channel number for multi-channel
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // f7000[0]=analog input value

EXPORTS WORD CALLBACK AnalogInFsr(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7011/12/13/14/17/18
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // channel number for multi-channel
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // f7000[0]=analog input value in %FSR format


EXPORTS WORD CALLBACK AnalogInHex(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7011/12/13/14/17/18
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // channel number for multi-channel
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // w7000[7] : analog input value



EXPORTS WORD CALLBACK AnalogIn8(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7017/18
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // f7000[0]=analog input value of channel_0
                         // f7000[1]=analog input value of channel_1
                         // ........................................
                         // f7000[7]=analog input value of channel_7


EXPORTS WORD CALLBACK In8_7017(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module Type: 08/09/0A/0B/0C/0D
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // f7000[0]=analog input value of channel_0
                         // f7000[1]=analog input value of channel_1
                         // ........................................
                         // f7000[7]=analog input value of channel_7


EXPORTS WORD CALLBACK AnalogOut(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];   // RS-232 port number: 1/2/3/4
// w7000[1];   // module address: 0x00 to 0xFF
// w7000[2];   // module ID: 0x7016/21/24
// w7000[3];   // checksum: 0=disable, 1=enable
// w7000[4];   // TimeOut constant: normal=100
// w7000[5];   // Not used if module ID is 7016/7021
                           // Channel No.(0 to 3) if module ID is 7024
// w7000[6];   // flag: 0=no save, 1=save send/receive string
// f7000[0];   // analog output value
//---------------------- output ------------------------------------
                           // void

EXPORTS WORD CALLBACK AnalogOutReadBack(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];  // RS-232 port number: 1/2/3/4
// w7000[1];  // module address: 0x00 to 0xFF
// w7000[2];  // module ID: 0x7021 or 0x7024
// w7000[3];  // checksum: 0=disable, 1=enable
// w7000[4];  // TimeOut constant: normal=100
// w7000[5];  // When 7016: Don't care
              // When 7021/7024
              //   0: command read back ($AA6)
              //   1: when module ID is 0x7021
              //        analog output of current path read back ($AA8)
              //        when module ID is 0x7024
              //        the updating value in a specific Slew rate($AA8)
              //        (see 7024 manual for more detail information)
// w7000[6];  // flag: 0=no save, 1=save send/receive string
// w7000[7];  // Channel No.(0 to 3), if module ID is 7024
              //   Not used,        else
//---------------------- output ------------------------------------
              //   f7000[0]=analog output value read back


EXPORTS WORD CALLBACK DigitalOut(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/60/63/65/66/67/42/43/44/11/12/14/80
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 16-bit digital data to output
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // void

EXPORTS WORD CALLBACK DigitalBitOut(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/60/63/65/66/67/42/43/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // don't care
// w7000[6];             // flag: 0=no save, 1=save send/receive string
// w7000[7];             // which channel to output
// w7000[8];             // data to output (0 or 1)
//---------------------- output ------------------------------------
                         // void

EXPORTS WORD CALLBACK DigitalOut_7016(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7016
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 2-bit digital data to output
// w7000[6];             // flag: 0=no save, 1=save send/receive string
// w7000[7];             // 0: Bit0, Bit1 output
                         // 1: Bit2, Bit3 output
//---------------------- output ------------------------------------
                         // void

EXPORTS WORD CALLBACK DigitalOutReadBack(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/60/67/42/43/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // w7000[5]=digital output data read back


EXPORTS WORD CALLBACK DigitalIn(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/52/53/60/63/65/41/44/11/12/14/16
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // w7000[5]=digital input data

EXPORTS WORD CALLBACK DigitalInCounterRead(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/52/53/60/63/65/41/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Channel Number
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// w7000[7]=Counter Value of Channel N's Digital Input


EXPORTS WORD CALLBACK ClearDigitalInCounter(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/52/53/60/63/65/41/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Channel Number
// w7000[6];             // flag: 0=no save, 1=save send/receive string


EXPORTS WORD CALLBACK DigitalInLatch(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/52/53/60/63/65/41/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Latch Low,   1: Latch High
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// w7000[7];             // latch value


EXPORTS WORD CALLBACK ClearDigitalInLatch(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7050/52/53/60/63/65/41/44
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't care
// w7000[6];             // flag: 0=no save, 1=save send/receive string

/*-------- 7011 Thermocouple Open Detection ---------*/
EXPORTS WORD CALLBACK ThermocoupleOpen_7011(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//w7000[5]   0: Close     1: Open


/*----- 7011/7012/7014 Set Power On and Safe value ---------*/
EXPORTS WORD CALLBACK SetPowerOnAndSafeValue(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   // 0xPPSS:  PP=Power On value   SS=safe value
              //              00: DO0 -> Off   DO1 -> Off
              //              01: DO0 -> On    DO1 -> Off
              //              02: DO0 -> Off   DO1 -> On
              //              03: DO0 -> On    DO1 -> On
//w7000[6];   // flag: 0=no save, 1=save send/receive string



/*----- 7011/7012/7014 Enable Alarm ---------*/
EXPORTS WORD CALLBACK EnableAlarm(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   // 0: Momentary,  1: Latch
//w7000[6];   // flag: 0=no save, 1=save send/receive string


/*----- 7011/7012/7014 Disable Alarm ---------*/
EXPORTS WORD CALLBACK DisableAlarm(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011/0x7012/0x7014
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   don't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string


/*-------- 7011/7012/7014/7016 Clear Latch Alarm ---------*/
EXPORTS WORD CALLBACK ClearLatchAlarm(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   don't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string


/*--- 7011/7012/7014/7016 Set Hi/Lo Alarm Limit Value ------*/
/*******************************************************/
/* please get the module's range code by calling       */
/* ReadConfigStatus(.......) function before           */
/* calling SetAlarmLimitValue(......).                 */
/*******************************************************/
EXPORTS WORD CALLBACK SetAlarmLimitValue(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   // 0: Lo Alarm     1: Hi Alarm
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//w7000[7];   // range code of this module
//f7000[0]:   // Alarm Limit Value

/*-------- 7011/7012/7014/7016 Read Hi/Lo Alarm Limit Value ---------*/
EXPORTS WORD CALLBACK ReadAlarmLimitValue(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ----------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7011
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   // 0: Lo Alarm     1: Hi Alarm
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ---------------------------------
//f7000[0]:   // Alarm Limit Value

/*---- Read Alarm State and Digital Output State ---------*/
EXPORTS WORD CALLBACK ReadOutputAlarmState(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // Don't Care
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//               Alarm State:
// w7000(7):0       Disable
// w7000(7):1       Momentary
// w7000(7):2       Latch
//               Digital Output
// w7000(8)    -> a decimal number to represent the digital output status


/*-------- 7011/7012/7014 Read Event Counter --------*/
EXPORTS WORD CALLBACK ReadEventCounter(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // Don't Care
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//w7000[7];   // The Event Counter Value

/*-------- 7011/7012/7014/7016 Clear Event Counter --------*/
EXPORTS WORD CALLBACK ClearEventCounter(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // Don't Care
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Set LED channel Display for 7033 --------*/
EXPORTS WORD CALLBACK SetLedDisplay_7033(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // set display channel for 7033, 0<=channel<=2
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Get LED channel Display for 7033 --------*/
EXPORTS WORD CALLBACK GetLedDisplay_7033(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output -----------------------------------
//w7000[5];   // which channel display

/**********************************************************/
/*-------- Set LED channel Display for 7016, 7033 --------*/
EXPORTS WORD CALLBACK SetLedDisplay(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // set display channel for 7013, 7033
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Get LED channel Display for 7016, 7033 --------*/
EXPORTS WORD CALLBACK GetLedDisplay(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output -----------------------------------
//w7000[5];   // which channel display


/*******************************************************/
/* please get the module's range code by calling       */
/* ReadConfigStatus(.......) function before           */
/* calling SetupLinearMapping(......).                 */
/*-------- setup linear mapping for 7014/7016 ---------*/
/*  making a linear mapping from [a,b] to [c,d]        */
/*  where:  f7000[0]=a,  f7000[1]=b                    */
/*          f7000[2]=c,  f7000[3]=d                    */
/*******************************************************/
EXPORTS WORD CALLBACK SetupLinearMapping(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   // do't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//w7000[7];   // range code of this module
//f7000[0]:   // source low value,  a
//f7000[1];   // source high value, b
//f7000[2];   // target low value,  c
//f7000[3];   // target high value, d

/*----- Enable linear mapping for 7014/7016 ---------*/
EXPORTS WORD CALLBACK EnableLinearMapping(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   don't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*----- Disable linear mapping for 7014/7016 ---------*/
EXPORTS WORD CALLBACK DisableLinearMapping(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[5];   don't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*----- Reading Linear Mapping Status for 7014/7016 -----*/
EXPORTS WORD CALLBACK ReadLinearMappingStatus(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//w7000[5];   // 0: linear mapping is disable
              // 1: linear mapping is enable

/*----- Reading Source Value of Linear Mapping for 7014/7016 -----*/
EXPORTS WORD CALLBACK ReadSourceValueOfLM(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//f7000[0];   // Low Source Value
//f7000[1];   // High Source Value

/*----- Reading Target Value of Linear Mapping for 7014/7016 -----*/
EXPORTS WORD CALLBACK ReadTargetValueOfLM(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7014, 0x7016
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100  , 0.1 second
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//f7000[0];   // Low Target Value
//f7000[1];   // High Target Value

/**************************************************************************/
EXPORTS WORD CALLBACK CounterIn_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Counter 0,  1: Counter 1
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // w7000[7]  Hi-Word of counter value
                         // w7000[8]  Lo-Word of counter value


EXPORTS WORD CALLBACK ReadCounterMaxValue_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Counter 0,  1: Counter 1
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // w7000[7]  Hi-Word of counter value
                         // w7000[8]  Lo-Word of counter value


EXPORTS WORD CALLBACK SetCounterMaxValue_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[], double MaxValue);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't Care
// w7000[6];             // flag: 0=no save, 1=save send/receive string
// MaxValue    Setting the Counter's Max Value


EXPORTS WORD CALLBACK ReadAlarmLimitValue_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Counter 0,  1: Counter 1
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// w7000[7]  Hi-Word of counter value
// w7000[8]  Lo-Word of counter value


EXPORTS WORD CALLBACK SetAlarmLimitValue_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[], double AlarmValue);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // when in 7080 alarm mode(mode 0)
//                                      0:   To set Counter 0 alarm value
//                              1:   To set Counter 1 alarm value
//                               // when in 7080D alarm mode(mode 0)
//                                      0:   To set Counter 0 high alarm value
//                                      1:   To set Counter 0 high-high alarm value
// w7000[6];             // flag: 0=no save, 1=save send/receive string


EXPORTS WORD CALLBACK EnableCounterAlarm_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: counter 0 enable, 1: counter 1 enable
// w7000[6];             // flag: 0=no save, 1=save send/receive string

EXPORTS WORD CALLBACK DisableCounterAlarm_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't care
// w7000[6];             // flag: 0=no save, 1=save send/receive string


EXPORTS WORD CALLBACK EnableCounterAlarm_7080D(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: momentary, 1: latch
// w7000[6];             // flag: 0=no save, 1=save send/receive string

EXPORTS WORD CALLBACK DisableCounterAlarm_7080D(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't care
// w7000[6];             // flag: 0=no save, 1=save send/receive string


EXPORTS WORD CALLBACK ReadCounterStatus_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Counter 0,  1: Counter 1
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// w7000[7]  0: Counting   1: Not Counting

EXPORTS WORD CALLBACK ClearCounter_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // 0: Counter 0,  1: Counter 1
// w7000[6];             // flag: 0=no save, 1=save send/receive string


EXPORTS WORD CALLBACK ReadOutputAlarmState_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't Care
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
   // w7000(7):0  for 7080 ->  Counter0:Disable   Counter1:Disable
   // w7000(7):1               Counter0:Disable   Counter1:Enable
   // w7000(7):2               Counter0:Enable    Counter1:Disable
   // w7000(7):3               Counter0:Enable    Counter1:Enable
   // w7000(7):0  for 7080D ->  Counter0:Disable
   // w7000(7):1                Counter0:Momentary
   // w7000(7):2                Counter0:Latch
   // w7000(8):0    Bit:1 Disable  Bit 0: Disable
   // w7000(8):1    Bit:1 Disable  Bit 0: Enable
   // w7000(8):2    Bit:1 Enable   Bit 0: Disable
   // w7000(8):3    Bit:1 Enable   Bit 0: Enable


EXPORTS WORD CALLBACK SetInputSignalMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Counter 0's and 1's signal mode
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- Input -------------------------------------
                    // w7000(5):0    Counter:0 TTL      Counter:0  TTL
                    // w7000(5):1    Counter:1 Photo    Counter:1  Photo
                    // w7000(5):2    Counter:0 TTL      Counter:1  Photo
                    // w7000(5):3    Counter:1 Photo    Counter:0  TTL


EXPORTS WORD CALLBACK ReadInputSignalMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
// w7000[0];             // RS-232 port number: 1/2/3/4
// w7000[1];             // module address: 0x00 to 0xFF
// w7000[2];             // module ID: 0x7080
// w7000[3];             // checksum: 0=disable, 1=enable
// w7000[4];             // TimeOut constant: normal=100
// w7000[5];             // Don't Care
// w7000[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                    // w7000(7):0    Counter:0 TTL      Counter:0  TTL
                    // w7000(7):1    Counter:1 Photo    Counter:1  Photo
                    // w7000(7):2    Counter:0 TTL      Counter:1  Photo
                    // w7000(7):3    Counter:1 Photo    Counter:0  TTL


/* ------------ 7080 Preset Counter 0/1 Counter Value ---------------- */
EXPORTS WORD CALLBACK PresetCounterValue_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[], double PresetValue);

/* ------------ 7080 Read Preset Counter 0/1 Counter Value ------------- */
EXPORTS WORD CALLBACK ReadPresetCounterValue_7080(WORD w7000[], float f7000[],
                            char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- output ------------------------------------
                      // w7000[7]= Hi-Word of Preset Counter Value
                      // w7000[8]= Lo-Word of Preset Counter Value

/*-------- 7080 Start Counting ---------*/
EXPORTS WORD CALLBACK StartCounting_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Counter 0   1: Counter 1
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//w7000[7];   // 0: Stop Counting  1: Start Counting


/*-------- Set 7080 module mode  ---------*/
EXPORTS WORD CALLBACK SetModuleMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: 7080 mode   1: 7080D mode
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Read 7080 module mode  ---------*/
EXPORTS WORD CALLBACK ReadModuleMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // don't care
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//------------------ output ---------------------
//w7000[5]= 0: 7080 mode,  1: 7080D mode


/*-------- 7080 Set Level Volt ---------*/
EXPORTS WORD CALLBACK SetLevelVolt_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Low Level,  1: High Level
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//f7000[0];   // Level Voltage


/*-------- 7080 Read Level Volt ---------*/
EXPORTS WORD CALLBACK ReadLevelVolt_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Low Level,  1: High Level
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output ----------------------------------
//f7000[0];        // Level Voltage


/* -------------- 7080 Set Min Signal Width ------------------------- */
EXPORTS WORD CALLBACK SetMinSignalWidth_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[], long MinWidth);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Low Level,  1: High Level
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//MinWidth    // the Input Signal Min Width

/* -------------- 7080 Read Min Signal Width ------------------------- */
EXPORTS WORD CALLBACK ReadMinSignalWidth_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ----------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Low Level,  1: High Level
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- Output ---------------------------------
//w7000[7]    // the Reading Input Signal Min Width


/* -------------- 7080 Set Gate Mode ------------------------- */
EXPORTS WORD CALLBACK SetGateMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[5];   // 0: Low,  1: High,  2: None
//w7000[6];   // flag: 0=no save, 1=save send/receive string

/* -------------- 7080 Read Gate Mode ------------------------- */
EXPORTS WORD CALLBACK ReadGateMode_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input ---------------------------------
//w7000[0];   // RS-232 port number: 1/2/3/4
//w7000[1];   // module address: 0x00 to 0xFF
//w7000[2];   // module ID: 0x7080
//w7000[3];   // checksum: 0=disable, 1=enable
//w7000[4];   // TimeOut constant: normal=100
//w7000[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output --------------------------------
//w7000[5];   // 0: Low,  1: High,  2: None


/*-------- 7080 Send data to LED  ---------*/
EXPORTS WORD CALLBACK DataToLED_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//---------------------- input -------------------------------------
//wPort=(char)w7000[0];   // RS-232 port number: 1/2/3/4
//wAddr=w7000[1];         // module address: 0x00 to 0xFF
//wID=w7000[2];           // module ID: 0x7080
//wCheckSum=w7000[3];     // checksum: 0=disable, 1=enable
//wTimeOut=w7000[4];      // TimeOut constant: normal=100
//wCounter=w7000[5];      // Don't Care
//wFlag=(char)w7000[6];   // flag: 0=no save, 1=save send/receive string
//fOutValue=f7000[0];     // Data to LED


/*-------- Set configuration for 7080 ---------*/
EXPORTS WORD CALLBACK SetConfiguration_7080(WORD w7000[], float f7000[],
        char szSendTo7000[], char szReceiveFrom7000[]);
//*** Notice: if you change the Baudrate or Checksum, please short the INIT*(pin6) to GND(pin10)
//---------------------- input -------------------------------------
//wPort=(char)w7000[0];   // RS-232 port number: 1/2/3/4
//wAddrOld=w7000[1];      // module original address: 0x00 to 0xFF
//wID=w7000[2];           // module ID: 0x7080
//wCheckSumOld=w7000[3];          // module original checksum: 0=disable, 1=enable
//wTimeOut=w7000[4];      // TimeOut constant: normal=100
//wFreqGateTime=w7000[5]; // desired frequency gate time:
                          //      0: 0.1 second
                          //      1: 1.0 second
                          // Don't care w7000[5],if set the module into Counter mode
//wFlag=(char)w7000[6];   // flag: 0=no save, 1=save send/receive string
//wAddrNew=w7000[7];      // desired new address
//wType=w7000[8];         // desired Type->  0:Counter mode
                          //                 1:Frequency mode
//wBaudrate=w7000[9];     // desired Baudrate:
                          //    3: 1200 BPS    4:  2400 BPS
                          //    5: 4800 BPS    6:  9600 BPS
                          //    7: 19200 BPS   8:  38400 BPS
                          //    9: 57600 BPS   10: 115200 BPS
//wCheckSumNew=w7000[10]; // desired Checksum Address

