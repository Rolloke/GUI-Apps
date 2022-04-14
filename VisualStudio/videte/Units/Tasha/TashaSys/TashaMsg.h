
//File: TashaMsg.mc
//

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_TASHA_ERROR_CODE        0x7
#define FACILITY_RPC_STUBS               0x3
#define FACILITY_RPC_RUNTIME             0x2
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: TASHA_MSG_LOGGING_ENABLED
//
// MessageText:
//
//  Event logging enabled for Tasha.
//
#define TASHA_MSG_LOGGING_ENABLED        ((NTSTATUS)0x60070001L)

//
// MessageId: TASHA_MSG_DRIVER_STARTING
//
// MessageText:
//
//  Tasha has successfully initialized.
//
#define TASHA_MSG_DRIVER_STARTING        ((NTSTATUS)0x60070002L)

//
// MessageId: TASHA_MSG_DRIVER_STOPPING
//
// MessageText:
//
//  Tasha has unloaded.
//
#define TASHA_MSG_DRIVER_STOPPING        ((NTSTATUS)0x60070003L)

//
// MessageId: TASHA_MSG_CANT_CLAIM_RESOURCES
//
// MessageText:
//
//  Tasha can't claim all resources.
//
#define TASHA_MSG_CANT_CLAIM_RESOURCES   ((NTSTATUS)0xE0070004L)

//
// MessageId: TASHA_MSG_CANT_ALLOCATE_JPEGBUFFERS
//
// MessageText:
//
//  Tasha can't allocate Jpegbuffers.
//
#define TASHA_MSG_CANT_ALLOCATE_JPEGBUFFERS ((NTSTATUS)0xE0070005L)

//
// MessageId: TASHA_MSG_CANT_ALLOCATE_CODEBUFFERS
//
// MessageText:
//
//  Tasha can't allocate Codebuffers.
//
#define TASHA_MSG_CANT_ALLOCATE_CODEBUFFERS ((NTSTATUS)0xE0070006L)

//
// MessageId: TASHA_MSG_CANT_INSTALL_ISR
//
// MessageText:
//
//  Tasha can't install ISR.
//
#define TASHA_MSG_CANT_INSTALL_ISR       ((NTSTATUS)0xE0070007L)

//
// MessageId: TASHA_MSG_CANT_CREATE_DEVICE
//
// MessageText:
//
//  Tasha can't create Ring0 Eventhandle.
//
#define TASHA_MSG_CANT_CREATE_DEVICE     ((NTSTATUS)0xE0070008L)

