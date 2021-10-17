#include "graphics_view.h"
#include "actions.h"
#include <QMenu>

#include<QPainter>

#include <QGraphicsPixmapItem>
#ifdef QT_SVG_LIB
#include <QGraphicsSvgItem>
#endif

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

void graphics_view::addItem2graphicsView(QGraphicsItem*item)
{
    mGraphicsScale = 1.0;
    resetTransform();
    scene()->addItem(item);
    if (mFitInView)
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
    menu.exec(mapToGlobal(pos));
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
        auto items = scene()->items();
        if (items.size())
        {
            fitInView(items[0], Qt::KeepAspectRatio);
        }
    }
    else
    {
        resetTransform();
    }
}



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

    m_text = "hello World";
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
    painter->drawText(rect, Qt::AlignLeft|Qt::AlignTop, m_text, &m_bounding_rect);
    painter->drawRoundedRect(m_bounding_rect, 2.5, 2.5);
}
