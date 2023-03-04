#include "qdockwidgetx.h"

#include <QCloseEvent>

QDockWidgetX::QDockWidgetX(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags),
      m_closed(false)
{

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
