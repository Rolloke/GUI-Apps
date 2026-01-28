#ifndef PLOT_VIEW_H
#define PLOT_VIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QPointF>

class PlotView : public QGraphicsView {
    Q_OBJECT

public:
    explicit PlotView(QWidget* parent = nullptr);

    void setCurves(const QVector<QVector<QPointF>>& curves, const QStringList& names = {});
    void set_show_hover_values(bool show);

protected:
    void resizeEvent(QResizeEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    void redraw();
    QPointF mapToData(const QPointF& scenePos) const;
    QPointF nearestPoint(const QPointF& dataPos) const;

    QVector<QVector<QPointF>> m_curves;
    QStringList m_names;

    QRectF m_plotRect;
    qreal minX, maxX, minY, maxY;

    bool m_panning = false;
    QPoint m_lastPanPoint;

    QGraphicsTextItem* m_hoverText = nullptr;
    bool m_show_hover_values = false;
};

#endif // PLOT_VIEW_H
