//Empfangsbuffergröße in Byte für Socketobjekt

#define RECEIVE_BUFFER_SIZE  1000
#define CONNECTED   30
#define MAIL_SEND   31
#define READY	    32
#define SMTP_ERROR  33
#define GET_MAIL    34
#define MAX_EMAIL	20	
#define NEXT_MAIL	35	

//Content-Types


#define _TEXXT		"Text"
#define _MULTIPART	"Multipart"
#define _MESSAGE	"Message"
#define _APPLICATION "Application"
#define _IMAGE		"Image"
#define	_AUDIO		"Audio"
#define	_VIDEO		"Video"

//Sub-Types

#define _PLAIN		"plain"
#define _MIXED		"mixed"
#define _ALTERNATIVE "alternative"
#define _DIGEST		"digest"
#define _PARALLEL	"parallel"
#define _RFC822		"rfc822"
#define _PARTIAL	"partial"
#define	_EXTERNAL_BODY "external-body"
#define _OCTET_STREAM  "octet-stream"
#define _POSTSCRIPT    "postscript"


#define MAX_ATTEMPS_TO_SEND 3


#define WM_EMAIL		(WM_USER+1)
#define WM_PICTURE		(WM_USER+2)
#define WM_MAILSEND		(WM_USER+3)
#define WM_COCO_DECODED	(WM_USER+4)



#define EMAIL_PARKING_TIMER 1
#define NETWORK_TIMER 2
#define HANG_UP_TIMER 3
#define DIAL_UP_ALIVE_CHECK 4

#define NETWORK_TIMEOUT 60 //40s zum Aufbau des Netzwerks(DFÜ...)
#define GETPICTURE_TIMEOUT 20
#define HANG_UP_TIME 50

#define YELLOW	20
#define GREY	21
#define RED		22
#define BLUE	23
#define GREEN   24





