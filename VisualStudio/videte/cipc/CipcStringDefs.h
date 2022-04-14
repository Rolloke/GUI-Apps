/* GlobalReplace: @[aA][lL][sS][oO] -->  */
// FILE: $Workfile: CipcStringDefs.h $ from $Archive: /Project/CIPC/CipcStringDefs.h $
// VERSION: $Date: 14.08.06 16:15 $ $Revision: 86 $
// LAST:	$Modtime: 14.08.06 16:15 $ by $Author: Rolf.kary-ehlers $
//$Nokeywords:$

#ifndef __CIPCStringDefs_H__
#define __CIPCStringDefs_H__

/*{CIPC String Defines|CIPCStringDefines}
{CDS_CONTRAST}
{CDS_BRIGHTNESS}
{CDS_SATURATION}
{CDS_MAX_OVERLAY_RECT}
{CDS_COLORKEY}

For {CIPC::DoRequestGetValue}, 
{CIPC::DoReuestSetValue} the following string are
predefined:
 CSD_CONTRAST
 CSD_BRIGHTNESS
 CSD_SATURATION
 CSD_MAX_OVERLAY_RECT
 CSD_COLORKEY

There are two utility functions to convert CRect to/from strings.
{RectToString} and {StringToRect}

 {CIPC}
*/
// 'C'IPC 'S'tring 'D'efine
#define CSD_CONTRAST              _T("Contrast")
#define CSD_BRIGHTNESS            _T("Brightness")
#define CSD_SATURATION            _T("Saturation")
#define CSD_COMPRESSION           _T("Compression")
#define CSD_RESOLUTION            _T("Resolution")
#define CSD_ROTATION	          _T("Rotation")
#define CSD_FRAMERATE	          _T("FrameRate")
#define CSD_CAM_PTZ_TYPE          _T("Type")
#define CSD_CAM_PTZ_ID            _T("ID")
#define CSD_CAM_PTZ_COMPORT       _T("ComPort")
#define CSD_COLORENABLE           _T("ColorEnable")

#define CSD_COMPRESSIONS          _T("Compressions")
#define CSD_RESOLUTIONS           _T("Resolutions")
#define CSD_ROTATIONS	          _T("Rotations")
#define CSD_FRAMERATES	          _T("FrameRates")
#define CSD_OUTPUTS               _T("Outputs")
#define CSD_INPUTS                _T("Inputs")
#define CSD_RELAIS                _T("Relais")

#define CSD_CONTRAST_PERCENT      _T("ContrastPercent")
#define CSD_BRIGHTNESS_PERCENT    _T("BrightnessPercent")
#define CSD_SATURATION_PERCENT    _T("SaturationPercent")

#define CSD_HUE_PERCENT                   _T("HuePercent")
#define CSD_SHARPNESS_PERCENT             _T("SharpnessPercent")
#define CSD_GAMMA_PERCENT                 _T("GammaPercent")
#define CSD_WHITEBALANCE_PERCENT          _T("WhiteBalancePercent")
#define CSD_BACKLIGHTCOMPENSATION_PERCENT _T("BacklightCompensationPercent")
#define CSD_VIDEOGAIN_PERCENT             _T("VideoGainPercent")

#define CSD_PAN_PERCENT       _T("PanPercent")
#define CSD_TILT_PERCENT      _T("TiltPercent")
#define CSD_ROLL_PERCENT      _T("RollPercent")
#define CSD_ZOOM_PERCENT      _T("ZoomPercent")
#define CSD_EXPOSURE_PERCENT  _T("ExposurePercent")
#define CSD_IRIS_PERCENT      _T("IrisPercent")
#define CSD_FOCUS_PERCENT     _T("FocusPercent")

#define CSD_MAX_OVERLAY_RECT      _T("MaxOverlayRect")
#define CSD_COLORKEY              _T("ColorKey")

#define CSD_APPLICATION_NAME      _T("ApplicationName")

#define CSD_SCAN_FOR_CAMERAS      _T("ScanForCameras")
#define CSD_CAMERA_CONTROL        _T("CameraControl")

