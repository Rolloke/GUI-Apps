#include "qdockwidgetx.h"

#include <QCloseEvent>


QDockWidgetX::QDockWidgetX(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags),
      m_closed(false)
{
    connect(this, SIGNAL(visibilityChanged(bool)), SLOT(change_visibility(bool)));
}

void QDockWidgetX::closeEvent(QCloseEvent *event)
{
    Q_EMIT signal_close(this, m_closed);
    if (m_closed)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool QDockWidgetX::event(QEvent *event)
{
    return QDockWidget::event(event);
}

bool QDockWidgetX::is_closing()
{
    return m_closed;
}

void QDockWidgetX::set_object_names(const QStringList& names)
{
    m_object_names = names;
}

void QDockWidgetX::change_visibility(bool visible)
{
    if (visible && m_object_names.contains(objectName()))
    {
        Q_EMIT signal_dock_widget_activated(this);
    }
}
