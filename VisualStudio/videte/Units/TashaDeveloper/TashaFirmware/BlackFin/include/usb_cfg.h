/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * USB configuration header file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/BlackFin/include/usb_cfg.h $
 * 
 * 1     5.01.04 9:53 Martin.lueck
 * 
 * 1     31.07.03 14:33 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _USB_CFG_H_
#define _USB_CFG_H_

//..............................................................................

#include "stdtyp.h"

//..............................................................................

enum {DEVICE_DESCR=1,CONFIG_DESCR,STRING_DESCR,IFACE_DESCR,EP_DESCR};

enum {EP_OUT=0, EP_IN};
enum {EP_CTL=0, EP_ISO, EP_BULK, EP_INT};
enum {EP0=0, EP1, EP2, EP3, EP4, EP5, EP6, EP7};
enum {LOG_EP0=0, LOG_EP1, LOG_EP2, LOG_EP3, LOG_EP4, LOG_EP5, LOG_EP6, 
      LOG_EP7, LOG_EP8, LOG_EP9, LOG_EP10, LOG_EP11, LOG_EP12,LOG_EP13,
      LOG_EP14, LOG_EP15};

enum {EP_ALT0=0, EP_ALT1};
enum {EP_IFACE0=0, EP_IFACE1};
enum {EP_CONFIG0=0,EP_CONFIG1};
//enum {USBD_MAX_8=0, USBD_MAX_16, USBD_MAX_32, USBD_MAX_64};
enum {USBC_TYP_CTL=0, USBD_TYP_BULK, USBD_TYP_INT, USBD_TYP_ISO};

#define epDmaBufSize         64
#define EP_BUF_SZ	     5
#define	MAX_PHY_EP	     8
#define	MAX_EP_CONFIG	     2
#define MAX_EP_ALT	     2
#define MAX_EP_IFACE	     2
#define MAX_EP_BUF	     (MAX_PHY_EP*MAX_EP_CONFIG*MAX_EP_ALT*MAX_EP_IFACE)

//..............................................................................

typedef struct _EP_BUF_INFO {

  uint32 ep_bufadrptr:16;  // 0:15
  uint32 ep_userbit:1;	   // 16
  uint32 ep_maxpktsize:10; // 17:26
  uint32 ep_dir:1;	   // 27
  uint32 ep_type:2;	   // 28:29
  uint32 ep_altsetting:2;  // 30:31
  uint8	 ep_interface:2;   // 0:1
  uint8	 ep_config:2;	   // 2:3
  uint8	 epnum:4;	   // 4:7
} EP_BUF_INFO, * PEP_BUF_INFO;

typedef union _EP_BUF {

  EP_BUF_INFO	s_ep_buf;
  uint8		a_ep_buf[EP_BUF_SZ];
} EP_BUF, *PEP_BUF;

typedef struct _USB_DEVICE_DESCRIPTOR {
  uint8  length;
  uint8  descriptorType;
  uint16 bcdUSB;
  uint8  deviceClass;
  uint8  deviceSubClass;
  uint8  deviceProtocol;
  uint8  maxPacketSize0;
  uint16 idVendor;
  uint16 idProduct;
  uint16 bcdDevice;
  uint8  manufacturer;
  uint8  product;
  uint8  serialNumber;
  uint8  numConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USB_CONFIGURATION_DESCRIPTOR {
  uint8  length;
  uint8  descriptorType;
  uint16 totalLength;
  uint8  numInterfaces;
  uint8  configurationValue;
  uint8  configuration;
  uint8  bmAttributes;
  uint8  maxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR {
  uint8 length;
  uint8 descriptorType;
  uint8 interfaceNumber;
  uint8 alternateSetting;
  uint8 numEndpoints;
  uint8 interfaceClass;
  uint8 interfaceSubClass;
  uint8 interfaceProtocol;
  uint8 interface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
  uint8 length;
  uint8 descriptorType;
  uint8 endpointAddress;
  uint8 bmAttributes;
  uint16 maxPacketSize;
  uint8 interval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

//..............................................................................

// Structures

extern uint8 ep0DmaBuf[];
extern const USB_DEVICE_DESCRIPTOR devDesc;
extern uint8 epDmaBuf[];
extern const EP_BUF ep_buf[];

extern const struct _cfgDesc { 
  USB_CONFIGURATION_DESCRIPTOR cfg;
  USB_INTERFACE_DESCRIPTOR iface;
  USB_ENDPOINT_DESCRIPTOR ep[7];
} cfgDesc;

//..............................................................................

#endif

