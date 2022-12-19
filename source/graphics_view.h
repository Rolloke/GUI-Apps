#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QGraphicsView>

class ActionList;
class QGraphicsItem;

class graphics_view : public QGraphicsView
{
    enum class graphic { off, set, rendered };
    Q_OBJECT
public:
    explicit graphics_view(QWidget *parent = 0);

    void addItem2graphicsView(QGraphicsItem*item, bool reset_view=true);
    void on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos);
    bool render_file(const QString& file_name, const QString& file_extension);
    bool has_rendered_graphic();

signals:

public slots:
    void clear();
    void zoomIn();
    void zoomOut();
    void fit_inView(bool);
    void updateView();
    void insert_history(const QStringList&);

private:
    double mGraphicsScale;
    bool    mFitInView;
    graphic mHistory;
};


#endif // GRAPHICS_VIEW_H