#define CSD_CAM_CONTROL_TYPE      _T("GetControlType")
#define CSD_CAM_CONTROL_TYPE_FMT  _T("GetControlType(%08x)")
#define CSD_CAM_CONTROL_FKT       _T("GetControlFunctions")
#define CSD_CAM_CONTROL_FKT_FMT   _T("GetControlFunctions(%08x)")
#define CSD_CAM_CONTROL_FKTEX     _T("GetControlFunctionsEx")
#define CSD_CAM_CONTROL_FKTEX_FMT _T("GetControlFunctionsEx(%08x)")

#define CSD_MODE						_T("MODE")
#define CSD_PIN							_T("PIN")
#define CSD_PIN_SUPERVISOR				_T("PIN_Supervisor")
#define CSD_PIN_OPERATOR				_T("PIN_Operator")
#define CSD_NAME						_T("NAME")
#define CSD_IS_TIMER_ACTIVE				_T("IsTimerActive")
#define CSD_DETECTOR_IGNORE_TIMER		_T("DetectorIgnoreTimer")
#define CSD_TARGET_DISPLAY				_T("TargetDisplay")
#define CSD_URL							_T("URL")

#define CSD_ALARM_OUTPUT_ON_OFF_STATE _T("AlarmOutputOnOffState")
#define CSD_ALARM_DIALING_NUMBER	  _T("AlarmDialingNumber")

#define CSD_MD_MASK_SENSITIVITY		_T("MDMaskSensitivity")
#define CSD_MD_ALARM_SENSITIVITY	_T("MDAlarmSensitivity")

#define CSD_HIGH	_T("High")
#define CSD_NORMAL	_T("Normal")
#define CSD_LOW		_T("Low")

#define CSD_MD		_T("MD")
#define CSD_UVV		_T("UVV")
#define CSD_TERM	_T("TERM")
#define CSD_FPS		_T("FPS")
#define CSD_AGC		_T("AGC")
#define CSD_VIDEO_NORM _T("VideoNorm")			// PAL/NTSC
#define CSD_VIDEO_FORMAT _T("VideoFormat")		// Compressiontype

#define CSD_PAL		_T("PAL")
#define CSD_NTSC	_T("NTSC")

#define CSD_ON		_T("1")
#define CSD_OFF		_T("0")

#define CSD_REQUEST	_T("Request")
#define CSD_YES		_T("Yes")
#define CSD_NO		_T("No")

#define CSD_CAM_TYPE              _T("CameraType")
#define CSD_CAM_CAPTURE           _T("Capture")		// DirectShow Capture devices
#define CSV_CONTROL_ID            0x0815


#define CSD_ERROR	_T("Error")

#define CSD_ACTIVE_CAMERA_MASK _T("ActiveCameraMask")

// Switches video signal of camera to video out port
// DoRequestSetValue(camID,CSD_V_OUT,CSD_PORTx,dwUserID)
#define CSD_V_OUT	_T("VOUT")
#define CSD_PORT0	_T("PORT0")
#define CSD_PORT1	_T("PORT1")
#define CSD_PORT2	_T("PORT2")
#define CSD_PORT3	_T("PORT3")

// Switches the mode or usage of the video out port
// Get
// DoRequestGetValue(SECID_NO_ID,CSD_PORTx,dwUserID)
//   -> OnConfirmGetValue(SECID_NO_ID,CSD_PORTx,CSD_VOUT_xxx,dwUserID)
// Set
// DoRequestSetValue(SECID_NO_ID,CSD_PORTx,CSD_VOUT_xxx,dwUserID)
//   -> OnConfirmSetValue(SECID_NO_ID,CSD_PORTx,CSD_VOUT_xxx,dwUserID)
#define CSD_VOUT_OFF				_T("VOUT_OFF")
#define CSD_VOUT_ACTIVITY			_T("VOUT_ACTIVITY")
#define CSD_VOUT_CLIENT				_T("VOUT_CLIENT")
#define CSD_VOUT_SEQUENCE			_T("VOUT_SEQUENCE")
#define CSD_VOUT_PERMANENT			_T("VOUT_PERMANENT")
#define CSD_VOUT_COUNT				_T("VOUT_COUNT")


// Set's and Get's the dwell time of a camera
// Values as CString in seconds, 5 seconds = "5"
// Get
// DoRequestGetValue(camID,CSD_DWELL,dwUserID); 
//   -> OnConfirmGetValue(camID,CSD_DWELL,sValue,dwUserID)
// Set
// DoRequestSetValue(camID,CSD_DWELL,sValue,dwUserID); 
//   -> OnConfirmSetValue(camID,CSD_DWELL,sValue,dwUserID)
#define CSD_DWELL					_T("DWELL")

