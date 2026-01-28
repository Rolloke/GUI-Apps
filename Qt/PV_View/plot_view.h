#ifndef PLOT_VIEW_H
#define PLOT_VIEW_H
#pragma once

#include <QGraphicsView>
#include <QVector>
#include <QPointF>

class PlotView : public QGraphicsView {
    Q_OBJECT

public:
    explicit PlotView(QWidget* parent = nullptr);

    void setCurves(const QVector<QVector<QPointF>>& curves);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void redraw();

    QVector<QVector<QPointF>> m_curves;
};

#endif // PLOT_VIEW_H
