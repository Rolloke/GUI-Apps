#include "plot_view.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <QWheelEvent>
#include <QtMath>

PlotView::PlotView(QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void PlotView::setCurves(const QVector<QVector<QPointF>>& curves, const QStringList& names)
{
    m_curves = curves;
    m_names = names;
    redraw();
}

void PlotView::set_show_hover_values(bool show)
{
    m_show_hover_values = show;
}

void PlotView::set_show_grid(bool show)
{
    m_show_grid = show;
    redraw();
}

void PlotView::set_show_axis(bool show)
{
    m_show_axis = show;
    redraw();
}

void PlotView::set_show_ticks(bool show)
{
    m_show_ticks = show;
    redraw();
}

void PlotView::set_show_legend(bool show)
{
    m_show_legend = show;
    redraw();
}

void PlotView::set_ticks(int ticks)
{
    m_ticks = ticks;
}

bool PlotView::show_hover_values()
{
    return m_show_hover_values;
}

bool PlotView::show_grid()
{
    return m_show_grid;
}

bool PlotView::show_axis()
{
    return m_show_axis;
}

bool PlotView::show_ticks()
{
    return m_show_ticks;
}

bool PlotView::show_legend()
{
    return m_show_legend;
}

int PlotView::ticks()
{
    return m_ticks;
}

void PlotView::resizeEvent(QResizeEvent*)
{
    redraw();
}

void PlotView::wheelEvent(QWheelEvent* e)
{
    const qreal scaleFactor = e->angleDelta().y() > 0 ? 1.15 : 0.87;
    scale(scaleFactor, scaleFactor);
}

void PlotView::mousePressEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        m_panning = true;
        m_lastPanPoint = e->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void PlotView::mouseMoveEvent(QMouseEvent* e)
{
    if (m_show_hover_values)
    {
        if (m_panning)
        {
            QPointF delta = mapToScene(e->pos()) - mapToScene(m_lastPanPoint);
            translate(delta.x(), delta.y());
            m_lastPanPoint = e->pos();
        }
        else
        {
            QPointF dataPos = mapToData(mapToScene(e->pos()));
            QPointF nearest = nearestPoint(dataPos);

            if (!m_hoverText)
            {
                m_hoverText = scene()->addText("");
                m_hoverText->setDefaultTextColor(Qt::darkGray);
            }

            m_hoverText->setPlainText(QString("x=%1\ny=%2").arg(nearest.x(), 0, 'g', 4).arg(nearest.y(), 0, 'g', 4));
            m_hoverText->setPos(mapToScene(e->pos()) + QPointF(10, -10));
        }
    }
}

void PlotView::mouseReleaseEvent(QMouseEvent*)
{
    m_panning = false;
    setCursor(Qt::ArrowCursor);
}

void PlotView::redraw()
{
    if (m_curves.isEmpty())
    {
        return;
    }
    /// TODO: refactor magic numbers

    scene()->clear();
    resetTransform();
    scene()->setSceneRect(viewport()->rect());

    const qreal margin = 60;
    QRectF r = scene()->sceneRect();
    m_plotRect = QRectF(margin, margin, r.width() - 2 * margin, r.height() - 2 * margin);

    // --- Bounds ---
    minX = maxX = m_curves[0][0].x();
    minY = maxY = m_curves[0][0].y();

    for (auto& c : m_curves)
    {
        for (auto& p : c)
        {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
            minY = qMin(minY, p.y());
            maxY = qMax(maxY, p.y());
        }
    }
    auto sx = [&](qreal x)
    {
        return m_plotRect.left()   + (x - minX) / (maxX - minX) * m_plotRect.width();
    };
    auto sy = [&](qreal y)
    {
        return m_plotRect.bottom() - (y - minY) / (maxY - minY) * m_plotRect.height();
    };

    // --- Grid + axes ---
    if (m_show_grid)
    {
        QPen gridPen(Qt::lightGray, 1, Qt::DashLine);

        for (int i = 0; i <= m_ticks; ++i)
        {
            qreal x = m_plotRect.left() + i * m_plotRect.width() / m_ticks;
            qreal y = m_plotRect.bottom() - i * m_plotRect.height() / m_ticks;

            scene()->addLine(x, m_plotRect.top(), x, m_plotRect.bottom(), gridPen);
            scene()->addLine(m_plotRect.left(), y, m_plotRect.right(), y, gridPen);
        }
    }

    // --- Axes ---
    if (m_show_axis)
    {
        scene()->addLine(m_plotRect.left(), m_plotRect.bottom(), m_plotRect.right(), m_plotRect.bottom(), QPen(Qt::black, 2));
        scene()->addLine(m_plotRect.left(), m_plotRect.top()   , m_plotRect.left() , m_plotRect.bottom(), QPen(Qt::black, 2));
    }
    if (m_show_ticks)
    {
        QFont labelFont;
        labelFont.setPointSize(8);

        for (int i = 0; i <= m_ticks; ++i) {
            qreal tx = m_plotRect.left() + i * m_plotRect.width() / m_ticks;
            qreal ty = m_plotRect.bottom() - i * m_plotRect.height() / m_ticks;

            // X ticks
            scene()->addLine(tx, m_plotRect.bottom(), tx, m_plotRect.bottom() + 5);
            qreal xVal = minX + i * (maxX - minX) / m_ticks;
            auto xLabel = scene()->addText(QString::number(xVal, 'g', 3), labelFont);
            xLabel->setPos(tx - 10, m_plotRect.bottom() + 8);

            // Y ticks
            scene()->addLine(m_plotRect.left() - 5, ty, m_plotRect.left(), ty);
            qreal yVal = minY + i * (maxY - minY) / m_ticks;
            auto yLabel = scene()->addText(QString::number(yVal, 'g', 3), labelFont);
            yLabel->setPos(5, ty - 8);
        }
    }

    if (!m_show_axis)
    {
        scene()->addRect(m_plotRect, QPen(Qt::black, 2));
    }

    // --- Curves ---
    QVector<QColor> colors =
    {
        Qt::red, Qt::blue, Qt::darkGreen,
        Qt::magenta, Qt::darkCyan
    };

    for (int c = 0; c < m_curves.size(); ++c)
    {
        QPainterPath path;
        path.moveTo(sx(m_curves[c][0].x()), sy(m_curves[c][0].y()));
        for (int i = 1; i < m_curves[c].size(); ++i)
        {
            path.lineTo(sx(m_curves[c][i].x()), sy(m_curves[c][i].y()));
        }
        scene()->addPath(path, QPen(colors[c % colors.size()], 2));
    }

    if (m_show_legend)
    {
        // --- Legend ---
        qreal lx = m_plotRect.right() - 120;
        qreal ly = m_plotRect.top()   + 10;

        for (int i = 0; i < m_curves.size(); ++i)
        {
            scene()->addRect(lx, ly + i * 20, 12, 12, QPen(), QBrush(colors[i % colors.size()]));
            scene()->addText(m_names.value(i, QString("Curve %1").arg(i + 1)))->setPos(lx + 18, ly + i * 20 - 2);
        }
    }
}

QPointF PlotView::mapToData(const QPointF& s) const
{
    qreal x = minX + (s.x() - m_plotRect.left())   / m_plotRect.width()  * (maxX - minX);
    qreal y = minY + (m_plotRect.bottom() - s.y()) / m_plotRect.height() * (maxY - minY);
    return {x, y};
}

QPointF PlotView::nearestPoint(const QPointF& p) const
{
    QPointF best;
    qreal bestDist = 1e100;

    for (auto& c : m_curves)
    {
        for (auto& pt : c)
        {
            qreal d = QLineF(p, pt).length();
            if (d < bestDist)
            {
                bestDist = d;
                best = pt;
            }
        }
    }
    return best;
}
