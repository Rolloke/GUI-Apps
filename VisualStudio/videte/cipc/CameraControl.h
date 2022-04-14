/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CameraControl.h $
// ARCHIVE:		$Archive: /Project/CIPC/CameraControl.h $
// CHECKIN:		$Date: 28.08.06 13:14 $
// VERSION:		$Revision: 64 $
// LAST CHANGE:	$Modtime: 28.08.06 13:14 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CameraControl_H
#define __CameraControl_H

/////////////////////////////////////////////////////////////////////////////
// DoRequestGetValue

#define REG_PTZF_CCFUNCTIONS		_T("CCFunctions")
#define REG_PTZF_CCFUNCTIONS_EX		_T("CCFunctionsEx")

/////////////////////////////////////////////////////////////////////////////
// CAVEAT: DO NOT CHANGE
// Values are sent via network between different versions!
#define PTZF_PAN_LEFT         0x00000001
#define PTZF_PAN_RIGHT        0x00000002
#define PTZF_TILT_UP          0x00000004
#define PTZF_TILT_DOWN        0x00000008
#define PTZF_FOCUS_NEAR       0x00000010
#define PTZF_FOCUS_FAR        0x00000020
#define PTZF_IRIS_OPEN        0x00000040
#define PTZF_IRIS_CLOSE       0x00000080
#define PTZF_ZOOM_IN          0x00000100
#define PTZF_ZOOM_OUT         0x00000200
#define PTZF_POS_1            0x00000400
#define PTZF_POS_2            0x00000800
#define PTZF_POS_3            0x00001000
#define PTZF_POS_4            0x00002000
#define PTZF_POS_5            0x00004000
#define PTZF_POS_6            0x00008000
#define PTZF_POS_HOME         0x00010000
#define PTZF_SPEED            0x00020000
#define PTZF_POS_7            0x00040000
#define PTZF_POS_8            0x00080000
#define PTZF_POS_9            0x00100000
#define PTZF_CONFIG           0x00200000
#define PTZF_FOCUS_AUTO       0x00400000
#define PTZF_IRIS_AUTO        0x00800000
#define PTZF_TURN_180         0x01000000
#define PTZF_SET_POS          0x02000000
#define PTZF_ROLL_LEFT        0x04000000
#define PTZF_ROLL_RIGHT       0x08000000
#define PTZF_EXPOSURE_LONGER  0x10000000
#define PTZF_EXPOSURE_SHORTER 0x20000000
#define PTZF_MOVE_DIAGONAL    0x40000000
#define PTZF_EXTENDED		  0x80000000
/////////////////////////////////////////////////////////////////////////////
#define PTZF_DEFAULT		PTZF_PAN_LEFT | PTZF_PAN_RIGHT | PTZF_TILT_UP | PTZF_TILT_DOWN | PTZF_ZOOM_IN | PTZF_ZOOM_OUT
#define PTZF_PAN			PTZF_PAN_LEFT | PTZF_PAN_RIGHT 
#define PTZF_TILT			PTZF_TILT_UP | PTZF_TILT_DOWN
#define PTZF_ZOOM			PTZF_ZOOM_IN | PTZF_ZOOM_OUT
#define PTZF_IRIS			PTZF_IRIS_OPEN | PTZF_IRIS_CLOSE
#define PTZF_FOCUS			PTZF_FOCUS_NEAR | PTZF_FOCUS_FAR
#define PTZF_POS			PTZF_POS_1|PTZF_POS_2|PTZF_POS_3|PTZF_POS_4|PTZF_POS_5|PTZF_POS_6|PTZF_POS_HOME
#define PTZF_POSALL			PTZF_POS_1|PTZF_POS_2|PTZF_POS_3|PTZF_POS_4|PTZF_POS_5|PTZF_POS_6|PTZF_POS_7|PTZF_POS_8|PTZF_POS_9|PTZF_POS_HOME
#define PTZF_ROLL			PTZF_ROLL_LEFT | PTZF_ROLL_RIGHT
#define PTZF_EXPOSURE		PTZF_EXPOSURE_LONGER | PTZF_EXPOSURE_SHORTER

