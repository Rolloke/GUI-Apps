#include "graphics_view.h"
#include "actions.h"
#include <QMenu>

graphics_view::graphics_view(QWidget *parent) :
    QGraphicsView(parent)
  , mGraphicsScale(1.0)
  , mFitInView(false)
{
    setScene(new QGraphicsScene ());
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
