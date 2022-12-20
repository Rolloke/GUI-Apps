#include "commit_graphics_item.h"
#include "history.h"

#include <QWidget>
#include <QPen>
#include <QFont>
#include <QPainter>

using namespace git;

commit_graphis_item::commit_graphis_item()
    : m_pen(0)
    , m_font(0)
    , m_position(0, 0)
{
    m_pen = new QPen(Qt::SolidLine);
    m_pen->setColor(Qt::darkRed);
    m_font_pen = new QPen(Qt::SolidLine);
    m_font_pen->setColor(Qt::black);

    m_font = new QFont("Courier", 12);
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

void commit_graphis_item::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRectF rect(m_position, QSizeF(1000, 1000));
    if (m_font) painter->setFont(*m_font);
    if (m_font_pen) painter->setPen(*m_font_pen);
    painter->drawText(rect, Qt::AlignLeft|Qt::AlignTop, get_subject_and_body().split("\n")[0], &m_bounding_rect);
    int text_height = m_bounding_rect.height();
    QRectF bounding;
    if (m_show_entry)
    {
        for (int entry = History::Entry::CommitHash; entry < History::Entry::NoOfEntries; ++ entry)
        {
            if (entry != History::Entry::SubjectAndBody && m_show_entry(entry))
            {
                rect.translate(QPointF(0, text_height));
                painter->drawText(rect, Qt::AlignLeft|Qt::AlignTop, m_items[entry], &bounding);
                m_bounding_rect = bounding.united(m_bounding_rect);
            }
        }
    }
    m_bounding_rect += QMargins(5, 1, 5, 1);
    if (m_font_pen) painter->setPen(*m_pen);
    painter->drawRoundedRect(m_bounding_rect, 5, 5);
    for (int i=0; i<m_connections.size(); ++i)
    {
        m_connections[i]->paint(painter, option, widget);
    }
}

void commit_graphis_item::set_pen(QPen *aPen)
{
    delete m_pen;
    m_pen   = aPen;
}

void commit_graphis_item::set_font(QFont *aFont)
{
    delete m_font;
    m_font  = aFont;
}

void commit_graphis_item::set_offset_pos(const QPointF &aOffset)
{
    m_position = aOffset;
}

const QPointF& commit_graphis_item::offset_pos() const
{
    return m_position;
}

void commit_graphis_item::set_history(const QStringList &items)
{
    m_items = items;
    QImage device(1000, 1000, QImage::Format_Mono);
    QPainter dummy(&device);
    paint(&dummy, nullptr, nullptr);
}

/// @brief parent hashes
/// @note may be more than one, and refer to different commit hashes
/// - one parent refers to the dividing commit hash
/// - another parent refers to the previous commit hash
/// - the commit of a parent pair is a uniting or merging commit
QString commit_graphis_item::get_parent_hash(parent parent_index) const
{
    QStringList parents = m_items[History::Entry::ParentHash].split(" ");
    int index = static_cast<int>(parent_index);
    if (index < parents.size())
    {
        return parents[index];
    }
    return "";
}

const QString &commit_graphis_item::get_tree_hash() const
{
    return m_items[History::Entry::TreeHash];
}

/// @brief commit hash
/// @note refer to a parent hash
/// if the commit refers to one parent of more parents this is a dividing commit
/// if the commit refers to the other parent this is the merging commit
const QString &commit_graphis_item::get_commit_hash() const
{
    return m_items[History::Entry::CommitHash];
}

const QString &commit_graphis_item::get_author() const
{
    return m_items[History::Entry::Author];
}

const QString &commit_graphis_item::get_author_date() const
{
    return m_items[History::Entry::AuthorDate];
}

const QString &commit_graphis_item::get_author_email() const
{
    return m_items[History::Entry::AuthoEmail];
}

const QString &commit_graphis_item::get_committer() const
{
    return m_items[History::Entry::Committer];
}

const QString &commit_graphis_item::get_committer_date() const
{
    return m_items[History::Entry::CommitterDate];
}

const QString &commit_graphis_item::get_subject_and_body() const
{
    return m_items[History::Entry::SubjectAndBody];
}

const QList<poly_line_item*>& commit_graphis_item::get_connection() const
{
    return m_connections;
}

void commit_graphis_item::add_connection(poly_line_item* connection)
{
    m_connections.push_back(connection);
}

void commit_graphis_item::create_connections(int index, const QList<QGraphicsItem *> &items)
{
    commit_graphis_item* cgi = dynamic_cast<commit_graphis_item*>(items[index]);

    QString parent_1 = cgi->get_parent_hash();
    QString parent_2 = cgi->get_parent_hash(commit_graphis_item::parent::second);
    for (int index_ref = 0; index_ref < items.size(); ++index_ref)
    {
        int diff = index - index_ref;
        qreal offset = 10;
        commit_graphis_item* cgi_ref = dynamic_cast<commit_graphis_item*>(items[index_ref]);

        Qt::GlobalColor line_color { Qt::transparent };
        if      (parent_1 == cgi_ref->get_commit_hash())                    { line_color = Qt::green;   }
        else if (parent_2.size() && parent_2 == cgi_ref->get_commit_hash()) { line_color = Qt::blue;    }
        else if (diff == 1)                                                 { line_color = Qt::magenta; }
        if (line_color != Qt::transparent)
        {
            QPen* line_pen = new QPen(QBrush(line_color), 2, Qt::SolidLine);
            poly_line_item * poly_line = new poly_line_item;
            poly_line->setPen(line_pen);
#if 1
            poly_line->connect(cgi->boundingRect().bottomLeft(), cgi_ref->boundingRect().topLeft(), diff, offset+diff*3);
#else
            poly_line->connect(QPointF(cgi->boundingRect().left(), cgi->boundingRect().center().ry()),
                               QPointF(cgi_ref->boundingRect().left(), cgi_ref->boundingRect().center().ry()), diff, offset+diff*2);
#endif
            cgi_ref->add_connection(poly_line);
        }
    }
}

poly_line_item::poly_line_item() : mpPen(0)
{

}

poly_line_item::~poly_line_item()
{
    delete mpPen;
}

QRectF poly_line_item::boundingRect() const
{
    return mPolyLine.boundingRect();
}

void poly_line_item::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    if (mpPen) painter->setPen(*mpPen);
    painter->drawPolyline(mPolyLine);
}

void poly_line_item::connect(const QPointF& p1, const QPointF& p2, int add_points, qreal horz_offset)
{
    qreal top    = p1.y();
    qreal bottom = p2.y();
    if (top < bottom) std::swap(top, bottom);
    mPolyLine.append(QPointF(p1.x(), top));
    if (add_points == 1)
    {
        mPolyLine.append(QPointF(p1.x()-horz_offset, (top + bottom)*0.5));
    }
    else if (add_points > 1)
    {
        mPolyLine.append(QPointF(p1.x()-horz_offset, top - horz_offset));
        mPolyLine.append(QPointF(p1.x()-horz_offset, bottom + horz_offset));
    }
    mPolyLine.append(QPointF(p2.x(), bottom));
}

