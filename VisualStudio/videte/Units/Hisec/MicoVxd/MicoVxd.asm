.386p
include vmm.inc

DECLARE_VIRTUAL_DEVICE	MICO, 1, 0, MICO_VXD_Control, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER

VxD_LOCKED_CODE_SEG

BeginProc MICO_VXD_Control
    Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, MICO_VXD_Dynamic_Init, sCall
    Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, MICO_VXD_Dynamic_Exit, sCall
    Control_Dispatch W32_DEVICEIOCONTROL,     MICO_VXD_W32_DeviceIOControl, sCall, <ecx, ebx, edx, esi>
    clc
    ret
EndProc MICO_VXD_Control

VxD_LOCKED_CODE_ENDS

END



