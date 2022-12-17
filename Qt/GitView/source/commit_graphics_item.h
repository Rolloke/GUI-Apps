#ifndef COMMIT_GRAPHICS_ITEM_H
#define COMMIT_GRAPHICS_ITEM_H
#include <QGraphicsItem>

class commit_graphis_item : public QGraphicsItem
{
 public:
     commit_graphis_item();
     virtual ~commit_graphis_item();

     void setText(const QStringList& items) { m_items = items; }
     void setPen(QPen *aPen);
     void setFont(QFont *aFont);
     void offsetPos(const QPointF& aOffset) { m_position += aOffset;  }
     const QString& get_parent_hash() const;
     const QString& get_tree_hash() const;
     const QString& get_commit_hash() const;
     const QString& get_subject_and_body() const;
     const QString& get_author() const;
     const QString& get_author_email() const;
     const QString& get_author_date() const;
     const QString& get_committer() const;
     const QString& get_committer_email() const;
     const QString& get_committer_date() const;

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     QPen*     m_pen;
     QFont*    m_font;
     QStringList m_items;
     QPointF m_position;
     QRectF  m_bounding_rect;
};

#endif // COMMIT_GRAPHICS_ITEM_H
