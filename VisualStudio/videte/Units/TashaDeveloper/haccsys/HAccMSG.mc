;
;//File: HAccMsg.mc
;//
;
MessageIdTypedef = NTSTATUS

SeverityNames = (
	Success		  = 0x0:STATUS_SEVERITY_SUCCESS
	Informational = 0x1:STATUS_SEVERITY_INFORMATIONAL
	Warning		  = 0x2:STATUS_SEVERITY_WARNING
	Error		  = 0x3:STATUS_SEVERITY_ERROR
	)

FacilityNames = (
	System		= 0x0
	RpcRuntime	= 0x2:FACILITY_RPC_RUNTIME
	RpcStubs	= 0x3:FACILITY_RPC_STUBS
	Io			= 0x4:FACILITY_IO_ERROR_CODE
	HAcc		= 0x7:FACILITY_HACC_ERROR_CODE
	)


MessageId=0x0001
Facility=HAcc
Severity=Informational
SymbolicName=HACC_MSG_LOGGING_ENABLED
Language=English
Event logging enabled for HAcc.
.

MessageId=+1
Facility=HAcc
Severity=Informational
SymbolicName=HACC_MSG_DRIVER_STARTING
Language=English
HAcc has successfully initialized.
.

MessageId=+1
Facility=HAcc
Severity=Informational
SymbolicName=HACC_MSG_DRIVER_STOPPING
Language=English
HAcc has unloaded.
.

MessageId=+1
Facility=HAcc
Severity=Error
SymbolicName=HACC_MSG_CANT_CLAIM_RESOURCES
Language=English
HAcc can't claim all resources.
.

MessageId=+1
Facility=HAcc
Severity=Error
SymbolicName=HACC_MSG_CANT_ALLOCATE_JPEGBUFFERS
Language=English
HAcc can't allocate Jpegbuffers.
.

MessageId=+1
Facility=HAcc
Severity=Error
SymbolicName=HACC_MSG_CANT_ALLOCATE_CODEBUFFERS
Language=English
HAcc can't allocate Codebuffers.
.

MessageId=+1
Facility=HAcc
Severity=Error
SymbolicName=HACC_MSG_CANT_INSTALL_ISR
Language=English
HAcc can't install ISR.
.

MessageId=+1
Facility=HAcc
Severity=Error
SymbolicName=HACC_MSG_CANT_CREATE_DEVICE
Language=English
HAcc can't create Ring0 Eventhandle.
.

