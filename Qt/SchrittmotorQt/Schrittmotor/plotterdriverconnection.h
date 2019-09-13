#ifndef PLOTTERDRIVERCONNECTION_H
#define PLOTTERDRIVERCONNECTION_H

#include <QObject>

class PlotterDriverConnection : public QObject
{
    Q_OBJECT
public:
    explicit PlotterDriverConnection(QObject *parent = 0);

    void UpdatePlotterParam(float, int);
    void UpdatePlotterPosition(int, int, int);

signals:

    void updateParam(float, int);
    void updatePosition(int, int, int);

public slots:
};

#endif // PLOTTERDRIVERCONNECTION_H
