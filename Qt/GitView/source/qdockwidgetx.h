#ifndef QDOCKWIDGETX_H
#define QDOCKWIDGETX_H

#include <QDockWidget>

class QDockWidgetX: public QDockWidget
{
    Q_OBJECT
public:
    explicit QDockWidgetX(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    bool is_closing();
    void set_object_names(const QStringList& names);

Q_SIGNALS:
    void signal_close(QDockWidgetX*, bool &close);
    void signal_dock_widget_activated(QDockWidget *dockWidget);
public Q_SLOTS:
    void change_visibility(bool visible);
protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private:
    bool m_closed;
    QStringList m_object_names;
};


#endif // QDOCKWIDGETX_H
