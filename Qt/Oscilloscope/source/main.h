#ifndef MAIN_H
#define MAIN_H

#include <qwidget.h>
#include "logger.h"

#ifndef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#endif

void setWidgetColor(QWidget& aWidget, QPalette::ColorRole aRole, const QColor& aColor);
void setWidgetStyleSheet(QWidget& aWidget, const QColor* aColor, const QColor* aBackgroundColor);




#endif // MAIN_H
