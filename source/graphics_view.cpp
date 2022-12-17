#include "graphics_view.h"
#include "actions.h"
#include "helper.h"
#include "commit_graphics_item.h"
#include <QMenu>

#include<QPainter>

#include <QGraphicsPixmapItem>
#ifdef QT_SVG_LIB
#include <QGraphicsSvgItem>
#endif


/// TODO: show branches graphically

graphics_view::graphics_view(QWidget *parent) :
    QGraphicsView(parent)
  , mGraphicsScale(1.0)
  , mFitInView(false)
{
    setScene(new QGraphicsScene ());
}

bool graphics_view::render_file(const QString& file_name, const QString& file_extension)
{
    const QStringList graphic_formats = {"bmp", "gif", "jpg", "jpeg", "png", "pbm", "pgm", "ppm", "xbm", "xpm" };
    bool file_added_to_grapics_view = false;
#ifdef QT_SVG_LIB
    if (file_extension == "svg")
    {
        auto svg_image = new QGraphicsSvgItem(file_name);
        int type = svg_image->type();
        if (type)
        {
            addItem2graphicsView(svg_image);

            file_added_to_grapics_view = true;
        }
    }
#endif

    if (graphic_formats.contains(file_extension))
    {
        QImage image(file_name);

        if(!image.isNull())
        {
            addItem2graphicsView(new QGraphicsPixmapItem(QPixmap::fromImage(image)));
            file_added_to_grapics_view = true;
        }
    }
    return file_added_to_grapics_view;
}

void graphics_view::addItem2graphicsView(QGraphicsItem*item, bool reset_view)
{
    if (reset_view)
    {
        mGraphicsScale = 1.0;
        resetTransform();
    }

    scene()->addItem(item);

    if (mFitInView && reset_view)
    {
        fitInView(item, Qt::KeepAspectRatio);
    }
}

void graphics_view::zoomIn()
{
    mGraphicsScale *= 1.1;
    scale(mGraphicsScale, mGraphicsScale);
}

void graphics_view::zoomOut()
{
    mGraphicsScale *= 0.9;
    scale(mGraphicsScale, mGraphicsScale);
}

void graphics_view::on_customContextMenuRequested(const ActionList& aActionList, const QPoint &pos)
{
    using namespace git;
    aActionList.getAction(Cmd::FitInView)->setChecked(mFitInView);
    QMenu menu(this);
    aActionList.fillContextMenue(menu, Cmd::mContextMenuGraphicsView);
    menu.addAction(tr("Cancel"));
    menu.exec(mapToGlobal(pos)+ menu_offset);
}

void graphics_view::clear()
{
    scene()->clear();
}

void graphics_view::fit_inView(bool fit)
{
    mFitInView = fit;
    if (mFitInView)
    {
        const auto& items = scene()->items();
        QRectF bounding_rect;
        for (const auto& item : items)
        {
            bounding_rect = bounding_rect.united(item->boundingRect());
        }
        setSceneRect(bounding_rect);
        //translate(-bounding_rect.width()/2, -bounding_rect.height()/2);
        fitInView(bounding_rect, Qt::KeepAspectRatio);
        //ensureVisible(bounding_rect);
    }
    else
    {
        resetTransform();
    }
}

void graphics_view::insert_history(const QStringList & list)
{
    auto item = new commit_graphis_item();
    int size = scene()->items().size();
    item->setText(list);
    item->offsetPos(QPointF(0, size * 25));
    addItem2graphicsView(item, false);
}

