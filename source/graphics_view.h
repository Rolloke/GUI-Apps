#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QGraphicsView>
class QGraphicsItem;
class ActionList;


class graphics_view : public QGraphicsView
{
    Q_OBJECT
public:
    explicit graphics_view(QWidget *parent = 0);

    void addItem2graphicsView(QGraphicsItem*item);
    void on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    void clear();

signals:

public slots:
    void zoomIn();
    void zoomOut();
    void fit_inView(bool);

private:
    double mGraphicsScale;
    bool   mFitInView;
};

#endif // GRAPHICS_VIEW_H