/////////////////////////////////////////////////////////////////////////////
// CAVEAT: DO NOT CHANGE
// Values are sent via network between different versions!
#define PTZF_EX_AUXILIARY_OUT1	0x00000001	// auxiliary out 1
#define PTZF_EX_AUXILIARY_OUT2	0x00000002	// auxiliary out 2
#define PTZF_EX_AUXILIARY_OUT3	0x00000004	// auxiliary out 3
#define PTZF_EX_AUXILIARY_OUT4	0x00000008	// auxiliary out 4
#define PTZF_EX_AUXILIARY_OUT5	0x00000010	// auxiliary out 5
#define PTZF_EX_AUXILIARY_OUT6	0x00000020	// auxiliary out 6
#define PTZF_EX_AUXILIARY_OUT7	0x00000040	// auxiliary out 7
#define PTZF_EX_AUXILIARY_OUT8	0x00000080	// auxiliary out 8
#define PTZF_EX_ERASE_PRESET	0x00000100	// preset positions eraseable
#define PTZF_EX_ZERO_PAN_POS	0x00000200	// zero pan position exists
#define PTZF_EX_SWITCH_ON_OFF	0x00000400	// camera can be switched on or off
#define PTZF_EX_ZOOM_LENS_SPEED 0x00000800	// zoom speed adjustable
#define PTZF_EX_ALARM_INPUTS	0x00001000	// alarm inputs available
#define PTZF_EX_ALARM_ACK		0x00002000	// alarm to be acknowledged
#define PTZF_EX_AUTO_PAN		0x00004000	// automatic object pursuit
#define PTZF_EX_ZONE_SCAN		0x00008000	// 
#define PTZF_EX_MORE_POSITIONS	0x00010000	// provides more than 10 presets
#define PTZF_EX_CONFIG_SET_3_4	0x00020000	// additional up down controls for osd menu
#define PTZF_EX_AUX_OUT_BITS	0x00040000	// auxiliary out is controlled bitwise all outputs together,
											// otherwise it is controlled by index for a single output
#define PTZF_EX_SPEED_READJUST	0x00080000	// protocol can readjust speed when moving
#define PTZF_EX_SPEED_XY		0x00100000	// protocol can use different speed for x and y

/////////////////////////////////////////////////////////////////////////////
#define PTZF_EX_AUXILIARY_OUT	PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_AUXILIARY_OUT3|PTZF_EX_AUXILIARY_OUT4|PTZF_EX_AUXILIARY_OUT5|PTZF_EX_AUXILIARY_OUT6|PTZF_EX_AUXILIARY_OUT7|PTZF_EX_AUXILIARY_OUT8
#define PTZF_EX_WHASH			PTZF_EX_AUXILIARY_OUT1
#define PTZF_EX_WIPE			PTZF_EX_AUXILIARY_OUT2
#define PTZF_EX_LIGHTS			PTZF_EX_AUXILIARY_OUT3

/////////////////////////////////////////////////////////////////////////////
// CAVEAT: DO NOT CHANGE
// Values are sent via network between different versions!
#define PTZF_CONFIG_PANASONIC_ON		0x00000001	// osd menu open
#define PTZF_CONFIG_PANASONIC_OFF		0x00000002	// osd menu close
#define PTZF_CONFIG_PANASONIC_UP		0x00000004	// Tilt up
#define PTZF_CONFIG_PANASONIC_DOWN		0x00000008	// Tilt down
#define PTZF_CONFIG_PANASONIC_LEFT		0x00000010	// Pan left
#define PTZF_CONFIG_PANASONIC_RIGHT		0x00000020	// pan right
#define PTZF_CONFIG_PANASONIC_SET1		0x00000040	// Enter
#define PTZF_CONFIG_PANASONIC_SET2		0x00000080	// Escape
#define PTZF_CONFIG_PANASONIC_RESET		0x00000100	// 
#define PTZF_CONFIG_PANASONIC_ALLRESET	0x00000200	// 
#define PTZF_CONFIG_SET3				0x00000400	// additional up
#define PTZF_CONFIG_SET4				0x00000800	// additional down

