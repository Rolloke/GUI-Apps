#ifndef COMMIT_GRAPHICS_ITEM_H
#define COMMIT_GRAPHICS_ITEM_H
#include <QGraphicsItem>
#include <functional>

class poly_line_item;
class commit_graphis_item : public QGraphicsItem
{
public:
    commit_graphis_item();
    virtual ~commit_graphis_item();
    enum class parent { first, second };

    void set_history(const QStringList& items);
    void set_pen(QPen *aPen);
    void set_font(QFont *aFont);
    void set_offset_pos(const QPointF& aOffset);

    const QPointF& offset_pos() const;
    QString        get_parent_hash(parent parent_index=parent::first) const;
    const QString& get_tree_hash() const;
    const QString& get_commit_hash() const;
    const QString& get_subject_and_body() const;
    const QString& get_author() const;
    const QString& get_author_email() const;
    const QString& get_author_date() const;
    const QString& get_committer() const;
    const QString& get_committer_email() const;
    const QString& get_committer_date() const;

    static void create_connections(int index, const QList<QGraphicsItem *> &items);

    QRectF boundingRect() const override;
    void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    std::function<bool (int)> m_show_entry;

private:
    const QList<poly_line_item*>& get_connection() const;
    void add_connection(poly_line_item* connection);

    QPen*     m_pen;
    QPen*     m_font_pen;
    QFont*    m_font;
    QStringList m_items;
    QPointF m_position;
    QRectF  m_bounding_rect;
    QList<poly_line_item*> m_connections;
};

class poly_line_item : public QGraphicsItem
{
 public:
     poly_line_item();
     virtual ~poly_line_item();

     void setPen(const QPen* aPen) { mpPen = aPen; }
     QPolygonF& getPolyLine() { return mPolyLine; }

     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
     QRectF boundingRect() const override;
     void   connect(const QPointF& p1, const QPointF& p2, int add_points, qreal horz_offset);

private:
     QPolygonF mPolyLine;
     const QPen*     mpPen;
 };

#endif // COMMIT_GRAPHICS_ITEM_H
