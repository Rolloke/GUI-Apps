#ifndef __TASHAFIRMWARE535_H__
#define __TASHAFIRMWARE535_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CORE_CLK	(300000000)
#define SYSTEM_CLK	(133000000)

void InitBasicMon();
int  InterruptPC(void);
void SetInterruptCallback(void*);
void MessageCallback(void);

#ifdef __cplusplus
  } // extern "C"
#endif

// TIMERx_CONFIG Registers
#define PWM_OUT		0x0001
#define WDTH_CAP	0x0002
#define EXT_CLK		0x0003
#define PULSE_HI	0x0004
#define PERIOD_CNT	0x0008
#define IRQ_ENA		0x0010
#define TIN_SEL		0x0020
#define OUT_DIS		0x0040
#define CLK_SEL		0x0080
#define TOGGLE_HI	0x0100
#define EMU_RUN		0x0200
#define ERR_TYP(x)	((x & 0x03) << 14)

// TIMERx_STATUS Register
#define IRQ0		0x0001
#define IRQ1		0x0002
#define IRQ2		0x0004
#define OVF_ERR0	0x0010
#define OVF_ERR1	0x0020
#define OVF_ERR2	0x0040
#define TIMEN0		0x0100
#define TIMDIS0		0x0200
#define TIMEN1		0x0400
#define TIMDIS1		0x0800
#define TIMEN2		0x1000
#define TIMDIS2		0x2000


//######## Timer ISR ########
BOOL InitTimerISR(int nTimer);
void StopTimerISR();
EX_INTERRUPT_HANDLER(TimerISR);		// timer interrupt handler

//######## Messages ########
BOOL SendMessage(int nMessageID, DWORD dwParm1=0, DWORD dwParm2=0, DWORD dwParm3=0, DWORD dwParm4=0);	
BOOL PostMessage(int nMessageID, DWORD dwParm1=0, DWORD dwParm2=0, DWORD dwParm3=0, DWORD dwParm4=0);	
BOOL ReceiveMessage(int& nMessageID, DWORD& dwParm1, DWORD& dwParm2, DWORD& dwParm3, DWORD& dwParm4);
BOOL HandleOutgoingMessages();
void HandleIncomingMessages();
void Delay(DWORD dwEnd);
BOOL IncludeFrameHeader(int nChannel, DWORD dwSDRAMOffset);

//######## DoRequest/OnConfirm ########
BOOL PollAlarm();
BOOL DoNotifyAlarm(DWORD dwAlarmState);

BOOL DoNotifyInformation(DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0, DWORD dwParam4=0);

BOOL DoReceiveNewCodecData();
BOOL OnConfirmReceiveNewCodecData();

BOOL OnRequestSetBrightness(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetBrightness(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetContrast(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetContrast(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetSaturation(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetSaturation(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetContrast(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetContrast(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetBrightness(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetBrightness(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetSaturation(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetSaturation(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue);
BOOL DoConfirmSetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetMDTreshold(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetMaskTreshold(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetMaskIncrement(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestGetMaskDelay(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmGetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue);

BOOL OnRequestSetPermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwAddr, DWORD dwLen);
BOOL DoConfirmSetPermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwAddr, DWORD dwLen);

BOOL OnRequestChangePermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwPos,  int nValue);
BOOL DoConfirmChangePermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwPos, int nValue);

BOOL OnRequestClearPermanentMask(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmClearPermanentMask(DWORD dwUserData, WORD wSlave);

BOOL OnRequestInvertPermanentMask(DWORD dwUserData, WORD wSlave);
BOOL DoConfirmInvertPermanentMask(DWORD dwUserData, WORD wSlave);

BOOL OnRequestResumeCapture(); 
BOOL DoConfirmResumeCapture(); 

BOOL OnRequestPauseCapture(); 
BOOL DoConfirmPauseCapture(); 

BOOL OnRequestStopCapture(); 
BOOL DoConfirmStopCapture(); 

BOOL OnRequestStartCapture(); 
BOOL DoConfirmStartCapture(); 

BOOL OnRequestScanForCameras(DWORD dwUserData);
BOOL DoConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);

BOOL OnRequestSetRelayState(WORD wRelayID, BOOL bOpenClose);
BOOL DoConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

BOOL OnRequestGetRelayState();
BOOL DoConfirmGetRelayState(DWORD dwRelayStateMask);

BOOL OnRequestGetAlarmState();
BOOL DoConfirmGetAlarmState(DWORD dwAlarmStateMask);

BOOL OnRequestSetAlarmEdge(DWORD dwAlarmEdgeMask);
BOOL DoConfirmSetAlarmEdge(DWORD dwAlarmEdgeMask);

BOOL OnRequestGetAlarmEdge();
BOOL DoConfirmGetAlarmEdge(DWORD dwAlarmEdgeMask);

BOOL OnRequestSetPCIFrameBufferAddress(DWORD dwPCIFrameBufferAddr);
BOOL DoConfirmSetPCIFrameBufferAddress(DWORD dwPCIFrameBufferAddr);

BOOL OnRequestEpldDataTransfer(DWORD dwAddr, DWORD dwLen);
BOOL DoConfirmEpldDataTransfer(DWORD dwAddr, DWORD dwLen);

BOOL OnRequestBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen);
BOOL DoConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen);

BOOL OnRequestWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
BOOL DoConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);

BOOL OnRequestReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
BOOL DoConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);

BOOL OnRequestSetChannel(WORD wSlave, WORD wNewSlave);
BOOL DoConfirmSetChannel(WORD wSlave, WORD wNewSlave);

BOOL DoConfirmInitComplete();
BOOL DoNotifySlaveInitComplete(WORD wSlave);

BOOL OnRequestTermination();
BOOL DoConfirmTermination();

BOOL DoIndicationNewData(DWORD dwSDRAMOffset, int nSPORT);

/////////////////////////////////////////////////////////////////////////
inline void WaitMicro(int nMicroSecond)		// Warteschleife
{
	DWORD dwEndCounter = (CORE_CLK/1000000)*nMicroSecond;
	startc();

	while (getCyclesLo() < dwEndCounter)
		asm("nop;");

	return;
}

#endif //__TASHAFIRMWARE535_H__


