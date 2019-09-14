#ifndef SERIAL_DEBUG_H
#define SERIAL_DEBUG_H

//#define SOFTWARE_SERIAL 1

#ifdef SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  
  #define DEBUG_PRINT(X) Serial.print(X);
  #define DEBUG_PRINTLN(X) Serial.println(X);
#else
  #define DEBUG_PRINT //
  #define DEBUG_PRINTLN //
#endif

#endif
