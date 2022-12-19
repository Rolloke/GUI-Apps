#include "graphics_view.h"
#include "actions.h"
#include "helper.h"
#include "commit_graphics_item.h"
#include <QMenu>
#include <QPen>

#include<QPainter>

#include <QGraphicsPixmapItem>
#ifdef QT_SVG_LIB
#include <QGraphicsSvgItem>
#endif



graphics_view::graphics_view(QWidget *parent) :
    QGraphicsView(parent)
  , mGraphicsScale(1.0)
  , mFitInView(false)
  , mHistory(graphic::off)
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

bool graphics_view::has_rendered_graphic()
{
    return mHistory != graphic::off;
}

void graphics_view::clear()
{
    scene()->clear();
    mHistory = graphic::off;
}

void graphics_view::updateView()
{
    scene()->update(scene()->sceneRect());
}

void graphics_view::fit_inView(bool fit)
{
    mFitInView = fit;
    const auto& items = scene()->items();
    QRectF bounding_rect;
    for (int i=0; i<items.size(); ++i)
    {
        commit_graphis_item* cgi = dynamic_cast<commit_graphis_item*>(items[i]);
        bounding_rect = bounding_rect.united(items[i]->boundingRect());
        if (mHistory == graphic::set)
        {
            QString parent_1 = cgi->get_parent_hash();
            QString parent_2 = cgi->get_parent_hash(commit_graphis_item::parent::second);
            for (int j=0; j<items.size(); ++j)
            {
                // TODO: validate hash connection
                int diff = std::abs(i - j);
                qreal offset = 10;
                commit_graphis_item* cgi_ref = dynamic_cast<commit_graphis_item*>(items[j]);
                if (parent_1 == cgi_ref->get_commit_hash())
                {
                    QPen* line_pen = new QPen(Qt::SolidLine);
                    line_pen->setColor(QColor(0, 200,0));
                    line_pen->setWidth(2);
                    poly_line_item * poly_line = new poly_line_item;
                    poly_line->setPen(line_pen);
                    poly_line->connect(cgi->boundingRect().bottomLeft(), cgi_ref->boundingRect().topLeft(), diff, offset+diff*2);
                    cgi_ref->add_connection(poly_line);
                }
                if (parent_2.size() && parent_2 == cgi_ref->get_commit_hash())
                {
                    QPen* line_pen = new QPen(Qt::SolidLine);
                    line_pen->setWidth(2);
                    line_pen->setColor(QColor(0, 0, 200));
                    poly_line_item * poly_line = new poly_line_item;
                    poly_line->setPen(line_pen);
                    poly_line->connect(cgi->boundingRect().bottomLeft(), cgi_ref->boundingRect().topLeft(), diff, offset+diff*2);
                    cgi_ref->add_connection(poly_line);
                }
            }
        }
    }
    if (mHistory == graphic::set)
    {
        bounding_rect.marginsAdded(QMargins(20, 20, 20, 20));
        setViewportUpdateMode(FullViewportUpdate);
        mHistory = graphic::rendered;
    }
    else
    {
        setViewportUpdateMode(MinimalViewportUpdate);
    }
    setSceneRect(bounding_rect);
    if (mFitInView)
    {
        fitInView(bounding_rect, Qt::KeepAspectRatio);
    }
    else
    {
        resetTransform();
    }
}

void graphics_view::insert_history(const QStringList & list)
{
    if (list.isEmpty())
    {
        fit_inView(mFitInView);
    }
    else
    {
        const auto items = scene()->items();
        const int size = items.size();
        int height { 0 };
        if (size > 0)
        {
             height = items[size-1]->boundingRect().height();
        }

        auto item = new commit_graphis_item();
        item->set_offset_pos(QPointF(50, size * (height + 10)));
        item->set_history(list);
        addItem2graphicsView(item, false);
        mHistory = graphic::set;
    }
}

