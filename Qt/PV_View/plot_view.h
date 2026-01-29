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
    void set_show_grid(bool show);
    void set_show_axis(bool show);
    void set_show_ticks(bool show);
    void set_show_legend(bool show);
    void set_ticks(int ticks);

    bool show_hover_values();
    bool show_grid();
    bool show_axis();
    bool show_ticks();
    bool show_legend();
    int  ticks();

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
    bool m_show_grid         = true;
    bool m_show_axis         = true;
    bool m_show_ticks        = true;
    bool m_show_legend       = true;
    int  m_ticks             = 5;
};

#endif // PLOT_VIEW_H
