;
;//File: TashaMsg.mc
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
	Tasha		= 0x7:FACILITY_TASHA_ERROR_CODE
	)


MessageId=0x0001
Facility=Tasha
Severity=Informational
SymbolicName=TASHA_MSG_LOGGING_ENABLED
Language=English
Event logging enabled for Tasha.
.

MessageId=+1
Facility=Tasha
Severity=Informational
SymbolicName=TASHA_MSG_DRIVER_STARTING
Language=English
Tasha has successfully initialized.
.

MessageId=+1
Facility=Tasha
Severity=Informational
SymbolicName=TASHA_MSG_DRIVER_STOPPING
Language=English
Tasha has unloaded.
.

MessageId=+1
Facility=Tasha
Severity=Error
SymbolicName=TASHA_MSG_CANT_CLAIM_RESOURCES
Language=English
Tasha can't claim all resources.
.

MessageId=+1
Facility=Tasha
Severity=Error
SymbolicName=TASHA_MSG_CANT_ALLOCATE_JPEGBUFFERS
Language=English
Tasha can't allocate Jpegbuffers.
.

MessageId=+1
Facility=Tasha
Severity=Error
SymbolicName=TASHA_MSG_CANT_ALLOCATE_CODEBUFFERS
Language=English
Tasha can't allocate Codebuffers.
.

MessageId=+1
Facility=Tasha
Severity=Error
SymbolicName=TASHA_MSG_CANT_INSTALL_ISR
Language=English
Tasha can't install ISR.
.

MessageId=+1
Facility=Tasha
Severity=Error
SymbolicName=TASHA_MSG_CANT_CREATE_DEVICE
Language=English
Tasha can't create Ring0 Eventhandle.
.

