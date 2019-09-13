#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include "defines.h"

size_t splitString(const std::string& aString,const std::string&aSeparator, stringlist& aList);
size_t trim_left(std::string& aString);
size_t trim_right(std::string& aString);

#endif // COMMONFUNCTIONS_H
