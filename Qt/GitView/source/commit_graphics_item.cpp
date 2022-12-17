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
    m_pen = new QPen(Qt::SolidLine);
    m_pen->setColor(QColor(200,0,0));

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

void commit_graphis_item::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    QRectF rect(m_position, QSizeF(1000, 1000));
    if (m_font) painter->setFont(*m_font);
    if (m_pen) painter->setPen(*m_pen);
    painter->drawText(rect, Qt::AlignLeft|Qt::AlignTop, get_subject_and_body().split("\n")[0], &m_bounding_rect);
    painter->drawRoundedRect(m_bounding_rect, 2.5, 2.5);
}

void commit_graphis_item::setPen(QPen *aPen)
{
    delete m_pen;
    m_pen   = aPen;
}

void commit_graphis_item::setFont(QFont *aFont)
{
    delete m_font;
    m_font  = aFont;
}

/// @brief parent hashes
/// @note may be more than one, and refer to different commit hashes
/// - the first parent refers to the dividing commit hash
/// - the second parent refers to the previous commit hash
/// - the commit of a parent pair is a uniting commit
const QString &commit_graphis_item::get_parent_hash() const
{
    return m_items[History::Entry::ParentHash];
}

const QString &commit_graphis_item::get_tree_hash() const
{
    return m_items[History::Entry::TreeHash];
}

/// @brief commit hash
/// @note refer to a parent hash
/// if the commit refers to a first parent of more parents this is a dividing commit
/// if the commit refers to a second parent this is the previous unite commit
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
