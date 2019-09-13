#ifndef DEFINES_H
#define DEFINES_H

#include <vector>
#include <string>

#define FORTH 1
#include "main.h"
//#ifdef _DEBUG
//#define TRACE printf
//#endif
//#define TRACE(#..) //

#define USE_LPT_DRIVER 1

#define SCALE_FACTOR 10.0 // for 10th of mm
typedef float FLOAT;
typedef unsigned int UINT;
typedef unsigned long WPARAM;
typedef long LPARAM;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

typedef std::vector< std::string > stringlist;

const FLOAT rad2deg = static_cast<FLOAT>(180.0 / M_PI);
const FLOAT deg2rad = static_cast<FLOAT>(M_PI / 180.0);

#endif // DEFINES_H
