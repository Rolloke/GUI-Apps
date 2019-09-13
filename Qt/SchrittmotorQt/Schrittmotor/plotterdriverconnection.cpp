#include "plotterdriverconnection.h"

PlotterDriverConnection::PlotterDriverConnection(QObject *parent) : QObject(parent)
{

}

void PlotterDriverConnection::UpdatePlotterParam(float aValue, int aType)
{
    Q_EMIT updateParam(aValue, aType);
}

void PlotterDriverConnection::UpdatePlotterPosition(int aX, int aY, int aZ)
{
    Q_EMIT updatePosition(aX, aY, aZ);
}
