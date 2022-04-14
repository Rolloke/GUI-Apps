// PlotterDriver.cpp : implementation file
//

#include "stdafx.h"
#include "SchrittmotorLPT.h"
#include "PlotterDriver.h"


// PlotterDriver
const BYTE PlotterDriver::mControlValue[4] = { 0x0, 0x1, 0x3, 0x2 };
IMPLEMENT_DYNCREATE(PlotterDriver, CWinThread)

PlotterDriver::PlotterDriver() :
  mPositionX(0)
, mPositionY(0)
, mPositionZ(0)
, mTestSteps(500)
, mDelay_ms(1)
{
    mControl.bits = 0;
    mMotors.b.bits = 0;
    mTransformation.identity(3);
}

PlotterDriver::~PlotterDriver()
{
}

BOOL PlotterDriver::setPortAddress(unsigned short aAdr)
{
    BOOL bReturn = mPort.setPortBaseAddress(aAdr);
    if (bReturn)
    {
        Bits data ={0};
        mPort.setBits(LPTPort::data, data);
        mPort.setBits(LPTPort::control, mControl);
    }
    return bReturn;
}

BOOL PlotterDriver::InitInstance()
{
	return TRUE;
}

int PlotterDriver::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(PlotterDriver, CWinThread)
   ON_MESSAGE(WM_TEST_RUN_LEFT, OnTestTurnLeft)
   ON_MESSAGE(WM_TEST_RUN_RIGHT, OnTestTurnRight)
   ON_MESSAGE(WM_PLOT_COMMANDS, OnPlotCommands)
   ON_MESSAGE(WM_CALIBRATE_PLOTTER, OnCalibratePlotter)
END_MESSAGE_MAP()


// PlotterDriver message handlers
LRESULT PlotterDriver::OnTestTurnLeft(WPARAM wParam, LPARAM lParam)
{
    mControl.c.data_strobe = 0;
    mPort.setBits(LPTPort::control, mControl);

    MotorControl mc = {0};
    for (int i=0; i<mTestSteps; ++i)
    {
        mc.m.motor1 = mControlValue[mPositionX&Mask];
        mPort.setBits(LPTPort::data, mc.b);
        --mPositionX;
        Sleep(mDelay_ms);
    }

    mControl.c.data_strobe = 1;
    mPort.setBits(LPTPort::control, mControl);
	return 0;
}

LRESULT PlotterDriver::OnCalibratePlotter(WPARAM wParam, LPARAM lParam)
{
// TODO! calibrate m per step
    mTransformation.identity(3);        
// TODO! create Transformation for calibrated Plotteraxis X/Y/Z
	return 0;
}

LRESULT PlotterDriver::OnTestTurnRight(WPARAM wParam, LPARAM lParam)
{
    mControl.c.data_strobe = 0;
    mPort.setBits(LPTPort::control, mControl);

    MotorControl mc = {0};
    for (int i=0; i<mTestSteps; ++i)
    {
        mc.m.motor1 = mControlValue[mPositionX&Mask];
        mPort.setBits(LPTPort::data, mc.b);
        ++mPositionX;
        Sleep(mDelay_ms);
    }

    mControl.c.data_strobe = 1;
    mPort.setBits(LPTPort::control, mControl);
	return 0;
}

LRESULT PlotterDriver::OnPlotCommands(WPARAM wParam, LPARAM lParam)
{
    std::vector<CPlotterDC::DrawCommand>::const_iterator it;
    int fX, fY;
    for (it = mPlotCommands.begin(); it != mPlotCommands.end(); ++it)
    {
        switch (it->mType)
        {
        case  CPlotterDC::DrawCommand::MoveTo:
            transform_point(it->mPosition[0], it->mPosition[1], fX, fY);
            MoveToPos(fX, fY);
            break;
        case  CPlotterDC::DrawCommand::LineTo: 
            transform_point(it->mPosition[0], it->mPosition[1], fX, fY);
            LineToPos(fX, fY);
            break;

        }
    }
	return 0;
}

void  PlotterDriver::transform_point(FLOAT aXin, FLOAT aYin, int &aXout, int& aYout)
{
    aXout = round(aXin * mTransformation[0][0] + aYin * mTransformation[0][1] + mTransformation[0][2]);
    aYout = round(aXin * mTransformation[1][0] + aYin * mTransformation[1][1] + mTransformation[1][2]);
}

void PlotterDriver::sendStep()
{
   // TODO! control 3 stepper with 2 bits (eagle project)
    mMotors.m.motor1 = mControlValue[mPositionX&Mask];
    mMotors.m.motor2 = mControlValue[mPositionY&Mask];
    mMotors.m.motor3 = mControlValue[mPositionZ&Mask];
    mPort.setBits(LPTPort::data, mMotors.b);
    Sleep(mDelay_ms);
}

/* signum function */
int sgn(int x)
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

void PlotterDriver::MoveToPos(int aPosX, int aPosY)
{
    int fStepsX = aPosX - mPositionX;
    int fStepsY = aPosY - mPositionY;
    int fIncX   = sgn(fStepsX);
    int fIncY   = sgn(fStepsY);
    fStepsX     = abs(fStepsX);
    fStepsY     = abs(fStepsY);
    int fSteps  = max(fStepsX, fStepsY);
    for (int i=0; i<fSteps; ++i)
    {
        if (mPositionX != aPosX) mPositionX += fIncX;
        if (mPositionY != aPosY) mPositionY += fIncY;
        sendStep();
    }
}
 
void PlotterDriver::LineToPos(int aPosX, int aPosY)
/*--------------------------------------------------------------
 * Bresenham-Algorithmus: Linien auf Rastergeräten zeichnen
 *
 * Eingabeparameter:
 *    int xstart, ystart        = Koordinaten des Startpunkts
 *    int xend, yend            = Koordinaten des Endpunkts
 *
 * Ausgabe:
 *    void SetPixel(int x, int y) setze ein Pixel in der Grafik
 *         (wird in dieser oder aehnlicher Form vorausgesetzt)
 *---------------------------------------------------------------
 */
{
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;
 //int xstart,int ystart,int xend,int yend
/* Entfernung in beiden Dimensionen berechnen */
    dx = aPosX - mPositionX;
    dy = aPosY - mPositionY;
 
/* Vorzeichen des Inkrements bestimmen */
   incx = sgn(dx);
   incy = sgn(dy);
   if(dx<0) dx = -dx;
   if(dy<0) dy = -dy;
 
/* feststellen, welche Entfernung größer ist */
   if (dx>dy)
   {
      /* x ist schnelle Richtung */
      pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
   } else
   {
      /* y ist schnelle Richtung */
      pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
   }
 
/* Initialisierungen vor Schleifenbeginn */
   x = mPositionX;
   y = mPositionY;
   err = el/2;
   //SetPixel(x,y);
 
/* Pixel berechnen */
   for(t=0; t<el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
   {
      /* Aktualisierung Fehlerterm */
      err -= es;
      if(err<0)
      {
          /* Fehlerterm wieder positiv (>=0) machen */
          err += el;
          /* Schritt in langsame Richtung, Diagonalschritt */
          mPositionX += ddx;
          mPositionY += ddy;
      } else
      {
          /* Schritt in schnelle Richtung, Parallelschritt */
          mPositionX += pdx;
          mPositionY += pdy;
      }
      sendStep();
   }
} /* gbham() */
