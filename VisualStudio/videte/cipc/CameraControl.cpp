/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CameraControl.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CameraControl.cpp $
// CHECKIN:		$Date: 29.08.06 16:41 $
// VERSION:		$Revision: 54 $
// LAST CHANGE:	$Modtime: 29.08.06 16:36 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"
#include "CameraControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/*{CameraControlCmd}
NYD

 {CameraControlType}, {NameOfEnum(CameraControlCmd)|NameOfEnum,CameraControlType}
*/
/////////////////////////////////////////////////////////////////////////////
/*{const char *NameOfEnum(CameraControlCmd ccc)|NameOfEnum,CameraControlCmd}
returns a string representation of the enum value.

 {CameraControlCmd}, {CameraControlType}, {NameOfEnum}
*/
const _TCHAR *NameOfEnum(CameraControlCmd ccc)
{
	switch(ccc)
	{
		NAME_OF_ENUM(CCC_INVALID);
		NAME_OF_ENUM(CCC_PAN_LEFT);
		NAME_OF_ENUM(CCC_PAN_RIGHT);
		NAME_OF_ENUM(CCC_PAN_STOP);
		NAME_OF_ENUM(CCC_TILT_UP);
		NAME_OF_ENUM(CCC_TILT_DOWN);
		NAME_OF_ENUM(CCC_TILT_STOP);
		NAME_OF_ENUM(CCC_FOCUS_NEAR);
		NAME_OF_ENUM(CCC_FOCUS_FAR);
		NAME_OF_ENUM(CCC_FOCUS_STOP);
		NAME_OF_ENUM(CCC_IRIS_OPEN);
		NAME_OF_ENUM(CCC_IRIS_CLOSE);
		NAME_OF_ENUM(CCC_IRIS_STOP);
		NAME_OF_ENUM(CCC_ZOOM_IN);
		NAME_OF_ENUM(CCC_ZOOM_OUT);
		NAME_OF_ENUM(CCC_ZOOM_STOP);
		NAME_OF_ENUM(CCC_POS_1);
		NAME_OF_ENUM(CCC_POS_2);
		NAME_OF_ENUM(CCC_POS_3);
		NAME_OF_ENUM(CCC_POS_4);
		NAME_OF_ENUM(CCC_POS_5);
		NAME_OF_ENUM(CCC_POS_6);
		NAME_OF_ENUM(CCC_POS_HOME);
		NAME_OF_ENUM(CCC_POS_7);
		NAME_OF_ENUM(CCC_POS_8);
		NAME_OF_ENUM(CCC_POS_9);
		NAME_OF_ENUM(CCC_CONFIG);
		NAME_OF_ENUM(CCC_FOCUS_AUTO);
		NAME_OF_ENUM(CCC_IRIS_AUTO);
		NAME_OF_ENUM(CCC_TURN_180);
		NAME_OF_ENUM(CCC_MOVE_LEFT_UP);
		NAME_OF_ENUM(CCC_MOVE_RIGHT_UP);
		NAME_OF_ENUM(CCC_MOVE_LEFT_DOWN);
		NAME_OF_ENUM(CCC_MOVE_RIGHT_DOWN);
		NAME_OF_ENUM(CCC_MOVE_STOP);
		NAME_OF_ENUM(CCC_AUXOUT);
		default:             return _T("CAMERA_COMMAND_INVALID");	break;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*{CameraControlType}
NYD

 {CameraControlCmd}, {NameOfEnum(CameraControlType)|NameOfEnum,CameraControlCmd}
*/

/////////////////////////////////////////////////////////////////////////////
/*{const char *NameOfEnum(CameraControlType cct)|NameOfEnum,CameraControlType}
returns a string representation of the enum value.

 {CameraControlType}, {CameraControlCmd}, {NameOfEnum}
*/
const _TCHAR *NameOfEnum(CameraControlType cct)
{
	switch(cct) 
	{
		NAME_OF_ENUM(CCT_RS232_TRANSPARENT);
		NAME_OF_ENUM(CCT_UNKNOWN);
		NAME_OF_ENUM(CCT_SENSORMATIC_TOUCHTRACKER);
		NAME_OF_ENUM(CCT_PANASONIC_WV_RM70);
		NAME_OF_ENUM(CCT_ERNITEC_BDR_510);
		NAME_OF_ENUM(CCT_ELBEX_97_6);
		NAME_OF_ENUM(CCT_PIEPER_KMS_850S);
		NAME_OF_ENUM(CCT_VIDEV_EC160);
		NAME_OF_ENUM(CCT_RELAY_PTZ);
		NAME_OF_ENUM(CCT_CANON_VISCA);
		NAME_OF_ENUM(CCT_VCL_CAMERA);
		NAME_OF_ENUM(CCT_VCL_MAXCOM);
		NAME_OF_ENUM(CCT_FASTRAX);
		NAME_OF_ENUM(CCT_BAXALL);
		NAME_OF_ENUM(CCT_TOA);
		NAME_OF_ENUM(CCT_PELCO_D);
		NAME_OF_ENUM(CCT_PELCO_P);
		NAME_OF_ENUM(CCT_BBV_RS422);
		NAME_OF_ENUM(CCT_SENSORMATIC);
		NAME_OF_ENUM(CCT_CBC_ZCNAF27);
		NAME_OF_ENUM(CCT_VIDEOTEC);
		NAME_OF_ENUM(CCT_BEWATOR);
		NAME_OF_ENUM(CCT_JVC_TK_C);
		NAME_OF_ENUM(CCT_MERIDIAN);
		NAME_OF_ENUM(CCT_IP_CAMERA);

		default: return _T("CAMERA_CONTROL_TYPE_INVALID");  break;
	}
}
//////////////////////////////////////////////////////////////////////
const _TCHAR *NameOfEnum(QuadControlType qct)
{
	switch(qct) 
	{
		NAME_OF_ENUM(QCT_SANTEC_SQV_80);
		NAME_OF_ENUM(QCT_GANZ_QD_04P);
		NAME_OF_ENUM(QCT_UNKNOWN);
		default: return _T("QUAD_CONTROL_TYPE_INVALID");  break;
	}
}
//////////////////////////////////////////////////////////////////////
CameraControlType NameToCameraControlType(const CString& name)
{
	RETURN_ENUM_OF_NAME(CCT_RS232_TRANSPARENT,name);
	RETURN_ENUM_OF_NAME(CCT_SENSORMATIC_TOUCHTRACKER,name);
	RETURN_ENUM_OF_NAME(CCT_PANASONIC_WV_RM70,name);
	RETURN_ENUM_OF_NAME(CCT_ERNITEC_BDR_510,name);
	RETURN_ENUM_OF_NAME(CCT_ELBEX_97_6,name);
	RETURN_ENUM_OF_NAME(CCT_PIEPER_KMS_850S,name);
	RETURN_ENUM_OF_NAME(CCT_VIDEV_EC160,name);
	RETURN_ENUM_OF_NAME(CCT_RELAY_PTZ,name);
	RETURN_ENUM_OF_NAME(CCT_CANON_VISCA,name);
	RETURN_ENUM_OF_NAME(CCT_VCL_CAMERA,name);
	RETURN_ENUM_OF_NAME(CCT_VCL_MAXCOM,name);
	RETURN_ENUM_OF_NAME(CCT_FASTRAX,name);
	RETURN_ENUM_OF_NAME(CCT_BAXALL,name);
	RETURN_ENUM_OF_NAME(CCT_TOA,name);
	RETURN_ENUM_OF_NAME(CCT_PELCO_D,name);
	RETURN_ENUM_OF_NAME(CCT_PELCO_P,name);
	RETURN_ENUM_OF_NAME(CCT_BBV_RS422,name);
	RETURN_ENUM_OF_NAME(CCT_SENSORMATIC,name);
	RETURN_ENUM_OF_NAME(CCT_CBC_ZCNAF27,name);
	RETURN_ENUM_OF_NAME(CCT_VIDEOTEC,name);
	RETURN_ENUM_OF_NAME(CCT_BEWATOR,name);
	RETURN_ENUM_OF_NAME(CCT_JVC_TK_C,name);
	RETURN_ENUM_OF_NAME(CCT_MERIDIAN,name);
	RETURN_ENUM_OF_NAME(CCT_IP_CAMERA,name);
	return CCT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////
const _TCHAR *NameOfEnumPTZRealName(CameraControlType cct)
{
// Used for PTZ only, other values are always empty	
	switch(cct) 
	{
//		case CCT_RS232_TRANSPARENT:			return _T("");						break;
//		case CCT_UNKNOWN:					return _T("unknown");				break;
		case CCT_SENSORMATIC_TOUCHTRACKER:	return _T("Sensormatic TT");		break;
		case CCT_PANASONIC_WV_RM70:			return _T("Panasonic WV-RM 70");	break;
		case CCT_ERNITEC_BDR_510:			return _T("Ernitec BDR 510");		break;
		case CCT_ELBEX_97_6:				return _T("Elbex EXC 80,90,1000");	break;
		case CCT_PIEPER_KMS_850S:			return _T("Pieper KMS 850S");		break;
		case CCT_VIDEV_EC160:				return _T("Videv EC160");			break;
		case CCT_RELAY_PTZ:					return _T("Relais PTZ");			break;
		case CCT_CANON_VISCA:				return _T("Sony VISCA Protokoll");	break;
		case CCT_VCL_CAMERA:				return _T("VCL Camera");			break;
		case CCT_VCL_MAXCOM:				return _T("VCL MaxCom");			break;
		case CCT_FASTRAX:					return _T("Fastrax II (HID-2404)");	break;
		case CCT_BAXALL:					return _T("Baxall");				break;
		case CCT_TOA:						return _T("CBC TOA");				break;
		case CCT_PELCO_D:					return _T("Pelco-D");				break;
		case CCT_PELCO_P:					return _T("Pelco-P");				break;
		case CCT_BBV_RS422:					return _T("BBV RS422");				break;
		case CCT_SENSORMATIC:				return _T("Sensormatic Ultra");		break;
		case CCT_CBC_ZCNAF27:				return _T("CBC ZC-NAF27");			break;
		case CCT_VIDEOTEC:					return _T("Videotec");				break;
		case CCT_BEWATOR:					return _T("Bewator/Molynx");		break;
		case CCT_JVC_TK_C:					return _T("JVC TK-C676/655");		break;
		case CCT_MERIDIAN:					return _T("Meridian (Mark Mercer)");break;
		case CCT_IP_CAMERA:					return _T("IP Camera Unit");		break;
		default:							return _T("");						break;
	}
}
//////////////////////////////////////////////////////////////////////
const _TCHAR *NameOfEnumQuadRealName(QuadControlType qct)
{
	switch(qct) 
	{
		case QCT_SANTEC_SQV_80:	return _T("Santec SQV 80");	break;
		case QCT_GANZ_QD_04P:	return _T("GANZ QD-04P");	break;
		default:				return _T("");				break;
	}
}
//////////////////////////////////////////////////////////////////////
DWORD GetCameraControlFunctions(CameraControlType cct)
{
	DWORD dwFunctions = PTZF_DEFAULT;
	switch (cct)
	{
	case CCT_RS232_TRANSPARENT:
		dwFunctions = 0;
		break;
	case CCT_SENSORMATIC_TOUCHTRACKER:
		dwFunctions |= PTZF_IRIS|PTZF_FOCUS|PTZF_SPEED|PTZF_POSALL|PTZF_EXTENDED;
		dwFunctions &= ~PTZF_POS_HOME;
		break;
	case CCT_PANASONIC_WV_RM70:
		dwFunctions |= PTZF_IRIS|PTZF_FOCUS|PTZF_MOVE_DIAGONAL|PTZF_SPEED|PTZF_POSALL|PTZF_SET_POS|PTZF_CONFIG|PTZF_EXTENDED;
		break;
	case CCT_ERNITEC_BDR_510:
		dwFunctions |= PTZF_IRIS|PTZF_IRIS_AUTO|PTZF_FOCUS|PTZF_FOCUS_AUTO|PTZF_MOVE_DIAGONAL|PTZF_SPEED|PTZF_SET_POS|PTZF_POS_1|PTZF_POS_2|PTZF_POS_3|PTZF_POS_4|PTZF_POS_5|PTZF_POS_6|PTZF_POS_7|PTZF_POS_8|PTZF_POS_9|PTZF_EXTENDED;
		break;
	case CCT_ELBEX_97_6:
		dwFunctions |= PTZF_IRIS | PTZF_FOCUS | PTZF_MOVE_DIAGONAL;
		break;
	case CCT_PIEPER_KMS_850S:
		dwFunctions |= PTZF_FOCUS;
		break;
	case CCT_VIDEV_EC160:
		dwFunctions |= PTZF_FOCUS;
		break;
	case CCT_RELAY_PTZ:
		dwFunctions |= PTZF_IRIS | PTZF_FOCUS;
		break;
	case CCT_CANON_VISCA:
		dwFunctions |= PTZF_IRIS | PTZF_FOCUS | PTZF_SPEED | PTZF_POS | PTZF_SET_POS | PTZF_MOVE_DIAGONAL;
		break;
	case CCT_VCL_CAMERA:
	case CCT_VCL_MAXCOM:
		dwFunctions |= PTZF_IRIS|PTZF_IRIS_AUTO|PTZF_FOCUS|PTZF_FOCUS_AUTO|PTZF_TURN_180|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_EXTENDED;
		break;
	case CCT_FASTRAX:
		dwFunctions |= PTZF_IRIS|PTZF_FOCUS|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_MOVE_DIAGONAL|PTZF_CONFIG|PTZF_EXTENDED;
		break;
	case CCT_BAXALL:
		dwFunctions |= PTZF_IRIS|PTZF_FOCUS|PTZF_POSALL|PTZF_SPEED|PTZF_MOVE_DIAGONAL|PTZF_EXTENDED;
		break;
	case CCT_TOA:
		dwFunctions |= /*PTZF_IRIS |*/ PTZF_FOCUS|PTZF_FOCUS_AUTO|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_MOVE_DIAGONAL|PTZF_CONFIG|PTZF_EXTENDED;
		break;
	case CCT_PELCO_D:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_TURN_180|PTZF_EXTENDED;
		break;
	case CCT_PELCO_P:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_TURN_180|PTZF_EXTENDED;
		break;
	case CCT_BBV_RS422:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_POSALL|PTZF_SPEED|PTZF_SET_POS|PTZF_TURN_180|PTZF_EXTENDED;
		break;
	case CCT_SENSORMATIC:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_SPEED|PTZF_POS_1|PTZF_POS_2|PTZF_POS_3|PTZF_POS_4|PTZF_POS_5|PTZF_POS_6|PTZF_POS_7|PTZF_SET_POS|PTZF_TURN_180|PTZF_EXTENDED|PTZF_FOCUS_AUTO|PTZF_IRIS_AUTO|PTZF_CONFIG;
		break;
	case CCT_CBC_ZCNAF27:
		dwFunctions = PTZF_FOCUS|PTZF_IRIS|PTZF_ZOOM|PTZF_FOCUS_AUTO|PTZF_IRIS_AUTO|PTZF_CONFIG|PTZF_SPEED;
		break;
	case CCT_VIDEOTEC:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_SPEED|PTZF_POSALL|PTZF_SET_POS|PTZF_EXTENDED|PTZF_FOCUS_AUTO|PTZF_IRIS_AUTO;
		break;
	case CCT_BEWATOR:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_SPEED|PTZF_POSALL|PTZF_SET_POS|PTZF_FOCUS_AUTO|PTZF_IRIS_AUTO|PTZF_EXTENDED;
		break;
	case CCT_JVC_TK_C:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_CONFIG|PTZF_SPEED|PTZF_POSALL|PTZF_SET_POS|PTZF_FOCUS_AUTO|PTZF_IRIS_AUTO|PTZF_EXTENDED;
		break;
	case CCT_MERIDIAN:
		dwFunctions |= PTZF_FOCUS|PTZF_IRIS|PTZF_MOVE_DIAGONAL|PTZF_CONFIG|PTZF_SPEED|PTZF_POSALL|PTZF_SET_POS|PTZF_EXTENDED|PTZF_IRIS_AUTO; // PTZF_FOCUS_AUTO|
		break;
	case CCT_IP_CAMERA: // determined by IPCameraUnit
		dwFunctions = PTZF_DEFAULT; 
		break;
	default:
	case CCT_UNKNOWN:
		dwFunctions = 0; 
		break;
	}
	return dwFunctions;
}
//////////////////////////////////////////////////////////////////////
DWORD GetCameraControlFunctionsEx(CameraControlType cct)
{
	DWORD dwFunctionsEx = 0;
	switch (cct)
	{
	case CCT_SENSORMATIC_TOUCHTRACKER:
		dwFunctionsEx = PTZF_EX_SPEED_READJUST;
		break;
	case CCT_PANASONIC_WV_RM70:
		dwFunctionsEx = PTZF_EX_SPEED_READJUST;
		break;
	case CCT_ERNITEC_BDR_510:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_AUXILIARY_OUT3|PTZF_EX_AUXILIARY_OUT4|PTZF_EX_AUXILIARY_OUT5|PTZF_EX_AUXILIARY_OUT6|PTZF_EX_AUXILIARY_OUT7|PTZF_EX_AUXILIARY_OUT8|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST;
		break;
	case CCT_VCL_CAMERA:
	case CCT_VCL_MAXCOM:
		dwFunctionsEx = PTZF_EX_SPEED_READJUST;
		break;
	case CCT_BAXALL:
		dwFunctionsEx = PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_PELCO_D:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT|PTZF_EX_ERASE_PRESET|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_PELCO_P:
		dwFunctionsEx = PTZF_EX_ERASE_PRESET|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_BBV_RS422:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT|PTZF_EX_ERASE_PRESET|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_TOA:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_ERASE_PRESET|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_FASTRAX:
		dwFunctionsEx = PTZF_EX_CONFIG_SET_3_4|PTZF_EX_ERASE_PRESET|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_SENSORMATIC:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_AUXILIARY_OUT3|PTZF_EX_AUXILIARY_OUT4|PTZF_EX_AUX_OUT_BITS|PTZF_EX_CONFIG_SET_3_4|PTZF_EX_SPEED_READJUST;
		break;
	case CCT_VIDEOTEC:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_AUXILIARY_OUT3|PTZF_EX_AUXILIARY_OUT4|PTZF_EX_AUXILIARY_OUT5|PTZF_EX_AUXILIARY_OUT6|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_BEWATOR:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_JVC_TK_C:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_AUXILIARY_OUT2|PTZF_EX_AUXILIARY_OUT3|PTZF_EX_ERASE_PRESET|PTZF_EX_CONFIG_SET_3_4|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	case CCT_MERIDIAN:
		dwFunctionsEx = PTZF_EX_AUXILIARY_OUT1|PTZF_EX_ERASE_PRESET|PTZF_EX_CONFIG_SET_3_4|PTZF_EX_MORE_POSITIONS|PTZF_EX_SPEED_READJUST|PTZF_EX_SPEED_XY;
		break;
	}
	return dwFunctionsEx;
}
//////////////////////////////////////////////////////////////////////
BOOL IsCameraIDRequired(CameraControlType cct)
{
	BOOL bReturn = FALSE;
	switch (cct)
	{
//		case CCT_RS232_TRANSPARENT:
//		case CCT_UNKNOWN:
//		case CCT_SENSORMATIC_TOUCHTRACKER:
		case CCT_PANASONIC_WV_RM70:
		case CCT_ERNITEC_BDR_510:
		case CCT_ELBEX_97_6:
//		case CCT_PIEPER_KMS_850S:
		case CCT_VIDEV_EC160:
//		case CCT_RELAY_PTZ:
		case CCT_CANON_VISCA:
		case CCT_VCL_CAMERA:
		case CCT_VCL_MAXCOM:
		case CCT_FASTRAX:
		case CCT_BAXALL:
		case CCT_TOA:
		case CCT_PELCO_D:
		case CCT_PELCO_P:
		case CCT_BBV_RS422:
		case CCT_SENSORMATIC:
		case CCT_CBC_ZCNAF27:
		case CCT_VIDEOTEC:
		case CCT_BEWATOR:
		case CCT_JVC_TK_C:
		case CCT_MERIDIAN:
			bReturn = TRUE;
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
int	GetLastCameraControlType(BOOL bIdip)
{
	// TODO! RKE: enable CCT_IP_CAMERA, when ready
	if (bIdip)
	{
#ifdef _DEBUG
		return CCT_IP_CAMERA;
#else
		return CCT_IP_CAMERA;
#endif
	}
	else
	{
#ifdef _DEBUG
		return CCT_IP_CAMERA;
#else
		return CCT_IP_CAMERA;
#endif
	}
}