#define CSD_ALARM_SPAN_OFF_FEATURE	_T("ASOF")
#define CSD_ALARM_SPAN_OFF_STATUS	_T("ASOS")

#define CSD_IS_DV					_T("ISDV")
									   
#define CSD_ALARMCALL_EVENT			_T("ACE")
#define CSD_ALARMCALL_EVENT_OFF		_T("ACE_OFF")
#define CSD_ALARMCALL_EVENT_A		_T("ACE_A")
#define CSD_ALARMCALL_EVENT_A1		_T("ACE_A1")
#define CSD_ALARMCALL_EVENT_A2		_T("ACE_A2")
#define CSD_ALARMCALL_EVENT_A12		_T("ACE_A12")
#define CSD_ALARMCALL_EVENT_1		_T("ACE_1")
#define CSD_ALARMCALL_EVENT_2		_T("ACE_2")
#define CSD_ALARMCALL_EVENT_12		_T("ACE_12")

#define CSD_EXPANSION_CODE			_T("EXP_CODE")

#define CSD_DIB_TRANSFER			_T("DIB_TRANSFER")

#define CSD_RESET					_T("RESET")
#define CSD_LED1_ON					_T("LED1_ON")
#define CSD_LED1_BLINK				_T("LED1_BLINK")
#define CSD_LED2_ON					_T("LED2_ON")
#define CSD_LED2_OFF				_T("LED2_OFF")

#define CSD_DV_CONFIG				_T("DV_CONFIG")
#define CSD_SYSTEM_INFO				_T("SYSTEM_INFO")
#define CSD_CARD_TYPE				_T("CARD_TYPE")
#define CSD_CARD_TYPE_JACOB			_T("CARD_TYPE_JACOB")	
#define CSD_CARD_TYPE_SAVIC			_T("CARD_TYPE_SAVIC")	
#define CSD_CARD_TYPE_TASHA			_T("CARD_TYPE_TASHA")	
#define CSD_CARD_TYPE_Q				_T("CARD_TYPE_Q")	

#define CSD_AUDIO_IN				_T("AUDIO_IN")
#define CSD_AUDIO_OUT				_T("AUDIO_OUT")
#define CSD_AUDIO_CAM				_T("AUDIO_CAM")
#define CSD_AUDIO_REC				_T("AUDIO_REC")

#define CSD_AUDIO_RECORDING_OFF		_T("OFF")
#define CSD_AUDIO_RECORDING_L		_T("L")
#define CSD_AUDIO_RECORDING_A		_T("A")
#define CSD_AUDIO_RECORDING_E		_T("E")
#define CSD_AUDIO_RECORDING_LA		_T("LA")
#define CSD_AUDIO_RECORDING_LE		_T("LE")
#define CSD_AUDIO_RECORDING_AE		_T("AE")
#define CSD_AUDIO_RECORDING_LAE		_T("LAE")

#define CSD_REJECT_ANNOUNCEMENT		_T("REJECT_ANNOUNCEMENT")

#define SI_GETLOGICALDRIVES			_T("GetLogicalDrives")
#define SI_GETDISKFREESPACE			_T("GetDiskFreeSpace")
#define SI_GETOSVERSION				_T("GetOSVersion")
#define SI_GETSYSTEMINFO			_T("GetSystemInfo")
#define SI_GLOBALMEMORYSTATUS		_T("GlobalMemoryStatus")
#define SI_GETCURRENTTIME			_T("GetCurrentTime")
#define SI_GETSOFTWAREVERSION		_T("GetSoftwareVersion")
#define SI_GETFILEVERSION			_T("GetFileVersion")
#define SI_GETWWWDIRECTORY			_T("GetWWWDirectory")
#define SI_EXPORTREGISTRY			_T("ExportRegistry")
#define SI_GETTIMEZONEINFORMATION	_T("GetTimeZoneInformation")
#define SI_GETPRODUCTINFO			_T("GetProductInfo")
#define SI_ENUMREGKEYS				_T("EnumRegKeys")
#define SI_ENUMREGVALUES			_T("EnumRegValues")
#define SI_GETREGKEY				_T("GetRegKey")
#define SI_SETREGKEY				_T("SetRegKey")
#define SI_DELETEREGKEY				_T("DeleteRegKey")


#endif
