

#include "plot_view.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <QtMath>

PlotView::PlotView(QWidget* parent): QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
}

void PlotView::setCurves(const QVector<QVector<QPointF>>& curves)
{
    m_curves = curves;
    redraw();
}

void PlotView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    redraw();
}

void PlotView::redraw()
{
    if (m_curves.isEmpty())
        return;

    scene()->clear();
    scene()->setSceneRect(viewport()->rect());

    const qreal margin = 50.0;
    QRectF r = scene()->sceneRect();
    QRectF plotRect(
        margin,
        margin,
        r.width() - 2 * margin,
        r.height() - 2 * margin
        );

    // --- Compute global bounds ---
    qreal minX = m_curves[0][0].x();
    qreal maxX = minX;
    qreal minY = m_curves[0][0].y();
    qreal maxY = minY;

    for (const auto& curve : m_curves) {
        for (const auto& p : curve) {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
            minY = qMin(minY, p.y());
            maxY = qMax(maxY, p.y());
        }
    }

    if (qFuzzyCompare(minX, maxX)) maxX += 1;
    if (qFuzzyCompare(minY, maxY)) maxY += 1;

    auto sx = [&](qreal x) {
        return plotRect.left() +
               (x - minX) / (maxX - minX) * plotRect.width();
    };
    auto sy = [&](qreal y) {
        return plotRect.bottom() -
               (y - minY) / (maxY - minY) * plotRect.height();
    };

    // --- Axes ---
    scene()->addLine(
        plotRect.left(), plotRect.bottom(),
        plotRect.right(), plotRect.bottom(),
        QPen(Qt::black, 2)
        );
    scene()->addLine(
        plotRect.left(), plotRect.top(),
        plotRect.left(), plotRect.bottom(),
        QPen(Qt::black, 2)
        );

    // --- Ticks & labels ---
    const int ticks = 5;
    QFont labelFont;
    labelFont.setPointSize(8);

    for (int i = 0; i <= ticks; ++i) {
        qreal tx = plotRect.left() + i * plotRect.width() / ticks;
        qreal ty = plotRect.bottom() - i * plotRect.height() / ticks;

        // X ticks
        scene()->addLine(tx, plotRect.bottom(), tx, plotRect.bottom() + 5);
        qreal xVal = minX + i * (maxX - minX) / ticks;
        auto xLabel = scene()->addText(QString::number(xVal, 'g', 3), labelFont);
        xLabel->setPos(tx - 10, plotRect.bottom() + 8);

        // Y ticks
        scene()->addLine(plotRect.left() - 5, ty, plotRect.left(), ty);
        qreal yVal = minY + i * (maxY - minY) / ticks;
        auto yLabel = scene()->addText(QString::number(yVal, 'g', 3), labelFont);
        yLabel->setPos(5, ty - 8);
    }

    // --- Draw curves ---
    QVector<QColor> colors = {
        Qt::red, Qt::blue, Qt::darkGreen,
        Qt::magenta, Qt::darkCyan
    };

    for (int c = 0; c < m_curves.size(); ++c) {
        const auto& curve = m_curves[c];
        if (curve.isEmpty())
            continue;

        QPainterPath path;
        path.moveTo(sx(curve[0].x()), sy(curve[0].y()));

        for (int i = 1; i < curve.size(); ++i)
            path.lineTo(sx(curve[i].x()), sy(curve[i].y()));

        scene()->addPath(
            path,
            QPen(colors[c % colors.size()], 2)
            );
    }
}