/////////////////////////////////////////////////////////////////////////////
// CAVEAT: DO NOT CHANGE
// Values are sent via network between different versions!
enum CameraControlCmd {
	CCC_INVALID=0,
	CCC_PAN_LEFT=1,	
	CCC_PAN_RIGHT=2,	
	CCC_PAN_STOP=3,	
	CCC_TILT_UP=4,	
	CCC_TILT_DOWN=5,	
	CCC_TILT_STOP=6,	
	CCC_FOCUS_NEAR=7,
	CCC_FOCUS_FAR=8,
	CCC_FOCUS_STOP=9,
	CCC_IRIS_OPEN=10,
	CCC_IRIS_CLOSE=11,
	CCC_IRIS_STOP=12,
	CCC_ZOOM_IN=13,
	CCC_ZOOM_OUT=14,
	CCC_ZOOM_STOP=15,
	CCC_POS_1=16,
	CCC_POS_2=17,
	CCC_POS_3=18,
	CCC_POS_4=19,
	CCC_POS_5=20,
	CCC_POS_6=21,
	CCC_POS_HOME=22,
	CCC_POS_7=23,
	CCC_POS_8=24,
	CCC_POS_9=25,
	CCC_CONFIG=26,
	CCC_FOCUS_AUTO=27,
	CCC_IRIS_AUTO=28,
	CCC_TURN_180=29,
	CCC_MOVE_LEFT_UP=30,
	CCC_MOVE_RIGHT_UP=31,
	CCC_MOVE_LEFT_DOWN=32,
	CCC_MOVE_RIGHT_DOWN=33,
	CCC_MOVE_STOP=34,
	CCC_AUXOUT=35,
	CCC_POS_X=36,
};
/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS const _TCHAR *NameOfEnum(CameraControlCmd ccc);
/////////////////////////////////////////////////////////////////////////////
enum CameraControlType
{
	CCT_RS232_TRANSPARENT=-1,
	CCT_UNKNOWN=0,
	CCT_SENSORMATIC_TOUCHTRACKER=1,
	CCT_PANASONIC_WV_RM70=2,
	CCT_ERNITEC_BDR_510=3,
	CCT_ELBEX_97_6=4,
	CCT_PIEPER_KMS_850S=5,
	CCT_VIDEV_EC160=6,
	CCT_RELAY_PTZ=7,
	CCT_CANON_VISCA=8,
	CCT_VCL_CAMERA=9,
	CCT_VCL_MAXCOM=10,
	CCT_FASTRAX=11,
	CCT_BAXALL=12,
	CCT_TOA=13,
	CCT_PELCO_D=14,
	CCT_PELCO_P=15,
	CCT_BBV_RS422=16,
	CCT_SENSORMATIC=17,
	CCT_CBC_ZCNAF27=18,
	CCT_VIDEOTEC=19,
	CCT_BEWATOR=20,
	CCT_JVC_TK_C=21,
	CCT_MERIDIAN=22,
	CCT_IP_CAMERA=23
};
enum QuadControlType
{
	QCT_UNKNOWN = 0,
	QCT_SANTEC_SQV_80 = 1,
	QCT_GANZ_QD_04P = 2,
};
/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS const _TCHAR *NameOfEnum(CameraControlType cct);
AFX_EXT_CLASS const _TCHAR *NameOfEnum(QuadControlType cct);
AFX_EXT_CLASS CameraControlType NameToCameraControlType(const CString& name);
AFX_EXT_CLASS const _TCHAR *NameOfEnumPTZRealName(CameraControlType cct);
AFX_EXT_CLASS const _TCHAR *NameOfEnumQuadRealName(QuadControlType cct);
AFX_EXT_CLASS DWORD GetCameraControlFunctions(CameraControlType cct);
AFX_EXT_CLASS DWORD GetCameraControlFunctionsEx(CameraControlType cct);
AFX_EXT_CLASS BOOL	IsCameraIDRequired(CameraControlType cct);
AFX_EXT_CLASS int	GetLastCameraControlType(BOOL bIdip);
/////////////////////////////////////////////////////////////////////////////
#endif
