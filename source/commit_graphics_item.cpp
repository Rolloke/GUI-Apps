#include "commit_graphics_item.h"
#include "history.h"
#include <QPen>
#include <QFont>
#include <QPainter>

using namespace git;

commit_graphis_item::commit_graphis_item()
    : m_pen(0)
    , m_font(0)
    , m_position(0, 0)
{
    auto * pen = new QPen(Qt::SolidLine);
    pen->setColor(QColor(200,0,0));
    setPen(pen);

    auto *font = new QFont("Courier", 12);
    setFont(font);
}

commit_graphis_item::~commit_graphis_item()
{
    delete m_pen;
    delete m_font;
}

QRectF commit_graphis_item::boundingRect() const
{
    return m_bounding_rect;
}

void commit_graphis_item::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    QRectF rect(m_position, QSizeF(1000, 1000));
    if (m_font) painter->setFont(*m_font);
    if (m_pen) painter->setPen(*m_pen);
    painter->drawText(rect, Qt::AlignLeft|Qt::AlignTop, m_items[History::Entry::SubjectAndBody].split("\n")[0], &m_bounding_rect);
    painter->drawRoundedRect(m_bounding_rect, 2.5, 2.5);
}
