#ifndef QDOCKWIDGETX_H
#define QDOCKWIDGETX_H

#include <QDockWidget>

class QDockWidgetX: public QDockWidget
{
    Q_OBJECT
public:
    explicit QDockWidgetX(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    bool is_closing();
Q_SIGNALS:
    void signal_close(QDockWidgetX*, bool &close);
public Q_SLOTS:

protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private:
    bool m_closed;
};


#endif // QDOCKWIDGETX_H
